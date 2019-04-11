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

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <cstdlib>
#include <exception>
#include <string>
#include <iostream>
#include <algorithm>

class UnitTest {
       private:
        std::string clsDescription;

        // Source: The Art of Computer Programming by Knuth
        // [TODO] It seems these functions break down when one operator is zero
        // (0).
        template <typename T>
        static bool approximatelyEqual(const T a, const T b,
                                       const T precision = 1e-6) {
                return std::abs(a - b) <=
                       ((std::abs(a) < std::abs(b) ? std::abs(b)
                                                   : std::abs(a)) *
                        precision);
        }  // end method

        template <typename T>
        static bool definitelyGreaterThan(const T a, const T b,
                                          const T precision = 1e-6) {
                return (a - b) > ((std::abs(a) < std::abs(b) ? std::abs(b)
                                                             : std::abs(a)) *
                                  precision);
        }  // end method

        template <typename T>
        static bool definitelyLessThan(const T a, const T b,
                                       const T precision = 1e-6) {
                return (b - a) > ((std::abs(a) < std::abs(b) ? std::abs(b)
                                                             : std::abs(a)) *
                                  precision);
        }  // end method

       public:
        class Exception : public std::exception {
                friend std::ostream &operator<<(std::ostream &out,
                                                const Exception &e) {
                        return out << e.what();
                }  // end method

               public:
                explicit Exception(const std::string &message)
                    : clsMsg(message) {}
                virtual ~Exception() throw() {}

                virtual const char *what() const throw() {
                        return clsMsg.c_str();
                }  // end method

               protected:
                std::string clsMsg;
        };  // end class

        ////////////////////////////////////////////////////////////////////////////
        // Constructor
        ////////////////////////////////////////////////////////////////////////////

        UnitTest(const std::string &description)
            : clsDescription(description) {}

        ////////////////////////////////////////////////////////////////////////////
        // Entry point for the unit test
        ////////////////////////////////////////////////////////////////////////////

        virtual void run() = 0;

        ////////////////////////////////////////////////////////////////////////////
        // Get/Set
        ////////////////////////////////////////////////////////////////////////////

        const std::string &getTitle() const { return clsDescription; }

        ////////////////////////////////////////////////////////////////////////////
        // Assertions
        ////////////////////////////////////////////////////////////////////////////

        static void assertFalse(const std::string &msg) {
                throw Exception(msg);
        }  // end method

        static void assertCondition(const bool value, const std::string &msg) {
                if (!value) {
                        throw Exception(msg);
                }  // end if
        }          // end method

        template <typename T>
        static void assertApproximatelyEqual(const T x, const T y,
                                             const std::string &msg,
                                             const T precision = 1e-6) {
                if (!approximatelyEqual(x, y, precision)) {
                        throw Exception(msg);
                }  // end if
        }          // end method

        template <typename T>
        static void assertDefinitelyGreaterThan(const T x, const T y,
                                                const std::string &msg,
                                                const T precision = 1e-6) {
                if (!definitelyGreaterThan(x, y, precision)) {
                        throw Exception(msg);
                }  // end if
        }          // end method

        template <typename T>
        static void assertDefinitelyLessThan(const T x, const T y,
                                             const std::string &msg,
                                             const T precision = 1e-6) {
                if (!definitelyLessThan(x, y, precision)) {
                        throw Exception(msg);
                }  // end if
        }          // end method

};  // end class

#endif
