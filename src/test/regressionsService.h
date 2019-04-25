/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   regressionsService.h
 * Author: isoliveira
 *
 * Created on April 11, 2019, 11:54 AM
 */

#ifndef REGRESSIONSSERVICE_H
#define REGRESSIONSSERVICE_H

#include "session/Session.h" 
#include "phy/PhysicalDesign.h"

class regressionsService : public Rsyn::Service{
public:
    regressionsService() = default;
    regressionsService(const regressionsService& orig) = default;
    virtual ~regressionsService() = default;
    
    virtual void start(const Rsyn::Json &params) override;
    virtual void stop() override;
private:
    Rsyn::Session clsSession;
    Rsyn::Design clsDesign;
    Rsyn::Module clsModule;
    Rsyn::PhysicalDesign clsPhDesign;
    
    //Instances, Nets, Pins, Ports
    
    void getAllInstances();
    void getAllNets();
    void getNumPins();
    void getAllPorts();
    
    //Physical Info
    
    void getPhysicalInfo();
    void getSitesPerRow();
    
    //Routing
    void getAllWires();
    void getAllVias();
    void getAllRects();
    
    //Power Ground
    void getAllWiresPG();
    void getAllViasPG();
    void getAllRectsPG();
    
    //Coordinates
    void getInstancesCoordinates();

};

#endif /* REGRESSIONSSERVICE_H */

