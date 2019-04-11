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

#ifndef RSYN_SESSION_H
#define RSYN_SESSION_H

#include <string>

#include "Service.h"
#include "Process.h"
#include "Reader.h"
#include "Message.h"

#include "util/Json.h"

// To be spun off as a 3rd party...
#include "script/ScriptCommand.h"

#include "core/Rsyn.h"
#include "util/Logger.h"
#include "util/Units.h"

#include "SessionObserver.h"

namespace Rsyn {

class PhysicalDesign;

typedef std::function<Service *()> ServiceInstantiatonFunction;
typedef std::function<Process *()> ProcessInstantiatonFunction;
typedef std::function<Reader *()> ReaderInstantiatonFunction;

////////////////////////////////////////////////////////////////////////////////
// Session Data
////////////////////////////////////////////////////////////////////////////////

struct SessionData {
        Message msgMessageRegistrationFail;

        ////////////////////////////////////////////////////////////////////////////
        // Session Variables
        ////////////////////////////////////////////////////////////////////////////

        std::map<std::string, Rsyn::Json> clsSessionVariables;

        ////////////////////////////////////////////////////////////////////////////
        // Message System
        ////////////////////////////////////////////////////////////////////////////

        MessageManager clsMessageManager;

        ////////////////////////////////////////////////////////////////////////////
        // Services
        ////////////////////////////////////////////////////////////////////////////

        // Generic functions used to instantiate optimizers by name.
        std::unordered_map<std::string, ServiceInstantiatonFunction>
            clsServiceInstanciationFunctions;
        std::unordered_map<std::string, Service *> clsRunningServices;

        ////////////////////////////////////////////////////////////////////////////
        // Processes
        ////////////////////////////////////////////////////////////////////////////

        // Generic functions used to instantiate optimizers by name.
        std::unordered_map<std::string, ProcessInstantiatonFunction>
            clsProcesses;

        ////////////////////////////////////////////////////////////////////////////
        // Loader
        ////////////////////////////////////////////////////////////////////////////

        // Generic functions used to instantiate optimizers by name.
        std::unordered_map<std::string, ReaderInstantiatonFunction> clsReaders;

        ////////////////////////////////////////////////////////////////////////////
        // Observers
        ////////////////////////////////////////////////////////////////////////////

        std::array<std::list<SessionObserver *>, NUM_SESSION_EVENTS> observers;
        bool clsDesignLoaded = false;

        ////////////////////////////////////////////////////////////////////////////
        // Design/Library
        ////////////////////////////////////////////////////////////////////////////

        Rsyn::Design clsDesign;
        Rsyn::Library clsLibrary;

        ////////////////////////////////////////////////////////////////////////////
        // Logger
        ////////////////////////////////////////////////////////////////////////////
        // @todo remove
        Logger *logger;

        ////////////////////////////////////////////////////////////////////////////
        // Configuration
        ////////////////////////////////////////////////////////////////////////////
        std::string clsInstallationPath;
        bool clsVerbose = false;

        ////////////////////////////////////////////////////////////////////////////
        // Script
        ////////////////////////////////////////////////////////////////////////////
        ScriptParsing::CommandManager clsCommandManager;

        ////////////////////////////////////////////////////////////////////////////
        // Misc
        ////////////////////////////////////////////////////////////////////////////
        std::list<std::string> clsPaths;
};  // end struct

////////////////////////////////////////////////////////////////////////////////
// Session Proxy
////////////////////////////////////////////////////////////////////////////////

class Session : public Rsyn::Proxy<SessionData> {
       public:
        Session() { data = sessionData; };

        Session(std::nullptr_t) { data = nullptr; }

        Session &operator=(const Session &other) {
                data = other.data;
                return *this;
        }

        static void init();

        //! @note To prevent "static variable order initialization fiasco", the
        //        static variable signaling that the engine was initialized is
        //        stored inside this function. In this way, we can guarantee it
        //        will
        //        be initialized to false before being used.
        static bool checkInitialized(const bool markAsInitialized = false) {
                static bool sessionInitialized = false;
                if (markAsInitialized) sessionInitialized = true;
                return sessionInitialized;
        }  // end method

