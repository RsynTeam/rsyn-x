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
 
#include <thread>
#include <iosfwd>
#include <mutex>
#include <boost/filesystem.hpp>

#include "Session.h"

#include "rsyn/3rdparty/json/json.hpp"
#include "rsyn/util/Environment.h"

namespace Rsyn {

static Startup initEngine([]{
	// This will ensure the engine singleton gets initialized when the program
	// starts.
});

// -----------------------------------------------------------------------------

SessionData * Session::sessionData = nullptr;

// -----------------------------------------------------------------------------

void Session::init() {
	if (isInitialized())
		return;

#ifdef __APPLE__
    setlocale(LC_ALL, "en_US.UTF-8");
#else
    std::setlocale(LC_ALL, "en_US.UTF-8");
#endif

	sessionData = new SessionData();

	// TODO: hard coded
	sessionData->clsInstallationPath = "../../rsyn/install";

	// Register messages.
	registerMessages();

	// Register processes.
	registerProcesses();
	
	// Register services
	registerServices();

	// Register readers.
	registerReaders();
	
	// Register some commands.
	registerDefaultCommands();

	// Create design.
	sessionData->clsDesign.create("__Root_Design__");

	// Cache messages.
	sessionData->msgMessageRegistrationFail = getMessage("SESSION-001");

	// Initialize logger
	const bool enableLog = Environment::getBoolean( "ENABLE_LOG", false );
	sessionData->logger = ( enableLog ) ? ( new Logger() ) : ( nullptr );
} // end constructor 

////////////////////////////////////////////////////////////////////////////////
// Message
////////////////////////////////////////////////////////////////////////////////

void Session::registerMessage(const std::string& label, const MessageLevel& level, const std::string& title, const std::string& msg) {
	sessionData->clsMessageManager.registerMessage(label, level, title, msg);
} // end method

// -----------------------------------------------------------------------------

Message Session::getMessage(const std::string &label) {
	return sessionData->clsMessageManager.getMessage(label);
} // end method

////////////////////////////////////////////////////////////////////////////////
// Script
////////////////////////////////////////////////////////////////////////////////
		
void Session::registerCommand(const ScriptParsing::CommandDescriptor &dscp, const CommandHandler handler) {
	dscp.check();

	sessionData->clsCommandManager.addCommand(dscp, [=](const ScriptParsing::Command &command) {
		handler(command);
	});
} // end method

// -----------------------------------------------------------------------------

void Session::evaluateString(const std::string &str) {
	sessionData->clsCommandManager.evaluateString(str);
} // end method

// -----------------------------------------------------------------------------

void Session::evaluateFile(const std::string &filename) {
	const std::string path = boost::filesystem::path(filename).parent_path().string();
	addPath(path, true);
	sessionData->clsCommandManager.evaluateFile(filename);
	removePath(path);
} // end method

// -----------------------------------------------------------------------------

void Session::registerDefaultCommands() {

	{ // help
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("help");
		dscp.setDescription("Shed some light in the world.");

		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			sessionData->clsCommandManager.printCommandList(std::cout);
		});
	} // end block

	{ // exit
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("exit");
		dscp.setDescription("Quit execution.");

		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			sessionData->clsCommandManager.exitRsyn(std::cout);
		});
	} // end block
	
	{ // history
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("history");
		dscp.setDescription("Output command history.");

		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			sessionData->clsCommandManager.printHistory(std::cout);
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

		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
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

		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
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
		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			const std::string format = command.getParam("format");
			Json options = command.getParam("options");
			if (getSessionVariableAsBool("globalPlacementOnly", false))
				options["globalPlacementOnly"] = true;
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
		
		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
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

		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
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
		registerCommand(dscp, [&](const ScriptParsing::Command &command) {
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

// -----------------------------------------------------------------------------

void Session::addPath(const std::string &path, const bool prepend) {
	if (prepend) {
		sessionData->clsPaths.push_front(path);
	} else {
		sessionData->clsPaths.push_back(path);
	} // end else
} // end method

// -----------------------------------------------------------------------------

void Session::removePath(const std::string &path) {
	sessionData->clsPaths.remove(path);
} // end method

// -----------------------------------------------------------------------------

std::string Session::mergePathAndFileName(const std::string &path, const std::string &fileName) {
	const char separator = boost::filesystem::path::preferred_separator;
	if (!path.empty() &&  (path.back() != separator && path.back() != '/')) {
		return path + separator + fileName;
	} else {
		return path + fileName;
	} // end else
} // end method

// -----------------------------------------------------------------------------

std::string Session::findFile(const std::string fileName, const std::string extraPath) {
	// Check if the file exists without any path.
	if (boost::filesystem::exists(fileName)) {
		return fileName;
	} // end if

	// Check if the file exists in the extra path.
	if (!extraPath.empty()) {
		const std::string fullFileName = mergePathAndFileName(extraPath, fileName);
		if (boost::filesystem::exists(fullFileName)) {
			return fullFileName;
		} // end if
	} // end if

	// Check if the file exists in the paths.
	for (const std::string &path : sessionData->clsPaths) {
		const std::string fullFileName = mergePathAndFileName(path, fileName);
		if (boost::filesystem::exists(fullFileName)) {
			return fullFileName;
		} // end if
	} // end for

	// File not found.
	return "";
} // end method

} /* namespace Rsyn */
