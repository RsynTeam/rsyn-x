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

namespace Rsyn {

// -----------------------------------------------------------------------------

inline void PhysicalDesign::setClockNet(Rsyn::Net net) {
	data->clsClkNet = net;
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalDesign::getDatabaseUnits(const DBUType type) const {
	return data->clsDBUs[type];
} // end method  

// -----------------------------------------------------------------------------

inline DBUxy PhysicalDesign::getHPWL() const {
	return data->clsHPWL;
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalDesign::getHPWL(const Dimension dim) const {
	return data->clsHPWL[dim];
}// end method

// -----------------------------------------------------------------------------

inline int PhysicalDesign::getNumElements(PhysicalType type) const {
	return data->clsNumElements[type];
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalDesign::getArea(const PhysicalType type) const {
	return data->clsTotalAreas[type];
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalDesign::isEnablePhysicalPins() const {
	return data->clsEnablePhysicalPins;
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalDesign::isEnableMergeRectangles() const {
	return data->clsEnableMergeRectangles;
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalDesign::isEnableNetPinBoundaries() const {
	return data->clsEnableNetPinBoundaries;
} // end method 

// -----------------------------------------------------------------------------

inline void PhysicalDesign::addPhysicalPin() {
	std::cout << "TODO " << __func__ << "\n";
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalLayer PhysicalDesign::getPhysicalLayerByName(const std::string & layerName) {
	std::unordered_map<std::string, std::size_t>::iterator element = data->clsMapPhysicalLayers.find(layerName);
	if (element == data->clsMapPhysicalLayers.end())
		return nullptr;
	const int id = element->second;
	Element<PhysicalLayerData> * phLayerDataElement = data->clsPhysicalLayers.get(id);
	return PhysicalLayer(&phLayerDataElement->value);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalSite PhysicalDesign::getPhysicalSiteByName(const std::string & siteName) {
	std::unordered_map<std::string, int>::iterator it = data->clsMapPhysicalSites.find(siteName);
	return it != data->clsMapPhysicalSites.end() ? data->clsPhysicalSites[it->second] : nullptr;
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalRegion PhysicalDesign::getPhysicalRegionByName(const std::string &siteName) {
	std::unordered_map<std::string, std::size_t>::iterator it = data->clsMapPhysicalRegions.find(siteName);
	if (it == data->clsMapPhysicalRegions.end())
		return nullptr;
	return data->clsPhysicalRegions[it->second];
} // end method 

// -----------------------------------------------------------------------------	

inline Rsyn::PhysicalGroup PhysicalDesign::getPhysicalGroupByName(const std::string &siteName) {
	std::unordered_map<std::string, std::size_t>::iterator it = data->clsMapPhysicalGroups.find(siteName);
	if (it == data->clsMapPhysicalGroups.end())
		return nullptr;
	return data->clsPhysicalGroups[it->second];
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalVia PhysicalDesign::getPhysicalViaByName(const std::string &viaName) {
	std::unordered_map<std::string, std::size_t>::iterator it = data->clsMapPhysicalVias.find(viaName);
	if (it == data->clsMapPhysicalVias.end())
		return nullptr;
	return data->clsPhysicalVias[it->second];
} // end method 

// -----------------------------------------------------------------------------

inline int PhysicalDesign::getNumLayers(const Rsyn::PhysicalLayerType type) const {
	return data->clsNumLayers[type];
} // end method 

// -----------------------------------------------------------------------------

inline int PhysicalDesign::getNumLayers() const {
	return data->clsPhysicalLayers.size();
} // end method 

// -----------------------------------------------------------------------------

inline Range<ListCollection<PhysicalLayerData, PhysicalLayer>>
PhysicalDesign::allPhysicalLayers() {
	return ListCollection<PhysicalLayerData, PhysicalLayer>(data->clsPhysicalLayers);
} // end method

// -----------------------------------------------------------------------------

inline std::size_t PhysicalDesign::getNumPhysicalVias() const {
	return data->clsPhysicalVias.size();
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<PhysicalVia> & PhysicalDesign::allPhysicalVias() const {
	return data->clsPhysicalVias;
} // end method 

// -----------------------------------------------------------------------------

inline std::size_t PhysicalDesign::getNumPhysicalTracks()const {
	return data->clsPhysicalTracks.size();
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<PhysicalTrack> & PhysicalDesign::allPhysicalTracks() const {
	return data->clsPhysicalTracks;
} // end method 

// -----------------------------------------------------------------------------

inline std::size_t PhysicalDesign::getNumPhysicalSpacing() const {
	return data->clsPhysicalSpacing.size();
} // end method 

// -----------------------------------------------------------------------------

inline Range<ListCollection<PhysicalSpacingData, PhysicalSpacing>>
PhysicalDesign::allPhysicalSpacing() const {
	return ListCollection<PhysicalSpacingData, PhysicalSpacing>(data->clsPhysicalSpacing);
} // end method 

// -----------------------------------------------------------------------------

inline std::size_t PhysicalDesign::getNumPhysicalRegions() const {
	return data->clsPhysicalRegions.size();
} // end method 

// -----------------------------------------------------------------------------

inline std::vector<PhysicalRegion> & PhysicalDesign::allPhysicalRegions() const {
	return data->clsPhysicalRegions;
} // end method 

// -----------------------------------------------------------------------------

inline std::size_t PhysicalDesign::getNumPhysicalGroups() const {
	return data->clsPhysicalGroups.size();
} // end method 

// -----------------------------------------------------------------------------

inline std::vector<PhysicalGroup> & PhysicalDesign::allPhysicalGroups() const {
	return data->clsPhysicalGroups;
} // end method 

// -----------------------------------------------------------------------------

inline std::vector<PhysicalSpecialNet> & PhysicalDesign::allPhysicalSpecialNets() const {
	return data->clsPhysicalSpecialNets;
} // end method 

// -----------------------------------------------------------------------------

//I'm assuming all rows have the same height.

inline DBU PhysicalDesign::getRowHeight() const {
	return data->clsPhysicalRows.get(0)->value.getHeight();
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalDesign::getRowSiteWidth() const {
	return data->clsPhysicalRows.get(0)->value.clsPhysicalSite.getWidth();
} // end method 

// -----------------------------------------------------------------------------

inline std::size_t PhysicalDesign::getNumRows() const {
	return data->clsPhysicalRows.size();
} // end method 

// -----------------------------------------------------------------------------

inline Range<ListCollection<PhysicalRowData, PhysicalRow>>
PhysicalDesign::allPhysicalRows() {
	return ListCollection<PhysicalRowData, PhysicalRow>(data->clsPhysicalRows);
} // end method

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalLibraryPin PhysicalDesign::getPhysicalLibraryPin(Rsyn::LibraryPin libPin) const {
	return PhysicalLibraryPin(&data->clsPhysicalLibraryPins[libPin]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalLibraryPin PhysicalDesign::getPhysicalLibraryPin(Rsyn::Pin pin) const {
	if (pin.getInstanceType() != Rsyn::CELL)
		return nullptr;
	return PhysicalLibraryPin(&data->clsPhysicalLibraryPins[pin.getLibraryPin()]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalLibraryCell PhysicalDesign::getPhysicalLibraryCell(Rsyn::Cell cell) const {
	Rsyn::LibraryCell libCell = cell.getLibraryCell();
	return PhysicalLibraryCell(&data->clsPhysicalLibraryCells[libCell]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalLibraryCell PhysicalDesign::getPhysicalLibraryCell(Rsyn::LibraryCell libCell) const {
	return PhysicalLibraryCell(&data->clsPhysicalLibraryCells[libCell]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalCell PhysicalDesign::getPhysicalCell(Rsyn::Cell cell) const {
	return PhysicalCell(&data->clsPhysicalInstances[cell]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalCell PhysicalDesign::getPhysicalCell(Rsyn::Pin pin) const {
	Rsyn::Instance instance = pin.getInstance();
	return getPhysicalCell(instance.asCell());
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalInstance PhysicalDesign::getPhysicalInstance(Rsyn::Instance instance) const {
	return PhysicalInstance(&data->clsPhysicalInstances[instance]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalInstance PhysicalDesign::getPhysicalInstance(Rsyn::Pin pin) const {
	return getPhysicalInstance(pin.getInstance());
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalPort PhysicalDesign::getPhysicalPort(Rsyn::Port port) const {
	return PhysicalPort(&data->clsPhysicalInstances[port]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalPort PhysicalDesign::getPhysicalPort(Rsyn::Pin pin) const {
	Rsyn::Instance instance = pin.getInstance();
	return getPhysicalPort(instance.asPort());
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalModule PhysicalDesign::getPhysicalModule(Rsyn::Module module) const {
	return PhysicalModule(&data->clsPhysicalInstances[module]);
} // end method 

// -----------------------------------------------------------------------------

inline int PhysicalDesign::getNumMovedCells() const {
	int count = 0;
	for (Rsyn::Instance instance : data->clsModule.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;
		Rsyn::Cell cell = instance.asCell(); // TODO: hack, assuming that the instance is a cell
		Rsyn::PhysicalCell phCell = getPhysicalCell(cell);
		if (instance.isFixed() || instance.isMacroBlock())
			continue;

		const DBUxy initialPos = phCell.getInitialPosition();
		const DBUxy currentPos = phCell.getPosition();
		if (initialPos[X] != currentPos[X] || initialPos[Y] != initialPos[Y])
			count++;
	} // end for
	return count;
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalModule PhysicalDesign::getPhysicalModule(Rsyn::Pin pin) const {
	Rsyn::Instance instance = pin.getInstance();
	return getPhysicalModule(instance.asModule());
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalDie PhysicalDesign::getPhysicalDie() const {
	return PhysicalDie(&data->clsPhysicalDie);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalPin PhysicalDesign::getPhysicalPin(Rsyn::Pin pin) const {
	if (!data->clsEnablePhysicalPins)
		return nullptr;
	return PhysicalPin(&data->clsPhysicalPins[pin]);
} // end method 

// -----------------------------------------------------------------------------

inline DBUxy PhysicalDesign::getPinDisplacement(Rsyn::Pin pin) const {
	Rsyn::Instance inst = pin.getInstance();
	if (inst.getType() == Rsyn::CELL) {
		Rsyn::LibraryPin libPin = pin.getLibraryPin();
		DBUxy displacement = data->clsPhysicalLibraryPins[libPin].clsLayerBound.computeCenter();
		return displacement;
	} // end if 
	return DBUxy(0, 0);
} // end method 

// -----------------------------------------------------------------------------

inline DBUxy PhysicalDesign::getPinPosition(Rsyn::Pin pin) const {
	// Position may be defined if the instance has info. 
	// I'm assuming the instance doesn't know what is its position. 
	DBUxy pos;
	Rsyn::InstanceType type = pin.getInstanceType();
	switch (type) {
		case Rsyn::CELL:
			pos = getPhysicalCell(pin).getPosition();
			break;
		case Rsyn::MODULE:
			pos = getPhysicalModule(pin).getPosition();
			break;
		case Rsyn::PORT:
			pos = getPhysicalPort(pin).getPosition();
			break;
		default:
			pos.apply(std::numeric_limits<DBU>::infinity());
			std::cout << "WARNING: Position for " << pin.getFullName() << " was not defined for the instance type\n";
	} // end switch 
	return pos + getPinDisplacement(pin);
} // end method 

// -----------------------------------------------------------------------------

// For pins of standard-cells, returns the cell position. For macro-blocks,
// returns the pin position itself.

inline DBUxy PhysicalDesign::getRelaxedPinPosition(Rsyn::Pin pin) const {
	// Position may be defined if the instance has info. 
	// I'm assuming the instance doesn't know what is its position. 
	DBUxy pos;
	Rsyn::InstanceType type = pin.getInstanceType();
	Rsyn::PhysicalCell phCell;
	switch (type) {
		case Rsyn::CELL:
			phCell = getPhysicalCell(pin);
			pos = phCell.getPosition();
			if (pin.isMacroBlockPin())
				pos += getPinDisplacement(pin);
			break;
		case Rsyn::MODULE:
			pos = getPhysicalModule(pin).getPosition();
			pos += getPinDisplacement(pin);
			break;
		case Rsyn::PORT:
			pos = getPhysicalPort(pin).getPosition();
			pos += getPinDisplacement(pin);
			break;
		default:
			pos.apply(std::numeric_limits<DBU>::infinity());
			std::cout << "WARNING: Position for " << pin.getFullName() << " was not defined for the instance type\n";
	} // end switch 
	return pos;
} // end method

// -----------------------------------------------------------------------------

inline DBU PhysicalDesign::getPinDisplacement(Rsyn::Pin pin, const Dimension dim) const {
	DBUxy disp = getPinDisplacement(pin);
	return disp[dim];
} // end method 

// -----------------------------------------------------------------------------

inline DBU PhysicalDesign::getPinPosition(Rsyn::Pin pin, const Dimension dim) const {
	DBUxy pos = getPinPosition(pin);
	return pos[dim];
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalNet PhysicalDesign::getPhysicalNet(Rsyn::Net net) const {
	return PhysicalNet(&data->clsPhysicalNets[net]);
} // end method 

// -----------------------------------------------------------------------------

inline PhysicalIndex PhysicalDesign::getId(Rsyn::PhysicalRow phRow) const {
	return phRow->id;
} // end method 

// -----------------------------------------------------------------------------

inline PhysicalIndex PhysicalDesign::getId(Rsyn::PhysicalLayer phLayer) const {
	return phLayer->id;
} // end method 

// -----------------------------------------------------------------------------

inline PhysicalIndex PhysicalDesign::getId(Rsyn::PhysicalSpacing spacing) const {
	return spacing->id;
} // end method

// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// Misc
////////////////////////////////////////////////////////////////////////////////

inline PhysicalAttributeInitializer PhysicalDesign::createPhysicalAttribute() {
	return PhysicalAttributeInitializer(*this);
} // end method

// -----------------------------------------------------------------------------

template<typename DefaultPhysicalValueType>
inline PhysicalAttributeInitializerWithDefaultValue<DefaultPhysicalValueType>
PhysicalDesign::createPhysicalAttribute(const DefaultPhysicalValueType &defaultValue) {
	return PhysicalAttributeInitializerWithDefaultValue<DefaultPhysicalValueType>(*this, defaultValue);
} // end method


// -----------------------------------------------------------------------------

// Caution when using dontNotifyObservers.
// We can use it when you may expect the move to be rolled back, but it is
// not, recall to mark the cell as dirty.

inline void PhysicalDesign::placeCell(Rsyn::PhysicalCell physicalCell, const DBU x, const DBU y, const bool dontNotifyObservers) {
	DBUxy previousPos = physicalCell.getPosition();
	const double preivousX = physicalCell.getCoordinate(LOWER, X);
	const double preivousY = physicalCell.getCoordinate(LOWER, Y);

	const bool moved = (x != physicalCell.getPosition(X)) || (y != physicalCell.getPosition(Y));

	// Notify observers.
	if (moved)
		for (auto f : data->clsPhysicalObservers[PHYSICAL_EVENT_PRE_INSTANCE_MOVED])
			f->onPreMovedInstance(physicalCell.getInstance());


	physicalCell->clsBounds.moveTo(x, y);

	
	// Notify observers.
	if (moved) {
		for (auto f : data->clsPhysicalObservers[PHYSICAL_EVENT_POS_INSTANCE_MOVED]) {
			f->onPostMovedInstance(physicalCell);
		} // end for 
	} // end if 
	
	// TODO REMOVE 
	// Only notify observers if the instance actually moved. We noted that many
	// times the cell end up in the exactly same position.
	if (!dontNotifyObservers) {
		if (preivousX != physicalCell.getCoordinate(LOWER, X) || preivousY != physicalCell.getCoordinate(LOWER, Y)) {
			// Notify observers...
			for (auto &f : data->callbackPostInstanceMoved)
				std::get<1>(f) (physicalCell);
		} // end if
	} // end if	
} // end method

// -----------------------------------------------------------------------------

inline void PhysicalDesign::placeCell(Rsyn::Cell cell, const DBU x, const DBU y, const bool dontNotifyObservers) {
	placeCell(getPhysicalCell(cell), x, y, dontNotifyObservers);
} // end method	

// -----------------------------------------------------------------------------

inline void PhysicalDesign::placeCell(Rsyn::PhysicalCell physicalCell, const DBUxy pos, const bool dontNotifyObservers) {
	placeCell(physicalCell, pos[X], pos[Y], dontNotifyObservers);
} // end method

// -----------------------------------------------------------------------------

inline void PhysicalDesign::placeCell(Rsyn::Cell cell, const DBUxy pos, const bool dontNotifyObservers) {
	placeCell(getPhysicalCell(cell), pos[X], pos[Y], dontNotifyObservers);
} // end method

// -----------------------------------------------------------------------------

inline void PhysicalDesign::notifyObservers(Rsyn::PhysicalInstance instance) {
	// Notify observers...
	for (auto &f : data->callbackPostInstanceMoved)
		std::get<1>(f) (instance);
} // end method	

// -----------------------------------------------------------------------------

inline void PhysicalDesign::notifyObservers(Rsyn::PhysicalInstance instance, const PostInstanceMovedCallbackHandler &ignoreObserver) {
	// Notify observers...
	for (PostInstanceMovedCallbackHandler it = data->callbackPostInstanceMoved.begin();
		it != data->callbackPostInstanceMoved.end(); it++) {
		if (it != ignoreObserver) {
			std::get<1>(*it)(instance);
		} // end if
	} // end for
} // end method
// -----------------------------------------------------------------------------

inline PhysicalDesign::PostInstanceMovedCallbackHandler
PhysicalDesign::addPostInstanceMovedCallback(const int priority, PostInstanceMovedCallback f) {

	// We want to compare only the first element of the tuple. The default
	// comparator tries to compare all elements.
	auto comparator = [](
		const std::tuple<int, PhysicalDesign::PostInstanceMovedCallback> &left,
		const std::tuple<int, PhysicalDesign::PostInstanceMovedCallback> &right
		) -> bool {
			return std::get<0>(left) < std::get<0>(right);
		};

	data->callbackPostInstanceMoved.push_back(std::make_tuple(priority, f));
	PhysicalDesign::PostInstanceMovedCallbackHandler handler = std::prev(
		data->callbackPostInstanceMoved.end());
	data->callbackPostInstanceMoved.sort(comparator);
	return handler;
} // end method



////////////////////////////////////////////////////////////////////////////////
// Events
////////////////////////////////////////////////////////////////////////////////

template<class T>
inline void PhysicalDesign::registerObserver(T *observer) {
	static_assert(std::is_base_of<PhysicalObserver, T>::value,
		"Unable to register class as observer. "
		"The observer class must inherit from Rsyn::PhysicalObserver.");

	observer->PhysicalObserver::clsPhDesign = PhysicalDesign(data);

	// Check if the observer implements (overwrites) the event callbacks if so
	// register it to receive notifications of the respective event.

	if (typeid (&PhysicalObserver::onPhysicalDesignDestruction) != typeid (&T::onPhysicalDesignDestruction)) {
		data->clsPhysicalObservers[PHYSICAL_EVENT_DESTRUCTION].push_back(observer);
	} // end if

	if (typeid (&PhysicalObserver::onPostCellRemap) != typeid (&T::onPostCellRemap)) {
		data->clsPhysicalObservers[PHYSICAL_EVENT_CELL_REMAP].push_back(observer);
	} // end if

	if (typeid (&PhysicalObserver::onPostMovedInstance) != typeid (&T::onPostMovedInstance)) {
		data->clsPhysicalObservers[PHYSICAL_EVENT_POS_INSTANCE_MOVED].push_back(observer);
	} // end if
	
	if (typeid (&PhysicalObserver::onPreMovedInstance) != typeid (&T::onPreMovedInstance)) {
		data->clsPhysicalObservers[PHYSICAL_EVENT_PRE_INSTANCE_MOVED].push_back(observer);
	} // end if

} // end method

// -----------------------------------------------------------------------------

inline void PhysicalDesign::unregisterObserver(PhysicalObserver *observer) {
	for (int i = 0; i < NUM_PHYSICAL_EVENTS; i++) {
		data->clsPhysicalObservers[i].remove(observer);
	} // end for
	observer->PhysicalObserver::clsPhDesign = nullptr;
} // end method





} // end namespace 