       private:
        static SessionData *sessionData;

        ////////////////////////////////////////////////////////////////////////////
        // Session Variables
        ////////////////////////////////////////////////////////////////////////////

       public:
        static void setSessionVariable(const std::string &name,
                                       const Rsyn::Json &value) {
                sessionData->clsSessionVariables[name] = value;
        }  // end method

        static void unsetSessionVariable(const std::string &name) {
                sessionData->clsSessionVariables.erase(name);
        }  // end method

        static const bool getSessionVariableAsBool(
            const std::string &name, const bool defaultValue = false) {
                auto it = sessionData->clsSessionVariables.find(name);
                return (it != sessionData->clsSessionVariables.end())
                           ? it->second.get<bool>()
                           : defaultValue;
        }  // end method

        static const int getSessionVariableAsInteger(
            const std::string &name, const int defaultValue = 0) {
                auto it = sessionData->clsSessionVariables.find(name);
                return (it != sessionData->clsSessionVariables.end())
                           ? it->second.get<int>()
                           : defaultValue;
        }  // end method

        static const float getSessionVariableAsFloat(
            const std::string &name, const float defaultValue = 0.0f) {
                auto it = sessionData->clsSessionVariables.find(name);
                return (it != sessionData->clsSessionVariables.end())
                           ? it->second.get<float>()
                           : defaultValue;
        }  // end method

        static const std::string getSessionVariableAsString(
            const std::string &name, const std::string &defaultValue = "") {
                auto it = sessionData->clsSessionVariables.find(name);
                return (it != sessionData->clsSessionVariables.end())
                           ? it->second.get<std::string>()
                           : defaultValue;
        }  // end method

        static const Rsyn::Json getSessionVariableAsJson(
            const std::string &name, const Rsyn::Json &defaultValue = {}) {
                auto it = sessionData->clsSessionVariables.find(name);
                return (it != sessionData->clsSessionVariables.end())
                           ? it->second
                           : defaultValue;
        }  // end method

        ////////////////////////////////////////////////////////////////////////////
        // Services
        ////////////////////////////////////////////////////////////////////////////

        // Register services.
        static void registerServices();

       public:
        // Helper class to allow seamless casting from a Service pointer to any
        // type
        // which implements operator=(Service *) is is directly compatible.

        class ServiceHandler {
               private:
                Service *clsService;

               public:
                ServiceHandler(Service *service) : clsService(service) {}

                template <typename T>
                operator T *() {
                        T *pointer = dynamic_cast<T *>(clsService);
                        if (pointer != clsService) {
                                std::cout << "ERROR: Trying to cast a service "
                                             "to the wrong type.\n";
                                throw Exception(
                                    "Trying to cast a service to the wrong "
                                    "type.");
                        }  // end if
                        return pointer;
                }  // end operator
        };         // end class

        // Register a service.
        template <typename T>
        static void registerService(const std::string &name) {
                auto it =
                    sessionData->clsServiceInstanciationFunctions.find(name);
                if (it != sessionData->clsServiceInstanciationFunctions.end()) {
                        std::cout << "ERROR: Service '" << name
                                  << "' was already "
                                     "registered.\n";
                        std::exit(1);
                } else {
                        if (sessionData->clsVerbose) {
                                std::cout << "Registering service '" << name
                                          << "' ...\n";
                        }  // end if
                        sessionData->clsServiceInstanciationFunctions[name] =
                            []() -> Service * { return new T(); };
                }  // end else
        }          // end method

        // Start a service.
        static bool startService(const std::string &name,
                                 const Rsyn::Json &params = {},
                                 const bool dontErrorOut = false);

        // Gets a running service.
        static ServiceHandler getService(
            const std::string &name,
            const ServiceRequestType requestType = SERVICE_MANDATORY) {
                Service *service = getServiceInternal(name);
                if (!service && (requestType == SERVICE_MANDATORY)) {
                        std::cout << "ERROR: Service '" << name
                                  << "' was not started.\n";
                        throw Exception("ERROR: Service '" + name +
                                        "' was not started");
                }  // end if
                return ServiceHandler(service);
        }  // end method

