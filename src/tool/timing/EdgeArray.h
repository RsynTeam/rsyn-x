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

#ifndef RSYN_EDGE_ARRAY_H
#define RSYN_EDGE_ARRAY_H

#include <ostream>
using std::ostream;

#include <cmath>

#include <algorithm>
using std::min;
using std::max;
using std::swap;

#include <utility>
using std::pair;

#include "util/FloatingPoint.h"
#include "TimingTypes.h"

namespace Rsyn {

inline ostream &operator<<(ostream &out, const TimingTransition &edgeType) {
        return out << ((edgeType == FALL) ? "FALL" : "RISE");
}  // end operator

class EdgeType {
        friend ostream &operator<<(ostream &out, const EdgeType &edgeType) {
                if (edgeType == FALL)
                        out << "FALL";
                else if (edgeType == RISE)
                        out << "RISE";
                else
                        out << "INVALID_EDGE";

                return out;
        }  // end operator

       private:
        int clsValue;

       public:
        EdgeType() { clsValue = -1; }
        EdgeType(const TimingTransition edgeTypeEnum) {
                clsValue = edgeTypeEnum;
        }
        EdgeType(const int edgeType) { clsValue = edgeType; }

        operator int() { return clsValue; }
        operator int() const { return clsValue; }

        void reverse() { clsValue = REVERSE_EDGE_TYPE[clsValue]; }
        EdgeType getReversed() const { return REVERSE_EDGE_TYPE[clsValue]; }
};

#define MAKE_SELF_OPERATOR(OP)                                             \
        friend void operator OP(EdgeArray<T> &v0, const EdgeArray<T> v1) { \
                v0[RISE] OP v1[RISE], v0[FALL] OP v1[FALL];                \
        }                                                                  \
        friend void operator OP(EdgeArray<T> &v0, const T v1) {            \
                v0[RISE] OP v1;                                            \
                v0[FALL] OP v1;                                            \
        }

#define MAKE_OPERATOR(OP)                                                    \
        friend EdgeArray<T> operator OP(const EdgeArray<T> v0,               \
                                        const EdgeArray<T> v1) {             \
                return EdgeArray<T>(v0[RISE] OP v1[RISE],                    \
                                    v0[FALL] OP v1[FALL]);                   \
        }                                                                    \
        friend EdgeArray<T> operator OP(const T v0, const EdgeArray<T> v1) { \
                return EdgeArray<T>(v0 OP v1[RISE], v0 OP v1[FALL]);         \
        }                                                                    \
        friend EdgeArray<T> operator OP(const EdgeArray<T> v0, const T v1) { \
                return EdgeArray<T>(v0[RISE] OP v1, v0[FALL] OP v1);         \
        }

template <typename T>
class EdgeArray {
        friend ostream &operator<<(ostream &out, const EdgeArray<T> array) {
                return out << "(" << array[FALL] << ", " << array[RISE] << ")";
        }  // end operator

        MAKE_OPERATOR(+);
        MAKE_OPERATOR(-);
        MAKE_OPERATOR(*);
        MAKE_OPERATOR(/);

        MAKE_SELF_OPERATOR(+=);
        MAKE_SELF_OPERATOR(-=);
        MAKE_SELF_OPERATOR(*=);
        MAKE_SELF_OPERATOR(/=);

        friend EdgeArray<T> operator-(const EdgeArray<T> &v0) {
                return EdgeArray<T>(-v0[RISE], -v0[FALL]);
        }

        friend EdgeArray<T> max(const EdgeArray<T> v0, const EdgeArray<T> v1) {
                return EdgeArray<T>(max(v0[RISE], v1[RISE]),
                                    max(v0[FALL], v1[FALL]));
        }
        friend EdgeArray<T> min(const EdgeArray<T> v0, const EdgeArray<T> v1) {
                return EdgeArray<T>(min(v0[RISE], v1[RISE]),
                                    min(v0[FALL], v1[FALL]));
        }

