///
/// \file StencilInfoDef.h
///
///
///     \author Stefan Breuer <s_breu03@uni-muenster.de>
///

#ifndef STENCILINFODEF_H_
#define STENCILINFODEF_H_

namespace skelcl {

template<typename Tin, typename Tout>
StencilInfo<Tout(Tin)>::StencilInfo(const Source& source, unsigned int north,
                                    unsigned int west, unsigned int south, unsigned int east,
                                    detail::Padding padding, Tin neutral_element, const std::string& func) :
  _userSource(source), _north(north), _west(west), _south(south), _east(east),
      _padding(padding), _neutral_element(neutral_element), _funcName(
          func), _program(createAndBuildProgram()) {
    LOG_DEBUG_INFO("Create new StencilInfo object (", this, ")");
}

template<typename Tin, typename Tout>
detail::Program StencilInfo<Tout(Tin)>::createAndBuildProgram() const {
  ASSERT_MESSAGE(!_userSource.empty(),
                 "Tried to create program with empty user source.");

  std::stringstream temp;

  //Determine the padding mode
  switch (_padding) {
    case detail::Padding::NEUTRAL: {
      temp << "#define NEUTRAL " << _neutral_element << "\n";
    }
      break;

    case detail::Padding::NEAREST:
    case detail::Padding::NEAREST_INITIAL:
      break;
  }

  temp << "#define SCL_NORTH (" << _north << ")\n"
       << "#define SCL_WEST  (" << _west  << ")\n"
       << "#define SCL_SOUTH (" << _south << ")\n"
       << "#define SCL_EAST  (" << _east  << ")\n"
       << "#define SCL_TILE_WIDTH  (get_local_size(0) + SCL_WEST + SCL_EAST)\n"
       << "#define SCL_TILE_HEIGHT (get_local_size(1) + SCL_NORTH + SCL_SOUTH)\n"
       << "#define SCL_COL   (get_global_id(0))\n"
       << "#define SCL_ROW   (get_global_id(1))\n"
       << "#define SCL_L_COL (get_local_id(0))\n"
       << "#define SCL_L_ROW (get_local_id(1))\n"
       << "#define SCL_L_COL_COUNT (get_local_size(0))\n"
       << "#define SCL_L_ROW_COUNT (get_local_size(1))\n"
       << "#define SCL_L_ID (SCL_L_ROW * SCL_L_COL_COUNT + SCL_L_COL)\n"
       << "#define SCL_ROWS (SCL_ELEMENTS / SCL_COLS)\n";

  // create program
  std::string s(detail::CommonDefinitions::getSource());
  s.append(Matrix<Tout>::deviceFunctions());
  s.append(temp.str());

  // If all of the boundaries are 0, then this is just a map
  // operation.
  auto ismap = _north == 0 && _south == 0 && _west == 0 && _east == 0;

  // Helper structs and functions.
  if (ismap) {
    s.append(
        R"(

    typedef float SCL_TYPE_0;
    typedef float SCL_TYPE_1;

    typedef struct {
        __global SCL_TYPE_1* data;
    } input_matrix_t;

    //In case, local memory is used
    SCL_TYPE_1 getData(input_matrix_t* matrix, int x, int y){
        return matrix->data[0];
    }

)");
  } else {
    s.append(
        R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;

typedef struct {
    __local SCL_TYPE_1* data;
} input_matrix_t;

//In case, local memory is used
SCL_TYPE_1 getData(input_matrix_t* matrix, int x, int y){
    int offsetNorth = SCL_NORTH * SCL_TILE_WIDTH;
    int currentIndex = SCL_L_ROW * SCL_TILE_WIDTH + SCL_L_COL;
    int shift = x - y * SCL_TILE_WIDTH;

    return matrix->data[currentIndex+offsetNorth+shift+SCL_WEST];
}

)");
  }

  // Add the user program.
  s.append(_userSource);

  // Append the appropiate kernel, based on the padding type, or if
  // it's a map.
  if (ismap) {
    s.append(
        R"(

__kernel void SCL_STENCIL(__global SCL_TYPE_0* SCL_IN,
                          __global SCL_TYPE_1* SCL_OUT,
                          __global SCL_TYPE_1* SCL_TMP,
                          __local SCL_TYPE_1* SCL_LOCAL_TMP,
                          const unsigned int SCL_ELEMENTS,
                          const unsigned int SCL_COLS)
{
  if (get_global_id(1)*SCL_COLS+get_global_id(0) < SCL_ELEMENTS) {
    input_matrix_t Mm;
    Mm.data = SCL_TMP+get_global_id(1)*SCL_COLS+get_global_id(0);
    SCL_OUT[get_global_id(1)*SCL_COLS+get_global_id(0)] = USR_FUNC(&Mm);
  }
}
)");

  } else {
    // Set the correct padding type.
    if (_padding == detail::Padding::NEUTRAL) {
      s.append(R"(
#define STENCIL_PADDING_NEUTRAL         1
#define STENCIL_PADDING_NEAREST         0
#define STENCIL_PADDING_NEAREST_INITIAL 0
)");
    } else if (_padding == detail::Padding::NEAREST) {
      s.append(R"(
#define STENCIL_PADDING_NEUTRAL         0
#define STENCIL_PADDING_NEAREST         1
#define STENCIL_PADDING_NEAREST_INITIAL 0
)");
    } else if (_padding == detail::Padding::NEAREST_INITIAL) {
      s.append(R"(
#define STENCIL_PADDING_NEUTRAL         0
#define STENCIL_PADDING_NEAREST         0
#define STENCIL_PADDING_NEAREST_INITIAL 1
)");
    }

    // Include the kernel definition.
    s.append(
#include "StencilKernel.cl"
);
  }

  // Build the program.
  auto program = detail::Program(s,
                                 detail::util::hash(
                                     "//Stencil\n" + Matrix<Tout>::deviceFunctions()
                                     + _userSource + _funcName));
  // Set the parameters and function names.
  if (!program.loadBinary()) {
    program.transferParameters(_funcName, 1, "SCL_STENCIL");
    program.transferArguments(_funcName, 1, "USR_FUNC");
    program.renameFunction(_funcName, "USR_FUNC");
    program.adjustTypes<Tin, Tout>();
  }

  program.build();
  return program;
}

template<typename Tin, typename Tout>
unsigned int StencilInfo<Tout(Tin)>::determineMaxWorkGroupSize() const {
  unsigned int maxWorkgroupSize = INT_MAX;
  for (auto iter = detail::globalDeviceList.begin();
       iter != detail::globalDeviceList.end(); ++iter) {
    if (iter->get()->maxWorkGroupSize() < maxWorkgroupSize) {
      maxWorkgroupSize = iter->get()->maxWorkGroupSize();
    }
  }
  return maxWorkgroupSize;
}

template<typename Tin, typename Tout>
unsigned int StencilInfo<Tout(Tin)>::determineTileWidth() const {
  return sqrt(determineMaxWorkGroupSize()) + _west + _east;
}

template<typename Tin, typename Tout>
unsigned int StencilInfo<Tout(Tin)>::determineTileHeight() const {
  return sqrt(determineMaxWorkGroupSize()) + _south + _north;
}

template<typename Tin, typename Tout>
    const unsigned int& StencilInfo<Tout(Tin)>::getNorth() const {
  return this->_north;
}

template<typename Tin, typename Tout>
const unsigned int& StencilInfo<Tout(Tin)>::getWest() const {
  return this->_west;
}

template<typename Tin, typename Tout>
const unsigned int& StencilInfo<Tout(Tin)>::getSouth() const {
  return this->_south;
}

template<typename Tin, typename Tout>
const unsigned int& StencilInfo<Tout(Tin)>::getEast() const {
  return this->_east;
}

template<typename Tin, typename Tout>
const detail::Padding& StencilInfo<Tout(Tin)>::getPadding() const {
  return this->_padding;
}

template<typename Tin, typename Tout>
const Tin& StencilInfo<Tout(Tin)>::getNeutralElement() const {
  return this->_neutral_element;
}

template<typename Tin, typename Tout>
const detail::Program& StencilInfo<Tout(Tin)>::getProgram() const {
  return this->_program;
}

template<typename Tin, typename Tout>
std::string StencilInfo<Tout(Tin)>::getDebugInfo() const {
  std::stringstream s;
  s << "north: " << _north << ", west: " << _west << ", south: " << _south
    << ", east: " << _east;
  return s.str();
}

}  // namespace

#endif  /* STENCILINFODEF_H_ */