        // Checks if a service is registered.
        static bool isServiceRegistered(const std::string &name) {
                auto it =
                    sessionData->clsServiceInstanciationFunctions.find(name);
                return (it !=
                        sessionData->clsServiceInstanciationFunctions.end());
        }  // end method

        // Checks if a service is running.
        static bool isServiceRunning(const std::string &name) {
                return getServiceInternal(name) != nullptr;
        }  // end method

       private:
        static Service *getServiceInternal(const std::string &name) {
                auto it = sessionData->clsRunningServices.find(name);
                return it == sessionData->clsRunningServices.end() ? nullptr
                                                                   : it->second;
        }  // end method

        static void listService(std::ostream &out = std::cout) {
                out << "List of services ";
                out << "([R] -> Running, [S] -> Stopped):\n";
                // print only running services
                for (std::pair<std::string, ServiceInstantiatonFunction> srv :
                     sessionData->clsServiceInstanciationFunctions) {
                        if (!isServiceRunning(srv.first)) continue;
                        out << "\t[R] " << srv.first << "\n";
                }  // end for
                // print only stopped services
                for (std::pair<std::string, ServiceInstantiatonFunction> srv :
                     sessionData->clsServiceInstanciationFunctions) {
                        if (isServiceRunning(srv.first)) continue;
                        out << "\t[S] " << srv.first << "\n";
                }  // end for
                out << "--------------------------------------\n";
        }  /// end method

        ////////////////////////////////////////////////////////////////////////////
        // Processes
        ////////////////////////////////////////////////////////////////////////////
       private:
        static void listProcess(std::ostream &out = std::cout) {
                out << "List of registered processes:\n";
                for (std::pair<std::string, ProcessInstantiatonFunction>
                         process : sessionData->clsProcesses) {
                        out << "\t" << process.first << "\n";
                }  // end for
                out << "--------------------------------------\n";
        }  // end method

        // Register processes.
        static void registerProcesses();

       public:
        // Register a process.
        template <typename T>
        static void registerProcess(const std::string &name) {
                auto it = sessionData->clsProcesses.find(name);
                if (it != sessionData->clsProcesses.end()) {
                        std::cout << "ERROR: Process '" << name
                                  << "' was already "
                                     "registered.\n";
                        std::exit(1);
                } else {
                        if (sessionData->clsVerbose) {
                                std::cout << "Registering process '" << name
                                          << "' ...\n";
                        }  // end if
                        sessionData->clsProcesses[name] = []() -> Process * {
                                return new T();
                        };
                }  // end else
        }          // end method

        // Run an optimizer.
        static bool runProcess(const std::string &name,
                               const Rsyn::Json &params = {});

        ////////////////////////////////////////////////////////////////////////////
        // Reader
        ////////////////////////////////////////////////////////////////////////////
       private:
        static void listReader(std::ostream &out = std::cout) {
                out << "List of registered Readers:\n";
                for (std::pair<std::string, ReaderInstantiatonFunction> reader :
                     sessionData->clsReaders) {
                        out << "\t" << reader.first << "\n";
                }  // end for
                out << "--------------------------------------\n";
        }  /// end method

        // Register loader.
        static void registerReaders();

       public:
        // Register a loader.
        template <typename T>
        static void registerReader(const std::string &name) {
                auto it = sessionData->clsReaders.find(name);
                if (it != sessionData->clsReaders.end()) {
                        std::cout << "ERROR: Reader '" << name
                                  << "' was already "
                                     "registered.\n";
                        std::exit(1);
                } else {
                        if (sessionData->clsVerbose) {
                                std::cout << "Registering reader '" << name
                                          << "' ...\n";
                        }  // end if
                        sessionData->clsReaders[name] = []() -> Reader * {
                                return new T();
                        };
                }  // end else
        }          // end method

        // Run an loader.
        static void runReader(const std::string &name,
                              const Rsyn::Json &params = {});

        ////////////////////////////////////////////////////////////////////////////
        // Messages
        ////////////////////////////////////////////////////////////////////////////

