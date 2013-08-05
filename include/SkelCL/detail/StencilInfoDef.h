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
				func), _tile_width(determineTileWidth()), _tile_height(
				determineTileHeight()), _program(createAndBuildProgram()) {
	LOG_DEBUG("Create new StencilInfo object (", this, ")");
	LOG_DEBUG(getDebugInfo());
}

template<typename Tin, typename Tout>
detail::Program StencilInfo<Tout(Tin)>::createAndBuildProgram() const {
//	ASSERT_MESSAGE(!_userSource.empty(),
//			"Tried to create program with empty user source.");

	std::stringstream temp;
	temp << "#define TILE_WIDTH " << _tile_width << std::endl;
	temp << "#define TILE_HEIGHT " << _tile_height << std::endl;
	if (_padding == detail::Padding::NEUTRAL) {
		temp << "#define NEUTRAL " << _neutral_element << std::endl;
	} else if (_padding == detail::Padding::NEAREST) {
		temp << "#define NEAREST " << 1 << std::endl;
	} else if (_padding == detail::Padding::NEAREST_INITIAL) {
		temp << "#define NEAREST_INITIAL " << 1 << std::endl;
	}
	// create program
	std::string s(Matrix<Tout>::deviceFunctions());
	s.append(temp.str());

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
} input_matrix_t;

//In case, local memory is used
SCL_TYPE_1 getData(input_matrix_t* matrix, int x, int y){
    int offsetNorth = matrix->offset_north * TILE_WIDTH;
    int currentIndex = matrix->local_row * TILE_WIDTH + matrix->local_column;
    int shift = x - y * TILE_WIDTH;

    return matrix->data[currentIndex+offsetNorth+shift+matrix->offset_west];
}

)");

	// user source
	s.append(_userSource);
	// allpairs skeleton source
	s.append(
#include "StencilKernel.cl"
	);
	LOG_DEBUG("D");
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
unsigned int StencilInfo<Tout(Tin)>::determineTileWidth() const {
	detail::Device firstDev = *(detail::globalDeviceList.at(0).get());
	int maxWorkgroupSize = sqrt(firstDev.maxWorkGroupSize());
	return maxWorkgroupSize + _west + _east;
}

template<typename Tin, typename Tout>
unsigned int StencilInfo<Tout(Tin)>::determineTileHeight() const {
	detail::Device firstDev = *(detail::globalDeviceList.at(0).get());
	int maxWorkgroupSize = sqrt(firstDev.maxWorkGroupSize());
	return maxWorkgroupSize + _south + _north;
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
	LOG_DEBUG("sinfo", this);
	return this->_program;
}

template<typename Tin, typename Tout>
const unsigned int& StencilInfo<Tout(Tin)>::getTileWidth() const {
	return this->_tile_width;
}

template<typename Tin, typename Tout>
const unsigned int& StencilInfo<Tout(Tin)>::getTileHeight() const {
	return this->_tile_height;
}

template<typename Tin, typename Tout>
std::string StencilInfo<Tout(Tin)>::getDebugInfo() const {
	std::stringstream s;
	s << "north: " << _north << ", west: " << _west << ", south: " << _south
			<< ", east: " << _east << ", tile width: " << _tile_width
			<< ", tile heigth: " << _tile_height;
	return s.str();
}

}

#endif /* STENCILINFODEF_H_ */
