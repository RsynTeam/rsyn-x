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

#include <thread>
#include <iosfwd>
#include <mutex>
#include <boost/filesystem.hpp>

#include "Session.h"

#include "util/Json.h"
#include "phy/PhysicalService.h"
#include "util/Environment.h"

namespace Rsyn {

static Startup initEngine([] {
        // This will ensure the engine singleton gets initialized when the
        // program
        // starts.
});

// -----------------------------------------------------------------------------

SessionData *Session::sessionData = nullptr;

// -----------------------------------------------------------------------------

void Session::init() {
        if (checkInitialized()) return;

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

        // Create library
        sessionData->clsLibrary = Library(new LibraryData);
        sessionData->clsLibrary->designData = sessionData->clsDesign.data;

        // Cache messages.
        sessionData->msgMessageRegistrationFail = getMessage("SESSION-001");

        // Initialize logger
        const bool enableLog = Environment::getBoolean("ENABLE_LOG", false);
        sessionData->logger = (enableLog) ? (new Logger()) : (nullptr);

        checkInitialized(true);
}  // end constructor

// -----------------------------------------------------------------------------

Rsyn::Design Session::getDesign() {
        return sessionData->clsDesign;
}  // end method

// -----------------------------------------------------------------------------

Rsyn::Library Session::getLibrary() {
        return sessionData->clsLibrary;
}  // end method

// -----------------------------------------------------------------------------

Rsyn::Module Session::getTopModule() {
        return getDesign().getTopModule();
}  // end method

// -----------------------------------------------------------------------------

Rsyn::PhysicalDesign Session::getPhysicalDesign() {
        if (isServiceRunning("rsyn.physical")) {
                PhysicalService *service = getService("rsyn.physical");
                return service->getPhysicalDesign();
        } else {
                return nullptr;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void Session::registerMessage(const std::string &label,
                              const MessageLevel &level,
                              const std::string &title,
                              const std::string &msg) {
        sessionData->clsMessageManager.registerMessage(label, level, title,
                                                       msg);
}  // end method

// -----------------------------------------------------------------------------

Message Session::getMessage(const std::string &label) {
        return sessionData->clsMessageManager.getMessage(label);
}  // end method

// -----------------------------------------------------------------------------

void Session::registerCommand(const ScriptParsing::CommandDescriptor &dscp,
                              const CommandHandler handler) {
        dscp.check();

        sessionData->clsCommandManager.addCommand(
            dscp,
            [=](const ScriptParsing::Command &command) { handler(command); });
}  // end method

// -----------------------------------------------------------------------------

void Session::evaluateString(const std::string &str) {
        sessionData->clsCommandManager.evaluateString(str);
}  // end method

// -----------------------------------------------------------------------------

void Session::evaluateFile(const std::string &filename) {
        const std::string path =
            boost::filesystem::path(filename).parent_path().string();
        addPath(path, true);
        sessionData->clsCommandManager.evaluateFile(filename);
        removePath(path);
}  // end method

// -----------------------------------------------------------------------------

void Session::registerDefaultCommands() {
        {  // help
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("help");
                dscp.setDescription("Shed some light in the world.");

                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            sessionData->clsCommandManager.printCommandList(
                                std::cout);
                    });
        }  // end block

        {  // exit
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("exit");
                dscp.setDescription("Quit execution.");

                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            sessionData->clsCommandManager.exitRsyn(std::cout);
                    });
        }  // end block

        {  // history
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("history");
                dscp.setDescription("Output command history.");

                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            sessionData->clsCommandManager.printHistory(
                                std::cout);
                    });
        }  // end block

        {  // run
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("run");
                dscp.setDescription("Run a process.");

                dscp.addPositionalParam(
                    "name", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_MANDATORY, "Process name.");

                dscp.addPositionalParam(
                    "params", ScriptParsing::PARAM_TYPE_JSON,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "Parameters to be passed to the process.");

                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string name = command.getParam("name");
                            const Rsyn::Json params =
                                command.getParam("params");
                            runProcess(name, params);
                    });
        }  // end block

        {  // set
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("set");
                dscp.setDescription("Set a session variable.");

                dscp.addPositionalParam("name",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Name of the session variable.");

                dscp.addPositionalParam("value", ScriptParsing::PARAM_TYPE_JSON,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Value of the session variable.");

                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string name = command.getParam("name");
                            const Rsyn::Json value = command.getParam("value");
                            setSessionVariable(name, value);
                    });
        }  // end block

        {  // open
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("open");
                dscp.setDescription("Open");

                dscp.addPositionalParam(
                    "format", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_MANDATORY, "Benchmark format.");

                dscp.addPositionalParam(
                    "options", ScriptParsing::PARAM_TYPE_JSON,
                    ScriptParsing::PARAM_SPEC_MANDATORY, "Benchmark format.");
                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string format =
                                command.getParam("format");
                            Rsyn::Json options = command.getParam("options");
                            if (getSessionVariableAsBool("globalPlacementOnly",
                                                         false))
                                    options["globalPlacementOnly"] = true;
                            options["path"] = command.getPath();

                            runReader(format, options);
                    });
        }  // end block

        {  // source
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("source");
                dscp.setDescription(
                    "Evaluates a script and executes the commands.");

                dscp.addPositionalParam("fileName",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "The name of the script file.");

                registerCommand(dscp,
                                [&](const ScriptParsing::Command &command) {
                                        const std::string fileName =
                                            command.getParam("fileName");
                                        evaluateFile(fileName);
                                });
        }  // end block

        {  // start
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("start");
                dscp.setDescription("Start a service.");

                dscp.addPositionalParam(
                    "name", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_MANDATORY, "Process name.");

                dscp.addPositionalParam(
                    "params", ScriptParsing::PARAM_TYPE_JSON,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "Parameters to be passed to the process.");

                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string name = command.getParam("name");
                            const Rsyn::Json params =
                                command.getParam("params");
                            startService(name, params);
                    });
        }  // end block

        {  // list
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("list");
                dscp.setDescription(
                    "Listing registered processes, services and readers.");
                dscp.addPositionalParam("name",
                                        ScriptParsing::PARAM_TYPE_STRING,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Parameter should be: \"process\", "
                                        "\"service\" or \"reader\".");
                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            const std::string name = command.getParam("name");
                            if (name.compare("process") == 0) {
                                    listProcess();
                            } else if (name.compare("service") == 0) {
                                    listService();
                            } else if (name.compare("reader") == 0) {
                                    listReader();
                            } else {
                                    std::cout
                                        << "ERROR: invalid list parameter: "
                                        << name << "\n";
                            }  // end if-else
                    });
        }  // end block

        {  // placePort
                ScriptParsing::CommandDescriptor dscp;
                dscp.setName("placePort");
                dscp.setDescription(
                    "Places a port in a user specified position");

                dscp.addPositionalParam(
                    "name", ScriptParsing::PARAM_TYPE_STRING,
                    ScriptParsing::PARAM_SPEC_MANDATORY, "Port name.");

                dscp.addPositionalParam("x", ScriptParsing::PARAM_TYPE_INTEGER,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Position x of the point of interest.");

                dscp.addPositionalParam("y", ScriptParsing::PARAM_TYPE_INTEGER,
                                        ScriptParsing::PARAM_SPEC_MANDATORY,
                                        "Position y of the point of interest.");

                dscp.addPositionalParam(
                    "orientation", ScriptParsing::PARAM_TYPE_INTEGER,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "Port orientation. 0 - N | 1 - S | 2 - W | 3 - E.", "-1");

                dscp.addPositionalParam(
                    "disableSnapping", ScriptParsing::PARAM_TYPE_BOOLEAN,
                    ScriptParsing::PARAM_SPEC_OPTIONAL,
                    "Position y of the point of interest.", "false");

                registerCommand(
                    dscp, [&](const ScriptParsing::Command &command) {
                            Rsyn::PhysicalDesign physicalDesign =
                                getPhysicalDesign();
                            Rsyn::PhysicalOrientation orient;

                            const std::string portName =
                                command.getParam("name");
                            const DBU x = (int)command.getParam("x");
                            const DBU y = (int)command.getParam("y");
                            const int orientation =
                                command.getParam("orientation");
                            const bool disableSnapping =
                                command.getParam("disableSnapping");

                            orient = (Rsyn::PhysicalOrientation)orientation;

                            Rsyn::PhysicalPort phPort;
                            if (physicalDesign.getPhysicalPortByName(portName,
                                                                     phPort)) {
                                    physicalDesign.placePort(
                                        phPort, x, y, orient, disableSnapping);
                            } else {
                                    std::cout << "Port not found!\n";
                            }
                    });
        }  // end block
}  // end method

