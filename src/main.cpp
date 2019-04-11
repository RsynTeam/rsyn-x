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

#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <csignal>

#include "session/Session.h"
#include "shell/Shell.h"

#ifndef RSYN_NO_GUI
#include <QApplication>
#include "gui/window/MainWindow.h"
#endif

#include "util/StreamLogger.h"

// -----------------------------------------------------------------------------

#ifdef ISPD18_BIN
#include "io/reader/ISPD2018Reader.h"

void runISPD18Flow(const boost::program_options::variables_map &vm) {
        Rsyn::Session session;

        std::cout << "Team number: 21\n";
        std::cout << "Team name: RsynRouter\n";
        std::cout << "Member: Andre Oliveira (1), Erfan Aghaeekiasaraee (2), "
                     "Jorge Ferreira (3), Marcelo Danigno (3), Eder Monteiro "
                     "(1), Mateus Fogaca (1), Paulo Butzen (3), Laleh Behjat "
                     "(2), Ricardo Reis (1)\n";
        std::cout << "Affiliation: (1) UFRGS, (2) U. Calgary, (3) Furg\n";

        std::string lefFile;
        if (!vm.count("lef")) {
                std::cout << "[ERROR] Please specify the .lef file...\n";
                return;
        }  // end if
        lefFile = vm.at("lef").as<std::string>();

        std::string defFile;
        if (!vm.count("def")) {
                std::cout << "[ERROR] Please specify the .def file...\n";
                return;
        }  // end if
        defFile = vm.at("def").as<std::string>();

        std::string guideFile;
        if (!vm.count("guide")) {
                std::cout << "[ERROR] Please specify the .guide file...\n";
                return;
        }  // end if
        guideFile = vm.at("guide").as<std::string>();

        if (!vm.count("threads"))
                session.setSessionVariable("ispd19.numThreads", 1);
        else
                session.setSessionVariable("ispd19.numThreads",
                                           vm.at("threads").as<int>());

        if (!vm.count("tat"))
                session.setSessionVariable("ispd19.tat", 86400);
        else
                session.setSessionVariable("ispd19.tat",
                                           vm.at("tat").as<int>());

        std::string outputFile = "solution.def";
        if (vm.count("output")) outputFile = vm.at("output").as<std::string>();

        std::cout << "---- Input configuration ---- \n";
        std::cout << std::left << std::setw(18) << "LEF file:";
        std::cout << lefFile << "\n";
        std::cout << std::setw(18) << "DEF file:";
        std::cout << defFile << "\n";
        std::cout << std::setw(18) << "Guide file:";
        std::cout << guideFile << "\n";
        std::cout << std::setw(18) << "Output file:";
        std::cout << outputFile << "\n";
        std::cout << std::setw(18) << "# of threads:";
        std::cout << session.getSessionVariableAsInteger("ispd19.numThreads")
                  << "\n";
        std::cout << std::setw(18) << "tat:";
        std::cout << session.getSessionVariableAsInteger("ispd19.tat") << "\n";
        std::cout << "----------------------------- \n\n";

        Rsyn::ISPD2018Reader reader;
        const Rsyn::Json params = {
            {"lefFile", lefFile},
            {"defFile", defFile},
            {"guideFile", guideFile},
        };
        reader.load(params);

        std::string path =
            outputFile.substr(0, outputFile.find_last_of('/') + 1);
        std::string file = outputFile.substr(outputFile.find_last_of('/') + 1,
                                             outputFile.size());

        Rsyn::Shell shell;

        std::stringstream cmd;
        cmd << "start \"rsyn.writerDEF\" {\"path\" : \"" << path
            << "\", \"filename\" : \"" + file + "\"};";
        shell.runCommand(cmd.str());

        cmd = std::stringstream();
        cmd << "writeDEFFile;";
        shell.runCommand(cmd.str());
}  // end function
#endif

// -----------------------------------------------------------------------------

void signalHandler(int signum) {
        std::cout << "Signal (" << signum << ") received. Exiting...\n";

        // cleanup and close up stuff here

        // terminate program
        std::exit(signum);
}  // end function

