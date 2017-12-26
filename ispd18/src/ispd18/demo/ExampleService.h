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

#ifndef EXAMPLE_SERVICE_H
#define EXAMPLE_SERVICE_H

#include <iostream>

#include "rsyn/core/Rsyn.h"
#include "rsyn/session/Service.h"
#include "rsyn/phy/PhysicalDesign.h"

namespace Rsyn {
class Session;
};

class ExampleService : public Rsyn::Service {
public:

	virtual void start(const Rsyn::Json &params);
	virtual void stop();

	void doNothing();

private:

	Rsyn::Design design;
	Rsyn::Module module; // top module

}; // end class

#endif