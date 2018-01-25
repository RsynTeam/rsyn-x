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

#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <csignal>

#include "rsyn/session/Session.h"
#include "rsyn/shell/Shell.h"

#ifndef RSYN_NO_GUI
#include <QApplication>
#include "rsyn/qt/MainWindow.h"
#endif

#include "rsyn/util/StreamLogger.h"
#include "rsyn/io/reader/ISPD2018Reader.h"

// -----------------------------------------------------------------------------

void signalHandler(int signum) {
    std::cout << "Signal (" << signum << ") received. Exiting...\n";

    // cleanup and close up stuff here

    // terminate program
	std::exit(signum);
} // end function

// -----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
	signal(SIGINT , signalHandler);
	signal(SIGTERM, signalHandler);

	//
	// Rsyn
	//

	Rsyn::StreamLogger::get()->capture(std::cout);
	//Rsyn::StreamLogger::get()->capture(std::cerr);

	// Source: http://patorjk.com/software/taag/#p=display&f=Big&t=Rsyn
	std::cout << "\n"; // skipping the terminal line
	std::cout << std::string(28, ' ') << R"( _____                  )" << "\n";
	std::cout << std::string(28, ' ') << R"(|  __ \                 )" << "\n";
	std::cout << std::string(28, ' ') << R"(| |__) |___ _   _ _ __  )" << "\n";
	std::cout << std::string(28, ' ') << R"(|  _  // __| | | | '_ \ )" << "\n";
	std::cout << std::string(28, ' ') << R"(| | \ \\__ \ |_| | | | |)" << "\n";
	std::cout << std::string(28, ' ') << R"(|_|  \_\___/\__, |_| |_|)" << "\n";
	std::cout << std::string(28, ' ') << R"(            __/ /       )" << "\n";
	std::cout << std::string(28, ' ') << R"(           |___/        )" << "\n";
	std::cout << "\n" << std::endl;

	//std::cout << "http://rsyn.design" <<  "\n" << std::endl;

	// Guilherme Flach - 2017/03/18 --------------------------------------------
	// Adding these messages as requested in FLUTE's license.
	std::cout << "-------------------------\n";
	std::cout << "3rd Party License Notices\n";
	std::cout << "-------------------------\n";
	std::cout << "1) FLUTE is a software developed by Dr. Chris C. N. Chu ";
	std::cout << "at Iowa State University ";
	std::cout << "(http://class.ee.iastate.edu/cnchu/)\n";
	std::cout << "\n";
	// -------------------------------------------------------------------------

	//
	// Command Line Parser
	//
	try {
		using namespace boost::program_options;

		std::string optScript;

		options_description desc{"Options"};
		#ifdef ISPD18_BIN
			desc.add_options()
				("lef", value<std::string>(), "Input .lef file.")
				("def", value<std::string>(), "Input .def file.")
				("guide", value<std::string>(), "Input .guide file.")
				("threads", value<int>(), "# of threads.")
				("output", value<std::string>(), "Output file name.");
		#else
			desc.add_options()
					("script", value<std::string>(&optScript), "The script to run at startup.")
					("interactive", "Does not exit after running the script.")
					("gui", "The user interface to start.")
					("no-gui", "The user interface to start.");
		#endif

		variables_map vm;
        store (command_line_parser(argc, argv).options(desc)
				.style (command_line_style::unix_style |
						command_line_style::allow_long_disguise)
				.run(), vm);
		notify(vm);

		#ifdef ISPD18_BIN
			runISPD18Flow(vm);
		#else
			if (!vm.count("no-gui")) {
				// User interface mode...
				#ifndef RSYN_NO_GUI
					Q_INIT_RESOURCE(images);

					QApplication app(argc, argv);
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
				shell.run(optScript, vm.count("interactive"));
			} // end else
		#endif

	} catch (const boost::program_options::error &e) {
		std::cerr << e.what() << '\n';
	} // end catch

} // end main