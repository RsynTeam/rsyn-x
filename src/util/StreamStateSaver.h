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

#ifndef STREAM_STATE_SAVER_H
#define STREAM_STATE_SAVER_H

#include <iostream>

// Inpired by Boost.

// Helper class to store output stream configuration (flags, precision, width)
// and restore them. Automatically restore the configuration and this class
// is destructred.

class StreamStateSaver {
       private:
        std::ostream &out;
        std::ios_base::fmtflags flags;
        std::streamsize precision;
        std::streamsize width;

       public:
        StreamStateSaver(std::ostream &out) : out(out) {
                flags = out.flags();
                precision = out.precision();
                width = out.width();
        }  // end constructor

        ~StreamStateSaver() { restore(); }  // end destructor

        void restore() {
                out.flags(flags);
                out.precision(precision);
                out.width(width);
        }  // end method

};  // end class

#endif
