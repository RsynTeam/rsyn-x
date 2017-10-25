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

	//! @brief Returns routed wire width segment. It is valid only for special nets.
	//! @warning Regular wire segments must get wire width from layer. 
	DBU getRoutedWidth() const;
	
	//! @brief Returns if wire segment is define by a sequence of points.
	bool hasPoints() const;
	//! @brief Returns rectangular boundary of wire
	const std::vector<DBUxy> & allSegmentPoints() const;
	//! @brief Returns point segment at given index (clsPoints[index];)
	DBUxy getPoint(const std::size_t index) const;
	//! @brief Returns number of points in segment (clsPoints.size())
	const std::size_t getNumPoints() const;
	//! @brief Returns wire extension of wire at a via.
	DBU getExtension() const;
	//! @brief Returns physical layer object related to wire
	Rsyn::PhysicalLayer getLayer() const;
	//! @brief Returns physical via object related to wire
	Rsyn::PhysicalVia getVia() const;
	//! @brief returns if wire segment has a via.
	bool hasVia() const;
	
	/*! @details
	 "RECT ( deltax1 deltay1 deltax2 deltay2 )
	 Indicates that a rectangle is created from the previous ( x y ) 
	 routing point using the delta values. The RECT values leave the 
	 current point and layer unchanged." Source: LEf/DEf Reference Manual 5.8
	 */
	const Bounds & getRectangle() const;

	//! @brief Returns if wire segment had defined a rectangle
	const bool hasRectangle() const;
	
	//! @brief Returns number of points in segment (clsPoints.size())
	const std::size_t getNumRoutingPoints() const;
	const std::vector<PhysicalRoutingPoint> & allRoutingPoints() const;
	
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALWIRESEGMENT_H */

