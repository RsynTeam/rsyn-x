/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RsttRoutingEstimatorModel.h
 * Author: eder
 *
 * Created on 29 de Junho de 2017, 16:44
 */

#ifndef RSTTROUTINGESTIMATORMODEL_H
#define RSTTROUTINGESTIMATORMODEL_H
#include "rsyn/core/Rsyn.h"
#include "rsyn/phy/PhysicalService.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/engine/Service.h"
#include "rsyn/engine/Engine.h"
#include "rsyn/model/scenario/Scenario.h"
#include "rsyn/model/routing/RoutingEstimationModel.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

#define ASC 0
#define DESC 1
#define UP 0
#define LOW 1
#define LEFT 0
#define RIGHT 1
#define VERTICAL 1
#define HORIZONTAL 0

namespace Rsyn {

class RsttRoutingEstimatorModel : public RoutingEstimationModel, public Service {
protected:
    Rsyn::Attribute<Rsyn::Net, Rsyn::RoutingTopologyDescriptor<int>> clsNetsTopologys;  
    
public:
    typedef boost::geometry::model::d2::point_xy<DBU> Point;
    typedef boost::geometry::model::segment<Point> Segment; 
    
    void start(Engine engine, const Json &params);

    void stop() {
    }
    RsttRoutingEstimatorModel();
    void generateRsttTopologyForAllNets();
    void updateRoutingEstimation(Net net, RoutingTopologyDescriptor<int>& topology, DBU &wirelength) override;
    DBU generateSteinerTree(Net net, RoutingTopologyDescriptor<int> &topology);
    double distance();
    DBU returnNetWirelength(Net net);
    const Rsyn::RoutingTopologyDescriptor<int> &getTree(Net net) const;

private: 
    const bool X = 0;
    const bool Y = 1;

    // Engine.
    Engine clsEngine;

    // Design.
    Design design;
    Module module; // top module
    PhysicalDesign clsPhysicalDesign;

    // Services
    Scenario * clsScenario = nullptr;
    PhysicalService * clsPhysical = nullptr;

    void reportWirelengthNets();

    void reportNumNets();

    Point returnMidPoint(std::vector<Point> P, bool XorY); // ---check--- //

    Point returnMinPoint(std::vector<Point> const &P); // ---check--- //

    Point returnMaxPoint(std::vector<Point> const &P); // ---check--- //

    void orderPoints(std::vector<Point> &myvector, bool AscOrDesc, bool XorY); // ---check--- //

    std::vector<Point> returnUpperOrLower(std::vector<Point> const &P, int Ymid, bool UorL); // ---check--- //

    std::vector<Point> returnLeftOrRight(std::vector<Point> const &P, int Xmid, bool LorR); // ---check--- //

    void connectPinV(Point Pin, std::vector<Segment> &trunk, std::vector<Segment> &stems, std::vector<Segment> &SegIni, int firstCon, bool UorL);

    void connectPinH(Point Pin, std::vector<Segment> &trunk, std::vector<Segment> &stems, std::vector<Segment> &SegIni, int firstCon, bool LorR);

    DBU returnWireLength(std::vector<Segment> const &stems);

    std::vector<Segment> returnHorizontalSteinerTree(std::vector<Point> Pins, std::vector<Segment> &Trunk);

    std::vector<Segment> returnVerticalSteinerTree(std::vector<Point> Pins, std::vector<Segment> &Trunk);

    void orderSegments(std::vector<Segment> &stems, bool XorY);

    void correctTrunk(std::vector<Segment> &trunk, std::vector<Segment> &stems, std::vector<Point> &Pins, Point med, bool HorV);

    void createNetsFile();

    int findPoint(std::vector<Point> const &points, Point const &point);

    int findSegment(const std::vector<Segment> &segments, const Segment &segment);

    void createTopology(std::vector<Segment> const &tree, std::vector<Point> const &Pins, std::vector<Pin> const &RsynPins, RoutingTopologyDescriptor<int>& topology);
};

}

#endif /* RSTTROUTINGESTIMATORMODEL_H */

