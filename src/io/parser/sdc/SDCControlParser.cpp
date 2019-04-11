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
 * SDCControlParser.cpp
 *
 *  Created on: Feb 9, 2015
 *      Author: jucemar
 */

#include "SDCControlParser.h"

SDCControlParser::SDCControlParser() {}  // end method

// -----------------------------------------------------------------------------

// =============================================================================
// SDC Format for ICCAD 2014 Contest
// =============================================================================
/*! ICCAD 2014 Contest - Synopsys Design Constraint format (SDC)
 *
 */

void SDCControlParser::parseSDC_iccad14(const string &sdcFile,
                                        ISPD13::SDCInfo &sdcInfos) {
        const double TIME_UNIT = 1e-12;  // [HARDCODED]
        const double CAP_UNIT = 1e-15;   // [HARDCODED]

        ISPD13::SdcParser sp(sdcFile);

        string clockName;
        string clockPort;
        double period;
        bool valid = sp.read_clock(clockName, clockPort, period);
        ISPD13::InputDelay inp_delay;
        ISPD13::InputDriver inp_driver;
        ISPD13::OutputLoad out_load;
        ISPD13::OutputDelay out_delay;

        assert(valid);
        // cout << "Clock " << clockName << " connected to port " << clockPort
        // << " has period " << period << endl ;

        sdcInfos.clk_name = clockName;
        sdcInfos.clk_port = clockPort;
        sdcInfos.clk_period = period * TIME_UNIT;

        do {
                string portName;
                double delay;

                valid = sp.read_input_delay(portName, delay);

                if (valid) {
                        // cout << "Input port " << portName << " has delay " <<
                        // delay << endl ;
                        inp_delay.port_name = portName;
                        inp_delay.delay = delay * TIME_UNIT;
                        sdcInfos.input_delays.push_back(inp_delay);
                }

        } while (valid);

        do {
                string portName;
                string driverSize;
                string driverPin;
                double inputTransitionFall;
                double inputTransitionRise;

                valid = sp.read_driver_info(portName, driverSize, driverPin,
                                            inputTransitionFall,
                                            inputTransitionRise);

                if (valid) {
                        // cout << "Input port " << portName << " is assumed to
                        // be connected to the " << driverPin << " pin of lib
                        // cell " << driverSize << endl ;
                        // cout << "This virtual driver is assumed to have input
                        // transitions: " << inputTransitionFall << " (fall) and
                        // " << inputTransitionRise << " (rise)" << endl ;
                        inp_driver.port_name = portName;
                        inp_driver.driver = driverSize;
                        inp_driver.rise = inputTransitionRise * TIME_UNIT;
                        inp_driver.fall = inputTransitionFall * TIME_UNIT;
                        sdcInfos.input_drivers.push_back(inp_driver);
                }

        } while (valid);

        do {
                string portName;
                double delay;

                valid = sp.read_output_delay(portName, delay);

                if (valid) {
                        // cout << "Output port " << portName << " has delay "
                        // << delay << endl ;
                        out_delay.port_name = portName;
                        out_delay.delay = delay * TIME_UNIT;
                        sdcInfos.output_delays.push_back(out_delay);
                }
        } while (valid);

        do {
                string portName;
                double load;

                valid = sp.read_output_load(portName, load);

                if (valid) {
                        // cout << "Output port " << portName << " has load " <<
                        // load << endl ;
                        out_load.port_name = portName;
                        out_load.load = load * CAP_UNIT;
                        sdcInfos.output_loads.push_back(out_load);
                }
        } while (valid);

        cout << "\tSDC input delays: " << sdcInfos.input_delays.size() << "\n";
        cout << "\tSDC input drivers: " << sdcInfos.input_drivers.size()
             << "\n";
        cout << "\tSDC output delays: " << sdcInfos.output_delays.size()
             << "\n";
        cout << "\tSDC output loads: " << sdcInfos.output_loads.size() << "\n";
}  // end method

// -----------------------------------------------------------------------------

