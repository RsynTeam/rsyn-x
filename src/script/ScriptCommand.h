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

#ifndef RSYN_SCRIPT_COMMAND_H
#define RSYN_SCRIPT_COMMAND_H

#include <string>
#include <iostream>
#include <regex>
#include <exception>

#include "ScriptReader.h"
#include "util/Json.h"

namespace Rsyn {
class Engine;
}

namespace ScriptParsing {
class ScriptReader;
}

namespace ScriptParsing {

class ParsedCommand;
class CommandDescriptor;
class Command;
class ParsedParam;
class ParamDescriptor;
class Param;

// This enumeration is used by the parser only and represent the raw type of the
// value as seen by the parser. Another enumeration is used to represent the
// actual type as seen by the client, which handle more general type as numbers
// (integer or floating point), positive integer, array of strings, etc.

enum ParsedParamType {
        PARSED_PARAM_TYPE_NULL,
        PARSED_PARAM_TYPE_BOOLEAN,
        PARSED_PARAM_TYPE_INTEGER,
        PARSED_PARAM_TYPE_FLOAT,
        PARSED_PARAM_TYPE_STRING,
        // PARSED_PARAM_TYPE_ARRAY,
        PARSED_PARAM_TYPE_JSON
};  // end enum

// This enumeration is used to represent the actual type of the value as seen
// by the client. These types are different from parsing types as we want to
// allow more generic types here (e.g. numbers, positive integer, array of
// strings) and handle implicitly some casting as treating 0 and 1 as false and
// true so the cast will not fail because the client requested a boolean, but
// the parser parsed the value as an integer.

enum ParamType {
        PARAM_TYPE_INVALID,

        PARAM_TYPE_BOOLEAN,
        PARAM_TYPE_INTEGER,
        PARAM_TYPE_NUMBER,
        PARAM_TYPE_STRING,
        PARAM_TYPE_JSON,

        NUM_TYPES
};  // end enum

enum ParamSpec { PARAM_SPEC_MANDATORY, PARAM_SPEC_OPTIONAL };  // end enum

// -----------------------------------------------------------------------------

class CommandParsing {
       private:
        CommandParsing();

       public:
        static bool checkCommandName(const std::string &name) {
                return std::regex_match(name,
                                        std::regex("[a-zA-Z_][a-zA-Z0-9_]*"));
        }  // end method

        static bool checkParamName(const std::string &name) {
                return std::regex_match(name,
                                        std::regex("[a-zA-Z_][a-zA-Z0-9_]*"));
        }  // end method

        static std::string getParamTypeString(const ParamType type) {
                switch (type) {
                        case PARAM_TYPE_BOOLEAN:
                                return "boolean";
                        case PARAM_TYPE_INTEGER:
                                return "integer";
                        case PARAM_TYPE_NUMBER:
                                return "number";
                        case PARAM_TYPE_STRING:
                                return "string";
                        case PARAM_TYPE_JSON:
                                return "json";
                        default:
                                assert(false);
                                return "";
                }  // end switch
        }          // end switch
};                 // end class

// -----------------------------------------------------------------------------

// This class is used by the parser only and stores the raw type and value of
// a parameter.

class ParsedParamValue {
       private:
        ParsedParamType clsType;
        Rsyn::Json clsValue;

       public:
        ParsedParamValue() { clsType = PARSED_PARAM_TYPE_NULL; }
        ParsedParamValue(const bool value) {
                clsType = PARSED_PARAM_TYPE_BOOLEAN;
                clsValue = value;
        }
        ParsedParamValue(const int value) {
                clsType = PARSED_PARAM_TYPE_INTEGER;
                clsValue = value;
        }
        ParsedParamValue(const float value) {
                clsType = PARSED_PARAM_TYPE_FLOAT;
                clsValue = value;
        }
        ParsedParamValue(const std::string &value) {
                clsType = PARSED_PARAM_TYPE_STRING;
                clsValue = value;
        }
        ParsedParamValue(const Rsyn::Json &value) {
                clsType = PARSED_PARAM_TYPE_JSON;
                clsValue = value;
        }

        ParsedParamType getType() const { return clsType; }
        Rsyn::Json getValue() const { return clsValue; }
};  // end class

// -----------------------------------------------------------------------------

class CommandException : public std::exception {
       public:
        CommandException(const std::string &msg) : msg(msg) {}
        CommandException() : msg("No message.") {}

        virtual const char *what() const throw() { return msg.c_str(); };

