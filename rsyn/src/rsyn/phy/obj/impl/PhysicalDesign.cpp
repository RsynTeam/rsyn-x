/* Copyright 2014-2018 Rsyn
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

#include <Rsyn/PhysicalDesign>

namespace Rsyn {

// -----------------------------------------------------------------------------

void PhysicalDesign::loadLibrary(const LefDscp & library) {
	if (!data) {
		std::cout << "ERROR: Physical Design was not configured!\n"
			<< "Please call first initPhysicalDesign!"
			<< std::endl;
		return;
	} // end if 

	if (library.clsLefUnitsDscp.clsHasDatabase) {
		if (getDatabaseUnits(LIBRARY_DBU) == 0) {
			data->clsDBUs[LIBRARY_DBU] = library.clsLefUnitsDscp.clsDatabase;
		} else {
			if (getDatabaseUnits(LIBRARY_DBU) != library.clsLefUnitsDscp.clsDatabase) {
				std::cout << "WARNING: Stored LEF database units "
					<< getDatabaseUnits(LIBRARY_DBU)
					<< " is not equal to LEF database units defined in Library "
					<< library.clsLefUnitsDscp.clsDatabase
					<< ".\n";
				std::cout << "WARNING: Lef library elements were NOT initialized!\n";
				return;
			} // end if 
		} // end if-else
	} // end if 

	// Initializing physical sites
	data->clsPhysicalSites.reserve(library.clsLefSiteDscps.size());
	for (const LefSiteDscp & lefSite : library.clsLefSiteDscps) {
		addPhysicalSite(lefSite);
	} // end for

	// Initializing physical layers
	data->clsPhysicalLayers.reserve(library.clsLefLayerDscps.size());
	Rsyn::PhysicalLayerData * lower = nullptr;
	for (const LefLayerDscp & lefLayer : library.clsLefLayerDscps) {
		lower = addPhysicalLayer(lefLayer, lower);
	} // end for 

	// Initializing physical vias
	data->clsPhysicalVias.reserve(library.clsLefViaDscps.size());
	for (const LefViaDscp & via : library.clsLefViaDscps) {
		addPhysicalVia(via);
	} // end for 

	// initializing physical spacing 
	for (const LefSpacingDscp & spc : library.clsLefSpacingDscps) {
		addPhysicalSpacing(spc);
	} // end for 

	// initializing physical cells (LEF macros)
	for (const LefMacroDscp & macro : library.clsLefMacroDscps) {
		// Adding Physical Library cell to Physical Layer
		Rsyn::LibraryCell libCell = addPhysicalLibraryCell(macro);

		// Adding Physical Library pins to Physical Layer
		for (const LefPinDscp &lpin : macro.clsPins) {
			addPhysicalLibraryPin(libCell, lpin);
		} // end for	
	} // end for

} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::loadDesign(const DefDscp & design) {
	if (data->clsLoadDesign) {
		std::cout << "WARNING: Design was already loaded. Skipping ... \n";
		return;
	} // end if
	data->clsLoadDesign = true;

	// Adding Library cell to Physical Layer
	data->clsDBUs[DESIGN_DBU] = design.clsDatabaseUnits;
	data->clsPhysicalDie.clsBounds = design.clsDieBounds;

	if (getDatabaseUnits(LIBRARY_DBU) % getDatabaseUnits(DESIGN_DBU) != 0) {
		std::cout << "ERROR: Invalid DEF database units " << getDatabaseUnits(DESIGN_DBU) << " (LEF database units: " << getDatabaseUnits(LIBRARY_DBU) << ").\n";
		std::cout << "DEF design units should be lower or equal to LEF design units and must have a integer multiple. Physical design was not initialized!\n";
		return;
	} // end if 

	// This operation always results in an integer number factor. 
	// LEF/DEF specifications prohibit division that results in a real number factor.
	data->clsDBUs[MULT_FACTOR_DBU] = getDatabaseUnits(LIBRARY_DBU) / getDatabaseUnits(DESIGN_DBU);

	// Adding design defined vias
	std::size_t numVias = data->clsPhysicalVias.size() + design.clsVias.size();
	data->clsPhysicalVias.reserve(numVias);
	for (const DefViaDscp & via : design.clsVias)
		addPhysicalDesignVia(via);

	// initializing physical cells (DEF Components)
	for (const DefComponentDscp & component : design.clsComps) {
		// Adding Physical cell to Physical Layer
		Rsyn::Cell cell = data->clsDesign.findCellByName(component.clsName);
		if (!cell) {
			throw Exception("Cell " + component.clsName + " not found.\n");
		} // end if
		addPhysicalCell(cell, component);
	} // end for

	// Initializing circuit ports
	for (const DefPortDscp & pin_port : design.clsPorts) {
		// Adding Library cell to Physical Layer
		Rsyn::Port cell = data->clsDesign.findPortByName(pin_port.clsName);
		if (!cell) {
			throw Exception("Port " + pin_port.clsName + " not found.\n");
		} // end if
		addPhysicalPort(cell, pin_port);
	} // end for

	//Initializing circuit rows and defining circuit bounds
	Rsyn::PhysicalModule phModule = getPhysicalModule(data->clsModule);
	Bounds & bounds = phModule->clsInstance->clsBounds;
	bounds[LOWER].apply(+std::numeric_limits<DBU>::max());
	bounds[UPPER].apply(-std::numeric_limits<DBU>::max());
	data->clsPhysicalRows.reserve(design.clsRows.size()); // reserving space for rows
	for (const DefRowDscp & defRow : design.clsRows) {
		addPhysicalRow(defRow);
	} // end for 

	data->clsMapPhysicalRegions.reserve(design.clsRegions.size());
	data->clsPhysicalRegions.reserve(design.clsRegions.size());
	for (const DefRegionDscp & defRegion : design.clsRegions)
		addPhysicalRegion(defRegion);

	data->clsMapPhysicalGroups.reserve(design.clsGroups.size());
	data->clsPhysicalGroups.reserve(design.clsGroups.size());
	for (const DefGroupDscp & defGroup : design.clsGroups)
		addPhysicalGroup(defGroup);

	for (const DefNetDscp & net : design.clsNets)
		addPhysicalNet(net);

	data->clsPhysicalSpecialNets.reserve(design.clsSpecialNets.size());
	for (const DefSpecialNetDscp & specialNet : design.clsSpecialNets)
		addPhysicalSpecialNet(specialNet);

	data->clsPhysicalTracks.reserve(design.clsTracks.size());
	for (const DefTrackDscp & track : design.clsTracks)
		addPhysicalTracks(track);
	initRoutingGrid();
	initLayerViaManager();
	// only to keep coherence in the design;
	data->clsNumElements[PHYSICAL_PORT] = data->clsDesign.getNumInstances(Rsyn::PORT);

	std::cout << "Loaded Design\n";
	std::cout << "\tName: " << data->clsDesign.getName() << "\n";
	std::cout << "\t#Moveable Insts: " << getNumElements(PhysicalType::PHYSICAL_MOVABLE) << "\n";
	std::cout << "\t#Fixed Insts: " << getNumElements(PhysicalType::PHYSICAL_FIXED) << "\n";
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::initPhysicalDesign(Rsyn::Design dsg, const Rsyn::Json &params) {
	if (data) {
		std::cout << "ERROR: design already set.\nSkipping initialize Physical Design\n";
		return;
	} // end if
	this->data = new PhysicalDesignData();

	if (!params.is_null()) {
		data->clsEnablePhysicalPins = params.value("clsEnablePhysicalPins", data->clsEnablePhysicalPins);
		data->clsEnableMergeRectangles = params.value("clsEnableMergeRectangles", data->clsEnableMergeRectangles);
		data->clsEnableNetPinBoundaries = params.value("clsEnableNetPinBoundaries", data->clsEnableNetPinBoundaries);
		data->clsMode = getPhysicalDesignModeType(params.value("clsPhysicalDesignMode", "ALL"));
	} // end if 

	data->clsDesign = dsg;
	data->clsModule = dsg.getTopModule();
	data->clsPhysicalInstances = dsg.createAttribute();
	data->clsPhysicalLibraryCells = dsg.createAttribute();
	data->clsPhysicalLibraryPins = dsg.createAttribute();
	if (data->clsEnablePhysicalPins)
		data->clsPhysicalPins = dsg.createAttribute();
	data->clsPhysicalNets = dsg.createAttribute();

	// Initialize data of top module.
	PhysicalInstanceData &phTopModule = data->clsPhysicalInstances[dsg.getTopModule()];
	phTopModule.clsInstance = dsg.getTopModule();
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::updateAllNetBounds(const bool skipClockNet) {
	if (skipClockNet && data->clsClkNet) {
		Rsyn::PhysicalNet phNet = getPhysicalNet(data->clsClkNet);
		data->clsHPWL -= phNet.getHPWL();
	} // end if 

	for (Rsyn::Net net : data->clsModule.allNets()) {
		//skipping clock network
		if (skipClockNet && (data->clsClkNet == net))
			continue;

		updateNetBound(net);
	} // end for
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::updateNetBound(Rsyn::Net net) {
	// net has not pins. The boundaries are defined by default to 0.
	if (net.getNumPins() == 0)
		return;

	PhysicalNetData &phNet = data->clsPhysicalNets[net];
	Bounds &bound = phNet.clsBounds;
	data->clsHPWL -= bound.computeLength(); // remove old net wirelength
	bound[UPPER].apply(-std::numeric_limits<DBU>::max());
	bound[LOWER].apply(+std::numeric_limits<DBU>::max());
	const bool updatePinBound = data->clsEnableNetPinBoundaries;
	for (Rsyn::Pin pin : net.allPins()) {
		DBUxy pos = getPinPosition(pin);

		// upper x corner pos
		if (pos[X] >= bound[UPPER][X]) {
			bound[UPPER][X] = pos[X];
			if (updatePinBound)
				phNet.clsBoundPins[UPPER][X] = pin;
		} // end if 

		// lower x corner pos 
		if (pos[X] <= bound[LOWER][X]) {
			bound[LOWER][X] = pos[X];
			if (updatePinBound)
				phNet.clsBoundPins[LOWER][X] = pin;
		} // end if 

		// upper y corner pos 
		if (pos[Y] >= bound[UPPER][Y]) {
			bound[UPPER][Y] = pos[Y];
			if (updatePinBound)
				phNet.clsBoundPins[UPPER][Y] = pin;
		} // end if 

		// lower y corner pos 
		if (pos[Y] <= bound[LOWER][Y]) {
			bound[LOWER][Y] = pos[Y];
			if (updatePinBound)
				phNet.clsBoundPins[LOWER][Y] = pin;
		} // end if 
	} // end for
	data->clsHPWL += bound.computeLength(); // update hpwl 
} // end method 

// -----------------------------------------------------------------------------

// Adding the new Site parameter to PhysicalDesign data structure.

void PhysicalDesign::addPhysicalSite(const LefSiteDscp & site) {
	std::unordered_map<std::string, int>::iterator it = data->clsMapPhysicalSites.find(site.clsName);
	if (it != data->clsMapPhysicalSites.end()) {
		std::cout << "WARNING: Site " << site.clsName << " was already defined. Skipping ...\n";
		return;
	} // end if 

	// Adding new lib site
	data->clsPhysicalSites.push_back(PhysicalSite(new PhysicalSiteData()));
	PhysicalSite phSite = data->clsPhysicalSites.back();
	data->clsMapPhysicalSites[site.clsName] = data->clsPhysicalSites.size() - 1;
	phSite->id = data->clsPhysicalSites.size() - 1;
	phSite->clsSiteName = site.clsName;
	double2 size = site.clsSize;
	size.scale(static_cast<double> (getDatabaseUnits(LIBRARY_DBU)));
	phSite->clsSize[X] = static_cast<DBU> (std::round(size[X]));
	phSite->clsSize[Y] = static_cast<DBU> (std::round(size[Y]));
	phSite->clsSiteClass = getPhysicalSiteClass(site.clsSiteClass);
} // end method 

// -----------------------------------------------------------------------------

Rsyn::PhysicalLayerData * PhysicalDesign::addPhysicalLayer(const LefLayerDscp& layer, Rsyn::PhysicalLayerData * lower) {
	std::unordered_map<std::string, std::size_t>::iterator it = data->clsMapPhysicalLayers.find(layer.clsName);
	if (it != data->clsMapPhysicalLayers.end()) {
		std::cout << "WARNING: Layer " << layer.clsName << " was already defined. Skipping ...\n";
		// TODO -> return the pointer to the phLayerData of already defined physical layer.
		return nullptr;
	} // end if 

	DBU libDBU = getDatabaseUnits(LIBRARY_DBU);
	Element<PhysicalLayerData> *element = data->clsPhysicalLayers.create();
	Rsyn::PhysicalLayerData * phLayer = &(element->value);
	phLayer->id = data->clsPhysicalLayers.lastId();
	phLayer->clsName = layer.clsName;
	phLayer->clsDirection = Rsyn::getPhysicalLayerDirection(layer.clsDirection);
	phLayer->clsType = Rsyn::getPhysicalLayerType(layer.clsType);
	phLayer->clsPitch[X] = static_cast<DBU> (std::round(layer.clsPitch[X] * libDBU));
	phLayer->clsPitch[Y] = static_cast<DBU> (std::round(layer.clsPitch[Y] * libDBU));
	phLayer->clsSpacing.reserve(layer.clsSpacingRules.size());
	for (const LefSpacingRuleDscp & spcRule : layer.clsSpacingRules) {
		phLayer->clsSpacing.push_back(PhysicalSpacingRule(new PhysicalSpacingRuleData()));
		PhysicalSpacingRule & spc = phLayer->clsSpacing.back();
		spc.data->clsEOL = static_cast<DBU> (std::round(spcRule.clsEOL * libDBU));
		spc.data->clsSpacing = static_cast<DBU> (std::round(spcRule.clsSpacing * libDBU));
		spc.data->clsEOLWithin = static_cast<DBU> (std::round(spcRule.clsEOLWithin * libDBU));
	} // end for 
	phLayer->clsMinWidth = static_cast<DBU> (std::round(layer.clsMinWidth * libDBU));
	phLayer->clsArea = static_cast<DBU> (std::round(layer.clsArea * libDBU * libDBU));
	phLayer->clsWidth = static_cast<DBU> (std::round(layer.clsWidth * libDBU));
	phLayer->clsRelativeIndex = data->clsNumLayers[phLayer->clsType];
	data->clsMapPhysicalLayers[layer.clsName] = phLayer->id;
	data->clsNumLayers[phLayer->clsType]++;
	if (lower) {
		lower->clsUpper = phLayer;
		phLayer->clsLower = lower;
	} // end if
	if (phLayer->clsType == Rsyn::PhysicalLayerType::ROUTING)
		data->clsPhysicalRoutingLayerIndeces.push_back(phLayer->id);
	return phLayer;
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalVia(const LefViaDscp & via) {
	std::unordered_map<std::string, std::size_t>::iterator it = data->clsMapPhysicalVias.find(via.clsName);
	if (it != data->clsMapPhysicalVias.end()) {
		std::cout << "WARNING: Site " << via.clsName << " was already defined. Skipping ...\n";
		return;
	} // end if 

	// Adding new lib site
	data->clsMapPhysicalVias[via.clsName] = data->clsPhysicalVias.size();
	data->clsPhysicalVias.push_back(PhysicalVia(new PhysicalViaData()));
	PhysicalVia phVia = data->clsPhysicalVias.back();
	phVia->id = data->clsPhysicalVias.size() - 1;
	phVia->clsName = via.clsName;

	std::vector<std::tuple<int, PhysicalViaLayerData *>> layers;
	for (const LefViaLayerDscp & layerDscp : via.clsViaLayers) {
		PhysicalViaLayerData * phViaLayer = new PhysicalViaLayerData();
		phViaLayer->clsPhVia = phVia;
		phViaLayer->clsLayer = getPhysicalLayerByName(layerDscp.clsLayerName);
		phViaLayer->clsBounds.reserve(layerDscp.clsBounds.size());
		for (DoubleRectangle doubleRect : layerDscp.clsBounds) {
			doubleRect.scaleCoordinates(static_cast<double> (getDatabaseUnits(LIBRARY_DBU)));
			phViaLayer->clsBounds.push_back(Bounds());
			Bounds & bds = phViaLayer->clsBounds.back();
			bds[LOWER][X] = static_cast<DBU> (std::round(doubleRect[LOWER][X]));
			bds[LOWER][Y] = static_cast<DBU> (std::round(doubleRect[LOWER][Y]));
			bds[UPPER][X] = static_cast<DBU> (std::round(doubleRect[UPPER][X]));
			bds[UPPER][Y] = static_cast<DBU> (std::round(doubleRect[UPPER][Y]));
		} // end for

		assert(phViaLayer->clsLayer);
		layers.push_back(std::make_tuple(phViaLayer->clsLayer.getIndex(), phViaLayer));
	} // end for

	std::sort(layers.begin(), layers.end());

	//	assert(layers.size() == NUM_VIA_LAYERS);
	for (int i = 0; i < NUM_VIA_LAYERS; i++) {
		phVia->clsViaLayers[i] = std::get<1>(layers[i]);
	} // end for
} // end method 

// -----------------------------------------------------------------------------

Rsyn::LibraryCell PhysicalDesign::addPhysicalLibraryCell(const LefMacroDscp& macro) {
	Rsyn::LibraryCell lCell = data->clsDesign.findLibraryCellByName(macro.clsMacroName);
	if (!lCell) {
		throw Exception("Library Cell " + macro.clsMacroName + " not found.\n");
	} // end if
	Rsyn::PhysicalLibraryCellData &phlCell = data->clsPhysicalLibraryCells[lCell];
	double2 size = macro.clsSize;
	size.scale(static_cast<double> (getDatabaseUnits(LIBRARY_DBU)));
	phlCell.clsSize[X] = static_cast<DBU> (std::round(size[X]));
	phlCell.clsSize[Y] = static_cast<DBU> (std::round(size[Y]));
	phlCell.clsMacroClass = Rsyn::getPhysicalMacroClass(macro.clsMacroClass);
	// Initializing obstacles in the physical library cell
	phlCell.clsObs.reserve(macro.clsObs.size());
	for (const LefObsDscp &libObs : macro.clsObs) {
		PhysicalObstacle phObs(new PhysicalObstacleData());

		// Guilherme Flach - 2016/11/04 - micron to dbu
		std::vector<Bounds> scaledBounds;
		scaledBounds.reserve(libObs.clsBounds.size());
		for (DoubleRectangle doubleRect : libObs.clsBounds) {
			// Jucemar Monteiro - 2017/05/20 - Avoiding cast round errors.
			doubleRect.scaleCoordinates(static_cast<double> (getDatabaseUnits(LIBRARY_DBU)));
			scaledBounds.push_back(Bounds());
			Bounds & bds = scaledBounds.back();
			bds[LOWER][X] = static_cast<DBU> (std::round(doubleRect[LOWER][X]));
			bds[LOWER][Y] = static_cast<DBU> (std::round(doubleRect[LOWER][Y]));
			bds[UPPER][X] = static_cast<DBU> (std::round(doubleRect[UPPER][X]));
			bds[UPPER][Y] = static_cast<DBU> (std::round(doubleRect[UPPER][Y]));
		} // end for

		phlCell.clsObs.push_back(phObs);
		if (data->clsEnableMergeRectangles && libObs.clsBounds.size() > 1) {
			std::vector<Bounds> bounds;
			mergeBounds(scaledBounds, bounds);
			phObs->clsBounds.reserve(bounds.size());
			for (Bounds & rect : bounds) {
				//rect.scaleCoordinates(data->clsDesignUnits);
				phObs->clsBounds.push_back(rect);
			} // end for 
		} else {
			phObs->clsBounds = scaledBounds;
		} // end if-else 
		phObs->clsLayer = getPhysicalLayerByName(libObs.clsMetalLayer);

		// Mateus - 2018/03/11 - Defensive programming to avoid crashes in ICCAD15 benchmarks.
		if (!phObs->clsLayer)
			continue;

		phObs->id = phlCell.clsObs.size() - 1;
		// Hard code. After implementing mapping structure in Rsyn, remove this line.
		if (libObs.clsMetalLayer.compare("metal1") == 0) {
			phlCell.clsLayerBoundIndex = phlCell.clsObs.size() - 1;
		} // end if 
		Rsyn::PhysicalObstacle topPhObs = phlCell.clsTopLayerObs;
		if (topPhObs != nullptr) {
			if (topPhObs.getLayer().getIndex() < phObs->clsLayer.getIndex())
				phlCell.clsTopLayerObs = phObs;
		} else {
			phlCell.clsTopLayerObs = phObs;
		}
	} // end for

	// adding physical library cell boundaries defined as a polygon
	// benchmarks of ICCAD contest define some macro boundaries as metal 1 blockages
	if (phlCell.clsLayerBoundIndex >= 0) {
		PhysicalObstacle phObs = phlCell.clsObs[phlCell.clsLayerBoundIndex];

		if (phObs.getNumObs() > 1) { // if boundaries are defined by a rectangle. Then boundaries number is one because it is rectangle.
			DBUxy lower, upper;
			const Bounds & fst = phObs.allBounds().front();
			lower = fst[LOWER];
			upper = fst[UPPER];
			upper[Y] = lower[Y];
			std::list<DBUxy> upperPoints;
			upperPoints.push_back(upper);
			phlCell.clsPolygonBounds.addPoint(lower[X], lower[Y]);
			for (const Bounds & bds : phObs.allBounds()) {
				DBUxy lowPt = bds[LOWER];
				DBUxy uppPt = bds[UPPER];
				if (lowPt[X] != lower[X]) {
					phlCell.clsPolygonBounds.addPoint(lower[X], lowPt[Y]);
					lower = lowPt;
					phlCell.clsPolygonBounds.addPoint(lower[X], lower[Y]);
				} // end if 
				if (uppPt[X] != upper[X]) {
					upperPoints.push_front(DBUxy(upper[X], lowPt[Y]));
					upper = uppPt;
					upper[Y] = lowPt[Y];
					upperPoints.push_front(upper);
				} // en if 
			} // end for 
			upperPoints.push_front(phObs.allBounds().back()[UPPER]);
			phlCell.clsPolygonBounds.addPoint(phObs.allBounds().back()[LOWER][X],
				phObs.allBounds().back()[UPPER][Y]);

			for (DBUxy point : upperPoints) {
				phlCell.clsPolygonBounds.addPoint(point[X], point[Y]);
			} // end for 
		} // end if 
	} // end if

	return lCell;
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalLibraryPin(Rsyn::LibraryCell libCell, const LefPinDscp& lefPin) {
	Rsyn::LibraryPin rsynLibraryPin = libCell.getLibraryPinByName(lefPin.clsPinName);
	if (!rsynLibraryPin) {
		throw Exception("Pin '" + lefPin.clsPinName + "' not found in library cell '" + libCell.getName() + "'.\n");
	} // end if
	Rsyn::PhysicalLibraryPinData &phyPin = data->clsPhysicalLibraryPins[rsynLibraryPin];
	phyPin.clsLibraryCell = getPhysicalLibraryCell(libCell);

	phyPin.clsDirection = Rsyn::getPhysicalPinDirection(lefPin.clsPinDirection);
	phyPin.clsUse = Rsyn::getPhysicalPinUseType(lefPin.clsPinUse);
	//Assuming that the pin has only one port.
	phyPin.clsPhysicalPinGeometries.reserve(lefPin.clsPorts.size());
	for (const LefPortDscp & lefPort : lefPin.clsPorts) {
		phyPin.clsPhysicalPinGeometries.push_back(PhysicalPinGeometry(new PhysicalPinGeometryData()));
		PhysicalPinGeometry phPinPort = phyPin.clsPhysicalPinGeometries.back();
		phPinPort->id = phyPin.clsPhysicalPinGeometries.size() - 1;

		phyPin.clsLayerBound[LOWER] = DBUxy(std::numeric_limits<DBU>::max(), std::numeric_limits<DBU>::max());
		phyPin.clsLayerBound[UPPER] = DBUxy(std::numeric_limits<DBU>::min(), std::numeric_limits<DBU>::min());

		phPinPort->clsPinLayers.reserve(lefPort.clsLefPortGeoDscp.size());
		for (const LefPortGeometryDscp & lefPortGeo : lefPort.clsLefPortGeoDscp) {
			phPinPort->clsPinLayers.push_back(PhysicalPinLayer(new PhysicalPinLayerData()));
			PhysicalPinLayer phPinLayer = phPinPort->clsPinLayers.back();
			phPinLayer->id = phPinPort->clsPinLayers.size() - 1;
			phPinLayer->clsLibLayer = getPhysicalLayerByName(lefPortGeo.clsMetalName);

			//ICCAD 2015 contest pin bounds definition
			if (!lefPortGeo.clsBounds.empty()) {
				phPinLayer->clsBounds.reserve(lefPortGeo.clsBounds.size());
				for (DoubleRectangle doubleRect : lefPortGeo.clsBounds) {
					doubleRect.scaleCoordinates(static_cast<double> (getDatabaseUnits(LIBRARY_DBU)));
					phPinLayer->clsBounds.push_back(Bounds());
					Bounds & bds = phPinLayer->clsBounds.back();
					bds[LOWER][X] = static_cast<DBU> (std::round(doubleRect[LOWER][X]));
					bds[LOWER][Y] = static_cast<DBU> (std::round(doubleRect[LOWER][Y]));
					bds[UPPER][X] = static_cast<DBU> (std::round(doubleRect[UPPER][X]));
					bds[UPPER][Y] = static_cast<DBU> (std::round(doubleRect[UPPER][Y]));

					phyPin.clsLayerBound[LOWER][X] = std::min(phyPin.clsLayerBound[LOWER][X], bds[LOWER][X]);
					phyPin.clsLayerBound[LOWER][Y] = std::min(phyPin.clsLayerBound[LOWER][Y], bds[LOWER][Y]);
					phyPin.clsLayerBound[UPPER][X] = std::max(phyPin.clsLayerBound[UPPER][X], bds[UPPER][X]);
					phyPin.clsLayerBound[UPPER][Y] = std::max(phyPin.clsLayerBound[UPPER][Y], bds[UPPER][Y]);
				} // end for
			} // end if 
			if (!lefPortGeo.clsLefPolygonDscp.empty()) {
				phyPin.clsLayerBound[LOWER] = DBUxy(std::numeric_limits<DBU>::max(), std::numeric_limits<DBU>::max());
				phyPin.clsLayerBound[UPPER] = DBUxy(std::numeric_limits<DBU>::min(), std::numeric_limits<DBU>::min());
				phPinLayer->clsPolygons.reserve(lefPortGeo.clsLefPolygonDscp.size());
				for (const LefPolygonDscp & poly : lefPortGeo.clsLefPolygonDscp) {
					phPinLayer->clsPolygons.resize(phPinLayer->clsPolygons.size() + 1);
					Polygon &polygon = phPinLayer->clsPolygons.back();

					for (double2 point : poly.clsPolygonPoints) {
						point.scale(static_cast<double> (getDatabaseUnits(LIBRARY_DBU)));
						DBUxy pt(static_cast<DBU> (std::round(point[X])), static_cast<DBU> (std::round(point[Y])));
						polygon.addPoint(pt[X], pt[Y]);
						phyPin.clsLayerBound[LOWER][X] = std::min(phyPin.clsLayerBound[LOWER][X], pt[X]);
						phyPin.clsLayerBound[LOWER][Y] = std::min(phyPin.clsLayerBound[LOWER][Y], pt[Y]);
						phyPin.clsLayerBound[UPPER][X] = std::max(phyPin.clsLayerBound[UPPER][X], pt[X]);
						phyPin.clsLayerBound[UPPER][Y] = std::max(phyPin.clsLayerBound[UPPER][Y], pt[Y]);
					} // end for
				} // end for 
			} // end if 
		} //  end for 
		if (phPinPort.getNumPinLayers() > 1) {
			std::sort(phPinPort->clsPinLayers.begin(), phPinPort->clsPinLayers.end(),
				[](PhysicalPinLayer phPinLayer0, PhysicalPinLayer phPinLayer1) {
					Rsyn::PhysicalLayer phLayer0 = phPinLayer0.getLayer();
					Rsyn::PhysicalLayer phLayer1 = phPinLayer1.getLayer();
					return phLayer0.getIndex() < phLayer1.getIndex();
				}); // end sort 
		} // end if 
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalCell(Rsyn::Instance cell, const DefComponentDscp& component) {
	PhysicalLibraryCellData &phLibCell = data->clsPhysicalLibraryCells[cell.asCell().getLibraryCell()]; // TODO: assuming instance is a cell

	PhysicalInstanceData & physicalCell = data->clsPhysicalInstances[cell];
	physicalCell.clsInstance = cell;
	Rsyn::InstanceTag tag = data->clsDesign.getTag(cell);
	tag.setFixed(component.clsIsFixed);
	tag.setMacroBlock(phLibCell.clsMacroClass == Rsyn::MACRO_BLOCK);
	if (component.clsIsFixed) {
		if (phLibCell.clsLayerBoundIndex > -1) {
			PhysicalObstacle obs = phLibCell.clsObs[phLibCell.clsLayerBoundIndex];
			data->clsNumElements[PHYSICAL_FIXEDBOUNDS] += obs->clsBounds.size();
		} else {
			data->clsNumElements[PHYSICAL_FIXEDBOUNDS]++;
		} // end if-else
		data->clsNumElements[PHYSICAL_FIXED]++;
	} // end if 
	physicalCell.clsPlaced = component.clsIsPlaced;
	physicalCell.clsBlock = phLibCell.clsMacroClass == Rsyn::MACRO_BLOCK;
	if (physicalCell.clsBlock)
		data->clsNumElements[PHYSICAL_BLOCK]++;
	if (cell.isMovable())
		data->clsNumElements[PHYSICAL_MOVABLE]++;
	physicalCell.clsHasLayerBounds = phLibCell.clsLayerBoundIndex > -1;
	physicalCell.clsInstance->clsOrientation = getPhysicalOrientation(component.clsOrientation);

	const DBU width = phLibCell.clsSize[X];
	const DBU height = phLibCell.clsSize[Y];

	DBUxy pos = component.clsPos;
	physicalCell.clsInitialPos = pos;
	pos[X] += width;
	pos[Y] += height;
	physicalCell.clsInstance->clsBounds.updatePoints(physicalCell.clsInitialPos, pos);

	DBU area = width * height;
	if (physicalCell.clsBlock)
		data->clsTotalAreas[PHYSICAL_BLOCK] += area;
	if (cell.isFixed()) {
		if (cell.isPort())
			data->clsTotalAreas[PHYSICAL_PORT] += area;
		else
			data->clsTotalAreas[PHYSICAL_FIXED] += area;
	} else {
		data->clsTotalAreas[PHYSICAL_MOVABLE] += area;
	} // end if-else 
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalPort(Rsyn::Instance cell, const DefPortDscp& port) {
	PhysicalInstanceData & physicalGate = data->clsPhysicalInstances[cell];
	physicalGate.clsPortLayer = getPhysicalLayerByName(port.clsLayerName);
	physicalGate.clsInstance = cell;
	physicalGate.clsInstance->clsOrientation = getPhysicalOrientation(port.clsOrientation);
	physicalGate.clsInstance->clsBounds = port.clsLayerBounds;
	physicalGate.clsInstance->clsPortPos = port.clsPos;
	physicalGate.clsPort = true;
	Rsyn::InstanceTag tag = data->clsDesign.getTag(cell);
	// TODO Getting from port descriptor if it is fixed.
	tag.setFixed(true);
	tag.setMacroBlock(false);
} // end method  

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalRow(const DefRowDscp& defRow) {
	PhysicalSite phSite = getPhysicalSiteByName(defRow.clsSite);
	if (!phSite)
		throw Exception("Site " + defRow.clsSite + " was not find for row " + defRow.clsName);

	// Creates a new cell in the data structure.
	PhysicalRowData * phRowData = &(data->clsPhysicalRows.create()->value); // TODO: awful
	phRowData->id = data->clsPhysicalRows.lastId();
	phRowData->clsRowName = defRow.clsName;
	phRowData->clsPhysicalSite = phSite;
	phRowData->clsSiteOrientation = Rsyn::getPhysicalOrientation(defRow.clsOrientation);
	phRowData->clsOrigin = defRow.clsOrigin;
	phRowData->clsStep[X] = phSite.getWidth();
	phRowData->clsStep[Y] = phSite.getHeight();
	phRowData->clsNumSites[X] = defRow.clsNumX;
	phRowData->clsNumSites[Y] = defRow.clsNumY;
	phRowData->clsBounds[LOWER][X] = defRow.clsOrigin[X];
	phRowData->clsBounds[LOWER][Y] = defRow.clsOrigin[Y];
	phRowData->clsBounds[UPPER][X] = defRow.clsOrigin[X] + phRowData->getWidth();
	phRowData->clsBounds[UPPER][Y] = defRow.clsOrigin[Y] + phRowData->getHeight();
	Rsyn::PhysicalModule phModule = getPhysicalModule(data->clsModule);
	Bounds & bounds = phModule->clsInstance->clsBounds;
	bounds[LOWER] = min(bounds[LOWER], phRowData->clsBounds[LOWER]);
	bounds[UPPER] = max(bounds[UPPER], phRowData->clsBounds[UPPER]);
	data->clsTotalAreas[PHYSICAL_PLACEABLE] += phRowData->clsBounds.computeArea();
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalRegion(const DefRegionDscp& defRegion) {
	data->clsPhysicalRegions.push_back(PhysicalRegion(new PhysicalRegionData()));
	Rsyn::PhysicalRegion phRegion = data->clsPhysicalRegions.back();
	phRegion->id = data->clsPhysicalRegions.size() - 1;
	phRegion->clsName = defRegion.clsName;
	phRegion->clsType = Rsyn::getPhysicalRegionType(defRegion.clsType);
	phRegion->clsBounds = defRegion.clsBounds;
	data->clsMapPhysicalRegions[defRegion.clsName] = phRegion->id;
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalGroup(const DefGroupDscp& defGroup) {
	data->clsPhysicalGroups.push_back(PhysicalGroup(new PhysicalGroupData()));
	Rsyn::PhysicalGroup phGroup = data->clsPhysicalGroups.back();
	phGroup->id = data->clsPhysicalGroups.size() - 1;
	phGroup->clsName = defGroup.clsName;
	phGroup->clsPatterns = defGroup.clsPatterns;
	phGroup->clsRegion = getPhysicalRegionByName(defGroup.clsRegion);
	data->clsMapPhysicalGroups[defGroup.clsName] = phGroup->id;
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalNet(const DefNetDscp & netDscp) {
	Rsyn::Net net = data->clsDesign.findNetByName(netDscp.clsName);
	PhysicalNetData & netData = data->clsPhysicalNets[net];
	Rsyn::PhysicalRouting & routing = netData.clsRouting;
	netData.clsNet = net;
	for (const DefWireDscp & wireDscp : netDscp.clsWires) {
		for (const DefWireSegmentDscp & segmentDscp : wireDscp.clsWireSegments) {
			Rsyn::PhysicalLayer physicalLayer = getPhysicalLayerByName(segmentDscp.clsLayerName);

			int numPoints = segmentDscp.clsRoutingPoints.size();
			Rsyn::PhysicalRoutingWire wire;
			for (const DefRoutingPointDscp & point : segmentDscp.clsRoutingPoints) {
				if (point.clsHasRectangle) {
					Bounds bds = point.clsRect;
					bds.translate(point.clsPos);
					routing.addRect(physicalLayer, bds);
				} else if (point.clsHasVia) {
					DBUxy pos = segmentDscp.clsRoutingPoints.back().clsPos;
					Rsyn::PhysicalVia physicalVia = getPhysicalViaByName(point.clsViaName);
					routing.addVia(physicalVia, pos);
				} // end if-else 
				if (numPoints > 1) {
					wire.addRoutingPoint(point.clsPos);
				} // end if 
			} // end for 
			// it is a wire 
			if (numPoints > 1) {
				wire.setLayer(physicalLayer);
				const DefRoutingPointDscp & source = segmentDscp.clsRoutingPoints.front();
				const DefRoutingPointDscp & target = segmentDscp.clsRoutingPoints.back();
				if (source.clsHasExtension)
					wire.setSourceExtension(source.clsExtension);
				if (target.clsHasExtension)
					wire.setTargetExtension(target.clsExtension);
				if (segmentDscp.clsRoutedWidth > 0) {
					wire.setWidth(segmentDscp.clsRoutedWidth);
				} // end if
				routing.addWire(wire);
			} // end if 
		} // end for
	} // end for
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalSpecialNet(const DefSpecialNetDscp & specialNet) {
	std::cout << "TODO: Handle special nets.\n";
	//	data->clsPhysicalSpecialNets.push_back(PhysicalSpecialNet(new PhysicalSpecialNetData()));
	//	Rsyn::PhysicalSpecialNet phSpecialNet = data->clsPhysicalSpecialNets.back();
	//	phSpecialNet->id = data->clsPhysicalSpecialNets.size() - 1;
	//	data->clsMapPhysicalSpecialNets[specialNet.clsName] = data->clsPhysicalSpecialNets.size() - 1;
	//	for (const DefWireDscp & wire : specialNet.clsWires) {
	//		phSpecialNet->clsWires.push_back(PhysicalWire(new PhysicalWireData()));
	//		PhysicalWire phWire = phSpecialNet->clsWires.back();
	//		addSpecialWireNet(wire, phWire, true);
	//	} // end for
} // end method 

//// -----------------------------------------------------------------------------
//
//void PhysicalDesign::addSpecialWireNet(const DefWireDscp & wire, PhysicalWire phWire, const bool isSpecialNet) {
//	phWire->clsWireSegments.reserve(wire.clsWireSegments.size());
//	for (const DefWireSegmentDscp & segmentDscp : wire.clsWireSegments) {
//		phWire->clsWireSegments.push_back(PhysicalWireSegment(new PhysicalWireSegmentData()));
//		PhysicalWireSegment phWireSegment = phWire->clsWireSegments.back();
//		addWireSegment(segmentDscp, phWireSegment, isSpecialNet);
//	} // end for
//} // end method
//
//// -----------------------------------------------------------------------------
//
//void PhysicalDesign::addWireSegment(const DefWireSegmentDscp & segmentDscp, PhysicalWireSegment phWireSegment, const bool isSpecialNet) {
//	phWireSegment->clsPhysicalLayer = getPhysicalLayerByName(segmentDscp.clsLayerName);
//	phWireSegment->clsRoutingPoints.reserve(segmentDscp.clsRoutingPoints.size());
//	if (isSpecialNet)
//		phWireSegment->clsRoutedWidth = segmentDscp.clsRoutedWidth;
//
//	for (const DefRoutingPointDscp & routingPoint : segmentDscp.clsRoutingPoints) {
//		phWireSegment->clsRoutingPoints.push_back(PhysicalRoutingPoint(new PhysicalRoutingPointData()));
//		PhysicalRoutingPoint phPoint = phWireSegment->clsRoutingPoints.back();
//		phPoint->clsExtension = routingPoint.clsExtension;
//		phPoint->clsPos = routingPoint.clsPos;
//		phPoint->clsOrientation = getPhysicalOrientation(routingPoint.clsOrientation);
//		if (routingPoint.clsHasVia)
//			phPoint->clsVia = getPhysicalViaByName(routingPoint.clsViaName);
//
//		if (routingPoint.clsHasRectangle) {
//			phPoint->clsRectangle = routingPoint.clsRect;
//			phPoint->clsHasRectangle = true;
//		} // end if
//	} // end for
//
//	postProcessWireSegment(phWireSegment);
//} // end method

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalTracks(const DefTrackDscp &track) {
	data->clsPhysicalTracks.push_back(PhysicalTracks(new PhysicalTracksData()));
	Rsyn::PhysicalTracks phTrack = data->clsPhysicalTracks.back();
	phTrack->id = data->clsPhysicalTracks.size() - 1;
	phTrack->clsDirection = Rsyn::getPhysicalTrackDirectionDEF(track.clsDirection);
	phTrack->clsLocation = track.clsLocation;
	phTrack->clsNumTracks = track.clsNumTracks;
	phTrack->clsSpace = track.clsSpace;
	phTrack->clsLayers.reserve(track.clsLayers.size());
	for (const std::string & layerName : track.clsLayers) {
		Rsyn::PhysicalLayer phLayer = getPhysicalLayerByName(layerName);
		if (phLayer) {
			phTrack->clsLayers.push_back(phLayer);
			std::vector<Rsyn::PhysicalTracks> & trackLayer = data->clsMapLayerToTracks[phLayer];
			trackLayer.push_back(phTrack);
		} // end if
	} // end for
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::initRoutingGrid() {
	for (Rsyn::PhysicalTracks track : allPhysicalTracks()) {
		for (Rsyn::PhysicalLayer phLayer : track.allLayers()) {
			if (data->clsMapLayerToRoutingGrid.find(phLayer) == data->clsMapLayerToRoutingGrid.end()) {
				data->clsPhysicalRoutingGrids.push_back(PhysicalRoutingGrid(new PhysicalRoutingGridData()));
				data->clsMapLayerToRoutingGrid[phLayer] = data->clsPhysicalRoutingGrids.back();
			} // end if 
			Rsyn::PhysicalRoutingGrid routingGrid = data->clsMapLayerToRoutingGrid[phLayer];
			routingGrid->clsLayer = phLayer;
			routingGrid->clsTracks.push_back(track);
		} // end for 
	} // end for 

	// to use std::sort algorithm, the operator int() const in Proxy must be public. Otherwise, there is a compiler error
	data->clsPhysicalRoutingGrids.clear();
	for (Rsyn::PhysicalLayer layer : allPhysicalLayers()) {
		if (!hasPhysicalRoutingGrid(layer))
			continue;
		PhysicalRoutingGrid routing = data->clsMapLayerToRoutingGrid[layer];
		data->clsPhysicalRoutingGrids.push_back(routing);
	} // end for 

	Rsyn::PhysicalRoutingGrid bottom;
	for (Rsyn::PhysicalRoutingGrid routing : allPhysicalRoutingGrids()) {
		for (Rsyn::PhysicalTracks track : routing.allTracks()) {
			if (track.getDirection() == Rsyn::TRACK_HORIZONTAL) {
				routing->clsBounds[LOWER][Y] = track->clsLocation;
				routing->clsNumTracks[Y] = track->clsNumTracks;
				routing->clsSpacing[Y] = track->clsSpace;
			} else if (track.getDirection() == Rsyn::TRACK_VERTICAL) {
				routing->clsBounds[LOWER][X] = track->clsLocation;
				routing->clsNumTracks[X] = track->clsNumTracks;
				routing->clsSpacing[X] = track->clsSpace;
			} else {
				std::cout << "ERROR: Invalid track direction " << track.getDirection() << "\n";
			} // end if-else 
		} // end for 
		routing->clsBounds[UPPER][X] = routing->clsBounds[LOWER][X] +
			(routing->clsSpacing[X] * (routing->clsNumTracks[X] - 1));
		routing->clsBounds[UPPER][Y] = routing->clsBounds[LOWER][Y] +
			(routing->clsSpacing[Y] * (routing->clsNumTracks[Y] - 1));
		routing->clsBottomRoutingGrid = bottom;
		if (bottom != nullptr)
			bottom->clsTopRoutingGrid = routing;
		bottom = routing;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalDesignVia(const DefViaDscp & via) {
	data->clsPhysicalVias.push_back(PhysicalVia(new PhysicalViaData()));
	Rsyn::PhysicalVia phVia = data->clsPhysicalVias.back();
	data->clsMapPhysicalVias[via.clsName] = data->clsPhysicalVias.size() - 1;
	phVia->id = data->clsPhysicalVias.size() - 1;
	phVia->clsDesignVia = true;
	phVia->clsName = via.clsName;

	std::vector<std::tuple<int, PhysicalViaLayerData *>> layers;
	for (const DefViaLayerDscp & layerDscp : via.clsViaLayers) {
		PhysicalViaLayerData * phLayer = new PhysicalViaLayerData();
		phLayer->clsPhVia = phVia;
		phLayer->clsBounds.push_back(layerDscp.clsBounds);
		phLayer->clsLayer = getPhysicalLayerByName(layerDscp.clsLayerName);
		assert(phLayer->clsLayer);
		layers.push_back(std::make_tuple(phLayer->clsLayer.getIndex(), phLayer));
	} // end for
	std::sort(layers.begin(), layers.end());
	//	assert(layers.size() == NUM_VIA_LAYERS);
	for (int i = 0; i < NUM_VIA_LAYERS; i++) {
		phVia->clsViaLayers[i] = std::get<1>(layers[i]);
	} // end if
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::addPhysicalSpacing(const LefSpacingDscp & spacing) {
	Element<PhysicalSpacingData> *element = data->clsPhysicalSpacing.create();
	Rsyn::PhysicalSpacingData * phSpacing = &(element->value);
	phSpacing->id = data->clsPhysicalSpacing.lastId();
	phSpacing->clsLayer1 = getPhysicalLayerByName(spacing.clsLayer1);
	phSpacing->clsLayer2 = getPhysicalLayerByName(spacing.clsLayer2);
	phSpacing->clsDistance = static_cast<DBU> (std::round(spacing.clsDistance * getDatabaseUnits(LIBRARY_DBU)));
} // end method 

// -----------------------------------------------------------------------------

Rsyn::PhysicalLayer
PhysicalDesign::getPhysicalLayerByIndex(const int index) {
	const int numLayers = data->clsPhysicalLayers.size();
	if (index < 0 || index >= numLayers)
		return PhysicalLayer(nullptr);
	Element<PhysicalLayerData> * phLayerDataElement = data->clsPhysicalLayers.get(index);
	return PhysicalLayer(&phLayerDataElement->value);
} // end method 

// -----------------------------------------------------------------------------

Rsyn::PhysicalLayer
PhysicalDesign::getPhysicalLayerByIndex(const Rsyn::PhysicalLayerType layerType,
	const int index) {
	if (index < 0)
		return PhysicalLayer(nullptr);
	int numLayers;
	int layerId;
	Element<PhysicalLayerData> * phLayerDataElement;
	switch (layerType) {
		case PhysicalLayerType::ROUTING:
			numLayers = data->clsPhysicalRoutingLayerIndeces.size();
			if (index >= numLayers)
				return PhysicalLayer(nullptr);
			layerId = data->clsPhysicalRoutingLayerIndeces[index];
			phLayerDataElement = data->clsPhysicalLayers.get(layerId);
			return PhysicalLayer(&phLayerDataElement->value);
			break;
		default:
			std::cout << "TODO " << layerType << "\n";
			return PhysicalLayer(nullptr);
	} // end switch 

} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::updatePhysicalCell(Rsyn::Cell cell) {
	PhysicalInstanceData & phCell = data->clsPhysicalInstances[cell.asCell()];
	PhysicalLibraryCellData &phLibCell = data->clsPhysicalLibraryCells[cell.asCell().getLibraryCell()]; // TODO: assuming instance is a cell

	const DBU width = phLibCell.clsSize[X];
	const DBU height = phLibCell.clsSize[Y];
	const DBUxy pos = phCell.clsInstance->clsBounds[LOWER];

	DBU area = (width * height) - phCell.clsInstance->clsBounds.computeArea();
	if (phLibCell.clsMacroClass == Rsyn::MACRO_BLOCK)
		data->clsTotalAreas[PHYSICAL_BLOCK] += area;
	if (cell.isFixed()) {
		if (cell.isPort())
			data->clsTotalAreas[PHYSICAL_PORT] += area;
		else
			data->clsTotalAreas[PHYSICAL_FIXED] += area;
	} else {
		data->clsTotalAreas[PHYSICAL_MOVABLE] += area;
	} // end if-else 

	phCell.clsInstance->clsBounds.updatePoints(pos, DBUxy(pos[X] + width, pos[Y] + height));
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::removePhysicalCell(Rsyn::Cell cell) {
	PhysicalInstanceData & physicalCell = data->clsPhysicalInstances[cell.asCell()];
	DBU area = physicalCell.clsInstance->clsBounds.computeArea();
	if (physicalCell.clsBlock)
		data->clsTotalAreas[PHYSICAL_BLOCK] -= area;
	if (cell.isFixed()) {
		if (cell.isPort())
			data->clsTotalAreas[PHYSICAL_PORT] -= area;
		else
			data->clsTotalAreas[PHYSICAL_FIXED] -= area;
	} else {
		data->clsTotalAreas[PHYSICAL_MOVABLE] -= area;
	} // end if-else 
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::mergeBounds(const std::vector<Bounds> & source,
	std::vector<Bounds> & target, const Dimension dim) {

	target.reserve(source.size());
	std::set<DBU> stripes;
	const Dimension reverse = REVERSE_DIMENSION[dim];
	DBUxy lower, upper;
	lower[reverse] = +std::numeric_limits<DBU>::max();
	upper[reverse] = -std::numeric_limits<DBU>::max();

	for (const Bounds & bound : source) {
		stripes.insert(bound[LOWER][dim]);
		stripes.insert(bound[UPPER][dim]);
		lower[reverse] = std::min(bound[LOWER][reverse], lower[reverse]);
		upper[reverse] = std::max(bound[UPPER][reverse], upper[reverse]);
	} // end for 

	lower[dim] = *stripes.begin();
	stripes.erase(0);
	for (DBU val : stripes) {
		upper[dim] = val;
		Bounds stripe(lower, upper);
		DBU low, upp;
		bool firstMatch = true;
		for (const Bounds & rect : source) {
			if (!rect.overlap(stripe)) {
				if (!firstMatch) {
					Bounds merged = stripe;
					merged[LOWER][reverse] = low;
					merged[UPPER][reverse] = upp;
					target.push_back(merged);
					firstMatch = true;
				} // end if 
				continue;
			} // end if 
			if (firstMatch) {
				low = rect[LOWER][reverse];
				upp = rect[UPPER][reverse];
				firstMatch = false;
			} else {
				if (upp == rect[LOWER][reverse]) {
					upp = rect[UPPER][reverse];
				} // end if 
			} // end if-else 
		} // end for 
		if (!firstMatch) {
			Bounds merged = stripe;
			merged[LOWER][reverse] = low;
			merged[UPPER][reverse] = upp;
			target.push_back(merged);
			firstMatch = true;
		} // end if 
		lower[dim] = val;
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

void PhysicalDesign::initLayerViaManager() {
	for (PhysicalVia via : allPhysicalVias()) {
		Rsyn::PhysicalLayer bottom = via.getBottomLayer().getLayer();
		Rsyn::PhysicalLayer top = via.getTopLayer().getLayer();

		std::vector<PhysicalVia> &bottomAll = data->clsLayerViaManager.clsVias[bottom];
		std::vector<PhysicalVia> &bottomTop = data->clsLayerViaManager.clsTopVias[bottom];
		bottomAll.push_back(via);
		bottomTop.push_back(via);

		std::vector<PhysicalVia> &topAll = data->clsLayerViaManager.clsVias[top];
		std::vector<PhysicalVia> &topBottom = data->clsLayerViaManager.clsBottomVias[top];
		topAll.push_back(via);
		topBottom.push_back(via);
	} // end for 
} // end method 

// -----------------------------------------------------------------------------

} // end namespace 