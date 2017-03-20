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
 
#ifndef RSYN_WX_APP_H
#define RSYN_WX_APP_H

#ifndef WX_PRECOMP
#include <wx/wx.h>
#else
#include "wxprecomp.h"
#endif

#include <string>

// -----------------------------------------------------------------------------

class MyApp: public wxApp {
private:
	virtual bool OnInit();
	static std::string clsScript;
	static std::string clsGui;
public:
	static void Init(const std::string &gui, const std::string &script);
}; // end class

#endif /* APP_H */