       protected:
        std::string msg;
};  // end class

// -----------------------------------------------------------------------------

class InvalidParamTypeException : public CommandException {
       public:
        InvalidParamTypeException()
            : CommandException("Invalid parameter type.") {}
};  // end class

// -----------------------------------------------------------------------------

class ParamNotFoundException : public CommandException {
       public:
        ParamNotFoundException(const std::string &paramName)
            : CommandException("Parameter '" + paramName + "' not found.") {}
};  // end class

// -----------------------------------------------------------------------------

class PositionalParamNotFoundException : public CommandException {
       public:
        PositionalParamNotFoundException(const int pos)
            : CommandException("Parameter '" + std::to_string(pos) +
                               "' not found.") {}
};  // end class

// -----------------------------------------------------------------------------

class InvalidCommandDescriptionException : public CommandException {
       public:
        InvalidCommandDescriptionException(const std::string &msg)
            : CommandException(msg) {}
};  // end class

// -----------------------------------------------------------------------------

class CommandDoesNotMatchDescription : public CommandException {
       public:
        CommandDoesNotMatchDescription(const std::string &commandName,
                                       const std::string &details) {
                msg = "Command '" + commandName +
                      "' does not match its description";
                if (details.empty()) {
                        msg += ".";
                } else {
                        msg += " (" + details + ")";
                }  // end else
        }          // end constructor
};                 // end class

// -----------------------------------------------------------------------------

// This class stores a parameter data as seen by the client.

class ParsedParam {
        friend class ParsedCommand;
        friend class Command;

       private:
        Rsyn::Json clsJson;
        ParsedParamType clsType;
        int clsPosition;
        std::string clsName;

        ParsedParam() : clsPosition(-1), clsType(PARSED_PARAM_TYPE_NULL) {}

        const Rsyn::Json &getValue() const { return clsJson; }  // end method

       public:
        // Gets the name of this param. The name is empty for positional params.
        const std::string &getName() const { return clsName; }  // end method

        // Gets the type of this param.
        ParsedParamType getType() const { return clsType; }

        // Gets the position of this param in the command. By definition,
        // positional
        // params have always lower positions than named params.
        int getPosition() const { return clsPosition; }  // end method

        // Checks whether or not this is a named param.
        bool isNamedParam() const { return getPosition() == -1; }  // end method

        // Checks whether or not this is a positional param.
        bool isPositionalParam() const {
                return !isNamedParam();
        }  // end method

        // Checks whether or not this param has a null value. Both positional
        // and
        // named parameters may have null values. For positional params, the
        // null
        // value can be only explicitly set:
        //
        // run "a" null "b";           # 2nd positional param has a null value
        //
        // For named params, a null value can be set explicitly or by omitting
        // the
        // value.
        //
        // run -a -b "text" -c null;   # a and c have null values
        //
        bool isNull() const {
                return getType() == PARSED_PARAM_TYPE_NULL;
        }  // end method

        // Checks whether or not this param has a numeric value.
        bool isNumeric() const {
                return getType() == PARSED_PARAM_TYPE_INTEGER ||
                       getType() == PARSED_PARAM_TYPE_FLOAT;
        }  // end method

        // Checks whether or not this param has an integer numeric value.
        bool isInteger() const {
                return getType() == PARSED_PARAM_TYPE_INTEGER;
        }  // end method

        // Checks whether or not this param has a boolean type.
        // Besides the keywords "false" and "true", integer values 0 (false) and
        // 1 (true) are also interpreted as boolean.
        //
        // For named params a null value indicates that the param has a true
        // value,
        // which handles switch type params. For instance, the param "-test"
        // below
        // has a null value, which is interpreted as true meaning that the
        // "-test"
        // switch is on.
        //
        // run -test -name "example"
        //
        bool isBoolean() const {
                return (getType() == PARSED_PARAM_TYPE_BOOLEAN) ||
                       (getType() == PARSED_PARAM_TYPE_NULL &&
                        getName() != "") ||
                       (getType() == PARSED_PARAM_TYPE_INTEGER &&
                        (asInteger() == 0 || asInteger() == 1));
        }  // end method

        // Checks whether or not this param is a Rsyn::Json.
        bool isJson() const {
                return getType() == PARSED_PARAM_TYPE_JSON;
        }  // end method

        // Checks whether or not the param value is compatible it a specific
        // type.
        bool checkCompatibility(const ParsedParamType type) {
                switch (type) {
                        case PARSED_PARAM_TYPE_NULL:
                                return isNull();
                        case PARSED_PARAM_TYPE_BOOLEAN:
                                return isBoolean();
                        case PARSED_PARAM_TYPE_INTEGER:
                                return isInteger();
                        case PARSED_PARAM_TYPE_FLOAT:
                                return isNumeric();
                        case PARSED_PARAM_TYPE_STRING:
                                return true;
                        case PARSED_PARAM_TYPE_JSON:
                                return isJson();
                        default:
                                assert(false);
                }  // end switch
        }          // end method