       private:
        static void registerMessages();

       public:
        static void registerMessage(const std::string &label,
                                    const MessageLevel &level,
                                    const std::string &title,
                                    const std::string &msg = "");
        static Message getMessage(const std::string &label);

        ////////////////////////////////////////////////////////////////////////////
        // Misc
        ////////////////////////////////////////////////////////////////////////////

        static Rsyn::Design getDesign();
        static Rsyn::Library getLibrary();
        static Rsyn::Module getTopModule();
        static Rsyn::PhysicalDesign getPhysicalDesign();

        static const std::string &getInstallationPath() {
                return sessionData->clsInstallationPath;
        }

        ////////////////////////////////////////////////////////////////////////////
        // Script
        ////////////////////////////////////////////////////////////////////////////
       private:
        static void registerDefaultCommands();

       public:
        typedef std::function<void(const ScriptParsing::Command &command)>
            CommandHandler;

        static void registerCommand(
            const ScriptParsing::CommandDescriptor &dscp,
            const CommandHandler handler);

        static void evaluateString(const std::string &str);
        static void evaluateFile(const std::string &filename);

        static ScriptParsing::CommandManager &getCommandManager() {
                return sessionData->clsCommandManager;
        }  // end method

        ////////////////////////////////////////////////////////////////////////////
        // Observer
        ////////////////////////////////////////////////////////////////////////////
       public:
        //! @brief Registers an observer to be notified about changes in the
        //!        session.
        template <class T>
        static void registerObserver(T *observer);

        //! @brief Unregisters an observer so it will no longer receives
        //!        notifications about changes in the session.
        static void unregisterObserver(SessionObserver *observer);

        //! @brief Returns true if the design was already loaded.
        static bool isDesignLoaded();

        //! @brief Notify session observers that the design has been loaded.
        static void notifyDesignLoaded();

        ////////////////////////////////////////////////////////////////////////////
        // Utilities
        ////////////////////////////////////////////////////////////////////////////

       private:
        static void addPath(const std::string &path, const bool prepend);
        static void removePath(const std::string &path);
        static std::string mergePathAndFileName(const std::string &path,
                                                const std::string &fileName);

       public:
        //! @brief Find a file in the current path. If found, returns its
        //! absolute
        //!        path, an empty string otherwise.
        //! @param extraPath can be used to specify an extra path location
        //! besides
        //!        the one stored internally in the current path list.
        std::string findFile(const std::string fileName,
                             const std::string extraPath = "");

};  // end class

// -----------------------------------------------------------------------------

template <class T>
inline void Session::registerObserver(T *observer) {
        static_assert(std::is_base_of<SessionObserver, T>::value,
                      "Unable to register class as observer. "
                      "The observer class must inherit from Rsyn::Observer.");

        // Check if the observer implements (overwrites) the event callbacks if
        // so
        // register it to receive notifications of the respective event.

        if (typeid(&SessionObserver::onDesignLoaded) !=
            typeid(&T::onDesignLoaded)) {
                sessionData->observers[EVENT_DESIGN_LOADED].push_back(observer);
        }  // end if

        if (typeid(&SessionObserver::onServiceStarted) !=
            typeid(&T::onServiceStarted)) {
                sessionData->observers[EVENT_SERVICE_STARTED].push_back(
                    observer);
        }  // end if

}  // end method

////////////////////////////////////////////////////////////////////////////////
// Startup
////////////////////////////////////////////////////////////////////////////////

// Helper class used to perform component initialization during the application
// startup. Declare a startup object in a cpp file:
//
// Rsyn::Startup startup([]{
//     Rsyn::Session::registerService(...);
//     Rsyn::Session::registerProcess(...);
//     Rsyn::Session::registerMessage(...);
// }); // end startup
//
// This will construct a global object that will be called during the
// application initialization.

class Startup {
       public:
        Startup(std::function<void()> f) {
                if (!Session::checkInitialized()) Session::init();
                f();
        }  // end constructor
};         // end class

}  // end namespace

#endif /* INFRA_ICCAD15_SESSION_H_ */
