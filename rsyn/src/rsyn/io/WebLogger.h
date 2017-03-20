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
 
#ifndef RSYN_WEB_LOGGER_H
#define RSYN_WEB_LOGGER_H

#include <fstream>
#include "rsyn/engine/Service.h"

namespace Rsyn {

class WebLogger : public Service {
private:
	std::ofstream clsContent;
public:

	virtual void start(Engine engine, const Json &params);
	virtual void stop();

	// Render a pure html string. No escaping is performed.
	void renderHtml(const std::string &html);

	// Render a new line.
	void renderLineBreak();

	// Render a heading.
	void renderHeading(const std::string &text, const int level);

	// Render a paragraph.
	void renderText(const std::string &text);

	// Render a chart using Chart.js.
	void renderChart(const Json &params);

	// Escape string.
	std::string escape(const std::string &str) const;

}; // end class

} // end namespace


#endif /* WEBLOGGER_H */

