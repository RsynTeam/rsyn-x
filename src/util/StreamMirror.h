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

#ifndef STREAM_MIRROR_H
#define STREAM_MIRROR_H

#include <iostream>  // std::streambuf, std::cout
#include <fstream>   // std::ofstream

/* StreamMirror allows to write in two std:ostream's at the same time.
 * It was originally designed to mirror the stdout to a log file*/
class StreamMirror : public std::streambuf {
       private:
        std::ostream& sourceStream;
        std::streambuf* sourceBuffer;

        std::ostream& targetStream;
        std::streambuf* targetBuffer;

       public:
        StreamMirror(std::ostream& sourceStream, std::ostream& targetStream)
            : sourceStream(sourceStream), targetStream(targetStream) {
                sourceBuffer = sourceStream.rdbuf();
                targetBuffer = targetStream.rdbuf();

                sourceStream.rdbuf(this);
                targetStream.rdbuf(this);
        }  // end constructor

        ~StreamMirror() {
                sourceBuffer->pubsync();
                targetBuffer->pubsync();
                sourceStream.rdbuf(sourceBuffer);
                targetStream.rdbuf(targetBuffer);
        }  // end destructor

        int overflow(int ch) {
                sourceBuffer->sputc(ch);
                targetBuffer->sputc(ch);
                return ch;
        }  // end overloaded method

        void flush() {
                std::flush(sourceStream);
                std::flush(targetStream);
        }  // end method

};  // end class

#endif