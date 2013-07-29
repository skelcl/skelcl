/*
 * StencilInfo.h
 *
 *  Created on: 21.06.2013
 *      Author: stefan
 */

#ifndef STENCILINFO_H_
#define STENCILINFO_H_

namespace skelcl {

template<typename > class StencilInfo;

template<typename Tin,
         typename Tout>
class StencilInfo<Tout(Tin)> {
public:

   StencilInfo(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
                                detail::Padding padding, Tin neutral_element, const std::string& func);

   const unsigned int& getNorth() const;
   const unsigned int& getWest() const;
   const unsigned int& getSouth() const;
   const unsigned int& getEast() const;

   const detail::Padding& getPadding() const;

   const Tin& getNeutralElement() const;

   const detail::Program& getProgram() const;

   const unsigned int& getTileWidth() const;
   const unsigned int& getTileHeight() const;

      std::string getDebugInfo() const;
private:


   // Programm erstellen
   detail::Program createAndBuildProgram() const;

   //Größe des Local memory bestimmen
   unsigned int determineTileWidth() const;
   unsigned int determineTileHeight() const;

    /// Source code as defined by the application developer
    std::string _userSource;

    unsigned int _north;
    unsigned int _west;
    unsigned int _south;
    unsigned int _east;

    /// Determines the padding method.
    detail::Padding _padding;

    /// The neutral element, which shall be used to fill the overlap region (only used if detail::Padding == NEUTRAL)
    Tin _neutral_element;

    /// Name of the main function defined in _userSource
    std::string _funcName;

    unsigned int _tile_width;
    unsigned int _tile_height;

    detail::Program _program;

};

} // skelcl

#include "detail/StencilInfoDef.h"

#endif /* STENCILINFO_H_ */
