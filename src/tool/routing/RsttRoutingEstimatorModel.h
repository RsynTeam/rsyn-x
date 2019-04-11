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

/*
 * File:   RsttRoutingEstimatorModel.h
 * Author: eder
 *
 * Created on 29 de Junho de 2017, 16:44
 */

#ifndef RSTTROUTINGESTIMATORMODEL_H
#define RSTTROUTINGESTIMATORMODEL_H
#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "phy/PhysicalDesign.h"
#include "session/Service.h"
#include "session/Session.h"
#include "tool/scenario/Scenario.h"
#include "RoutingEstimationModel.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

namespace Rsyn {

class RsttRoutingEstimatorModel : public RoutingEstimationModel,
                                  public Service {
       protected:
        Rsyn::Attribute<Rsyn::Net, Rsyn::RoutingTopologyDescriptor<int>>
            clsNetsTopologys;

       public:
        typedef boost::geometry::model::d2::point_xy<DBU> Point;
        typedef boost::geometry::model::segment<Point> Segment;

        void start(const Rsyn::Json &params);

        void stop() {}
        RsttRoutingEstimatorModel();
        void generateRsttTopologyForAllNets();
        void updateRoutingEstimation(Net net,
                                     RoutingTopologyDescriptor<int> &topology,
                                     DBU &wirelength) override;
        DBU generateSteinerTree(Net net,
                                RoutingTopologyDescriptor<int> &topology);
        double distance();
        DBU returnNetWirelength(Net net);
        const Rsyn::RoutingTopologyDescriptor<int> &getTree(Net net) const;

       private:
        const bool X = 0;
        const bool Y = 1;
        const int VERTICAL = 1;
        const int HORIZONTAL = 0;
        const int ASC = 0;
        const int DESC = 1;
        const int UP = 0;
        const int LOW = 1;
        const int LEFT = 0;
        const int RIGHT = 1;

        // Design.
        Design design;
        Module module;  // top module
        PhysicalDesign clsPhysicalDesign;

        // Services
        Scenario *clsScenario = nullptr;

        void reportWirelengthNets();

        void reportNumNets();

        Point returnMidPoint(std::vector<Point> P,
                             bool XorY);  // ---check--- //

        Point returnMinPoint(std::vector<Point> const &P);  // ---check--- //

        Point returnMaxPoint(std::vector<Point> const &P);  // ---check--- //

        void orderPoints(std::vector<Point> &myvector, bool AscOrDesc,
                         bool XorY);  // ---check--- //

        std::vector<Point> returnUpperOrLower(std::vector<Point> const &P,
                                              int Ymid,
                                              bool UorL);  // ---check--- //

        std::vector<Point> returnLeftOrRight(std::vector<Point> const &P,
                                             int Xmid,
                                             bool LorR);  // ---check--- //

        void connectPinV(Point Pin, std::vector<Segment> &trunk,
                         std::vector<Segment> &stems,
                         std::vector<Segment> &SegIni, int firstCon, bool UorL);

        void connectPinH(Point Pin, std::vector<Segment> &trunk,
                         std::vector<Segment> &stems,
                         std::vector<Segment> &SegIni, int firstCon, bool LorR);

        DBU returnWireLength(std::vector<Segment> const &stems);

        std::vector<Segment> returnHorizontalSteinerTree(
            std::vector<Point> Pins, std::vector<Segment> &Trunk);

        std::vector<Segment> returnVerticalSteinerTree(
            std::vector<Point> Pins, std::vector<Segment> &Trunk);

        void orderSegments(std::vector<Segment> &stems, bool XorY);

        void correctTrunk(std::vector<Segment> &trunk,
                          std::vector<Segment> &stems, std::vector<Point> &Pins,
                          Point med, bool HorV);

        void createNetsFile();

        int findPoint(std::vector<Point> const &points, Point const &point);

        int findSegment(const std::vector<Segment> &segments,
                        const Segment &segment);

        void removeLoopSegments(std::vector<Segment> &tree);

        void createTopology(std::vector<Segment> const &tree,
                            std::vector<Point> const &Pins,
                            std::vector<Pin> const &RsynPins,
                            RoutingTopologyDescriptor<int> &topology);
};
}

#endif /* RSTTROUTINGESTIMATORMODEL_H */
