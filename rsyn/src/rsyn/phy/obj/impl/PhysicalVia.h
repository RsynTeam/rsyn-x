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
 * File:   PhysicalVia.h
 * Author: jucemar
 *
 * Created on 14 de Maio de 2017, 15:25
 */

namespace Rsyn {

inline const std::string & PhysicalVia::getName() const {
	return data->clsName;
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalVia::isDesignVia() const {
	return data->clsDesignVia;
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalViaLayer PhysicalVia::getTopLayer() const {
	return PhysicalViaLayer(data->clsViaLayers[TOP_VIA_LAYER]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalViaLayer PhysicalVia::getCutLayer() const {
	return PhysicalViaLayer(data->clsViaLayers[CUT_VIA_LAYER]);
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalViaLayer PhysicalVia::getBottomLayer() const {
	return PhysicalViaLayer(data->clsViaLayers[BOTTOM_VIA_LAYER]);
} // end method

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalViaLayer PhysicalVia::getLayer(const PhysicalViaLayerType type) const {
	return PhysicalViaLayer(data->clsViaLayers[type]);
} // end method

// -----------------------------------------------------------------------------

} // end namespace 

