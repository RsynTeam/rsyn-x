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
 * Engine.h
 *
 *  Created on: May 8, 2015
 *      Author: jucemar
 */

#ifndef RSYN_ENGINE_H
#define RSYN_ENGINE_H

#include <string>

#include "rsyn/engine/Service.h"
#include "rsyn/engine/Process.h"
#include "rsyn/engine/Reader.h"

#include "rsyn/3rdparty/json/json.hpp"

// To be spun off as a 3rd party...
#include "rsyn/io/parser/script/ScriptCommand.h"

#include "rsyn/core/Rsyn.h"
#include "rsyn/util/Logger.h"
#include "rsyn/util/Units.h"

namespace Rsyn {

typedef nlohmann::json Json;

enum GraphicsEvent {
	GRAPHICS_EVENT_DESIGN_LOADED,
	GRAPHICS_EVENT_REFRESH,
	GRAPHICS_EVENT_UPDATE_OVERLAY_LIST
}; // end enum

enum MessageLevel {
	INFO,
	WARNING,
	ERROR
}; // end enum

typedef std::function<Service *()> ServiceInstantiatonFunction;
typedef std::function<Process *()> ProcessInstantiatonFunction;
typedef std::function<Reader *()> ReaderInstantiatonFunction;

////////////////////////////////////////////////////////////////////////////////
// Startup
////////////////////////////////////////////////////////////////////////////////

// Helper class used to perform component initialization during the application
// startup. Declare a startup object in a cpp file:
//
// Rsyn::Startup startup([]{
//     Rsyn::Engine::registerService(...);
//     Rsyn::Engine::registerProcess(...);
//     Rsyn::Engine::registerMessage(...);
// }); // end startup
//
// This will construct a global object that will be called during the
// application initialization.

class Startup {
public:
	Startup(std::function<void()> f) {
		f();
	} // end constructor
}; // end class

////////////////////////////////////////////////////////////////////////////////
// Engine Data
////////////////////////////////////////////////////////////////////////////////

struct EngineData {
	////////////////////////////////////////////////////////////////////////////
	// Session Variables
	////////////////////////////////////////////////////////////////////////////
	
	std::map<std::string, Json> clsSessionVariables;
	
	////////////////////////////////////////////////////////////////////////////
	// Services
	////////////////////////////////////////////////////////////////////////////
	
	// Generic functions used to instantiate optimizers by name.
	std::unordered_map<std::string, ServiceInstantiatonFunction> clsServiceInstanciationFunctions;
	std::unordered_map<std::string, Service *> clsRunningServices;
	
	////////////////////////////////////////////////////////////////////////////
	// Processes
	////////////////////////////////////////////////////////////////////////////
	
	// Generic functions used to instantiate optimizers by name.
	std::unordered_map<std::string, ProcessInstantiatonFunction> clsProcesses;
	
	////////////////////////////////////////////////////////////////////////////
	// Loader
	////////////////////////////////////////////////////////////////////////////
	
	// Generic functions used to instantiate optimizers by name.
	std::unordered_map<std::string, ReaderInstantiatonFunction> clsReaders;

	////////////////////////////////////////////////////////////////////////////
	// Design
	////////////////////////////////////////////////////////////////////////////

	Rsyn::Design clsDesign;
	Rsyn::Module clsModule;
		
	////////////////////////////////////////////////////////////////////////////
	// Logger
	////////////////////////////////////////////////////////////////////////////
	Logger* logger;
	
	////////////////////////////////////////////////////////////////////////////
	// Configuration
	////////////////////////////////////////////////////////////////////////////
	std::string clsInstallationPath;
	bool clsVerbose = false;
	
	////////////////////////////////////////////////////////////////////////////
	// Script
	////////////////////////////////////////////////////////////////////////////
	ScriptParsing::CommandManager clsCommandManager;
	
	std::function<void(const GraphicsEvent event)> clsGraphicsCallback = nullptr;
}; // end struct

////////////////////////////////////////////////////////////////////////////////
// Engine Proxy
////////////////////////////////////////////////////////////////////////////////
	
class Engine : public Rsyn::Proxy<EngineData> {
public:
	Engine() {};	
	
	Engine(std::nullptr_t) { data = nullptr; }
	
	Engine &operator=(const Engine &other) {
		data = other.data;
		return *this;
	}	
	
//	~Engine();
	
	void create();
	void destroy();

	////////////////////////////////////////////////////////////////////////////
	// Session Variables
	////////////////////////////////////////////////////////////////////////////

public:

	void setSessionVariable(const std::string &name, const Json &value) {
		data->clsSessionVariables[name] = value;
	} // end method

	void unsetSessionVariable(const std::string &name) {
		data->clsSessionVariables.erase(name);
	} // end method

