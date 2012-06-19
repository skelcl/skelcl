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
 * DeltaTree.h
 */

#ifndef DELTATREE_H_
#define DELTATREE_H_

#include <memory>
#include <string>

#include <clang-c/Index.h>

#include "SourceLocation.h"
#include "Delta.h"

namespace ssedit {

class DeltaTree {
  class DeltaNode;
public:
  class Iterator {
  public:
    Iterator(std::shared_ptr<DeltaNode> node);

    Delta& operator*();

    std::shared_ptr<Delta> operator->();

    bool operator==(const DeltaTree::Iterator &rhs);

    bool operator!=(const DeltaTree::Iterator &rhs);

    Iterator& operator++();

  private:
    std::shared_ptr<DeltaNode> _node;
  };

  class ReverseIterator {
  public:
    ReverseIterator(std::shared_ptr<DeltaNode> node);

    Delta& operator*();

    std::shared_ptr<Delta> operator->();

    bool operator==(const DeltaTree::ReverseIterator &rhs);

    bool operator!=(const DeltaTree::ReverseIterator &rhs);

    ReverseIterator& operator++();

  private:
    std::shared_ptr<DeltaNode> _node;
  };

  friend class Iterator;
  friend class ReverseIterator;

  DeltaTree();

  DeltaTree(const DeltaTree&) = default;

  ~DeltaTree() = default;

  DeltaTree& operator=(const DeltaTree&) = default;

  const std::shared_ptr<Delta> searchDelta(Delta delta);

  void insertDelta(Delta delta);

  void removeDelta(Delta delta);

  int getTotalDelta();

  Iterator begin();

  Iterator end();

  ReverseIterator rbegin();

  ReverseIterator rend();

  bool empty();

  void printTree();

private:
  class DeltaNode {
  public:
    DeltaNode() = default;

    DeltaNode(const Delta& delta, std::shared_ptr<DeltaNode> parent);

    ~DeltaNode() = default;

    std::shared_ptr<DeltaNode>  _leftNode;
    std::shared_ptr<DeltaNode>  _rightNode;
    std::shared_ptr<DeltaNode>  _parent;
    std::shared_ptr<Delta>      _delta;
    short                       _balance;
    bool                        _visited;
  };

  void recClearVisitedFlags(const std::shared_ptr<DeltaNode>& node);

  void recPrintTree(std::shared_ptr<DeltaNode> node);

  void rightRotation(std::shared_ptr<DeltaNode> start);

  void leftRotation(std::shared_ptr<DeltaNode> start);

  void restructureTree(std::shared_ptr<DeltaNode> parent);

  std::shared_ptr<Delta> searchDeltaRec(DeltaNode& current,
                                        Delta& delta);

  void insertDeltaRecLeft(std::shared_ptr<DeltaNode> current,
                          const Delta& delta);

  void insertDeltaRecRight(std::shared_ptr<DeltaNode> current,
                           const Delta& delta);

  void insertDeltaRec(std::shared_ptr<DeltaNode> current,
                      const Delta& delta);

  std::shared_ptr<DeltaNode>  _root;
  int                         _totalDelta;
  unsigned int                _level;
};

} // namespace ssedit

#endif /* DELTATREE_H_ */

