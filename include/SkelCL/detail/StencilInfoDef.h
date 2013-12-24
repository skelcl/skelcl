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
		_userSource(source), _north(north), _west(west), _south(south), _east(
				east), _padding(padding), _neutral_element(neutral_element), _funcName(
                func), _program(createAndBuildProgram()) {
    LOG_DEBUG_INFO("Create new StencilInfo object (", this, ")");
}

template<typename Tin, typename Tout>
detail::Program StencilInfo<Tout(Tin)>::createAndBuildProgram() const {
    ASSERT_MESSAGE(!_userSource.empty(), "Tried to create program with empty user source.");

	std::stringstream temp;

    //Determine the padding mode
    switch(_padding) {
    case detail::Padding::NEUTRAL: {
        temp << "#define NEUTRAL " << _neutral_element << std::endl;
    }
    }

	// create program
	std::string s(Matrix<Tout>::deviceFunctions());
	s.append(temp.str());
	
    if(_north == 0 && _south == 0 && _west == 0 && _east == 0) {
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

	// helper structs and functions
	s.append(
R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;

typedef struct {
    __local SCL_TYPE_1* data;
    int local_row;
    int local_column;
    int offset_north;
    int offset_west;
    int tile_width;
} input_matrix_t;

//In case, local memory is used
SCL_TYPE_1 getData(input_matrix_t* matrix, int x, int y){
    int offsetNorth = matrix->offset_north * matrix->tile_width;
    int currentIndex = matrix->local_row * matrix->tile_width + matrix->local_column;
    int shift = x - y * matrix->tile_width;

    return matrix->data[currentIndex+offsetNorth+shift+matrix->offset_west];
}

)");
    }
	// user source
	s.append(_userSource);
    if(_north == 0 && _south == 0 && _west == 0 && _east == 0) {
s.append(R"(

__kernel void SCL_STENCIL(
            __global SCL_TYPE_0* SCL_IN, __global SCL_TYPE_1* SCL_OUT, __global SCL_TYPE_1* SCL_TMP, __local SCL_TYPE_1* SCL_LOCAL_TMP, const unsigned int SCL_TILE_WIDTH,
                        const unsigned int SCL_TILE_HEIGHT, const unsigned int SCL_ELEMENTS, const unsigned int SCL_NORTH, const unsigned int SCL_WEST, const unsigned int SCL_SOUTH,
                        const unsigned int SCL_EAST, const unsigned int SCL_COLS)
{
if (get_global_id(1)*SCL_COLS+get_global_id(0) < SCL_ELEMENTS) {
    input_matrix_t Mm;
    Mm.data = SCL_TMP+get_global_id(1)*SCL_COLS+get_global_id(0);
    SCL_OUT[get_global_id(1)*SCL_COLS+get_global_id(0)] = USR_FUNC(&Mm);
}
}
)");
    } else {
        if(_padding==detail::Padding::NEAREST)
            s.append(
        #include "StencilKernelNearest.cl"
            );

        else if(_padding==detail::Padding::NEUTRAL){
            s.append(
                #include "StencilKernelNeutral.cl"
            );
        }

        else if(_padding==detail::Padding::NEAREST_INITIAL){
            s.append(
                #include "StencilKernelNearestInitial.cl"
            );
        }
    }
	auto program = detail::Program(s,
			detail::util::hash(
					"//Stencil\n" + Matrix<Tout>::deviceFunctions()
							+ _userSource + _funcName));
	// modify program
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
    for (auto iter  = detail::globalDeviceList.begin();
              iter != detail::globalDeviceList.end();
            ++iter) {
        if(iter->get()->maxWorkGroupSize()<maxWorkgroupSize) {
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

}

#endif /* STENCILINFODEF_H_ */