        // Cast this param as string. All param types can be casted as string
        // so no exception is ever raised.
        std::string asString() const {
                return clsJson.get<std::string>();
        }  // end method

        // Cast this param as boolean. If the cast is not possible, raises an
        // exception.
        bool asBoolean() const {
                if (!isBoolean()) throw InvalidParamTypeException();
                return isNamedParam() && isNull() ? true : clsJson.get<bool>();
        }  // end method

        // Cast this param as integer. If the cast is not possible, raises an
        // exception.
        int asInteger() const {
                if (!isInteger()) throw InvalidParamTypeException();
                return clsJson.get<int>();
        }  // end method

        // Cast this param as a real (float-point) numeric value. If the cast is
        // not
        // possible, raises  an exception.
        float asNumber() const {
                if (!isNumeric()) throw InvalidParamTypeException();
                return clsJson.get<float>();
        }  // end method

        // Cast this param as a Rsyn::Json objects. If the cast is not possible,
        // raises
        // an exception.
        Rsyn::Json asJson() const {
                if (!isJson()) throw InvalidParamTypeException();
                return clsJson;
        }  // end method

        // Print this param.
        void print(std::ostream &out) const {
                if (isNamedParam()) {
                        out << "-" << getName() << " ";
                }  // end if

                switch (getType()) {
                        case PARSED_PARAM_TYPE_NULL:
                                if (isPositionalParam()) out << "null";
                                break;
                        case PARSED_PARAM_TYPE_BOOLEAN:
                                out << (asBoolean() ? "true" : "false");
                                break;
                        case PARSED_PARAM_TYPE_INTEGER:
                        case PARSED_PARAM_TYPE_FLOAT:
                                out << asNumber();
                                break;
                        case PARSED_PARAM_TYPE_STRING:
                                out << '"' << asString() << '"';
                                break;
                        case PARSED_PARAM_TYPE_JSON:
                                out << clsJson;
                                break;
                        default:
                                assert(false);
                }  // end switch
        }          // end method

};  // end class

// -----------------------------------------------------------------------------

class ParsedCommand {
        friend class ScriptParsing::ScriptReader;

       private:
        std::string clsName;
        std::string clsPath;
        std::vector<ParsedParam> clsPositionalParams;
        std::vector<ParsedParam> clsNamedParams;
        std::map<std::string, int> clsNamedParamMapping;

        void setName(const std::string &name) { clsName = name; }
        void setPath(const std::string &path) { clsPath = path; }

        void addPositionalParam(const ParsedParamValue &value) {
                ParsedParam param;
                param.clsType = value.getType();
                param.clsJson = value.getValue();
                param.clsPosition = clsPositionalParams.size();
                clsPositionalParams.push_back(param);
        }  // end method

        void addNamedParam(const std::string &name,
                           const ParsedParamValue &value) {
                ParsedParam param;
                param.clsName = name;
                param.clsType = value.getType();
                param.clsJson = value.getValue();
                param.clsPosition = -1;
                clsNamedParamMapping[name] = clsNamedParams.size();
                clsNamedParams.push_back(param);
        }  // end method

       public:
        const std::string &getName() const { return clsName; }
        const std::string &getPath() const { return clsPath; }
        int getNumPositionalParams() const {
                return clsPositionalParams.size();
        }
        int getNumNamedParams() const { return clsNamedParams.size(); }

        bool hasNamedParam(const std::string &name) const {
                return clsNamedParamMapping.count(name);
        }  // end method

        bool hasPositionalParam(const int pos) const {
                return pos >= 0 && pos < getNumPositionalParams();
        }  // end method

        const ParsedParam &getPositionalParam(const int pos) const {
                if (!hasPositionalParam(pos))
                        throw PositionalParamNotFoundException(pos);
                return clsPositionalParams[pos];
        }  // end method

        const ParsedParam &getNamedParam(const std::string &name) const {
                auto it = clsNamedParamMapping.find(name);
                if (it == clsNamedParamMapping.end())
                        throw ParamNotFoundException(name);
                return clsNamedParams[it->second];
        }  // end method

        const std::vector<ParsedParam> &allPositionalParams() const {
                return clsPositionalParams;
        }  // end method

        const std::vector<ParsedParam> &allNamedParams() const {
                return clsNamedParams;
        }  // end method

