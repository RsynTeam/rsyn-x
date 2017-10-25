/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SPEFRoutingEstimationModel.h
 * Author: Henrique
 *
 * Created on 22 de Setembro de 2017
 */

#ifndef RCTREEEXTRACTOR_H
#define RCTREEEXTRACTOR_H

#include "rsyn/core/Rsyn.h"
#include "rsyn/engine/Engine.h"
#include "rsyn/model/scenario/Scenario.h"
#include "rsyn/model/routing/RCTree.h"
#include "rsyn/model/routing/RoutingTopology.h"
#include "rsyn/io/parser/spef/SPEFControlParser.h"
#include "rsyn/io/parser/parser_helper.h"

namespace Rsyn {
    
class RCTreeExtractor {
    
private:
    
    Rsyn::Engine clsEngine;
    Rsyn::Scenario * clsScenario;
    ISPD13::SPEFInfo *spefInfo;
    
    std::unordered_map<std::string, int> clsNodeMap;
    std::unordered_map<std::string, Rsyn::Pin> clsPinMap;
    
    Rsyn::UnitPrefix unitPrefixForResistance;
    Rsyn::UnitPrefix unitPrefixForCapacitance;
    
    std::string getPinNameInSPEFFormat(Rsyn::Pin pin);
    void mapSPEFPinNameToRsynPin(Rsyn::Net net);
    int getNodeIndex(const std::string &nodeName);
    void addDefaultCapacitance(int node, RCTreeDescriptor &dscp, 
                               bool useDefaultCapacitance);
    void setLoadCapacitances(Rsyn::RCTree &tree);
    
public:
    
    enum SPEFNetModel {
        LUMPED_CAPACITANCE_NET_MODEL = 0,
        RC_TREE_NET_MODEL = 1
    };
    
    RCTreeExtractor() = default;
    RCTreeExtractor(Engine engine, ISPD13::SPEFInfo *info);
    ~RCTreeExtractor() = default;
    void extractRCTreeFromSPEF(SPEFNetModel netModel, Rsyn::Net net, Rsyn::RCTree &tree);
    
    
};
    
} // end namespace 

#endif