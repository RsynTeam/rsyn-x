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
 * File:   TDQuadraticFlow.cpp
 * Author: mpfogaca
 * 
 * Created on 13 de Setembro de 2016, 09:25
 */

#include "TDQuadraticFlow.h"
#include "rsyn/model/timing/Timer.h"
#include "x/infra/iccad15/Infrastructure.h"

namespace ICCAD15{

bool TDQuadraticFlow::run(Rsyn::Engine engine, const Rsyn::Json& params) {
	clsEngine = engine;
	clsTimer = engine.getService("rsyn.timer");
	clsInfra = engine.getService("ufrgs.ispd16.infra");
	clsJezz = clsInfra->getJezz();
	
	if (params.count("overlapFlow")) {
		if(params["overlapFlow"]=="Jezz")
			overlapFlow();
		else
			overlapFlowMIP();
		return true;
	} // end if
	
	defaultFlow();
	
	return true;
} // end method

//------------------------------------------------------------------------------

void TDQuadraticFlow::defaultFlow() {
	clsTimer->updateTimingFull();
	clsJezz->jezz_Legalize();
	clsJezz->jezz_storeSolution( "initial" );
	
	clsInfra->updateTDPSolution( true );
	
	vector<Rsyn::Pin> ep;
	clsTimer->queryTopCriticalEndpoints( Rsyn::LATE, 3, ep );
	const double initAlpha = clsTimer->getSmoothedCriticality( ep.back(), Rsyn::LATE );
	
	while( true ) {
		clsEngine.runProcess( "ufrgs.incrementalTimingDrivenQP",
            { {"alpha", initAlpha}, {"beta", 1.0}, {"flow", "netWeighting"} } );
            
        clsInfra->reportSummary( "QP-NetWeighting-Step1" );
		
		if( !clsInfra->updateTDPSolution(false, 1e-5) )
			break;
		
		clsEngine.runProcess("ufrgs.balancing", {{"type", "cell-driver-sink"}});
	}; // end while
	clsInfra->statePush("qp-net-weighting");
	
	clsEngine.runProcess("ufrgs.ISPD16Flow");
} // end method

//------------------------------------------------------------------------------

void TDQuadraticFlow::overlapFlow() {
	clsTimer->updateTimingFull();
	clsJezz->jezz_Legalize();
	clsJezz->jezz_storeSolution( "initial" );
	
	clsInfra->updateTDPSolution( true );
	
	while( true ) {
		clsEngine.runProcess( "ufrgs.incrementalTimingDrivenQP",
            { {"alpha", 0.9}, {"beta", 1.0}, {"flow", "netWeighting"} } );
            
        clsInfra->reportSummary( "QP-NetWeighting-Step1" );
		
		if( !clsInfra->updateTDPSolution(false, 1e-5) )
			break;		
	}; // end while
} // end method

//------------------------------------------------------------------------------

void TDQuadraticFlow::overlapFlowMIP() {
	clsTimer->updateTimingFull();
	clsJezz->jezz_Legalize();
	clsJezz->jezz_storeSolution( "initial" );
	
	clsInfra->updateTDPSolution( true );
	
	while( true ) {
		clsEngine.runProcess( "ufrgs.incrementalTimingDrivenQP",
			{ {"alpha", 0.9}, {"beta", 1.0}, {"flow", "elmore"}, {"legMode","NONE"}});
			
		clsEngine.runProcess("ufrgs.overlapRemover", {});
			
		clsTimer->updateTimingIncremental();
			
        clsInfra->reportSummary( "QP-NetWeighting-Step1" );
		
		if( !clsInfra->updateTDPSolution(false, 1e-5) )
			break;		
	}; // end while
} // end method

}

