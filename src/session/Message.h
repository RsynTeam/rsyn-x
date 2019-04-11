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

#ifndef RSYN_MESSAGE_H
#define RSYN_MESSAGE_H

#include <unordered_map>
#include <deque>
#include <sstream>

namespace Rsyn {

enum MessageLevel {
        INFO,
        WARNING,
        ERROR,
        DEBUG_MSG,  // added _MSG to avoid conflict with macro DEBUG

        NUM_MESSAGE_LEVELS
};  // end enum

// -----------------------------------------------------------------------------

class MessageManager;
class MessageDescriptor;

// -----------------------------------------------------------------------------

class Message {
        friend class MessageManager;
        friend class MessageDescriptor;

       private:
        //! @note We define these as mutable so the message can be used inside
        //!       const methods.
        mutable MessageManager *clsMessageManager = nullptr;
        mutable MessageDescriptor *clsMessageDescriptor = nullptr;
        mutable std::unordered_map<std::string, std::string> clsMapping;

       public:
        //! @brief Replaces a pattern <pattern> in the message text by a string.
        void replace(const std::string &pattern, const std::string &text) const;

        //! @brief Replaces a pattern <pattern> in the message text by any type
        //!        that can be casted to a string.
        template <typename T>
        void replace(const std::string &pattern, const T &data) const {
                std::ostringstream oss;
                oss << data;
                replace(pattern, oss.str());
        }  // end function

        //! @brief Prints this message (if it is not filtered).
        void print() const;

        //! @brief Returns true if this message will not be printed for any
        //! reason
        //!        (e.g. user disabled it, it reached the maximum number of
        //!        prints).
        bool filtered() const;
};  // end class

// -----------------------------------------------------------------------------

class MessageDescriptor {
       private:
        std::string clsLabel;
        std::string clsTitle;
        std::string clsText;
        MessageLevel clsLevel;
        int clsCounter = 0;
        int clsMaxPrintCount = 10;

       public:
        const std::string &getLabel() const { return clsLabel; }
        const std::string &getTitle() const { return clsTitle; }
        const std::string &getText() const { return clsText; }
        MessageLevel getLevel() const { return clsLevel; }
        int getCounter() const { return clsCounter; }
        int getMaxPrintCount() const { return clsMaxPrintCount; }

        void setLabel(const std::string &label) { clsLabel = label; }
        void setTitle(const std::string &title) { clsTitle = title; }
        void setText(const std::string &text) { clsText = text; }
        void setLevel(const MessageLevel level) { clsLevel = level; }
        void setCounter(const int counter) { clsCounter = counter; }
        void setMaxPrintCount(const int count) { clsMaxPrintCount = count; }

        void incrementCounter() { clsCounter++; }

        bool filtered() const { return clsCounter >= clsMaxPrintCount; }

        //! @brief Replaces the place holders in the message text with the
        //! actual
        //!         values and return the final message text.
        std::string compile(const Message &message) const;

        //! @brief Prints a message if the message is not filtered.
        void print(const Message &message);
};  // end class

// -----------------------------------------------------------------------------

class MessageManager {
        friend class Message;

       private:
        std::unordered_map<std::string, int> clsMapping;
        std::deque<MessageDescriptor> clsMessageDescriptors;

        Message msgNotRegistered;
        Message msgAlreadyRegistered;
        Message msgNotFound;

       public:
        MessageManager();

        void registerMessage(const std::string &label,
                             const MessageLevel &level,
                             const std::string &title,
                             const std::string &msg = "");

        Message getMessage(const std::string &label);

};  // end class

}  // end namespace

#endif
