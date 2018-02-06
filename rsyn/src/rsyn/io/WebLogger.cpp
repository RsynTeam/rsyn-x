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
 
#include "WebLogger.h"
#include <Rsyn/Session>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp> 
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>

namespace Rsyn {

void
WebLogger::start(const Rsyn::Json &params) {
	Rsyn::Session session;

	try {
		const std::string from = session.getInstallationPath() + "/html/Chart.bundle.min.js";
		const std::string to = "./Chart.bundle.min.js";

		boost::filesystem::copy_file(from, to,
				boost::filesystem::copy_option::overwrite_if_exists);
	} catch (...) {
		std::cout << "EXCEPTION: Unable to copy Chart.bundle.min.js.\n";
	} // end catch

	clsContent.open("log.html");

	const std::string html =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<script src='Chart.bundle.min.js'></script>\n"
		"<body>\n";
	renderHtml(html);
} // end method

// -----------------------------------------------------------------------------

void
WebLogger::stop() {
	const std::string html =
		"</body>\n"
		"</html>\n";
	renderHtml(html);

	clsContent.close();
} // end method

// -----------------------------------------------------------------------------

void
WebLogger::renderHtml(const std::string &html) {
	clsContent << html << "\n";
	//clsContent << std::flush;
} // end method

// -----------------------------------------------------------------------------

void
WebLogger::renderLineBreak() {
	renderHtml("</br>");
} // end method

// -----------------------------------------------------------------------------

void
WebLogger::renderHeading(const std::string &text, const int level) {
	renderHtml("<h" + std::to_string(level) + ">" +
			escape(text) + "</h" + std::to_string(level) + ">" );
} // end method

// -----------------------------------------------------------------------------

void
WebLogger::renderText(const std::string &text) {
	renderHtml("<p>" + escape(text) + "</p>");
} // end method

// -----------------------------------------------------------------------------

void
WebLogger::renderChart(const Rsyn::Json &params) {
	Rsyn::Json json = params;
	json["options"] = {{"responsive", false}};

    const boost::uuids::uuid uuid = boost::uuids::random_generator()();
    const std::string id = boost::lexical_cast<std::string>(uuid);

	std::string html;
	html += "<div max-width='640px' max-height='480px'>\n";
	html += "<canvas id='" + id + "' width='640px' height='480px'></canvas>\n";
	html += "</div>\n";
	html += "<script>\n";
	html += "var ctx = document.getElementById('" + id + "').getContext('2d');\n";
	html += "new Chart(ctx,\n";
	html += json.dump();
	html += "\n";
	html += ");\n";
	html += "</script>\n";

	renderHtml(html);
} // end method

// -----------------------------------------------------------------------------

std::string
WebLogger::escape(const std::string &str) const {
	//boost::archive::iterators::xml_escape; // I don't know how to use this :(
	return str;
} // end method

} // end namespace