	const bool getSessionVariableAsBool(const std::string &name, const bool defaultValue = false) const {
		auto it = data->clsSessionVariables.find(name);
		return (it != data->clsSessionVariables.end())? it->second.get<bool>() : defaultValue;
	} // end method

	const int getSessionVariableAsInteger(const std::string &name, const int defaultValue = 0) const {
		auto it = data->clsSessionVariables.find(name);
		return (it != data->clsSessionVariables.end())? it->second.get<int>() : defaultValue;
	} // end method

	const float getSessionVariableAsFloat(const std::string &name, const float defaultValue = 0.0f) const {
		auto it = data->clsSessionVariables.find(name);
		return (it != data->clsSessionVariables.end())? it->second.get<float>() : defaultValue;
	} // end method

	const std::string getSessionVariableAsString(const std::string &name, const std::string &defaultValue = "") const {
		auto it = data->clsSessionVariables.find(name);
		return (it != data->clsSessionVariables.end())? it->second.get<std::string>() : defaultValue;
	} // end method

	const Json getSessionVariableAsJson(const std::string &name, const Json &defaultValue = {}) const {
		auto it = data->clsSessionVariables.find(name);
		return (it != data->clsSessionVariables.end())? it->second : defaultValue;
	} // end method
	
	////////////////////////////////////////////////////////////////////////////
	// Services
	////////////////////////////////////////////////////////////////////////////

public:
	
	// Helper class to allow seamless casting from a Service pointer to any type
	// which implements operator=(Service *) is is directly compatible.
	
	class ServiceHandler {
	private:
		Service *clsService;
		
	public:
		ServiceHandler(Service *service) : clsService(service) {}
		
		template<typename T>
		operator T *() {
			T * pointer = dynamic_cast<T *>(clsService);
			if (pointer != clsService) {
				std::cout << "ERROR: Trying to cast a service to the wrong type.\n";
				throw Exception("Trying to cast a service to the wrong type.");
			} // end if
			return pointer;
		} // end operator
	}; // end class
	
	// Register services.
	void registerServices();
	
	// Register a service.
	template<typename T>
	void registerService(const std::string &name) {
		auto it = data->clsServiceInstanciationFunctions.find(name);
		if (it != data->clsServiceInstanciationFunctions.end()) {
			std::cout << "ERROR: Service '" << name << "' was already "
					"registered.\n";
			std::exit(1);
		} else {
			if (data->clsVerbose) {
				std::cout << "Registering service '" << name << "' ...\n";
			} // end if
			data->clsServiceInstanciationFunctions[name] = []() -> Service *{
				return new T();
			};
		} // end else
	} // end method
	
	// Start a service.
	bool startService(const std::string &name, const Json &params = {}) {
		auto it = data->clsServiceInstanciationFunctions.find(name);
		if (it == data->clsServiceInstanciationFunctions.end()) {
			std::cout << "ERROR: Service '" << name << "' was not "
					"registered.\n";
			std::exit(1);
			return false;
		} else {
			Service * service = getServiceInternal(name);
			if (!service) {
				service = it->second();
				service->start(*this, params);
				data->clsRunningServices[name] = service;
				postGraphicsEvent(GRAPHICS_EVENT_UPDATE_OVERLAY_LIST);
				return true;
			} else {
				std::cout << "WARNING: Service '" << name << "' is already running.\n";
				return false;
			} // end else
		} // end else
	} // end method
	
	// Gets a running service.
	ServiceHandler getService(const std::string &name, 
			const ServiceRequestType requestType = SERVICE_MANDATORY) {
		Service *service = getServiceInternal(name);
		if (!service && (requestType == SERVICE_MANDATORY)) {
			std::cout << "ERROR: Service '" << name << "' was not started.\n";
			throw Exception("ERROR: Service '" + name + "' was not started");
		} // end if
		return ServiceHandler(service);
	} // end method
	
	// Checks if a service is running.
	bool isServiceRunning(const std::string &name) {
		return getServiceInternal(name) != nullptr;
	} // end method
	
private:
	
	Service * getServiceInternal(const std::string &name) {
		auto it = data->clsRunningServices.find(name);
		return it == data->clsRunningServices.end()? nullptr : it->second;
	} // end method

	void listService(std::ostream & out = std::cout) {
		out<<"List of services ";
		out<<"([R] -> Running, [S] -> Stopped):\n";
		// print only running services
		for (std::pair<std::string, ServiceInstantiatonFunction> srv : data->clsServiceInstanciationFunctions) {
			if (!isServiceRunning(srv.first))
				continue;
			out << "\t[R] " << srv.first << "\n";
		} // end for 
		// print only stopped services 
		for (std::pair<std::string, ServiceInstantiatonFunction> srv : data->clsServiceInstanciationFunctions) {
			if (isServiceRunning(srv.first))
				continue;
			out << "\t[S] "<<srv.first << "\n";
		} // end for 
		out << "--------------------------------------\n";
	} /// end method 
	
