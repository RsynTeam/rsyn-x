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

#ifndef ROUTING_ESTIMATOR_MODEL_H
#define ROUTING_ESTIMATOR_MODEL_H

#include "util/DBU.h"

namespace Rsyn {

template <class NameType>
class RoutingTopologyDescriptor;

class RoutingEstimationModel {
       private:
       public:
        virtual void updateRoutingEstimation(
            Rsyn::Net net, Rsyn::RoutingTopologyDescriptor<int> &topology,
            DBU &wirelength) = 0;

};  // end class

}  // end namespace

#endif /* ROUTINGESTIMATORMODEL_H */
