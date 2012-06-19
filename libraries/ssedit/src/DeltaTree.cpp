/*****************************************************************************
 * Copyright (c) 2011-2012 The SkelCL Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of SkelCL.                                              *
 * SkelCL is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * SkelCL is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * SkelCL is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at michel.steuwer@uni-muenster.de      *
 *                                                                           *
 *****************************************************************************/

/*
 * DeltaTree.cpp
 *
 *      An AVL-Tree
 */

#include <iostream>
#include <memory>

#include "ssedit/DeltaTree.h"

namespace {

class DeltaComparator {
public:
  int operator()(const ssedit::Delta &lhs, const ssedit::Delta &rhs) {
    auto lhsOffset = lhs.getRange().getStart().offset;
    auto rhsOffset = rhs.getRange().getStart().offset;
    if(lhsOffset < rhsOffset) {
      return -1;
    } else if(lhsOffset > rhsOffset) {
      return 1;
    } else {
      return 0;
    }
  }
};

} // namespace

namespace ssedit {

///////////////// Iterator ///////////////////////////////

DeltaTree::Iterator::Iterator(std::shared_ptr<DeltaTree::DeltaNode> node)
  : _node(node)
{
}

Delta& DeltaTree::Iterator::operator*()
{
  return *(_node->_delta);
}

std::shared_ptr<Delta> DeltaTree::Iterator::operator->()
{
  return _node->_delta;
}

bool DeltaTree::Iterator::operator==(const DeltaTree::Iterator &rhs)
{
  return (_node == rhs._node);
}

bool DeltaTree::Iterator::operator!=(const DeltaTree::Iterator &rhs)
{
  return (_node != rhs._node);
}

/**
 * Traverses the DeltaTree in-order
 */
DeltaTree::Iterator& DeltaTree::Iterator::operator++()
{
  if(_node == nullptr) {
    return *this;
  }

  _node->_visited = true;

  if(_node->_rightNode == nullptr) {
    while (    _node->_parent != nullptr
            && _node->_visited == true ) {
      _node = _node->_parent;
    }
#if 0 // old solution without _visited
    //We are at a leaf. Traverse the tree up
    if(   _node->_parent != nullptr
       && _node->_parent->_leftNode == _node) {
      //We are coming from the left subtree
      _node = _node->_parent;
      return *this;
    } else {
      //we are coming from the right subtree, which means that we traversed the whole right subtree of a node. Climb the tree up to this node
      while(   _node->_parent != nullptr
            && _node->_parent->_leftNode != _node) {
        _node = _node->_parent;
      }
      _node = _node->_parent;
      return *this;
    }

    if(_node->_parent == nullptr) {
      _node = std::shared_ptr<DeltaNode>();
    }
#endif
    return *this;
  } else {
    // traverse downwards
    _node = _node->_rightNode;
    //Set _node to the most left leaf
    while(_node->_leftNode != nullptr){
      _node = _node->_leftNode;
    }
  }
  return *this;
}

///////////////// ReverseIterator ///////////////////

DeltaTree::ReverseIterator::ReverseIterator(std::shared_ptr<DeltaNode> node)
  : _node(node)
{
}

Delta& DeltaTree::ReverseIterator::operator*()
{
  return *(_node->_delta);
}

std::shared_ptr<Delta> DeltaTree::ReverseIterator::operator->()
{
  return _node->_delta;
}

bool DeltaTree::ReverseIterator::operator==(const DeltaTree::ReverseIterator &rhs) {
  return (_node == rhs._node);
}

bool DeltaTree::ReverseIterator::operator!=(const DeltaTree::ReverseIterator &rhs) {
  return (_node != rhs._node);
}

/**
 * Traverses the DeltaTree reverse in-order
 */
DeltaTree::ReverseIterator& DeltaTree::ReverseIterator::operator++()
{
  if (_node == nullptr) {
    return *this;
  }
  _node->_visited = true; // mark current node as visited
  // leftNode == NULL => leaf. Traverse upwards
  if (_node->_leftNode == nullptr) {
    while(    _node->_parent != nullptr
           && _node->_visited == true ) {
      _node = _node->_parent;
    }
#if 0 // old (confirmed not working) solution without _visited
    if (   _node->_parent != nullptr
        && _node->_parent->_leftNode == _node) {
      // left tree visited; traverse upwards two times in total
      _node = _node->_parent;
    }

    _node = _node->_parent; // traverse upwards
#endif
    return *this;
  } else { // LeftNode != NULL => Traverse downwards
    _node =  _node->_leftNode;
    // search the most right leaf
    while(_node->_rightNode != nullptr) {
      _node = _node->_rightNode;
    }
    return *this;
  }
}

/////////////////DeltaNode//////////////////////////

DeltaTree::DeltaNode::DeltaNode(const Delta& delta,
                                std::shared_ptr<DeltaTree::DeltaNode> parent)
  : _leftNode(), _rightNode(), _parent(parent),
    _delta(std::make_shared<Delta>(delta)), _balance(0),
    _visited(false)
{
}

////////////////// DeltaTree /////////////////////////////

DeltaTree::DeltaTree()
  : _root(std::make_shared<DeltaNode>(Delta(SourceRange(),
                                            std::string(),
                                            Delta::INSERT),
                                      std::shared_ptr<DeltaNode>())),
    _totalDelta(0), _level(0)
{
}

const std::shared_ptr<Delta> DeltaTree::searchDelta(Delta delta)
{
  return searchDeltaRec(*_root,delta);
}

std::shared_ptr<Delta> DeltaTree::searchDeltaRec(DeltaNode& current,
                                                 Delta& delta)
{
#if 0
  if(&current == NULL) {
    //Delta is not part of the tree
    return NULL;
  }
#endif
  auto comp = ::DeltaComparator();
  int comparison = comp(delta,*(current._delta));
  if(comparison == -1) {
    //Search in the left subtree
    return searchDeltaRec(*(current._leftNode),delta);
  } else if(comparison == 1) {
    //Search in the right subtree
    return searchDeltaRec(*(current._rightNode),delta);
  } else {
    //Found delta
    return current._delta;
  }
}

void DeltaTree::insertDelta(Delta delta)
{
  if(delta.getKind() == Delta::REPLACE) {
    _totalDelta += delta.getNewToken().length()-delta.getRange().length();
  } else { // some kind of insert
    _totalDelta += delta.getNewToken().length();
  }

  insertDeltaRec(_root, delta);
}

void DeltaTree::insertDeltaRecLeft(std::shared_ptr<DeltaNode> current,
                                   const Delta& delta)
{
  if(current->_leftNode == nullptr) {
    // Insert new node
    current->_leftNode = std::make_shared<DeltaNode>(delta, current);
    if(current->_balance == 1) {
      // The left subtree was higher then the right.
      // Now it's balanced
      current->_balance = 0;
    } else {
      // Case 3: balance = 0;
      // After an insertion one subtree increases its height by one.
      // The tree is unbalanced now and we have to restructure the tree.
      current->_balance = -1;
      restructureTree(current);
    }
  } else {
    // Go deeper
    insertDeltaRec(current->_leftNode,delta);
  }
}

void DeltaTree::insertDeltaRecRight(std::shared_ptr<DeltaNode> current,
                                    const Delta& delta)
{
  if(current->_rightNode == nullptr) {
    // insert new node
    current->_rightNode = std::make_shared<DeltaNode>(delta, current);
    if(current->_balance == -1) {
      // The left subtree was higher then the right.
      // Now it's balanced
      current->_balance = 0;
    } else {
      // Case 3: balance = 0;
      // After an insertion one subtree increases its height by one.
      // The tree is unbalanced now and we have to restructure the tree
      current->_balance = 1;
      restructureTree(current);
    }
  } else {
    //Go deeper
    insertDeltaRec(current->_rightNode,delta);
  }
}

void DeltaTree::insertDeltaRec(std::shared_ptr<DeltaNode> current,
                               const Delta& delta)
{
  auto comp = ::DeltaComparator();
  int comparison = comp(delta,*(current->_delta));
  if(comparison == -1) {
    insertDeltaRecLeft(current, delta);
  } else if(comparison == 1) {
    insertDeltaRecRight(current, delta);
  } else {
    // current delta is equal to delta to be inserted
    switch (delta.getKind()) {
    case Delta::REPLACE:
      // multiple replacements on the same location are ignored
      break;
    case Delta::INSERT:
      auto tmpDeltaPtr = current->_delta;
      while( tmpDeltaPtr->getNextDelta() != nullptr ) {
        tmpDeltaPtr = tmpDeltaPtr->getNextDelta();
      }
      // append delta at end of delta line
      tmpDeltaPtr->setNextDelta(std::make_shared<Delta>(delta));
      break;
    }
  }
}

void DeltaTree::restructureTree(std::shared_ptr<DeltaNode> p)
{
  if(p->_parent == nullptr) {
    //We reached the parent of the root (which is nullptr)
    return;
  }
  //parent(p)'s left subtree was higher then the right one, and the right subtree, of which p is the root increased its height by one
  if(   p == p->_parent->_rightNode
     && p->_parent->_balance == -1) {
    p->_parent->_balance = 0;
    return;
  }
  //The same as above
  if(   p == p->_parent->_leftNode
     && p->_parent->_balance == 1) {
    p->_parent->_balance = 0;
    return;
  }
  if(p->_parent->_balance == 0) {
    if(p == p->_parent->_rightNode) {
      //Right subtree growed
      p->_parent->_balance = 1;
    } else {
      //Left subtree growed
      p->_parent->_balance = -1;
    }
    return restructureTree(p->_parent);
  }
  if(p->_parent->_balance == -1) {
    //parent(p)'s left subtree was already higher then the right one. The rules of an AVL tree are violated here.
    //Depending on the balance of p, we have to do a right or double rotation( left-right-rotation)
    if(p->_balance == -1) {
      p->_balance = 0;
      p->_parent->_balance = 0;
      rightRotation(p);
      if(p->_parent == nullptr) {
        _root = p;
      }
    } else {
      auto tmpDeltaNodePtr = p->_rightNode;

      leftRotation(tmpDeltaNodePtr);
      rightRotation(tmpDeltaNodePtr);

      if(tmpDeltaNodePtr->_parent == nullptr) {
        _root = tmpDeltaNodePtr;
      }
    }
  } else {
    //The same as above, just vice versa
    if(p->_balance == 1) {
      p->_balance = 0;
      p->_parent->_balance = 0;
      leftRotation(p);
      if(p->_parent == nullptr) {
        _root = p;
      }
    } else {
      auto tmpDeltaNodePtr = p->_leftNode;
      auto bal = tmpDeltaNodePtr->_balance;

      tmpDeltaNodePtr->_balance = 0;
      tmpDeltaNodePtr->_parent->_balance =  bal;
      tmpDeltaNodePtr->_parent->_parent->_balance = bal;
      rightRotation(tmpDeltaNodePtr);
      leftRotation(tmpDeltaNodePtr);

      if(tmpDeltaNodePtr->_parent == nullptr) {
        _root = tmpDeltaNodePtr;
      }
    }
  }
}

void DeltaTree::rightRotation(std::shared_ptr<DeltaNode> start)
{
  //RigthNode of start becomes the leftNode of start->parent and (1)
  //RightNode of start becomes start->parent and                 (2)
  //parent    of start becomes start->parent->parent
  //parent    of start->parent-Parent becomes start

  //Accomplish (1) and (2)

  auto tmpDeltaNodePtr = start->_rightNode;
  start->_rightNode = start->_parent;
  start->_parent->_leftNode = tmpDeltaNodePtr;

  if(tmpDeltaNodePtr != nullptr) {
    tmpDeltaNodePtr->_parent =  start->_parent;
  }

  //Swap parents
  tmpDeltaNodePtr = start->_parent->_parent;
  start->_parent->_parent = start;
  start->_parent = tmpDeltaNodePtr;

  if(start->_parent == nullptr) {
    return;
  }

  if(start->_parent->_leftNode == start->_rightNode) {
    start->_parent->_leftNode = start;
  } else {
    start->_parent->_rightNode = start;
  }
}

void DeltaTree::leftRotation(std::shared_ptr<DeltaNode> start)
{
  auto tmpDeltaNodePtr = start->_leftNode;
  start->_leftNode = start->_parent;
  start->_parent->_rightNode = tmpDeltaNodePtr;

  if(tmpDeltaNodePtr != nullptr)
    tmpDeltaNodePtr->_parent =  start->_parent;

  //Swap parents
  tmpDeltaNodePtr = start->_parent->_parent;
  start->_parent->_parent = start;
  start->_parent = tmpDeltaNodePtr;

  if(start->_parent == nullptr) {
    return;
  }

  if(start->_parent->_leftNode == start->_leftNode) {
    start->_parent->_leftNode = start;
  } else {
    start->_parent->_rightNode = start;
  }
}

int DeltaTree::getTotalDelta()
{
  return _totalDelta;
}

DeltaTree::Iterator DeltaTree::begin()
{
  recClearVisitedFlags(_root);
  auto tmpDeltaNodePtr = _root;
  // search leftmost node
  while(   tmpDeltaNodePtr != nullptr
        && tmpDeltaNodePtr->_leftNode != nullptr) {
    tmpDeltaNodePtr = tmpDeltaNodePtr->_leftNode;
  }
  // and return parent (since left most node is only a dummy)
  return Iterator(tmpDeltaNodePtr->_parent);
}

DeltaTree::Iterator DeltaTree::end()
{
  return Iterator(std::shared_ptr<DeltaNode>());
}

DeltaTree::ReverseIterator DeltaTree::rbegin()
{
  recClearVisitedFlags(_root);
  auto tmpDeltaNodePtr = _root;
  // search rightmost node
  while(   tmpDeltaNodePtr != nullptr
        && tmpDeltaNodePtr->_rightNode != nullptr) {
    tmpDeltaNodePtr = tmpDeltaNodePtr->_rightNode;
  }
  return ReverseIterator(tmpDeltaNodePtr);
}

DeltaTree::ReverseIterator DeltaTree::rend()
{
  auto tmpDeltaNodePtr = _root;
  // search leftmost node (a dummy marking the end of the sequence)
  while (   tmpDeltaNodePtr != nullptr
         && tmpDeltaNodePtr->_leftNode != nullptr) {
    tmpDeltaNodePtr = tmpDeltaNodePtr->_leftNode;
  }
  return ReverseIterator(tmpDeltaNodePtr);
}
bool DeltaTree::empty() {
  return (_root->_leftNode == nullptr && _root->_rightNode == nullptr);
}

void DeltaTree::printTree()
{
  recPrintTree(_root);
}

void DeltaTree::recPrintTree(std::shared_ptr<DeltaNode> node)
{
  if(node->_leftNode != nullptr) {
    ++_level;
    recPrintTree(node->_leftNode);
    --_level;
  }
  std::cout << "Level "    << _level << " | "
            << "Location " << node->_delta->getRange().getStart().line
            <<         " " << node->_delta->getRange().getStart().column
            << " | "       << node->_delta->getNewToken()
                           << std::endl;
  if(node->_rightNode != nullptr) {
    ++_level;
    recPrintTree(node->_rightNode);
    --_level;
  }
}

void DeltaTree::recClearVisitedFlags(const std::shared_ptr<DeltaNode>& node)
{
  if (node == nullptr) {
    return;
  }
  recClearVisitedFlags(node->_leftNode);
  node->_visited = false;
  recClearVisitedFlags(node->_rightNode);
}

} // namespace ssedit

