/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   regressionsService.cpp
 * Author: isoliveira
 * 
 * Created on April 11, 2019, 11:54 AM
 */

#include "regressionsService.h"

void regressionsService::start(const Rsyn::Json &params){
    clsDesign = clsSession.getDesign();
    clsModule = clsDesign.getTopModule();
    clsPhDesign = clsSession.getPhysicalDesign();
    
                
            {
                ScriptParsing::CommandDescriptor dscp;
		dscp.setName("regressionsInstances");
		dscp.setDescription("Run regression tests on instances, nets, ports and pins.");
                
                clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			getAllInstances();
                        getAllNets();
                        getAllPorts();
                        getNumPins();
		}); // end command 
            }
    
            {
                ScriptParsing::CommandDescriptor dscp;
		dscp.setName("regressionsPhysicalInfo");
		dscp.setDescription("Run regression tests on Physical Information");
                
                clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			getPhysicalInfo();
                        getSitesPerRow();
		}); // end command 
            }
            
            {
                ScriptParsing::CommandDescriptor dscp;
		dscp.setName("regressionsRouting");
		dscp.setDescription("Run regression tests on routing");
                
                clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			getAllWires();
                        getAllVias();
                        getAllRects();
		}); // end command 
            }

            {
                ScriptParsing::CommandDescriptor dscp;
		dscp.setName("regressionsPG");
		dscp.setDescription("Run regression tests on power ground");
                
                clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			getAllWiresPG();
                        getAllViasPG();
                        getAllRectsPG();
		}); // end command 
            }
            
             {
                ScriptParsing::CommandDescriptor dscp;
		dscp.setName("regressionsCoordinates");
		dscp.setDescription("Run regression tests on instances coordinates");
                
                clsSession.registerCommand(dscp, [&](const ScriptParsing::Command & command) {
			getInstancesCoordinates();
		}); // end command 
            }

}

void regressionsService::stop(){
}

/////////////////////////////////////////////
///     Instances, Nets, Pins, Ports     ///
///////////////////////////////////////////


void regressionsService::getAllInstances(){
    for (Rsyn::Instance inst : clsModule.allInstances()){
        if (inst.getType() != Rsyn::PORT)
            std::cout << "#InstName: "<< inst.getName() << std::endl;
    }
}

void regressionsService::getAllNets(){
    for (Rsyn::Net net : clsModule.allNets()){
        std::cout << "#NetName: " << net.getName() << std::endl;
    }
}

void regressionsService::getNumPins(){
    long nPins = 0;
    for (Rsyn::Instance inst : clsModule.allInstances()){
        for (int i=0; i < inst.getNumPins(); i++){
            nPins++;
        }
    }
    std::cout << "#NumberOfPins: "<< nPins << std::endl;
}

void regressionsService::getAllPorts(){
    for (Rsyn::Port port : clsModule.allPorts()){
        std::cout << "#PortName: "<< port.getName() << std::endl;
    }
}

/////////////////////////////////////////////
///            Physical Info             ///
///////////////////////////////////////////

void regressionsService::getPhysicalInfo(){
    std::cout << "#FloorplanArea: " << clsPhDesign.getPhysicalDie().getArea() << std::endl;
    std::cout << "#NumRows: " << clsPhDesign.getNumRows() << std::endl;
    std::cout << "#NumPhysicalVias: " << clsPhDesign.getNumPhysicalVias() << std::endl;
    std::cout << "#NumLayers: " << clsPhDesign.getNumLayers() << std::endl;
    std::cout << "#RowHeight: "<< clsPhDesign.getRowHeight() << std::endl;
    std::cout << "#RowSite: " << clsPhDesign.getRowSiteWidth() << std::endl;
}

void regressionsService::getSitesPerRow(){
    
    for (Rsyn::PhysicalRow row : clsPhDesign.allPhysicalRows()){
        std::cout << "#SitesPerRow: " << row.getBounds() << " " << row.getNumSites(X) << std::endl;
    }
}


/////////////////////////////////////////////
///               Routing                ///
///////////////////////////////////////////

void regressionsService::getAllWires(){
    for (Rsyn::Net net : clsModule.allNets()){
        if (net.getUse() == Rsyn::POWER || net.getUse() == Rsyn::GROUND)
            continue;
        Rsyn::PhysicalRouting phRouting = clsPhDesign.getNetRouting(net);
        int nWires = phRouting.allWires().size();
        std::cout << "#NetWires: " << net.getName() << " " << nWires << std::endl;    
    }
}

void regressionsService::getAllVias(){
    for (Rsyn::Net net : clsModule.allNets()){
        if (net.getUse() == Rsyn::POWER || net.getUse() == Rsyn::GROUND)
            continue;
        Rsyn::PhysicalRouting phRouting = clsPhDesign.getNetRouting(net);
        int nVias = phRouting.allVias().size();
        std::cout << "#NetVias: " << net.getName() << " " << nVias << std::endl;    
    }
}

void regressionsService::getAllRects(){
    for (Rsyn::Net net : clsModule.allNets()){
        if (net.getUse() == Rsyn::POWER || net.getUse() == Rsyn::GROUND)
            continue;
        Rsyn::PhysicalRouting phRouting = clsPhDesign.getNetRouting(net);
        int nRects = phRouting.allRects().size();
        std::cout << "#NetRects: " << net.getName() << " " << nRects << std::endl;    
    }
}

/////////////////////////////////////////////
///            Power Ground              ///
///////////////////////////////////////////

void regressionsService::getAllWiresPG(){
    
    for (Rsyn::Net net : clsModule.allNets()){
        if (net.getUse() != Rsyn::POWER && net.getUse() != Rsyn::GROUND)
            continue;
        Rsyn::PhysicalRouting phRouting = clsPhDesign.getNetRouting((Rsyn::Net)net);
        int nWires = phRouting.allWires().size();
        std::cout << "#PGNetWires: " << nWires << std::endl;    
    }
}

void regressionsService::getAllViasPG(){
    
    for (Rsyn::Net net : clsModule.allNets()){
        if (net.getUse() != Rsyn::POWER && net.getUse() != Rsyn::GROUND)
            continue;
        Rsyn::PhysicalRouting phRouting = clsPhDesign.getNetRouting(net);
        int nVias = phRouting.allVias().size();
        std::cout << "#PGNetVias: " << nVias << std::endl;    
    }
}

void regressionsService::getAllRectsPG(){
    
    for (Rsyn::Net net : clsModule.allNets()){
        if (net.getUse() != Rsyn::POWER && net.getUse() != Rsyn::GROUND)
            continue;
        Rsyn::PhysicalRouting phRouting = clsPhDesign.getNetRouting(net);
        int nRects = phRouting.allRects().size();
        std::cout << "#PGNetRects: " << nRects << std::endl;    
    }
}


/////////////////////////////////////////////
///            Coordinates               ///
///////////////////////////////////////////


void regressionsService::getInstancesCoordinates(){
    for (Rsyn::Instance inst : clsModule.allInstances()){
        if (inst.getType() != Rsyn::PORT)
            std::cout << "#InstCoordinate: "<< inst.getName() << " " << inst.getBounds() << std::endl;
    }
}

