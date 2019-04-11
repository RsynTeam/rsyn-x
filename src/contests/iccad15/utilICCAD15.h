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

#ifndef INFRA_ICCAD15_UTILICCAD15_H_
#define INFRA_ICCAD15_UTILICCAD15_H_

#include <string>
using std::string;
#include <vector>

#include "util/Bounds.h"
#include "util/DBU.h"
#include "phy/PhysicalDesign.h"
using Rsyn::PhysicalCell;
namespace ICCAD15 {

enum LegalizationMethod {
        LEG_NONE = 0,
        LEG_EXACT_LOCATION,
        LEG_MIN_LINEAR_DISPLACEMENT,
        LEG_NEAREST_WHITESPACE
};

// Score
enum CostFunction {
        COST_NONE,
        COST_WIRELENGTH,
        COST_RC,
        COST_LOCAL_DELAY,
};  // end enum

}  // end namespace

#endif /* INFRA_ICCAD15_UTILICCAD15_H_ */