        void print(std::ostream &out) const {
                out << clsName;

                for (const ParsedParam &param : allPositionalParams()) {
                        out << " ";
                        param.print(out);
                }  // end for

                for (const ParsedParam &param : allNamedParams()) {
                        out << " ";
                        param.print(out);
                }  // end for

                out << "\n";
        }  // end method
};         // end class

// -----------------------------------------------------------------------------

class ParamDescriptor {
        friend class CommandDescriptor;

       private:
        std::string clsName;
        ParamType clsType;
        ParamSpec clsSpec;
        int clsPosition;
        std::string clsDescription;
        Rsyn::Json clsDefaultValue;

        template <typename T>
        bool parseDefaultValue_Generic(const std::string &value) {
                T data;
                std::istringstream iss(value);
                iss >> data;
                if (iss.fail()) {
                        return false;
                } else {
                        clsDefaultValue = data;
                        return true;
                }  // end else
        }          // end operator

        bool parseDefaultValue_Boolean(const std::string &value) {
                if (value == "0" || value == "false") {
                        clsDefaultValue = false;
                } else if (value == "1" || value == "true") {
                        clsDefaultValue = true;
                } else {
                        return false;
                }  // end else
                return true;
        }  // end operator

        bool parseDefaultValue_String(const std::string &value) {
                clsDefaultValue = value;
                return true;
        }  // end operator

        bool parseDefaultValue_Json(const std::string &value) {
                try {
                        clsDefaultValue.parse(value);
                } catch (...) {
                        return false;
                }  // end catch
                return true;
        }  // end operator

        void parseDefaultValue(const std::string &value) {
                bool success = false;
                switch (getType()) {
                        case PARAM_TYPE_BOOLEAN:
                                success = parseDefaultValue_Boolean(value);
                                break;
                        case PARAM_TYPE_INTEGER:
                                success = parseDefaultValue_Generic<int>(value);
                                break;
                        case PARAM_TYPE_NUMBER:
                                success =
                                    parseDefaultValue_Generic<float>(value);
                                break;
                        case PARAM_TYPE_STRING:
                                success = parseDefaultValue_String(value);
                                break;
                        case PARAM_TYPE_JSON:
                                if (value == "") {
                                        // Rsyn::Json parse fails for empty
                                        // string, but an empty
                                        // string should be considered as a null
                                        // Rsyn::Json.
                                        success = true;
                                } else {
                                        success = parseDefaultValue_Json(value);
                                }
                                break;
                        default:
                                assert(false);
                }  // end switch

                if (!success) {
                        throw InvalidCommandDescriptionException(
                            "The type of the default value does not match the "
                            "parameter type.");
                }  // end if
        }          // end method

       public:
        ParamDescriptor()
            : clsType(PARAM_TYPE_INVALID),
              clsSpec(PARAM_SPEC_MANDATORY),
              clsPosition(-1) {}

        const std::string &getName() const { return clsName; }  // end method

        ParamType getType() const { return clsType; }  // end method

        ParamSpec getSpec() const { return clsSpec; }  // end method

        const std::string &getDescription() const {
                return clsDescription;
        }  // end method

        const Rsyn::Json &getDefaultValue() const {
                return clsDefaultValue;
        }  // end method

        int getPosition() const { return clsPosition; }  // end method

        bool isMandatory() const {
                return clsSpec == PARAM_SPEC_MANDATORY;
        }  // end method

        bool isOptional() const {
                return clsSpec == PARAM_SPEC_OPTIONAL;
        }  // end method

        bool match(const ParsedParam &param,
                   const bool ignoreName = false) const {
                if (!ignoreName && param.getName() != getName()) return false;

                switch (getType()) {
                        case PARAM_TYPE_BOOLEAN:
                                return param.isBoolean();
                        case PARAM_TYPE_INTEGER:
                                return param.isInteger();
                        case PARAM_TYPE_NUMBER:
                                return param.isNumeric();
                        case PARAM_TYPE_STRING:
                                return !param.isNull();
                        case PARAM_TYPE_JSON:
                                // Note: All basic types can be parsed as a
                                // Rsyn::Json. I'm not sure
                                // if the actual Rsyn::Json standard consider
                                // that, for instance, a
                                // boolean is a valid Rsyn::Json, but our
                                // Rsyn::Json type allows that.
                                return true;
                        default:
                                assert(false);
                }  // end switch
                return false;
        }  // end method

};  // end class

// -----------------------------------------------------------------------------

class CommandDescriptor {
       private:
        std::string clsName;
        std::string clsDescription;
        std::vector<ParamDescriptor> clsPositionalParams;
        std::vector<ParamDescriptor> clsNamedParams;
        std::map<std::string, int> clsNamedParamMapping;

       public:
        CommandDescriptor() {}

