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

#include <cmath>
#include <iomanip>

#include "Histogram.h"
#include "StreamStateSaver.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

Histogram::Histogram() {}  // end method

// -----------------------------------------------------------------------------

void Histogram::clear() { *this = Histogram(); }  // end method

// -----------------------------------------------------------------------------

void Histogram::addSample(const double value) {
        _samples.push_back(value);
}  // end method

// -----------------------------------------------------------------------------

void Histogram::binning(const HistogramOptions &options, std::vector<int> &bins,
                        double &start, double &step) const {
        const int numBins = options.getNumBins();

        double lower = options.getLowerBound();
        double upper = options.getUpperBound();

        double minValue = +std::numeric_limits<double>::infinity();
        double maxValue = -std::numeric_limits<double>::infinity();

        for (double value : _samples) {
                minValue = std::min(minValue, value);
                maxValue = std::max(maxValue, value);
        }  // end for

        if (std::isinf(lower)) lower = minValue;

        if (std::isinf(upper)) upper = maxValue;

        start = lower;
        step = (upper - lower) / numBins;
        bins.assign(numBins, 0);

        for (double value : _samples) {
                int index = (value - start) / step;
                if (value < 0 || value >= numBins) {
                        if (!options.getClamp()) continue;
                        index = std::max(0, std::min(numBins, index));
                }  // end if
                bins[index]++;
        }  // end method
}  // end method

// -----------------------------------------------------------------------------

void Histogram::print(const HistogramOptions &options, const std::string &title,
                      std::ostream &out) const {
        const int N = options.getMaxHistogramWidth();
        const int numBins = options.getNumBins();

        std::vector<int> bins;
        double start;
        double step;
        binning(options, bins, start, step);

        StreamStateSaver sss(out);
        out << std::fixed;

        int maxCount = options.getMaxPrintableSamplesPerBin();
        if (maxCount == 0) {
                for (int i = 0; i < numBins; i++) {
                        maxCount = std::max(maxCount, bins[i]);
                }  // end for
        }          // end if

        out << "Histogram: " << title << "\n";
        for (int i = 0; i < numBins; i++) {
                const double value = start + i * step;
                bool overflow = false;
                int width = (int)std::ceil(N * (bins[i] / float(maxCount)));
                if (width > N) {
                        width = N - 1;
                        overflow = true;
                }  // end if
                out << std::setw(15) << std::setprecision(4) << value << " "
                    << "[" << std::setw(6) << bins[i] << "] "
                    << "[" << std::setw(6) << std::setprecision(2)
                    << (100 * bins[i] / float(_samples.size())) << "%] "
                    << std::string(width, '=') << (overflow ? "|" : "") << "\n";
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

}  // end namespace