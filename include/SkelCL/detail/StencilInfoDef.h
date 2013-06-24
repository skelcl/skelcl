/*
 * StencilInfoDef.h
 *
 *  Created on: 21.06.2013
 *      Author: stefan
 */

#ifndef STENCILINFODEF_H_
#define STENCILINFODEF_H_

namespace skelcl {

template<typename Tin, typename Tout>
StencilInfo<Tout(Tin)>::StencilInfo(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
                            detail::Padding padding, Tin neutral_element, const std::string& func):
    _userSource(source), _north(north), _west(west), _south(south), _east(east), _padding(padding), _neutral_element(neutral_element),
    	_funcName(func), _program(createAndBuildProgram()){
    LOG_DEBUG("Create new StencilInfo object (", this, ")");
    LOG_DEBUG(getDebugInfo());
}

template<typename Tin, typename Tout>
detail::Program StencilInfo<Tout(Tin)>::createAndBuildProgram() const {
//	ASSERT_MESSAGE(!_userSource.empty(),
//			"Tried to create program with empty user source.");

	std::stringstream temp;

	detail::Device firstDev = *(detail::globalDeviceList.at(0).get());

    //int maxWorkgroupSize = sqrt(firstDev.maxWorkGroupSize());

    //temp << "#define TILE_WIDTH " << maxWorkgroupSize + 2*_overlap_range+1 << std::endl;
	if (_padding == detail::Padding::NEUTRAL) {
		temp << "#define NEUTRAL " << _neutral_element << std::endl;
	}
	//LOG_DEBUG("TileWidth: %i", maxWorkgroupSize);

	// create program
	std::string s(Matrix<Tout>::deviceFunctions());
	s.append(temp.str());

	// helper structs and functions
	s.append(
			R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;

typedef struct {
    const __global SCL_TYPE_0* data;
    unsigned int dimension;
    unsigned int row;
} lmatrix_t;

typedef struct {
    const __global SCL_TYPE_1* data;
    unsigned int width;
    unsigned int column;
} rmatrix_t;

/*
 * DeviceFunctions
 */

//In case, global memory is used (has to be specified by the MapOverlap-class. The user cannot choose.
SCL_TYPE_0 getElem2DGlobal(__global SCL_TYPE_0* vector, int x, int y, int cols) {

	int col = get_global_id(0) % cols;

#ifdef NEUTRAL
   if((col+x)<0){
        return NEUTRAL;
    }
	//Hier ist nur rechts daneben
   else if((col+x)>=cols){
        return NEUTRAL;
    }
    //Standardfall
    return vector[x+y*cols];
#else
    if((col+x)<0){
        return vector[-col+y*cols];
    }
	//Hier ist nur rechts daneben
    else if((col+x)>=cols){
        return vector[cols-col-1+y*cols];
    }
	//Standardfall
    return vector[x+y*cols];
#endif

}

//In case, local memory is used
//SCL_TYPE_0 getElem2D(__local SCL_TYPE_0* vector, int x, int y){
//    return vector[x+TILE_WIDTH*y];
//}

)");

	// user source
	s.append(_userSource);
	// allpairs skeleton source
	s.append(
#include "StencilKernel.cl"
	);

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
    s << "north: " << _north << ", west: " << _west << ", south: " << _south << ", east: " << _east;
    return s.str();
}

}


#endif /* STENCILINFODEF_H_ */