        const std::string &getName() const { return clsName; }
        const std::string &getDescription() const { return clsDescription; }
        int getNumPositionalParams() const {
                return clsPositionalParams.size();
        }
        int getNumNamedParams() const { return clsNamedParams.size(); }

        bool hasParam(const std::string &name) const {
                return clsNamedParamMapping.count(name);
        }  // end method

        const ParamDescriptor &getPositionalParam(const int pos) const {
                if (pos < 0 && pos >= getNumPositionalParams())
                        throw PositionalParamNotFoundException(pos);
                return clsPositionalParams[pos];
        }  // end method

        const ParamDescriptor &getNamedParam(const std::string &name) const {
                auto it = clsNamedParamMapping.find(name);
                if (it == clsNamedParamMapping.end())
                        throw ParamNotFoundException(name);
                return clsNamedParams[it->second];
        }  // end method

        const std::vector<ParamDescriptor> &allPositionalParamDescriptors()
            const {
                return clsPositionalParams;
        }  // end method

        const std::vector<ParamDescriptor> &allNamedParamDescriptors() const {
                return clsNamedParams;
        }  // end method

        void setName(const std::string &name) {
                if (!CommandParsing::checkCommandName(name))
                        throw InvalidCommandDescriptionException(
                            "Invalid command name.");
                clsName = name;
        }  // end method

        void setDescription(const std::string &description) {
                clsDescription = description;
        }  // end method

        void addPositionalParam(const std::string &name, const ParamType type,
                                const ParamSpec spec,
                                const std::string &description,
                                const std::string &defaultValue = "") {
                // Optional positional params must be after mandatory ones.
                if ((spec == PARAM_SPEC_MANDATORY) &&
                    !clsPositionalParams.empty() &&
                    clsPositionalParams.back().isOptional()) {
                        throw InvalidCommandDescriptionException(
                            "Cannot add a mandatory positional parameters "
                            "after an optional one.");
                }  // end if

                ParamDescriptor dscp;
                dscp.clsName = name;
                dscp.clsType = type;
                dscp.clsSpec = spec;
                dscp.clsPosition = clsPositionalParams.size();
                dscp.clsDescription = description;
                if (spec == PARAM_SPEC_OPTIONAL) {
                        dscp.parseDefaultValue(defaultValue);
                } else if (defaultValue != "") {
                        std::cout << "WARNING: Ignoring default value for "
                                     "mandatory parameter.\n";
                }  // end else
                clsPositionalParams.push_back(dscp);
        }  // end method

        void addNamedParam(const std::string &name, const ParamType type,
                           const ParamSpec spec, const std::string &description,
                           const std::string &defaultValue = "") {
                if (!CommandParsing::checkParamName(name))
                        throw InvalidCommandDescriptionException(
                            "Invalid parameter name.");

                ParamDescriptor dscp;
                dscp.clsName = name;
                dscp.clsType = type;
                dscp.clsSpec = spec;
                dscp.clsDescription = description;
                if (spec == PARAM_SPEC_OPTIONAL) {
                        dscp.parseDefaultValue(defaultValue);
                } else if (defaultValue != "") {
                        std::cout << "WARNING: Ignoring default value for "
                                     "mandatory parameter.\n";
                }  // end else
                clsNamedParamMapping[name] = clsNamedParams.size();
                clsNamedParams.push_back(dscp);
        }  // end method

        bool match(const ParsedCommand &command,
                   std::string &explanation) const {
                if (getName() != command.getName()) {
                        explanation = "command name does not match";
                        return false;
                }  // end if

                const int numPositionalParams = getNumPositionalParams();
                for (int i = 0; i < numPositionalParams; i++) {
                        const ParamDescriptor &paramDescriptor =
                            getPositionalParam(i);

                        if (command.hasPositionalParam(i)) {
                                const ParsedParam &param =
                                    command.getPositionalParam(i);
                                if (!paramDescriptor.match(param, true)) {
                                        explanation =
                                            "see parameter '" +
                                            paramDescriptor.getName() + "'";
                                        return false;
                                }  // end if
                        } else {
                                if (paramDescriptor.isMandatory()) {
                                        explanation =
                                            "missing positional parameter '" +
                                            paramDescriptor.getName() + "'";
                                        return false;
                                }  // end if
                        }          // end else
                }                  // end for

                for (const ParamDescriptor &paramDescriptor :
                     allNamedParamDescriptors()) {
                        const std::string &name = paramDescriptor.getName();

                        if (paramDescriptor.isMandatory()) {
                                if (!command.hasNamedParam(name) ||
                                    !paramDescriptor.match(
                                        command.getNamedParam(name))) {
                                        explanation =
                                            "see parameter '" +
                                            paramDescriptor.getName() + "'";
                                        return false;
                                }  // end if
                        } else {
                                if (command.hasNamedParam(name) &&
                                    !paramDescriptor.match(
                                        command.getNamedParam(name))) {
                                        explanation =
                                            "see parameter '" +
                                            paramDescriptor.getName() + "'";
                                        return false;
                                }  // end if
                        }          // end if
                }                  // end for

                explanation = "";
                return true;
        }  // end method

