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
 
#ifndef PROPAGATION_HANDLER_H
#define PROPAGATION_HANDLER_H

#ifndef WX_PRECOMP
#include <wx/wx.h>
#else
#include "wxprecomp.h"
#endif

class PropagationHandler : public wxEvtHandler
{
public:
    enum PropFlags
    {   // warning: these are meant as bitflags not incremental integer values
        Key_Flag           = 0x0001, // propagate all key events
        Mouse_Flag         = 0x0002, // propagate all mouse events
        Key_And_Mouse_Flag = 0x0003, // propagate all key and mouse events
        SomeOther_Flag     = 0x0004, // propagate all other events
        All_Flags          = 0xFFFF  // propagate everything
    };
public:
    PropagationHandler (PropFlags propwhat = All_Flags)
    {
        if (propwhat & Key_Flag)
        {   // add key events to propagate
            Connect (wxEVT_KEY_DOWN, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_KEY_UP, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            // add char events to propagate (is this usefull?)
            Connect (wxEVT_CHAR, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
        }
        if (propwhat & Mouse_Flag)
        {   // add mouse events to propagate
            Connect (wxEVT_ENTER_WINDOW, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_LEAVE_WINDOW, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_LEFT_DOWN, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_LEFT_UP, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_LEFT_DCLICK, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_MIDDLE_DOWN, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_MIDDLE_UP, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_MIDDLE_DCLICK, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_RIGHT_DOWN, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_RIGHT_UP, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_RIGHT_DCLICK, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_MOTION, wxObjectEventFunction (&PropagationHandler::PropagateEvent));
            Connect (wxEVT_MOUSEWHEEL, wxObjectEventFunction         (&PropagationHandler::PropagateEvent));
        }
        if (propwhat & SomeOther_Flag)
        {   // is there something else?
        }
    }
private:
    void PropagateEvent (wxEvent &event)
    {   // set events propagationlevel to run down through the parents
        event.ResumePropagation (wxEVENT_PROPAGATE_MAX);
        event.Skip (); // continue the event             
    }
};

#endif
