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

/* 
 * File:   DensityGridService.h
 * Author: jucemar
 *
 * Created on 29 de Outubro de 2016, 10:19
 */

#ifndef DENSITYGRIDSERVICE_H
#define DENSITYGRIDSERVICE_H

#include "rsyn/session/Service.h"
#include "rsyn/model/congestion/DensityGrid/DensityGrid.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/phy/PhysicalDesign.h"

namespace Rsyn {

class DensityGridService : public Rsyn::Service {
private:
	Rsyn::DensityGrid clsDensityGrid;
public:

	DensityGridService () {}
	virtual void start(const Rsyn::Json &params) override;
	virtual void stop() override;

	Rsyn::DensityGrid getDensityGrid() { return clsDensityGrid; }
}; // end class

} // end namespace 

#endif /* DENSITYGRIDSERVICE_H */