	////////////////////////////////////////////////////////////////////////////
	// Processes
	////////////////////////////////////////////////////////////////////////////
private:
	void listProcess(std::ostream & out = std::cout) {
		out<<"List of registered processes:\n";
		for(std::pair<std::string, ProcessInstantiatonFunction> process : data->clsProcesses) {
			out<<"\t"<<process.first<<"\n";
		} // end for 
		out<<"--------------------------------------\n";
	} /// end method 
public:
	
	// Register processes.
	void registerProcesses();
	
	// Register a process.
	template<typename T>
	void registerProcess(const std::string &name) {
		auto it = data->clsProcesses.find(name);
		if (it != data->clsProcesses.end()) {
			std::cout << "ERROR: Process '" << name << "' was already "
					"registered.\n";
			std::exit(1);
		} else {
			if (data->clsVerbose) {
				std::cout << "Registering process '" << name << "' ...\n";
			} // end if
			data->clsProcesses[name] = []() -> Process *{
				return new T();
			};
		} // end else
	} // end method
	
	// Run an optimizer.
	bool runProcess(const std::string &name, const Json &params = {}) {
		bool result = false;
		
		auto it = data->clsProcesses.find(name);
		if (it == data->clsProcesses.end()) {
			std::cout << "ERROR: Process '" << name << "' was not "
					"registered.\n";
		} else {
			std::unique_ptr<Process> opto(it->second());
			result = opto->run(*this, params);
		} // end else
		
		return result;
	} // end method
		
	////////////////////////////////////////////////////////////////////////////
	// Loader
	////////////////////////////////////////////////////////////////////////////
private:
	void listReader(std::ostream & out = std::cout) {
		out<<"List of registered Readers:\n";
		for(std::pair<std::string, ReaderInstantiatonFunction> reader : data->clsReaders) {
			out<<"\t"<<reader.first<<"\n";
		} // end for 
		out<<"--------------------------------------\n";
	} /// end method
public:
	
	// Register loader.
	void registerReaders();
	
	// Register a loader.
	template<typename T>
	void registerReader(const std::string &name) {
		auto it = data->clsReaders.find(name);
		if (it != data->clsReaders.end()) {
			std::cout << "ERROR: Reader '" << name << "' was already "
					"registered.\n";
			std::exit(1);
		} else {
			if (data->clsVerbose) {
				std::cout << "Registering reader '" << name << "' ...\n";
			} // end if
			data->clsReaders[name] = []() -> Reader *{
				return new T();
			};
		} // end else
	} // end method
	
	// Run an loader.
	void runReader(const std::string &name, const Json &params = {}) {
		auto it = data->clsReaders.find(name);
		if (it == data->clsReaders.end()) {
			std::cout << "ERROR: Reader '" << name << "' was not "
					"registered.\n";
		} else {
			std::unique_ptr<Reader> opto(it->second());
			opto->load(*this, params);
			postGraphicsEvent(GRAPHICS_EVENT_DESIGN_LOADED);
		} // end else
	} // end method

	////////////////////////////////////////////////////////////////////////////
	// Messages
	////////////////////////////////////////////////////////////////////////////

public:

	static void registerMessage(const std::string &label, const MessageLevel &level, const std::string &title, const std::string &msg);

	////////////////////////////////////////////////////////////////////////////
	// Misc
	////////////////////////////////////////////////////////////////////////////

	Rsyn::Design getDesign() { return data->clsDesign; }

	const std::string &getInstallationPath() const { return data->clsInstallationPath; }

	////////////////////////////////////////////////////////////////////////////
	// Script
	////////////////////////////////////////////////////////////////////////////
private:

	void registerDefaultCommands();
	
public:	

	typedef std::function<void(Engine engine, const ScriptParsing::Command &command)> CommandHandler;
	
	void registerCommand(const ScriptParsing::CommandDescriptor &dscp, const CommandHandler handler);
	
	void evaluateString(const std::string &str);
	void evaluateFile(const std::string &filename);

	ScriptParsing::CommandManager &getCommandManager() {
		return data->clsCommandManager;
	} // end method

	////////////////////////////////////////////////////////////////////////////
	// Graphics
	////////////////////////////////////////////////////////////////////////////
public:

	void registerGraphicsCallback(const std::function<void(const GraphicsEvent event)> &callback) {
		data->clsGraphicsCallback = callback;
	} // end method

	void postGraphicsEvent(const GraphicsEvent event) {
		if (data->clsGraphicsCallback)
			data->clsGraphicsCallback(event);
	} // end method

}; // end class

} // end namespace

#endif /* INFRA_ICCAD15_ENGINE_H_ */