// -----------------------------------------------------------------------------

void Session::addPath(const std::string &path, const bool prepend) {
        if (prepend) {
                sessionData->clsPaths.push_front(path);
        } else {
                sessionData->clsPaths.push_back(path);
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void Session::removePath(const std::string &path) {
        sessionData->clsPaths.remove(path);
}  // end method

// -----------------------------------------------------------------------------

std::string Session::mergePathAndFileName(const std::string &path,
                                          const std::string &fileName) {
        const char separator = boost::filesystem::path::preferred_separator;
        if (!path.empty() && (path.back() != separator && path.back() != '/')) {
                return path + separator + fileName;
        } else {
                return path + fileName;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

std::string Session::findFile(const std::string fileName,
                              const std::string extraPath) {
        // Check if the file exists without any path.
        if (boost::filesystem::exists(fileName)) {
                return fileName;
        }  // end if

        // Check if the file exists in the extra path.
        if (!extraPath.empty()) {
                const std::string fullFileName =
                    mergePathAndFileName(extraPath, fileName);
                if (boost::filesystem::exists(fullFileName)) {
                        return fullFileName;
                }  // end if
        }          // end if

        // Check if the file exists in the paths.
        for (const std::string &path : sessionData->clsPaths) {
                const std::string fullFileName =
                    mergePathAndFileName(path, fileName);
                if (boost::filesystem::exists(fullFileName)) {
                        return fullFileName;
                }  // end if
        }          // end for

        // File not found.
        return "";
}  // end method

// -----------------------------------------------------------------------------

void Session::unregisterObserver(SessionObserver *observer) {
        for (int i = 0; i < NUM_SESSION_EVENTS; i++) {
                sessionData->observers[i].remove(observer);
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

bool Session::isDesignLoaded() {
        return sessionData->clsDesignLoaded;
}  // end method

// -----------------------------------------------------------------------------

void Session::notifyDesignLoaded() {
        sessionData->clsDesignLoaded = true;

        // Start some default services.
        startService("rsyn.graphics", {}, false);

        // Notify observers.
        for (SessionObserver *observer :
             sessionData->observers[EVENT_DESIGN_LOADED]) {
                observer->onDesignLoaded();
        }  // end for
}  // end methods

// -----------------------------------------------------------------------------

void Session::runReader(const std::string &name, const Rsyn::Json &params) {
        if (isDesignLoaded()) {
                std::cout << "ERROR: Sorry, currently only one reader can be "
                             "called per session.\n";
                return;
        }  // end method

        auto it = sessionData->clsReaders.find(name);
        if (it == sessionData->clsReaders.end()) {
                std::cout << "ERROR: Reader '" << name << "' was not "
                                                          "registered.\n";
        } else {
                std::unique_ptr<Reader> reader(it->second());
                if (reader->load(params)) {
                        notifyDesignLoaded();
                }  // end if
        }          // end else
}  // end method

// -----------------------------------------------------------------------------

bool Session::runProcess(const std::string &name, const Rsyn::Json &params) {
        bool result = false;

        auto it = sessionData->clsProcesses.find(name);
        if (it == sessionData->clsProcesses.end()) {
                std::cout << "ERROR: Process '" << name << "' was not "
                                                           "registered.\n";
        } else {
                std::unique_ptr<Process> opto(it->second());
                result = opto->run(params);
        }  // end else

        return result;
}  // end method

// -----------------------------------------------------------------------------

bool Session::startService(const std::string &name, const Rsyn::Json &params,
                           const bool dontErrorOut) {
        auto it = sessionData->clsServiceInstanciationFunctions.find(name);
        if (it == sessionData->clsServiceInstanciationFunctions.end()) {
                if (!dontErrorOut) {
                        std::cout << "ERROR: Service '" << name
                                  << "' was not "
                                     "registered.\n";
                        std::exit(1);
                }  // end if
                return false;
        } else {
                Service *service = getServiceInternal(name);
                if (!service) {
                        service = it->second();
                        service->start(params);
                        sessionData->clsRunningServices[name] = service;

                        // Notify observers.
                        for (SessionObserver *observer :
                             sessionData->observers[EVENT_SERVICE_STARTED]) {
                                observer->onServiceStarted(name);
                        }  // end for

                        return true;
                } else {
                        if (!dontErrorOut) {
                                std::cout << "WARNING: Service '" << name
                                          << "' is already running.\n";
                        }  // end if
                        return false;
                }  // end else
        }          // end else
}  // end method

} /* namespace Rsyn */
