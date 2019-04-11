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

#include "FastPlace.h"

#include "tool/timing/Timer.h"
#include "io/writer/Writer.h"

#include "util/AsciiProgressBar.h"
#include "util/Stepwatch.h"
#include <boost/algorithm/string.hpp>

#include "util/Json.h"

#include <cstdio>

namespace ICCAD15 {

bool FastPlace::run(const Rsyn::Json &params) {
        this->session = session;
        this->design = session.getDesign();
        this->timer = session.getService("rsyn.timer");
        this->writer = session.getService("rsyn.writer");
        this->module = design.getTopModule();

        const std::string &designName = session.getDesign().getName();
        const std::string tmp = createTemporaryDirectory();

        writer->writeBookshelf2(tmp);

        std::string cmd;
        cmd += session.getInstallationPath() + "/bin/FastPlace3.0_Linux32_GP ";
        cmd += tmp + " ";
        cmd += designName + ".aux ";
        cmd += tmp;

        std::cout << "Temporary directory:\n" << tmp << "\n";
        std::cout << "Command line:\n" << cmd << "\n";
        exec(cmd);

        Rsyn::Json config;
        config["path"] = tmp + "/" + designName + "_FP_gp.pl";
        session.runReader("loadDesignPosition", config);

        return true;
}  // end method

// -----------------------------------------------------------------------------

std::string FastPlace::createTemporaryDirectory() {
        FILE *in;
        char buff[1024];

        std::string result;

        if (!(in = popen("mktemp -d", "r"))) {
                return "";
        }  // end if
        while (fgets(buff, sizeof(buff), in) != NULL) {
                result += buff;
        }  // end while
        pclose(in);

        boost::trim(result);
        return result;
}  // end method

// -----------------------------------------------------------------------------

bool FastPlace::exec(const std::string &cmd, std::ostream &out) {
        FILE *in;
        char buff[1024];

        if (!(in = popen((cmd + " 2>&1").c_str(), "r"))) {
                return false;
        }  // end if
        while (fgets(buff, sizeof(buff), in) != NULL) {
                out << buff;
        }  // end while
        pclose(in);

        return true;
}  // end method

}  // end namescape
