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
 * Engine.cpp
 *
 *  Created on: May 8, 2015
 *      Author: jucemar
 */

#include <thread>
#include <iosfwd>
#include <mutex>
#include <boost/filesystem.hpp>

#include "Engine.h"

#include "rsyn/3rdparty/json/json.hpp"
#include "rsyn/util/Environment.h"

namespace Rsyn {

void Engine::create() {
	data = new EngineData();
	
	// TODO: hard coded
	data->clsInstallationPath = "../../rsyn/install";

	// Register processes.
	registerProcesses();
	
	// Register services
	registerServices();

	// register Loaders
	registerReaders();
	
	// Initialize logger
	const bool enableLog = Environment::getBoolean( "ENABLE_LOG", false );
	data->logger = ( enableLog ) ? ( new Logger() ) : ( nullptr );

	// Register some commands.
	registerDefaultCommands();
	
	// Create design.
	data->clsDesign.create("__Root_Design__");
} // end constructor 

// -----------------------------------------------------------------------------

void Engine::destroy() {
	// Stop all services.
	for (auto it : data->clsRunningServices) {
		it.second->stop();
	} // end for

//	// Delete all services.
//	for (auto it : clsRunningServices) {
//		delete it.second;
//	} // end for

	// Delete logger.
	delete data->logger;
} // end destructor

////////////////////////////////////////////////////////////////////////////////
// Message
////////////////////////////////////////////////////////////////////////////////

void Engine::registerMessage(const std::string& label, const MessageLevel& level, const std::string& title, const std::string& msg) {
	//std::cout << "Registering message '" << label << "'.\n";
} // end method

////////////////////////////////////////////////////////////////////////////////
// Script
////////////////////////////////////////////////////////////////////////////////
		
void Engine::registerCommand(const ScriptParsing::CommandDescriptor &dscp, const CommandHandler handler) {
	dscp.check();

	data->clsCommandManager.addCommand(dscp, [=](const ScriptParsing::Command &command) {
		handler(*this, command);
	});
} // end method

// -----------------------------------------------------------------------------

void Engine::evaluateString(const std::string &str) {
	data->clsCommandManager.evaluateString(str);
} // end method

// -----------------------------------------------------------------------------

void Engine::evaluateFile(const std::string &filename) {
	data->clsCommandManager.evaluateFile(filename);
} // end method

// -----------------------------------------------------------------------------

void Engine::registerDefaultCommands() {

	{ // help
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("help");
		dscp.setDescription("Shed some light in the world.");

		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			data->clsCommandManager.printCommandList(std::cout);
		});
	} // end block

	{ // exit
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("exit");
		dscp.setDescription("Quit execution.");

		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			data->clsCommandManager.exitRsyn(std::cout);
		});
	} // end block
	
	{ // history
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("history");
		dscp.setDescription("Output command history.");

		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			data->clsCommandManager.printHistory(std::cout);
		});
	} // end block

	{ // run
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("run");
		dscp.setDescription("Run a process.");

		dscp.addPositionalParam("name",
				ScriptParsing::PARAM_TYPE_STRING,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"Process name."
		);

		dscp.addPositionalParam("params",
				ScriptParsing::PARAM_TYPE_JSON,
				ScriptParsing::PARAM_SPEC_OPTIONAL,
				"Parameters to be passed to the process."
		);

		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			const std::string name = command.getParam("name");
			const Json params = command.getParam("params");
			runProcess(name, params);			
		});
	} // end block

	{ // set
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("set");
		dscp.setDescription("Set a session variable.");

		dscp.addPositionalParam("name",
				ScriptParsing::PARAM_TYPE_STRING,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"Name of the session variable."
		);

		dscp.addPositionalParam("value",
				ScriptParsing::PARAM_TYPE_JSON,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"Value of the session variable."
		);

		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			const std::string name = command.getParam("name");
			const Json value = command.getParam("value");
			setSessionVariable(name, value);
		});
	} // end block

	{ // open
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("open");
		dscp.setDescription("Open");

		dscp.addPositionalParam("format",
				ScriptParsing::PARAM_TYPE_STRING,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"Benchmark format."
		);

		dscp.addPositionalParam("options",
				ScriptParsing::PARAM_TYPE_JSON,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"Benchmark format."
		);
		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			const std::string format = command.getParam("format");
			Json options = command.getParam("options");
			options["globalPlacementOnly"] = getSessionVariableAsBool("globalPlacementOnly", false);
			options["path"] = command.getPath();

			runReader(format, options);
		});
	} // end block

	{ // source
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("source");
		dscp.setDescription("Evaluates a script and executes the commands.");

		dscp.addPositionalParam("fileName",
				ScriptParsing::PARAM_TYPE_STRING,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"The name of the script file."
		);
		
		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			const std::string fileName = command.getParam( "fileName" );
			evaluateFile(fileName);
		});
	} // end block
	
	{ // start
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("start");
		dscp.setDescription("Start a service.");

		dscp.addPositionalParam("name",
				ScriptParsing::PARAM_TYPE_STRING,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"Process name."
		);

		dscp.addPositionalParam("params",
				ScriptParsing::PARAM_TYPE_JSON,
				ScriptParsing::PARAM_SPEC_OPTIONAL,
				"Parameters to be passed to the process."
		);

		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			const std::string name = command.getParam("name");
			const Json params = command.getParam("params");
			startService(name, params);
		});
	} // end block
	
	{ // list 
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("list");
		dscp.setDescription("Listing registered processes, services and readers.");
		dscp.addPositionalParam("name",
				ScriptParsing::PARAM_TYPE_STRING,
				ScriptParsing::PARAM_SPEC_MANDATORY,
				"Parameter should be: \"process\", \"service\" or \"reader\"."
		);
		registerCommand(dscp, [&](Engine engine, const ScriptParsing::Command &command) {
			const std::string name = command.getParam("name");
			if(name.compare("process") == 0) {
				listProcess();
			} else if (name.compare("service") == 0) {
				listService();
			} else if (name.compare("reader") == 0) {
				listReader();
			} else {
				std::cout<<"ERROR: invalid list parameter: "<<name<<"\n";
			} // end if-else 
		});
	} // end block
} // end method

} /* namespace Rsyn */
