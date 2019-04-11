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

#include <iostream>
#include <regex>

#include "Message.h"

namespace Rsyn {

static const std::string MESSAGE_LEVEL_TEXT[NUM_MESSAGE_LEVELS] = {
    "INFO", "WARNING", "ERROR", "DEBUG"};

}  // end namespace

////////////////////////////////////////////////////////////////////////////////
// Message
////////////////////////////////////////////////////////////////////////////////

namespace Rsyn {

void Message::replace(const std::string &pattern,
                      const std::string &text) const {
        clsMapping[pattern] = text;
}  // end if

// -----------------------------------------------------------------------------

void Message::print() const {
        if (clsMessageDescriptor) clsMessageDescriptor->print(*this);
}  // end if

// -----------------------------------------------------------------------------

bool Message::filtered() const {
        return clsMessageDescriptor ? clsMessageDescriptor->filtered() : true;
}  // end if

}  // end namespace

////////////////////////////////////////////////////////////////////////////////
// MessageDescriptor
////////////////////////////////////////////////////////////////////////////////

namespace Rsyn {

std::string MessageDescriptor::compile(const Message &message) const {
        std::string result = getText();
        for (auto it : message.clsMapping) {
                const std::string &pattern = "<" + it.first + ">";
                const std::string &str = it.second;
                result = std::regex_replace(result, std::regex(pattern), str);
        }  // end for

        return result;
}  // end method

// -----------------------------------------------------------------------------

void MessageDescriptor::print(const Message &message) {
        MessageDescriptor *dscp = message.clsMessageDescriptor;
        if (dscp != this) {
                return;
        }  // end if

        if (!filtered()) {
                const std::string msg = compile(message);
                std::cout << MESSAGE_LEVEL_TEXT[dscp->getLevel()] << ": " << msg
                          << " " << getLabel() << "\n";
        } else if (clsCounter == clsMaxPrintCount) {
                std::cout << "Message " << getLabel()
                          << " reached its maximum print count and will be "
                             "suppressed hereafter.\n";
        }  // end else if

        clsCounter++;
}  // end method

}  // end namespace

////////////////////////////////////////////////////////////////////////////////
// MessageManager
////////////////////////////////////////////////////////////////////////////////

namespace Rsyn {

MessageManager::MessageManager() {
        registerMessage("MSG-000", WARNING, "Invalid message.",
                        "Message <label> is not registered.");
        registerMessage("MSG-001", WARNING, "Message is already registered.",
                        "Message <label> is already registered.");
        registerMessage("MSG-002", WARNING, "Message is not found.",
                        "Message <label> not found.");

        msgNotRegistered = getMessage("MSG-000");
        msgAlreadyRegistered = getMessage("MSG-001");
        msgNotFound = getMessage("MSG-002");
}  // end method

// -----------------------------------------------------------------------------

void MessageManager::registerMessage(const std::string &label,
                                     const MessageLevel &level,
                                     const std::string &title,
                                     const std::string &msg) {
        MessageDescriptor dscp;

        if (clsMapping.count(label)) {
                msgAlreadyRegistered.replace("label", label);
                msgAlreadyRegistered.print();
        } else {
                dscp.setLabel(label);
                dscp.setLevel(level);
                dscp.setTitle(title);
                dscp.setText(msg.empty() ? title : msg);

                clsMapping[label] = clsMessageDescriptors.size();
                clsMessageDescriptors.push_back(dscp);
        }  // end if
}  // end method

// -----------------------------------------------------------------------------

Message MessageManager::getMessage(const std::string &label) {
        auto it = clsMapping.find(label);
        if (it == clsMapping.end()) {
                msgNotFound.replace("label", label);
                msgNotFound.print();
                return msgNotRegistered;
        } else {
                Message message;
                message.clsMessageDescriptor =
                    &clsMessageDescriptors[it->second];
                message.clsMessageManager = this;
                return message;
        }  // end else
}  // end method

}  // end namespace