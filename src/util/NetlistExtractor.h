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
 * File:   NetlistDecomposer.h
 * Author: mpfogaca
 *
 * Created on 31 de Março de 2017, 14:49
 */

#ifndef NETLISTDECOMPOSER_H
#define NETLISTDECOMPOSER_H

#include "core/Rsyn.h"
#include "session/Session.h"
#include "phy/PhysicalDesign.h"

#include <unordered_set>
#include <unordered_map>
#include <vector>

class NetlistExtractor : public Rsyn::Service {
       public:
        struct Transistor {
                std::string name;
                std::string source;
                std::string bulk;
                std::string drain;
                std::string gate;
                std::string type;
                double l;
                double w;
                double ad;
                double as;
                double pd;
                double ps;
                double x;
                double y;
                bool d;
                bool f;
        };

        struct Subckt {
                std::string name;
                std::unordered_set<std::string> interface;
                std::vector<Transistor> netlist;
        };

       private:
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;
        Rsyn::PhysicalDesign clsPhysicalDesign;

        std::string clsSpiceFile;
        std::unordered_map<std::string, Subckt> clsEntries;
        std::vector<Transistor> clsExtractedNetlist;

        void init();
        void parseSubcktInterface(const std::string input, Subckt& subckt);
        void parseTransistor(const std::string input, Transistor& transistor);
        void extract();
        void extractCellSubckt(const Rsyn::Cell cell, const Subckt& subckt);
        const std::string getNodeName(const Rsyn::Cell cell,
                                      const std::string originalName,
                                      const Subckt& subckt) const;

       public:
        NetlistExtractor(){};

        const std::vector<Transistor>& getNetlist() const {
                return clsExtractedNetlist;
        };  // end method

        void start(const Rsyn::Json& params) override;
        void stop() override{};
};

#endif /* NETLISTDECOMPOSER_H */