// -----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        //
        // Rsyn
        //

        Rsyn::StreamLogger::get()->capture(std::cout);
// Rsyn::StreamLogger::get()->capture(std::cerr);

#ifndef ISPD18_BIN
        // Source: http://patorjk.com/software/taag/#p=display&f=Big&t=Rsyn
        std::cout << "\n";  // skipping the terminal line
        std::cout << std::string(28, ' ') << R"( _____                  )"
                  << "\n";
        std::cout << std::string(28, ' ') << R"(|  __ \                 )"
                  << "\n";
        std::cout << std::string(28, ' ') << R"(| |__) |___ _   _ _ __  )"
                  << "\n";
        std::cout << std::string(28, ' ') << R"(|  _  // __| | | | '_ \ )"
                  << "\n";
        std::cout << std::string(28, ' ') << R"(| | \ \\__ \ |_| | | | |)"
                  << "\n";
        std::cout << std::string(28, ' ') << R"(|_|  \_\___/\__, |_| |_|)"
                  << "\n";
        std::cout << std::string(28, ' ') << R"(            __/ /       )"
                  << "\n";
        std::cout << std::string(28, ' ') << R"(           |___/        )"
                  << "\n";
        std::cout << "\n" << std::endl;

        // std::cout << "http://rsyn.design" <<  "\n" << std::endl;

        // Guilherme Flach - 2017/03/18
        // --------------------------------------------
        // Adding these messages as requested in FLUTE's license.
        std::cout << "-------------------------\n";
        std::cout << "3rd Party License Notices\n";
        std::cout << "-------------------------\n";
        std::cout << "1) FLUTE is a software developed by Dr. Chris C. N. Chu ";
        std::cout << "at Iowa State University ";
        std::cout << "(http://class.ee.iastate.edu/cnchu/)\n";
        std::cout << "\n";
// -------------------------------------------------------------------------
#endif

        //
        // Command Line Parser
        //
        try {
                using namespace boost::program_options;

                std::string optScript;

                options_description desc{"Options"};
#ifdef ISPD18_BIN
                desc.add_options()("lef", value<std::string>(),
                                   "Input .lef file.")(
                    "def", value<std::string>(), "Input .def file.")(
                    "guide", value<std::string>(), "Input .guide file.")(
                    "threads", value<int>(), "# of threads.")(
                    "tat", value<int>(), "Runtime limit (s).")(
                    "output", value<std::string>(), "Output file name.");
#else
                desc.add_options()("script", value<std::string>(&optScript),
                                   "The script to run at startup.")(
                    "interactive", "Does not exit after running the script.")(
                    "gui", "The user interface to start.")(
                    "no-gui", "The user interface to start.");
#endif

                variables_map vm;
                store(command_line_parser(argc, argv)
                          .options(desc)
                          .style(command_line_style::unix_style |
                                 command_line_style::allow_long_disguise)
                          .run(),
                      vm);
                notify(vm);

#ifdef ISPD18_BIN
                runISPD18Flow(vm);
#else
                if (!vm.count("no-gui")) {
// User interface mode...
#ifndef RSYN_NO_GUI
                        Q_INIT_RESOURCE(images);

                        QApplication app(argc, argv);

                        QCoreApplication::setOrganizationName("Rsyn");
                        QCoreApplication::setOrganizationDomain("rsyn.design");
                        QCoreApplication::setApplicationName("Rsyn");

#ifdef __APPLE__
                        setlocale(LC_ALL, "en_US.UTF-8");
#else
                        std::setlocale(LC_ALL, "en_US.UTF-8");
#endif
                        app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

                        Rsyn::MainWindow window;
                        window.show();

                        return app.exec();
#else
                        std::cout << "Rsyn was compiled without GUI...\n";
#endif
                } else {
                        // Text mode...
                        Rsyn::Shell shell;
                        shell.runScript(optScript, vm.count("interactive"));
                }  // end else
#endif

        } catch (const boost::program_options::error &e) {
                std::cerr << e.what() << '\n';
        }  // end catch

}  // end main
