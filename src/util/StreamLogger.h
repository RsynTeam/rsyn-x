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

#ifndef RSYN_STREAM_LOGGER_H
#define RSYN_STREAM_LOGGER_H

#include <tuple>
#include <vector>
#include <deque>
#include <ostream>
#include <string>
#include <functional>
#include <mutex>

namespace Rsyn {

class StreamLogger : public std::streambuf {
       public:
        typedef std::function<void()> Callback;

        static StreamLogger *get() {
                if (!instance) instance = new StreamLogger();
                return instance;
        }  // end method

        void capture(std::ostream &out);

        std::string getLog() {
                int dummy;
                return getLog(0, dummy);
        }  // end method

        std::string getLog(const int startFromLine, int &endedAtLine) {
                std::lock_guard<std::mutex> guard(moo);
                std::string log;
                bool first = true;
                const int numLines =
                    ((int)lines.size() -
                     1);  // -1 as the current last line is not finished yet
                for (int i = std::max(0, startFromLine); i < numLines; i++) {
                        if (!first)
                                log += '\n';
                        else
                                first = false;
                        log += lines[i];
                }  // end for
                endedAtLine = numLines;
                return log;
        }  // end method

        const int getLastLineIndex() const {
                std::lock_guard<std::mutex> guard(moo);
                return lines.size() - 1;
        }  // end method

        void addCallback(const Callback &callback) {
                callbacks.push_back(callback);
        }  // end method

        ~StreamLogger() {
                for (auto &t : streams) {
                        std::get<1>(t)->pubsync();
                        std::get<0>(t)->rdbuf(std::get<1>(t));
                }  // end for
        }          // end destructor

        int overflow(int ch) {
                std::lock_guard<std::mutex> guard(moo);
                for (auto &t : streams) {
                        std::get<1>(t)->sputc(ch);
                }  // end for
                if (ch == '\n') {
                        for (auto &f : callbacks) {
                                f();
                        }  // end for
                        lines.push_back("");
                } else if (ch == '\r') {
                        lines.back().clear();
                } else {
                        lines.back().push_back((char)ch);
                }  // end else
                return ch;
        }  // end overloaded method

        void flush() {
                for (auto &t : streams) {
                        std::get<0>(t)->flush();
                }  // end for
        }          // end method

       private:
        StreamLogger() { lines.assign(1, ""); }  // end constructor

        static StreamLogger *instance;

        mutable std::mutex moo;

        std::deque<std::string> lines;
        std::vector<std::tuple<std::ostream *, std::streambuf *>> streams;
        std::vector<Callback> callbacks;

};  // end class

}  // end namespace

#endif /* STREAMLOGGER_H */