        friend EdgeArray<T> abs(const EdgeArray<T> v) {
                return EdgeArray<T>(std::abs(v[RISE]), std::abs(v[FALL]));
        }
        friend EdgeArray<T> pow(const EdgeArray<T> v, const double exp) {
                return EdgeArray<T>(std::pow(v[RISE], exp),
                                    std::pow(v[FALL], exp));
        }
        friend EdgeArray<T> pow2(const EdgeArray<T> v) {
                return EdgeArray<T>(v[RISE] * v[RISE], v[FALL] * v[FALL]);
        }
        friend EdgeArray<T> pow3(const EdgeArray<T> v) {
                return EdgeArray<T>(v[RISE] * v[RISE] * v[RISE],
                                    v[FALL] * v[FALL] * v[FALL]);
        }
        friend EdgeArray<T> sqrt(const EdgeArray<T> v) {
                return EdgeArray<T>(std::sqrt(v[RISE]), std::sqrt(v[FALL]));
        }
        friend EdgeArray<T> exp(const EdgeArray<T> v) {
                return EdgeArray<T>(std::exp(v[RISE]), std::exp(v[FALL]));
        }

       private:
        T clsValue[2];

       public:
        T &operator[](const EdgeType edgeType) { return clsValue[edgeType]; }
        T operator[](const EdgeType edgeType) const {
                return clsValue[edgeType];
        }

        EdgeArray &operator=(const EdgeArray &array) {
                clsValue[RISE] = array[RISE];
                clsValue[FALL] = array[FALL];
                return *this;
        }  // end operator

        EdgeArray(const T rise, const T fall) {
                clsValue[RISE] = rise;
                clsValue[FALL] = fall;
        }  // end constructor

        // This constructor is explicit to avoid unintended assignment from
        // scalar
        // to EdgeArray.
        explicit EdgeArray(const T value) {
                clsValue[RISE] = value;
                clsValue[FALL] = value;
        }  // end constructor

        EdgeArray(){};

        void set(const T rise, const T fall) {
                clsValue[RISE] = rise;
                clsValue[FALL] = fall;
        }  // end method

        void setBoth(const T value) {
                clsValue[RISE] = value;
                clsValue[FALL] = value;
        }  // end method

        T getMax() const { return max(clsValue[RISE], clsValue[FALL]); }
        T getMin() const { return min(clsValue[RISE], clsValue[FALL]); }
        T getSum() const { return clsValue[RISE] + clsValue[FALL]; }
        T getAvg() const { return (clsValue[RISE] + clsValue[FALL]) / ((T)2); }
        T getRise() const { return clsValue[RISE]; }
        T getFall() const { return clsValue[FALL]; }

        TimingTransition getMaxEdge() const {
                return (clsValue[RISE] > clsValue[FALL]) ? RISE : FALL;
        }
        TimingTransition getMinEdge() const {
                return (clsValue[RISE] < clsValue[FALL]) ? RISE : FALL;
        }

        EdgeArray<T> getReversed() const {
                return EdgeArray(getFall(), getRise());
        }

        void reverse() { return swap(clsValue[RISE], clsValue[FALL]); }

        T aggregate() const { return clsValue[RISE] + clsValue[FALL]; }

        void replaceUninitBy(const T value) {
                if (FloatingPoint::isUninit(clsValue[RISE])) {
                        clsValue[RISE] = value;
                }
                if (FloatingPoint::isUninit(clsValue[FALL])) {
                        clsValue[FALL] = value;
                }
        }  // end method

        std::pair<TimingTransition, T> maximum() const {
                return (clsValue[RISE] >= clsValue[FALL])
                           ? make_pair(RISE, clsValue[RISE])
                           : make_pair(FALL, clsValue[FALL]);
        }  // end method

        std::pair<TimingTransition, T> minimum() const {
                return (clsValue[RISE] <= clsValue[FALL])
                           ? make_pair(RISE, clsValue[RISE])
                           : make_pair(FALL, clsValue[FALL]);
        }  // end method

};  // end class

// -----------------------------------------------------------------------------

#undef MAKE_OPERATOR
#undef MAKE_OPERATOR_SCALAR
#undef MAKE_SELF_OPERATOR
#undef MAKE_SELF_OPERATOR_SCALAR
#undef MAKE_FUNCTION_1
#undef MAKE_FUNCTION_2

}  // end namespace

#endif
