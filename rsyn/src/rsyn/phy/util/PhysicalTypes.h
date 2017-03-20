/* Copyright 2014-2017 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/* 
 * File:   PhysicalTypes.h
 * Author: jucemar
 *
 * Created on 29 de Outubro de 2015, 13:29
 */

#ifndef PHYSICALDESIGN_PHYSICALTYPES_H
#define	PHYSICALDESIGN_PHYSICALTYPES_H

namespace Rsyn {

enum DBUType : std::int8_t {
	INVALID_DBU = -1,
	LIBRARY_DBU = 0, // DBU from Lef Library
	DESIGN_DBU = 1, // DBU from def Design
	MULT_FACTOR_DBU = 2, // LEF_DBU = DEF_DBU * DEF_MULT, e.g. 2000 = 1000 * 2
	NUM_DBU = 3
}; // end enum 

// -----------------------------------------------------------------------------

enum PhysicalType : std::int8_t {
	INVALID_PHYSICAL_TYPE = -1,
	PHYSICAL_FIXED = 0,
	PHYSICAL_MOVABLE = 1,
	PHYSICAL_BLOCK = 2,
	PHYSICAL_PORT = 3,
	PHYSICAL_FIXEDBOUNDS = 4,
	PHYSICAL_PLACEABLE = 5,
	NUM_PHYSICAL_TYPES = 6
}; // end enum 

// -----------------------------------------------------------------------------

enum PhysicalLayerType : std::int8_t {
	INVALID_PHY_LAYER_TYPE = -1,
	ROUTING = 0,
	CUT = 1,
	OVERLAP = 2,
	MASTERSLICE = 3, // They are typically polysilicon 
	IMPLANT = 4,
	NUM_PHY_LAYER = 5
};

enum PhysicalLayerDirection : std::int8_t {
	INVALID_PHY_LAYER_DIRECTION = -1,
	HORIZONTAL = 0, // horizontal
	VERTICAL = 1, /// vertical 
	NUM_PHY_LAYER_DIRECTION = 2	
};

// -----------------------------------------------------------------------------

enum PhysicalSiteClass : std::int8_t {
	INVALID_SITECLASS = -1,
	PAD = 0,
	CORE = 1 ,
	NUM_PHY_SITECLASS = 2
};

// -----------------------------------------------------------------------------

enum PhysicalPinDirection : std::int8_t {
	PIN_INVALID_DIRECTION = -1,
	PIN_INPUT = 0,
	PIN_OUTPUT = 1, 
	NUM_PHY_PIN_DIRECTION = 2
};

// -----------------------------------------------------------------------------

enum PhysicalMacroClass : std::int8_t {
	MACRO_INVALID_CLASS = -1,
	MACRO_COVER = 0,
	MACRO_RING = 1,
	MACRO_BLOCK = 2,
	MACRO_PAD = 3,
	MACRO_CORE = 4,
	MACRO_ENDCAP = 5,
	NUM_PHY_MACRO_CLASS = 6
};

// -----------------------------------------------------------------------------

enum PhysicalMacroBlockClass : std::int8_t {
	MACRO_INVALID_BLOCK_CLASS = -1,
	MACRO_BLOCK_BLACKBOX = 0,
	MACRO_BLOCK_SOFT = 1,
	NUM_MACRO_BLOCK_CLASS = 2
};

// -----------------------------------------------------------------------------

// [NOTE] The default orientation is "vertically and face up" - N (North). 
//        Rotate by 90deg clockwise to get E, S and W, flip to get FN, FE, 
//        FS and FW. (think of a dial).		
//        Source: http://vlsicad.ucsd.edu/GSRC/bookshelf/Slots/Placement/plFormats.html

enum PhysicalOrientation : std::int8_t {
	ORIENTATION_INVALID = -1,

	ORIENTATION_N = 0,
	ORIENTATION_E = 1,
	ORIENTATION_S = 2,
	ORIENTATION_W = 3,

	ORIENTATION_FN = 4,
	ORIENTATION_FE = 5,
	ORIENTATION_FS = 6,
	ORIENTATION_FW = 7,
	NUM_PHY_ORIENTATION = 8
}; // end enum

// -----------------------------------------------------------------------------

// [NOTE] X and Y - allows flips around axis, ROT90 - allows any of 
//        rotations by 0, 90, 180 or 270 degrees.
//        Source: http://vlsicad.ucsd.edu/GSRC/bookshelf/Slots/Fundamental/HGraph/

// [TODO] ROT90 ???

enum PhysicalSymmetry : std::int8_t {
	SYMMETRY_INVALID = -1,
	SYMMETRY_X = 0,
	SYMMETRY_Y = 1,
	NUM_PHY_SYMMETRY = 2
}; // end enum

// -----------------------------------------------------------------------------

enum PhysicalPinPortClass : std::int8_t {
	PINPORTCLASS_INVALID = -1,
	PINPORTCLASS_NONE = 0, // default port type
	PINPORTCLASS_CORE = 1,
	PINPORTCLASS_BUMP = 2
}; // end enum 

} // end namespace 

#endif	/* PHYSICALDESIGN_PHYSICALTYPES_H */

