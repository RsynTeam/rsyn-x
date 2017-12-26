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

#include "rsyn/session/Session.h"
#include "ispd18/demo/ExampleService.h"
#include "ispd18/demo/ExampleProcess.h"

bool ExampleProcess::run(const Rsyn::Json &params) {
	Rsyn::Session session;

	Rsyn::Message msg = session.getMessage("DEMO-000");
	msg.replace("label", "world");
	msg.print();

	ExampleService *service = session.getService("example.service");
	service->doNothing();

	return true;
} // end method