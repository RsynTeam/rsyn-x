/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PhysicalGCell.h
 * Author: jucemar
 *
 * Created on January 22, 2018, 9:23 PM
 */

namespace Rsyn {
//! @brief Returns GCell direction

inline Rsyn::PhysicalGCellDirection PhysicalGCell::getDirection() const {
	return data->clsDirection;
} // end method 

// -----------------------------------------------------------------------------

//! @brief If direction is vertical, then numTracks means the number of columns
//! If direction is Horizontal, then numTracks means the number of rows

inline int PhysicalGCell::getNumTracks() const {
	return data->clsNumTracks;
} // end method 

// -----------------------------------------------------------------------------
//! @brief If direction is Horizontal, then location is Y value.
//! If direction is vertical, then location is X value.

inline DBU PhysicalGCell::getLocation() const {
	return data->clsLocation;
} // end method 

// -----------------------------------------------------------------------------

//! @brief Returns the spacing between GCell tracks. 

inline DBU PhysicalGCell::getStep() const {
	return data->clsStep;
} // end method 

// -----------------------------------------------------------------------------

} // end namespace 
