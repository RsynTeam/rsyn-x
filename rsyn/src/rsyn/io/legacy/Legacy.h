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
 
#ifndef RSYN_LEGACY_H
#define RSYN_LEGACY_H

#include <string>

#include "rsyn/core/Rsyn.h"

namespace Legacy {

inline Rsyn::Direction lefPinDirectionFromString(const std::string &direction) {
	if (direction == "INPUT"   ) return Rsyn::IN;
	if (direction == "OUTPUT"  ) return Rsyn::OUT;
	if (direction == "INOUT"   ) return Rsyn::BIDIRECTIONAL;
	if (direction == "FEEDTHRU") return Rsyn::UNKNOWN_DIRECTION;
	return Rsyn::UNKNOWN_DIRECTION;
} // end function

inline Rsyn::Direction bookshelfPinDirectionFromString(const std::string &direction) {
	if (direction == "I") return Rsyn::IN;
	if (direction == "O") return Rsyn::OUT;
	if (direction == "B") return Rsyn::OUT; // Due to Rsyn restrictions, I am considering the Bidirectional is only output.
	return Rsyn::UNKNOWN_DIRECTION;
} // end function

inline std::string bookshelfPinDirectionToString(const Rsyn::Direction direction) {
	if (direction == Rsyn::IN) return "I";
	if (direction == Rsyn::OUT) return "O";
	if (direction == Rsyn::BIDIRECTIONAL) return "B";
	return "<DIRECTION_UNKNOWN>";
} // end function

} // end namespace

#endif