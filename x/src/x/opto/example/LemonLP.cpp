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
 * File:   LemonLP.cpp
 * Author: mpfogaca
 * 
 * Created on 26 de Setembro de 2016, 09:21
 */

/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library.
 *
 * Copyright (C) 2003-2010
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#include "LemonLP.h"
#include <lemon/lp.h>

namespace ICCAD15 {
	
bool LemonLP::run(const Rsyn::Json& params) {
	using namespace lemon;
	
	// Create an instance of the default LP solver class
	// (it will represent an "empty" problem at first)
	Lp lp;
	// Add two columns (variables) to the problem
	Lp::Col x1 = lp.addCol();
	Lp::Col x2 = lp.addCol();
	// Add rows (constraints) to the problem
	lp.addRow(x1 - 5 <= x2);
	lp.addRow(0 <= 2 * x1 + x2 <= 25);

	// Set lower and upper bounds for the columns (variables)
	lp.colLowerBound(x1, 0);
	lp.colUpperBound(x2, 10);

	// Specify the objective function
	lp.max();
	lp.obj(5 * x1 + 3 * x2);

	// Solve the problem using the underlying LP solver
	lp.solve();
	// Print the results
	if (lp.primalType() == Lp::OPTIMAL) {
	  std::cout << "Objective function value: " << lp.primal() << std::endl;
	  std::cout << "x1 = " << lp.primal(x1) << std::endl;
	  std::cout << "x2 = " << lp.primal(x2) << std::endl;
	} else {
	  std::cout << "Optimal solution not found." << std::endl;
	}

	return 0;
}

}
