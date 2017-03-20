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
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PhysicalDesign.h
 * Author: jucemar
 *
 * Created on 12 de Setembro de 2016, 20:05
 */

#ifndef PHYSICALDESIGN_PHYSICALDESIGN_H
#define PHYSICALDESIGN_PHYSICALDESIGN_H

#include <list>

namespace Rsyn {

class PhysicalDesign : public Proxy<PhysicalDesignData> {
	friend class PhysicalService;
	
	template<typename _PhysicalObject, typename _PhysicalObjectReference, typename _PhysicalObjectExtension> friend class PhysicalAttributeBase;
	template<typename _PhysicalObject, typename _PhysicalObjectExtension> friend class PhysicalAttributeImplementation;


protected:
	PhysicalDesign(PhysicalDesignData * dsg) : Proxy(dsg) {}
public:
	PhysicalDesign() : Proxy(nullptr) {}
	PhysicalDesign(std::nullptr_t) : Proxy(nullptr) {}
	
	void loadLibrary(const LefDscp & library);
	// UPlace stores the dimensions of design elements using LEF Database Units resolution
	void loadDesign(const DefDscp & design);
	
	void initPhysicalDesign(Rsyn::Design dsg, const Json &params = {});
	
	
	void setClockNet(Rsyn::Net net);
	void updateAllNetBounds(const bool skipClockNet = false);
	void updateNetBound(Rsyn::Net net);

	DBU getDatabaseUnits(const DBUType type) const;
	DBUxy getHPWL() const;
	DBU getHPWL(const Dimension dim) const;
	
	// add to a physicalDie object 
//	DBUxy getDieCoordinate(const Boundary bound) const;
//	DBU getDieCoordinate(const Boundary bound, const Dimension dim) const;
//	const Bounds & getDieBounds() const;
//	DBU getDieLength(const Dimension dim) const;
//	DBUxy getDiePosition(const Boundary boundary = LOWER) const;
//	DBUxy getDieCenterPosition() const;
//	DBU getDieArea() const;
	
	int getNumElements(const PhysicalType type) const;
	// PHYSICAL_FIXED      -> Returning total area of the fixed cells inside of core bounds
	// PHYSICAL_MOVABLE    -> Returning total area of the movable cells inside of core bounds
	// PHYSICAL_BLOCK      -> Returning total area of the block cells inside of the core bounds
	// PHYSICAL_PORT       -> Returning total area of the circuit ports
	// PHYSICAL_PLACEABLE  -> Returning total area of the rows
	DBU getArea(const PhysicalType type ) const; 

	bool isEnablePhysicalPins() const;
	bool isEnableMergeRectangles() const;
	bool isEnableNetPinBoundaries() const;
	
	
	Rsyn::PhysicalLibraryPin getPhysicalLibraryPin(Rsyn::LibraryPin libPin) const;
	Rsyn::PhysicalLibraryPin getPhysicalLibraryPin(Rsyn::Pin pin) const;
	Rsyn::PhysicalLibraryCell getPhysicalLibraryCell(Rsyn::LibraryCell libCell) const;
	Rsyn::PhysicalLibraryCell getPhysicalLibraryCell(Rsyn::Cell cell) const;
	
	void updatePhysicalCell(Rsyn::Cell cell);
	void removePhysicalCell(Rsyn::Cell cell);
	Rsyn::PhysicalCell getPhysicalCell(Rsyn::Cell cell) const;
	Rsyn::PhysicalCell getPhysicalCell(Rsyn::Pin pin) const;
	Rsyn::PhysicalInstance getPhysicalInstance(Rsyn::Instance instance) const;
	Rsyn::PhysicalInstance getPhysicalInstance(Rsyn::Pin pin) const;
	Rsyn::PhysicalPort getPhysicalPort(Rsyn::Port port) const;
	Rsyn::PhysicalPort getPhysicalPort(Rsyn::Pin pin) const;
	Rsyn::PhysicalModule getPhysicalModule(Rsyn::Module module) const;
	Rsyn::PhysicalModule getPhysicalModule(Rsyn::Pin pin) const;
	int getNumMovedCells() const;
	
	Rsyn::PhysicalDie getPhysicalDie()  const;
	
	Rsyn::PhysicalPin getPhysicalPin(Rsyn::Pin pin) const;
	
