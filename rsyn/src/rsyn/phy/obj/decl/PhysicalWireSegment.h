/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PhysicalWireSegment.h
 * Author: jucemar
 *
 * Created on 13 de Maio de 2017, 15:59
 */

#ifndef PHYSICALDESIGN_PHYSICALWIRESEGMENT_H
#define PHYSICALDESIGN_PHYSICALWIRESEGMENT_H

namespace Rsyn {

//! @brief Physical wire path class stores wire segments of routed nets. 
class PhysicalWireSegment : public Proxy<PhysicalWireSegmentData> {
	friend class PhysicalDesign;
protected:
	//! @brief Constructs a Rsyn::PhysicalWireSegment object with a pointer to Rsyn::PhysicalWireSegmentData.

	PhysicalWireSegment(PhysicalWireSegmentData * data) : Proxy(data) {
	}
public:
	//! @brief Constructs a Rsyn::PhysicalWireSegment object with a null pointer to Rsyn::PhysicalWireSegmentData.

	PhysicalWireSegment() : Proxy(nullptr) {
	}
	//! @brief Constructs a Rsyn::PhysicalWireSegment object with a null pointer to Rsyn::PhysicalWireSegmentData.

	PhysicalWireSegment(std::nullptr_t) : Proxy(nullptr) {
	}

	//! @brief Returns the start point of this wire segment taking into account
	//! routing point extension.
	DBUxy getSourcePosition() const;

	//! @brief Returns the end point of this wire segment taking into account
	//! routing point extension.
	DBUxy getTargetPosition() const;

	//! @brief Returns routed wire width segment. It is valid only for special nets.
	//! @warning Regular wire segments must get wire width from layer. 
	DBU getRoutedWidth() const;
	
	//! @brief Returns physical layer object related to wire
	Rsyn::PhysicalLayer getLayer() const;

	//! @brief Returns number of points in segment (clsPoints.size())
	const std::size_t getNumRoutingPoints() const;
	const std::vector<PhysicalRoutingPoint> & allRoutingPoints() const;

}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALWIRESEGMENT_H */

