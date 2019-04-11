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

#ifndef STEPWATCH_H
#define STEPWATCH_H

#include <iostream>

#include "MemoryUsage.h"
#include "Stopwatch.h"

class Stepwatch {
       private:
        static int clsDepth;

        Stopwatch clsStopwatch;
        const std::string clsMsg;
        int clsInitialMemoryUsage;
        double elapsedRuntime;
        bool clsPrint;

        void printMsg() const { std::cout << clsMsg << "..."; }  // end method

       public:
        Stepwatch(const std::string &msg, const bool print = true)
            : clsMsg(msg) {
                clsPrint = print;
                clsInitialMemoryUsage = MemoryUsage::getMemoryUsage();
                clsStopwatch.start();
                if (clsPrint) {
                        printMsg();
                        std::cout << std::endl;
                }  // end if
                clsDepth++;
                elapsedRuntime = 0;

        }  // end constructor

        double getElapsedTime(const bool minutes = false) const {
                if (minutes) return elapsedRuntime / 60;
                return elapsedRuntime;
        }  // end method

        void finish() {
                const int memoryUsage =
                    MemoryUsage::getMemoryUsage() - clsInitialMemoryUsage;
                clsStopwatch.stop();
                clsDepth--;
                elapsedRuntime = clsStopwatch.getElapsedTime();
                if (clsPrint) {
                        printMsg();
                        std::cout << " Done (runtime: " << elapsedRuntime
                                  << " seconds "
                                  << "memory: " << std::showpos << memoryUsage
                                  << std::noshowpos << " MB)" << std::endl;
                }  // end if
        }          // end method

        ~Stepwatch() {
                if (clsStopwatch.isRunning()) finish();
        }  // end constructor
};         // end class

#endif