	DBUxy getPinDisplacement(Rsyn::Pin pin) const; 
	DBUxy getPinPosition(Rsyn::Pin pin) const;
	DBUxy getRelaxedPinPosition(Rsyn::Pin pin) const;
	DBU getPinDisplacement(Rsyn::Pin pin, const Dimension dim) const;
	DBU getPinPosition(Rsyn::Pin pin, const Dimension dim) const;

	Rsyn::PhysicalNet getPhysicalNet(Rsyn::Net net) const ;

	Rsyn::PhysicalLayer getPhysicalLayerByName(const std::string & layerName);
	Rsyn::PhysicalSite getPhysicalSiteByName(std::string siteName);	
	std::size_t getNumLayers() const;
	std::size_t getNumRoutingLayers() const;
	std::size_t getNumOverlapLayers() const;
	std::size_t getNumCutLayers() const;
	const std::vector<PhysicalLayer>  & allPhysicalLayers() const;
	const std::vector<PhysicalLayer> & allRoutingPhysicalLayers() const;
	const std::vector<PhysicalLayer> & allOverlapPhysicalLayers() const;
	const std::vector<PhysicalLayer> & allCutPhysicalLayers() const;

	std::size_t getNumSpacing() const;
	const std::vector<PhysicalSpacing> & allSpacing() const;
	
	//I'm assuming all rows have the same height.
	DBU getRowHeight() const;
	DBU getRowSiteWidth() const ;
	std::size_t getNumRows() const ;
	Range<ListCollection<PhysicalRowData, PhysicalRow>> allPhysicalRows();
	
protected:
	void addPhysicalSite(const LefSiteDscp & site);
	void addPhysicalLayer(const LefLayerDscp& layer);
	Rsyn::LibraryCell addPhysicalLibraryCell(const LefMacroDscp& macro);
	void addPhysicalLibraryPin(Rsyn::LibraryCell libCell, const LefPinDscp& lefPin);
	void addPhysicalCell(Rsyn::Instance cell, const DefComponentDscp& component);
	void addPhysicalPort(Rsyn::Instance cell, const DefPortDscp& port);
	void addPhysicalRow(const DefRowDscp& defRow);
	void addPhysicalSpacing(const LefSpacingDscp & spacing);
	void addPhysicalPin();
	// works only for rectangles 
	void mergeBounds(const std::vector<Bounds> & source, std::vector<Bounds> & target, const Dimension dim = X);
	
private:
	PhysicalIndex getId(Rsyn::PhysicalRow phRow) const;

public:
	PhysicalAttributeInitializer createPhysicalAttribute();

	template<typename DefaultPhysicalValueType>
	PhysicalAttributeInitializerWithDefaultValue<DefaultPhysicalValueType>
	createPhysicalAttribute(const DefaultPhysicalValueType &defaultValue);

public:

	typedef std::function<void(Rsyn::PhysicalInstance instance)> PostInstanceMovedCallback;
	typedef std::list<std::tuple<int, PostInstanceMovedCallback>>::iterator PostInstanceMovedCallbackHandler;

	////////////////////////////////////////////////////////////////////////////
	// Placement
	////////////////////////////////////////////////////////////////////////////	
	// Caution when using dontNotifyObservers.
	// We can use it when you may expect the move to be rolled back, but it is
	// not, recall to mark the cell as dirty.
	void placeCell(Rsyn::PhysicalCell physicalCell, const DBU x, const DBU y, const bool dontNotifyObservers = false);
	void placeCell(Rsyn::Cell cell, const DBU x, const DBU y, const bool dontNotifyObservers = false);
	void placeCell(Rsyn::PhysicalCell physicalCell, const DBUxy pos, const bool dontNotifyObservers = false);
	void placeCell(Rsyn::Cell cell, const DBUxy pos, const bool dontNotifyObservers = false);
	// Explicitly notify observer that a cell was moved. This is only necessary
	// if "dontNotifyObservers = true" in "placeCell" methods.
	void notifyObservers(Rsyn::PhysicalInstance instance);
	void notifyObservers(Rsyn::PhysicalInstance instance, const PostInstanceMovedCallbackHandler &ignoreObserver);
	
	////////////////////////////////////////////////////////////////////////////
	// Notification
	////////////////////////////////////////////////////////////////////////////		

public:
	

	PostInstanceMovedCallbackHandler 
	addPostInstanceMovedCallback(const int priority, PostInstanceMovedCallback f);

	void 
	deletePostInstanceMovedCallback(PostInstanceMovedCallbackHandler &handler);	
	

	
}; // end class 

} // end namespace 


#endif /* PHYSICALDESIGN_PHYSICALDESIGN_H */

