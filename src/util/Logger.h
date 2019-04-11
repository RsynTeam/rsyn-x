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

#ifndef LOGGER_H
#define LOGGER_H

#include <ostream>
#include <ctime>
#include <string>
#include <list>

#include <boost/filesystem.hpp>

#include "StreamMirror.h"

/* A class that simply redirects the standard output to a log file. */
class Logger {
       private:
        static bool exitHandlerInitialized;
        static std::list<Logger *> loggers;

        std::list<Logger *>::iterator me;
        std::ofstream file;
        StreamMirror *streamMirror;

        const std::string generateASCIIDateAndTime() const {
                time_t rawtime;
                struct tm *timeinfo;
                char buffer[128];

                time(&rawtime);
                timeinfo = localtime(&rawtime);

                strftime(buffer, 80, "%Y-%m-%d-%H-%M", timeinfo);

                return std::string(buffer);
        };

       public:
        Logger(const std::string logFilename = "") {
                loggers.push_front(this);
                me = loggers.begin();

                if (!exitHandlerInitialized) {
                        exitHandlerInitialized = true;
                        /* Not working on newer gcc...
                        std::atexit([&]{
                                std::cout << "Logger is being killed...\n";
                                for (Logger * logger : loggers) {
                                        logger->flush();
                                } // end for
                        });
                        */
                }  // end if

                boost::filesystem::path dir("log");
                if (boost::filesystem::create_directory(dir)) {
                        std::cerr << "[INFO] Log directory created.\n";
                }  // end if

                if (logFilename != "")
                        file.open(logFilename);
                else
                        file.open("log/uplace-" + generateASCIIDateAndTime() +
                                  ".log");

                if (!file.is_open()) {
                        std::cout << "[WARNING] Unable to open log file.\n";
                        return;
                }  // end if

                streamMirror = new StreamMirror(std::cout, file);
        }  // end constructor

        void flush() {
                std::cerr << "flusing...\n";
                if (streamMirror) {
                        std::cerr << "1";
                        streamMirror->flush();
                }  // end if

                if (file.is_open()) {
                        std::cerr << "2";
                        file.flush();
                }  // end if
        }          // end method

        virtual ~Logger() {
                loggers.erase(me);
                delete streamMirror;

                if (file.is_open()) file.close();
        }  // end destructor

};  // end class

#endif