        // Prints the usage of this command.
        void printUsage(std::ostream &out) const {
                const int N = 20;

                out << "Usage:\n";

                out << getName();
                for (const ParamDescriptor &paramDescriptor :
                     allPositionalParamDescriptors()) {
                        out << " ";
                        out << (paramDescriptor.isMandatory() ? "<" : "[");
                        out << paramDescriptor.getName();
                        out << (paramDescriptor.isMandatory() ? ">" : "]");
                }  // end for

                for (const ParamDescriptor &paramDescriptor :
                     allNamedParamDescriptors()) {
                        out << " ";
                        out << (paramDescriptor.isMandatory() ? "<" : "[");
                        out << "-" << paramDescriptor.getName();
                        out << (paramDescriptor.isMandatory() ? ">" : "]");
                }  // end for

                out << "\n\n" << getDescription() << "\n";

                for (const ParamDescriptor &paramDescriptor :
                     allPositionalParamDescriptors()) {
                        out << std::setw(N) << std::left
                            << paramDescriptor.getName();
                        out << "(type : " << CommandParsing::getParamTypeString(
                                                 paramDescriptor.getType());
                        if (paramDescriptor.isOptional()) {
                                out << ", default = "
                                    << paramDescriptor.getDefaultValue().dump();
                        }  // end if

                        out << ")\n";
                        out << paramDescriptor.getDescription() << "\n";
                        out << "\n";
                }  // end for

                for (const ParamDescriptor &paramDescriptor :
                     allNamedParamDescriptors()) {
                        out << std::setw(N) << std::left
                            << paramDescriptor.getName();
                        out << "(type : " << CommandParsing::getParamTypeString(
                                                 paramDescriptor.getType());
                        if (paramDescriptor.isOptional()) {
                                out << ", default = "
                                    << paramDescriptor.getDefaultValue().dump();
                        }  // end if

                        out << ")\n";
                        out << paramDescriptor.getDescription() << "\n";
                        out << "\n";
                }  // end for
        }          // end method

        // Checks whether this descriptor describes a valid command.
        void check() const {
                // Note: Command and parameter name were already checked when
                // defined.

                // Checks if a command name was set.
                if (clsName.empty()) {
                        std::cout << "ERROR: Missing command name.\n";
                        throw InvalidCommandDescriptionException(
                            "Command has no name.");
                }  // end if

                // Checks if a description was provided.
                if (clsDescription.empty()) {
                        std::cout << "ERROR: Missing command description.\n";
                        throw InvalidCommandDescriptionException(
                            "Command has no description.");
                }  // end if
        }          // end method

};  // end class

// -----------------------------------------------------------------------------

class Param {
        friend class Command;

       private:
        std::string clsName;
        ParamType clsType;
        Rsyn::Json clsValue;
        bool clsUserSpecified;

        void copyFromDescriptor(const ParamDescriptor &dscp) {
                clsName = dscp.getName();
                clsType = dscp.getType();
                clsValue = dscp.getDefaultValue();
        }  // end method

        void copyValue(const Rsyn::Json &value) {
                clsValue = value;
        }  // end method

       public:
        Param() : clsType(PARAM_TYPE_INVALID), clsUserSpecified(false) {}

        operator bool() const {
                if (clsType != PARAM_TYPE_BOOLEAN)
                        throw InvalidParamTypeException();
                return clsValue.get<bool>();
        }  // end operator

        operator int() const {
                if (clsType != PARAM_TYPE_INTEGER)
                        throw InvalidParamTypeException();
                return clsValue.get<int>();
        }  // end operator

        operator float() const {
                if (clsType != PARAM_TYPE_INTEGER &&
                    clsType != PARAM_TYPE_NUMBER)
                        throw InvalidParamTypeException();
                return clsValue.get<float>();
        }  // end operator

        operator std::string() const {
                return clsValue.is_string() ? clsValue.get<std::string>()
                                            : clsValue.dump();
        }  // end operator

        operator Rsyn::Json() const { return clsValue; }  // end operator

       public:
};  // end class

// -----------------------------------------------------------------------------

class Command {
       private:
        std::vector<Param> clsParams;
        std::map<std::string, int> clsMapping;
        std::string clsName;
        std::string clsPath;

