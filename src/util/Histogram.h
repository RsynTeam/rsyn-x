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

#ifndef RSYN_HISTOGRAM_H
#define RSYN_HISTOGRAM_H

#include <limits>
#include <vector>
#include <iostream>

namespace Rsyn {

// -----------------------------------------------------------------------------

class HistogramOptions {
       public:
        int getNumBins() const { return _numBins; }
        double getLowerBound() const { return _lowerBound; }
        double getUpperBound() const { return _upperBound; }
        bool getClamp() const { return _clamp; }
        int getMaxHistogramWidth() const { return _maxHistogramWidth; }
        int getMaxPrintableSamplesPerBin() const {
                return _maxPrintableSamplesPerBin;
        }

        void setNumBins(const int numBins) { _numBins = numBins; }
        void setLowerBound(const double value) { _lowerBound = value; }
        void setUpperBound(const double value) { _upperBound = value; }
        void setClamp(const bool value) { _clamp = value; }
        void setMaxHistogramWidth(const int value) {
                _maxHistogramWidth = value;
        }
        void setMaxPrintableSamplesPerBin(const int value) {
                _maxPrintableSamplesPerBin = value;
        }

       private:
        //! @brief The number of bins to slot the samples in.
        int _numBins = 20;

        //! @brief
        double _lowerBound = -std::numeric_limits<double>::infinity();

        //! @brief
        double _upperBound = +std::numeric_limits<double>::infinity();

        //! @brief
        bool _clamp = true;

        //! @brief Maximum histogram width.
        int _maxHistogramWidth = 50;

        //! @brief The reference maximum number of sample per bin. If zero, use
        //! the
        //! the actual maximum value throughout all bins. Setting a number
        //! different
        //! than zero is useful when comparing two histogram.
        int _maxPrintableSamplesPerBin = 0;

};  // end class

// -----------------------------------------------------------------------------

class Histogram {
       public:
        Histogram();

        //! @brief Clears all the data stored by this histogram.
        void clear();

        //! @brief Adds a sample to the histogram.
        void addSample(const double value);

        //! @brief Generate bins.
        void binning(const HistogramOptions &options, std::vector<int> &bins,
                     double &start, double &step) const;

        //! @brief Prints this histogram.
        void print(const HistogramOptions &options, const std::string &title,
                   std::ostream &out = std::cout) const;

       private:
        std::vector<double> _samples;

};  // end class

// -----------------------------------------------------------------------------

}  // end namespace

#endif