void SDCControlParser::parseSDC_iccad15(const string &sdcFile,
                                        ISPD13::SDCInfo &sdcInfos) {
        // const double TIME_UNIT = 1e-12; // [HARDCODED]
        // const double CAP_UNIT = 1e-15; // [HARDCODED]

        ISPD13::SdcParser sp(sdcFile);

        string clockName;
        string clockPort;
        double period;
        bool valid = sp.read_clock(clockName, clockPort, period);
        ISPD13::InputDelay inp_delay;
        ISPD13::InputDriver inp_driver;
        ISPD13::OutputLoad out_load;
        ISPD13::OutputDelay out_delay;

        assert(valid);
        // cout << "Clock " << clockName << " connected to port " << clockPort
        // << " has period " << period << endl ;

        sdcInfos.clk_name = clockName;
        sdcInfos.clk_port = clockPort;
        sdcInfos.clk_period = period;  //*TIME_UNIT;

        do {
                string portName;
                double delay;

                valid = sp.read_input_delay(portName, delay);

                if (valid) {
                        // cout << "Input port " << portName << " has delay " <<
                        // delay << endl ;
                        inp_delay.port_name = portName;
                        inp_delay.delay = delay;  //*TIME_UNIT;
                        sdcInfos.input_delays.push_back(inp_delay);
                }

        } while (valid);

        do {
                string portName;
                string driverSize;
                string driverPin;
                double inputTransitionFall;
                double inputTransitionRise;

                valid = sp.read_driver_info(portName, driverSize, driverPin,
                                            inputTransitionFall,
                                            inputTransitionRise);

                if (valid) {
                        // cout << "Input port " << portName << " is assumed to
                        // be connected to the " << driverPin << " pin of lib
                        // cell " << driverSize << endl ;
                        // cout << "This virtual driver is assumed to have input
                        // transitions: " << inputTransitionFall << " (fall) and
                        // " << inputTransitionRise << " (rise)" << endl ;
                        inp_driver.port_name = portName;
                        inp_driver.driver = driverSize;
                        inp_driver.rise = inputTransitionRise;  //*TIME_UNIT;
                        inp_driver.fall = inputTransitionFall;  //*TIME_UNIT;
                        sdcInfos.input_drivers.push_back(inp_driver);
                }

        } while (valid);

        do {
                string portName;
                double delay;

                valid = sp.read_output_delay(portName, delay);

                if (valid) {
                        // cout << "Output port " << portName << " has delay "
                        // << delay << endl ;
                        out_delay.port_name = portName;
                        out_delay.delay = delay;  //*TIME_UNIT;
                        sdcInfos.output_delays.push_back(out_delay);
                }
        } while (valid);

        do {
                string portName;
                double load;

                valid = sp.read_output_load(portName, load);

                if (valid) {
                        // cout << "Output port " << portName << " has load " <<
                        // load << endl ;
                        out_load.port_name = portName;
                        out_load.load = load;  //*CAP_UNIT;
                        sdcInfos.output_loads.push_back(out_load);
                }
        } while (valid);

        cout << "\tSDC input delays: " << sdcInfos.input_delays.size() << "\n";
        cout << "\tSDC input drivers: " << sdcInfos.input_drivers.size()
             << "\n";
        cout << "\tSDC output delays: " << sdcInfos.output_delays.size()
             << "\n";
        cout << "\tSDC output loads: " << sdcInfos.output_loads.size() << "\n";
}  // end method

// =============================================================================
// TIMING - SDC Format for TAU 2015 Contest
// =============================================================================
/*! TAU 2015 timing file is a format like Synopsys Design Constraint
 *
 */
void SDCControlParser::parseTiming(const string &timingFile,
                                   ISPD13::TimingInfo &timingInfo) {
        ISPD13::TimingParser tp(timingFile);

        cout << "\tTiming file: " << timingFile << "\n";
        bool valid;
        string name1;
        string name2;
        double val1, val2, val3, val4;

        do {
                valid =
                    tp.read_timing_tau15(name1, name2, val1, val2, val3, val4);

                if (!valid) break;

                if (name1 == "clock") {
                        timingInfo.clockPort = name2;
                        timingInfo.clockPeriod = val1;
                        timingInfo.clockLow = val2;
                } else if (name1 == "at") {
                        ISPD13::TimingStruct ts(name2, val1, val2, val3, val4);
                        timingInfo.ats.push_back(ts);
                } else if (name1 == "slew") {
                        ISPD13::TimingStruct ts(name2, val1, val2, val3, val4);
                        timingInfo.slews.push_back(ts);
                } else if (name1 == "rat") {
                        ISPD13::TimingStruct ts(name2, val1, val2, val3, val4);
                        timingInfo.rats.push_back(ts);
                } else if (name1 == "load") {
                        ISPD13::TimingCap tc(name2, val1);
                        timingInfo.caps.push_back(tc);
                }
        } while (valid);

}  // end method

// -----------------------------------------------------------------------------

SDCControlParser::~SDCControlParser() {
        // TODO Auto-generated destructor stub
}  // end method

// -----------------------------------------------------------------------------