        Param &getParam(const std::string &name) {
                auto it = clsMapping.find(name);
                if (it == clsMapping.end()) throw ParamNotFoundException(name);
                return clsParams[it->second];
        }  // end method

       public:
        Command() {}
        Command(const CommandDescriptor &commandDescriptor,
                const ParsedCommand &parsedCommand) {
                compile(commandDescriptor, parsedCommand);
        }  // end constructor

        void compile(const CommandDescriptor &commandDescriptor,
                     const ParsedCommand &parsedCommand) {
                // Check if the parsed command matches the command
                // specification.
                std::string explanation;
                if (!commandDescriptor.match(parsedCommand, explanation)) {
                        throw CommandDoesNotMatchDescription(
                            commandDescriptor.getName(), explanation);
                }  // end if

                clsName = parsedCommand.getName();
                clsPath = parsedCommand.getPath();

                // Initialize all commands.
                for (const ParamDescriptor &paramDscp :
                     commandDescriptor.allPositionalParamDescriptors()) {
                        Param param;
                        param.copyFromDescriptor(paramDscp);
                        clsMapping[paramDscp.getName()] = clsParams.size();
                        clsParams.push_back(param);
                }  // end for

                for (const ParamDescriptor &paramDscp :
                     commandDescriptor.allNamedParamDescriptors()) {
                        Param param;
                        param.copyFromDescriptor(paramDscp);
                        clsMapping[paramDscp.getName()] = clsParams.size();
                        clsParams.push_back(param);
                }  // end for

                // Overwrite default values with the user specified ones.
                for (const ParamDescriptor &paramDescriptor :
                     commandDescriptor.allPositionalParamDescriptors()) {
                        if (parsedCommand.hasPositionalParam(
                                paramDescriptor.getPosition())) {
                                Param &param =
                                    getParam(paramDescriptor.getName());
                                param.clsValue =
                                    parsedCommand
                                        .getPositionalParam(
                                            paramDescriptor.getPosition())
                                        .getValue();
                                param.clsUserSpecified = true;
                        }  // end if
                }          // end for

                for (const ParamDescriptor &paramDescriptor :
                     commandDescriptor.allNamedParamDescriptors()) {
                        if (parsedCommand.hasNamedParam(
                                paramDescriptor.getName())) {
                                Param &param =
                                    getParam(paramDescriptor.getName());
                                param.clsValue =
                                    parsedCommand
                                        .getNamedParam(
                                            paramDescriptor.getName())
                                        .getValue();
                                param.clsUserSpecified = true;
                        }  // end if
                }          // end for
        }                  // end method

        const Param &getParam(const std::string &name) const {
                auto it = clsMapping.find(name);
                if (it == clsMapping.end()) throw ParamNotFoundException(name);
                return clsParams[it->second];
        }  // end method

        const std::string &getName() const { return clsName; }  // end method

        const std::string &getPath() const { return clsPath; }  // end method

};  // end class

// -----------------------------------------------------------------------------

// TODO: Improve history management.

class CommandManager {
       public:
        typedef std::function<void(const Command &command)> Handler;

       private:
        std::map<std::string, std::tuple<CommandDescriptor, Handler>>
            clsRegisteredCommands;

        mutable std::vector<std::string> clsHistory;
        mutable int clsCurrentHistory;

        int moveToPast() {
                const int index = clsCurrentHistory;
                if (clsCurrentHistory - 1 < 0)
                        clsCurrentHistory = clsHistory.size() - 1;
                else
                        clsCurrentHistory--;
                return index;
        }  // end method

        int moveToFuture() {
                const int index = clsCurrentHistory;
                if (clsCurrentHistory + 1 >= clsHistory.size())
                        clsCurrentHistory = 0;
                else
                        clsCurrentHistory++;
                return index;
        }  // end method

        void evaluateCommand(const ParsedCommand &parsedCommand) {
                auto it = clsRegisteredCommands.find(parsedCommand.getName());
                if (it != clsRegisteredCommands.end()) {
                        const CommandDescriptor &commandDescriptor =
                            std::get<0>(it->second);
                        Handler handler = std::get<1>(it->second);

                        try {
                                Command command(commandDescriptor,
                                                parsedCommand);
                                handler(command);
                        } catch (const CommandException &e) {
                                std::cout << "ERROR: " << e.what() << "\n\n";
                                commandDescriptor.printUsage(std::cout);
                        }  // end catch
                } else {
                        std::cout << "ERROR: Command '"
                                  << parsedCommand.getName()
                                  << "' is not registered.\n";
                }  // end else
        }          // end method

