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

#ifndef RSYN_DEFAULT_ROUTING_EXTRACTION_MODEL_H
#define RSYN_DEFAULT_ROUTING_EXTRACTION_MODEL_H

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Service.h"
#include "session/Session.h"
#include "tool/scenario/Scenario.h"
#include "RoutingExtractionModel.h"

namespace Rsyn {
template <class NameType>
class RoutingTopologyDescriptor;
}

namespace Rsyn {

class DefaultRoutingExtractionModel : public RoutingExtractionModel,
                                      public Service {
       public:
        virtual void start(const Rsyn::Json &params) override;
        virtual void stop() override;

        void initialize(const Number wireResistancePerUnitLength,
                        const Number wireCapacitancePerUnitLength,
                        const DBU maxWireSegmentLength);

       private:
        // Design.
        Rsyn::Design design;
        Rsyn::Module module;  // top module
        Rsyn::PhysicalDesign clsPhysicalDesign;

        // Services
        Scenario *clsScenario = nullptr;

        // Config
        static const bool ENABLE_LONG_WIRE_SLICING;

        Number LOCAL_WIRE_RES_PER_UNIT_LENGTH;  // kOhm / dbu
        Number LOCAL_WIRE_CAP_PER_UNIT_LENGTH;  // fFarad / dbu
        DBU MAX_WIRE_SEGMENT_LENGTH;            // dbu

        // Slice long wires.
        struct SlicingPoint {
                SlicingPoint() : id(0), x(0), y(0) {}
                SlicingPoint(const int id, const DBU x, const DBU y)
                    : id(id), x(x), y(y) {}

                int id;
                DBU x;
                DBU y;
        };  // end struct

        int generateSteinerTree_SliceLongWire(
            const DBU longWirelengthThreshold, const Number wireCapPerMicron,
            const Number wireResPerMicron, const int index_i, const int index_j,
            const DBU xi, const DBU yi, const DBU xj, const DBU yj,
            std::deque<SlicingPoint> &slicingPoints, RCTreeDescriptor &dscp,
            int &sliceId) const;

       public:
        DefaultRoutingExtractionModel() {}

        virtual void extract(const RoutingTopologyDescriptor<int> &topology,
                             RCTree &tree) override;
        virtual void updateDownstreamCap(Rsyn::RCTree &tree) override;

        virtual Number getLocalWireResPerUnitLength() const override {
                return LOCAL_WIRE_RES_PER_UNIT_LENGTH;
        }
        virtual Number getLocalWireCapPerUnitLength() const override {
                return LOCAL_WIRE_CAP_PER_UNIT_LENGTH;
        }

};  // end class

}  // end namespace

#endif