       public:
        CommandManager() { clsCurrentHistory = 0; }  // end construct

        void addCommand(const CommandDescriptor &command,
                        const Handler handler) {
                if (clsRegisteredCommands.count(command.getName()))
                        throw CommandException("Command already exists.");
                clsRegisteredCommands.insert(std::make_pair(
                    command.getName(), std::make_tuple(command, handler)));
        }  // end method

        void printCommandList(std::ostream &out = std::cout) {
                for (const auto &e : clsRegisteredCommands) {
                        out << e.first << "\n";
                        out << "    " << std::get<0>(e.second).getDescription()
                            << "\n";
                }  // end for
        }          // end method

        bool printCommandUsage(const std::string &name,
                               std::ostream &out = std::cout) {
                const auto it = clsRegisteredCommands.find(name);
                if (it != clsRegisteredCommands.end()) {
                        const CommandDescriptor &command =
                            std::get<0>(it->second);
                        command.printUsage(out);
                        return true;
                } else {
                        return false;
                }  // end if
        }          // end method

        void printHistory(std::ostream &out = std::cout) {
                for (const auto &cmd : clsHistory) {
                        out << cmd << "\n";
                }  // end for
        }          // end method

        // Push a command to the history.
        void pushHistory(const std::string &record) {
                clsHistory.push_back(record);
                clsCurrentHistory = clsHistory.size() - 1;
        }  // end method

        const std::string &getHistory(const int index) const {
                return clsHistory[index];
        }  // end method

        int getHistorySize() const { return clsHistory.size(); }  // end method

        // Move to the next command recorded in the history.
        std::string nextHistory() {
                if (clsHistory.empty()) return "";
                return clsHistory[moveToFuture()];
        }  // end method

        // Move to the previous command recorded in the history.
        std::string previousHistory() {
                if (clsHistory.empty()) return "";
                return clsHistory[moveToPast()];
        }  // end method

        // Get current history command if any.
        std::string currentHistoryCommand() const {
                if (clsHistory.empty()) return "";
                return clsHistory[clsCurrentHistory];
        }  // end method

        // Get current history index.
        int currentHistoryCommandIndex() const {
                if (clsHistory.empty()) return -1;
                return clsCurrentHistory;
        }  // end method

        std::string autoComplete(const std::string &base) const {
                if (base == "") return "";

                const auto itEnd = clsRegisteredCommands.end();
                const auto itLowerBound =
                    clsRegisteredCommands.lower_bound(base);

                if (itLowerBound == itEnd) return "";

                // I'm sure there is a clever way to do this, but this is
                // supposed to
                // be run in human-speed, so a lazy implementation is not a big
                // deal.

                // Create a vector to store the candidate commands.
                std::vector<std::string> words;

                // Copy the commands to the candidate command vector.
                for (auto it = itLowerBound; it != itEnd; it++) {
                        if (it->first.find(base) == 0)
                                words.push_back(it->first);
                }  // end for

                // Check trivial cases.
                if (words.size() == 0) {
                        return "";  // returns nothing
                } else if (words.size() == 1) {
                        return words[0].substr(base.size(), std::string::npos) +
                               " ";
                }  // end else

                // Find the longest common string in the candidate commands.
                const std::string referenceWord = words[0];

                int index;
                for (index = base.size() - 1; index < referenceWord.size();
                     index++) {
                        bool keepGoing = true;

                        // Check if the kth charactered matches in all words.
                        for (int i = 1; i < words.size(); i++) {
                                const std::string &word = words[i];
                                if (index >= word.size() ||
                                    word[index] != referenceWord[index]) {
                                        keepGoing = false;
                                        break;
                                }  // end if
                        }          // end for

                        if (!keepGoing) {
                                break;
                        }  // end if
                }          // end for

                return referenceWord.substr(base.size(), index - base.size());
        }  // end method

        void evaluateString(const std::string &str) {
                pushHistory(str);
                ScriptReader reader;
                reader.parseFromString(str);
                for (const ParsedCommand &command : reader.allCommands()) {
                        evaluateCommand(command);
                }  // end for
        }          // end method

        void evaluateFile(const std::string &filename) {
                ScriptReader reader;
                reader.parseFromFile(filename);
                for (const ParsedCommand &command : reader.allCommands()) {
                        evaluateCommand(command);
                }  // end for
        }          // end method

        void exitRsyn(std::ostream &out = std::cout) {
                out << "        Exiting...\n";
                out << "        ***  Auf Wiedersehen!  *** \n";
                exit(0);
        }  // end method
};         // end class

}  // end namespace

#endif
