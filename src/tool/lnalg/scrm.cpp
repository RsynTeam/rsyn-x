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
#include <cfloat>
#include <cassert>

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <iomanip>
using std::scientific;
using std::setprecision;
#include <string>
using std::string;
#include <algorithm>
using std::max;
using std::min;
#include <utility>
using std::pair;
using std::make_pair;

#include "scrm.h"

// =============================================================================
// SparseMatrixDescriptor
// =============================================================================

SparseMatrixDescriptor::SparseMatrixDescriptor() {
        Clear();
}  // end constructor

// -----------------------------------------------------------------------------

void SparseMatrixDescriptor::Clear() {
        clsMaxCol = -1;
        clsMaxRow = -1;

        clsNumNonZeroElements = 0;
        clsRows.clear();
}  // end method

// -----------------------------------------------------------------------------

void SparseMatrixDescriptor::AddElement(const int row, const int col,
                                        const double value) {
        if (col < 0) throw SCRMException("Negative col index.");
        if (row < 0) throw SCRMException("Negative row index.");

        if (row >= clsRows.size()) clsRows.resize(row + 1);

        ColMap &m = clsRows[row];
        pair<ColMap::iterator, bool> result = m.insert(make_pair(col, value));
        if (!result.second)
                result.first->second += value;
        else
                clsNumNonZeroElements++;

        clsMaxRow = max(clsMaxRow, row);
        clsMaxCol = max(clsMaxCol, col);
}  // end method

// -----------------------------------------------------------------------------

void SparseMatrixDescriptor::AddDiagonalElement(const int row,
                                                const double value) {
        AddElement(row, row, value);
}  // end method

// -----------------------------------------------------------------------------

double SparseMatrixDescriptor::ElementAt(const int row, const int col) const {
        if (row >= clsRows.size()) return 0;

        ColMap::const_iterator itcol = clsRows[row].find(col);
        if (itcol == clsRows[row].end()) return 0;

        return itcol->second;
}  // end method

// -----------------------------------------------------------------------------

bool SparseMatrixDescriptor::CheckIsNumber(int &row, int &col) const {
        const int numRows = clsRows.size();
        for (row = 0; row < numRows; row++) {
                const ColMap &m = clsRows[row];
                for (ColMap::const_iterator itcol = m.begin(); itcol != m.end();
                     itcol++) {
                        const double val0 = itcol->second;
                        const double val1 = ElementAt(itcol->first, row);

                        if (std::isnan(val0) || std::isinf(val0)) return false;
                        if (std::isnan(val1) || std::isinf(val1)) return false;
                }  // end for
        }          // end for

        return true;
}  // end method

// -----------------------------------------------------------------------------

bool SparseMatrixDescriptor::CheckSymmetry(int &row, int &col) const {
        const int numRows = clsRows.size();
        for (row = 0; row < numRows; row++) {
                const ColMap &m = clsRows[row];
                for (ColMap::const_iterator itcol = m.begin(); itcol != m.end();
                     itcol++) {
                        const double val0 = itcol->second;
                        const double val1 = ElementAt(itcol->first, row);

                        if (val0 != val1) {
                                col = itcol->first;
                                return false;
                        }  // end if
                }          // end for
        }                  // end for

        return true;
}  // end method

// -----------------------------------------------------------------------------

bool SparseMatrixDescriptor::CheckUpper(int &row, int &col) const {
        const int numRows = clsRows.size();
        for (row = 0; row < numRows; row++) {
                const ColMap &m = clsRows[row];
                for (ColMap::const_iterator itcol = m.begin(); itcol != m.end();
                     itcol++) {
                        if (itcol->first < row) {
                                col = itcol->first;
                                return false;
                        }  // end if
                }          // end for
        }                  // end for

        return true;
}  // end method

// -----------------------------------------------------------------------------

bool SparseMatrixDescriptor::CheckLower(int &row, int &col) const {
        const int numRows = clsRows.size();
        for (row = 0; row < numRows; row++) {
                const ColMap &m = clsRows[row];
                for (ColMap::const_iterator itcol = m.begin(); itcol != m.end();
                     itcol++) {
                        if (itcol->first > row) {
                                col = itcol->first;
                                return false;
                        }  // end if
                }          // end for
        }                  // end for

        return true;
}  // end method

// -----------------------------------------------------------------------------

void SparseMatrixDescriptor::Print(ostream &out) const {
        out << "NumRows " << (clsMaxRow + 1) << "\n";
        out << "NumCols " << (clsMaxCol + 1) << "\n";
        out << "NumNonZeros " << clsNumNonZeroElements << "\n";

        // Fulfill the matrix.
        const int numRows = clsRows.size();
        for (int row = 0; row < numRows; row++) {
                const ColMap &m = clsRows[row];
                for (ColMap::const_iterator itcol = m.begin(); itcol != m.end();
                     itcol++) {
                        out << row << " " << itcol->first << " "
                            << itcol->second << "\n";
                }  // end for
        }          // end for
}  // end method

// =============================================================================
// UpperSparseMatrixDescriptor
// =============================================================================

void UpperSparseMatrixDescriptor::AddElement(const int row, const int col,
                                             const double value) {
        if (col < row)
                throw SCRMException(
                    "Trying to set a lower element in an upper triangular "
                    "matrix.");

        SparseMatrixDescriptor::AddElement(row, col, value);
}  // end method

// =============================================================================
// LowerSparseMatrixDescriptor
// =============================================================================

void LowerSparseMatrixDescriptor::AddElement(const int row, const int col,
                                             const double value) {
        if (col > row)
                throw SCRMException(
                    "Trying to set an upper element in a lower triangular "
                    "matrix.");

        SparseMatrixDescriptor::AddElement(row, col, value);
}  // end method

// =============================================================================
// AbstractSquareCompressedRowMatrix
// =============================================================================

AbstractSquareCompressedRowMatrix::AbstractSquareCompressedRowMatrix() {
        Reset();
}  // end constructor

// -----------------------------------------------------------------------------

void AbstractSquareCompressedRowMatrix::Reset() {
        clsOff.clear();
        clsRow.clear();
        clsDig.clear();
}  // end method

// -----------------------------------------------------------------------------

void AbstractSquareCompressedRowMatrix::Consolidate(
    const SparseMatrixDescriptor &dscp, const ConsolidationMode mode) {
        const int numRows = dscp.GetNumRow();
        const int numCols = dscp.GetNumCol();

        if (numRows != numCols) throw SCRMException("Non square matrix.");

        // Clean-up.
        Reset();

        // Define the matrix dimension.
        const int dimension = numRows;

        // Resize vectors.
        clsOff.resize(dscp.GetNumNonZeroElements());
        clsRow.resize(dimension + 1);  // + 1 adds a dummy row allowing to get
                                       // the size of the row n using
                                       // clsRow[n+1] - clsRow[n]
        clsDig.resize(dimension);

        // Fulfill the matrix.
        SparseMatrixDescriptor::ColMap::const_iterator itcol;

        int nextEntry = 0;

        for (int row = 0; row < numRows; row++) {
                clsRow[row] = nextEntry;

                const SparseMatrixDescriptor::ColMap &m = dscp.GetColMap(row);
                for (itcol = m.begin(); itcol != m.end(); itcol++) {
                        const int col = itcol->first;
                        const double val = itcol->second;

                        if (row != col) {
                                switch (mode) {
                                        case CONSOLIDATION_MODE_DISCARD_NONE:
                                                break;
                                        case CONSOLIDATION_MODE_DISCARD_LOWER:
                                                if (col < row) continue;
                                                break;
                                        case CONSOLIDATION_MODE_DISCARD_UPPER:
                                                if (col > row) continue;
                                                break;
                                        default:
                                                throw SCRMException(
                                                    "Unknown consolidation "
                                                    "mode.");
                                }  // end switch

                                OffDiagonalElement &off = clsOff[nextEntry];
                                off.col = col;
                                off.val = val;
                                nextEntry++;
                        } else {
                                clsDig[row] = val;
                        }  // end else
                }          // end for
        }                  // end for

        clsRow.back() = nextEntry;

        // Since we may discard elements when building the crm matrix, the
        // number
        // of elements may differ from the number of non zero entries in the
        // descriptor.
        clsOff.resize(nextEntry);
        clsOff.shrink_to_fit();
}  // end method

// -----------------------------------------------------------------------------

void AbstractSquareCompressedRowMatrix::Build(RawSparseMatrixDescriptor &dscp) {
        clsOff.swap(dscp.off);
        clsDig.swap(dscp.dig);
        clsRow.swap(dscp.row);
}  // end method

// -----------------------------------------------------------------------------

bool AbstractSquareCompressedRowMatrix::HasNullDiagonalElement(
    int &row, const double epsilon) const {
        const int numRows = GetDimension();
        for (row = 0; row < numRows; row++) {
                if (std::abs(clsDig[row]) <= epsilon) break;
        }  // end for
        return row < numRows;
}  // end method

// -----------------------------------------------------------------------------

bool AbstractSquareCompressedRowMatrix::HasEmptyRow(int &row) const {
        const int numRows = GetDimension();
        for (row = 0; row < numRows; row++) {
                const int numOffDiagonalElements =
                    clsRow[row + 1] - clsRow[row];
                if (numOffDiagonalElements == 0) break;
        }  // end for
        return row < numRows;
}  // end method

// =============================================================================
// SquareCompressedRowMatrix
// =============================================================================

void SquareCompressedRowMatrix::Initialize(const SparseMatrixDescriptor &dscp,
                                           const bool checkSymmetry) {
        int symmetryFailCol;
        int symmetryFailRow;
        if (checkSymmetry &&
            !dscp.CheckSymmetry(symmetryFailRow, symmetryFailCol)) {
                throw SCRMException("Non symmetric matrix.");
        }  // end if

        Consolidate(dscp, CONSOLIDATION_MODE_DISCARD_NONE);
}  // end method

// -----------------------------------------------------------------------------

void SquareCompressedRowMatrix::MarkovNormalization() {
        vector<double> sum(GetDimension(), 0.0);

        // Compute column sums.
        for (size_t i = 0; i < clsOff.size(); i++) {
                const OffDiagonalElement &off = clsOff[i];
                sum[off.col] += off.val;
        }  // end for
        for (size_t i = 0; i < clsDig.size(); i++) {
                sum[i] += clsDig[i];
        }  // end for

        // Normalize.
        for (size_t i = 0; i < clsOff.size(); i++) {
                OffDiagonalElement &off = clsOff[i];
                off.val /= sum[off.col];
        }  // end for
        for (size_t i = 0; i < clsDig.size(); i++) {
                clsDig[i] /= sum[i];
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void SquareCompressedRowMatrix::PrintMatlab(ostream &out) const {
        out << "m = " << GetDimension() << "\n";
        out << "n = " << GetDimension() << "\n";
        out << "\n";

        // Rows
        out << "row = [ ";
        for (size_t i = 0; i < clsRow.size() - 1; i++)
                for (int j = clsRow[i]; j < clsRow[i + 1]; j++)
                        out << i + 1 << "\n";

        for (size_t i = 0; i < clsDig.size(); i++) out << i + 1 << "\n";
        out << " ];\n\n";

        // Cols
        out << "col = [";
        for (size_t i = 0; i < clsOff.size(); i++)
                out << clsOff[i].col + 1 << "\n";

        for (size_t i = 0; i < clsDig.size(); i++) out << i + 1 << "\n";
        out << " ];\n\n";

        // Vals
        out << "val = [";
        for (size_t i = 0; i < clsOff.size(); i++) out << clsOff[i].val << "\n";

        for (size_t i = 0; i < clsDig.size(); i++) out << clsDig[i] << "\n";
        out << " ];\n\n";

        // Sparse matrix
        out << "M = sparse(row,col,val,m,n);\n";
}  // end method

// -----------------------------------------------------------------------------

void SquareCompressedRowMatrix::Print(ostream &out) const {
        out << "Val = [ ";
        for (size_t i = 0; i < clsOff.size(); i++) out << clsOff[i].val << "\n";
        out << "]\n";

        out << "Ind = [ ";
        for (size_t i = 0; i < clsOff.size(); i++) out << clsOff[i].col << "\n";
        out << "]\n";

        out << "Ptr = [ ";
        for (size_t i = 0; i < clsRow.size(); i++) out << clsRow[i] << "\n";
        out << "]\n";

        out << "Dig = [ ";
        for (size_t i = 0; i < clsDig.size(); i++) out << clsDig[i] << "\n";
        out << "]\n";
}  // end method

// -----------------------------------------------------------------------------

void SquareCompressedRowMatrix::IncompleteCholesky() {
        int g, h, k;
        double z;

        const int N = clsDig.size() - 1;
        for (k = 0; k < N; k++) {
                z = clsDig[k] = sqrt(clsDig[k]);
                for (int l = clsRow[k]; l < clsRow[k + 1]; l++)
                        clsOff[l].val /= z;

                for (int i = clsRow[k]; i < clsRow[k + 1]; i++) {
                        const OffDiagonalElement &off = clsOff[i];
                        z = off.val;
                        h = off.col;
                        g = i;

                        clsDig[h] = clsDig[h] - z * z;

                        for (int j = clsRow[h]; j < clsRow[h + 1]; j++)
                                for (; g < clsRow[k + 1] &&
                                       clsOff[g + 1].col <= clsOff[j].col;
                                     g++)
                                        if (clsOff[g].col == clsOff[j].col)
                                                clsOff[j].val -=
                                                    z * clsOff[g].val;
                }  // end for
        }          // end for

        clsDig[k] = sqrt(clsDig[k]);
}  // end method

// =============================================================================
// SymmetricCompressedRowMatrix
// =============================================================================

void SymmetricCompressedRowMatrix::Initialize(
    const SparseMatrixDescriptor &dscp, const bool checkMatrix) {
        // Check symmetry.
        int symmetryFailCol;
        int symmetryFailRow;
        if (checkMatrix) {
                if (!dscp.CheckIsNumber(symmetryFailRow, symmetryFailCol))
                        throw SCRMException(
                            "The value at (" + std::to_string(symmetryFailRow) +
                            "," + std::to_string(symmetryFailCol) +
                            ") is not a number (nan) or infinity (inf).");

                if (!dscp.CheckSymmetry(symmetryFailRow, symmetryFailCol))
                        throw SCRMException(
                            "Non symmetric matrix at (" +
                            std::to_string(symmetryFailRow) + "," +
                            std::to_string(symmetryFailCol) + ").");
        }  // end if

        // Consolidate.
        Consolidate(dscp, CONSOLIDATION_MODE_DISCARD_LOWER);
}  // end method

// -----------------------------------------------------------------------------

void SymmetricCompressedRowMatrix::Initialize(
    const UpperSparseMatrixDescriptor &dscp) {
        Consolidate(dscp, CONSOLIDATION_MODE_DISCARD_NONE);
}  // end method

// -----------------------------------------------------------------------------

void SymmetricCompressedRowMatrix::Print(ostream &out) const {
        out << "Val = [";
        for (size_t i = 0; i < clsOff.size(); i++) out << clsOff[i].val << "\n";
        out << "]\n";

        out << "Col = [";
        for (size_t i = 0; i < clsOff.size(); i++) out << clsOff[i].col << "\n";
        out << "]\n";

        out << "Row = [";
        for (size_t i = 0; i < clsRow.size(); i++) out << clsRow[i] << "\n";
        out << "]\n";

        out << "Dig = [";
        for (size_t i = 0; i < clsDig.size(); i++) out << clsDig[i] << "\n";
        out << "]\n";
}  // end method

// -----------------------------------------------------------------------------

void SymmetricCompressedRowMatrix::PrintMatlab(ostream &out) const {
        out << "m = " << GetDimension() << "\n";
        out << "n = " << GetDimension() << "\n";
        out << "\n";

        // Rows
        out << "row = [ ";
        const int N = clsRow.size() - 1;
        for (int i = 0; i < N; i++)
                for (int j = clsRow[i]; j < clsRow[i + 1]; j++)
                        out << i + 1 << "\n";

        for (size_t i = 0; i < clsDig.size(); i++) out << i + 1 << "\n";
        out << " ];\n\n";

        // Cols
        out << "col = [";
        for (size_t i = 0; i < clsOff.size(); i++)
                out << clsOff[i].col + 1 << "\n";

        for (size_t i = 0; i < clsDig.size(); i++) out << i + 1 << "\n";
        out << " ];\n\n";

        // Vals
        out << scientific << setprecision(16);

        out << "val = [";
        for (size_t i = 0; i < clsOff.size(); i++) out << clsOff[i].val << "\n";

        for (size_t i = 0; i < clsDig.size(); i++) out << clsDig[i] << "\n";
        out << " ];\n\n";

        // Sparse matrix
        out << "S = sparse(row,col,val,m,n);\n";
        out << "S = S + S';\n";
        out << "for k=1:length(S), S(k,k) = S(k,k)/2;, end\n";
}  // end method

// -----------------------------------------------------------------------------

void SymmetricCompressedRowMatrix::CopyUpper(
    SquareCompressedRowMatrix &M) const {
        M.clsOff = clsOff;
        M.clsRow = clsRow;
        M.clsDig = clsDig;
}  // end method

// =============================================================================
// SparceMatrixBlock
// =============================================================================

void SparseMatrixBlock::Initialize(const SparseMatrixDescriptor &dscp) {
        dimension = dscp.GetNumCol();

        int NumRows = dscp.GetNumRow();
        int NumCols = dscp.GetNumCol();
        int NumNonZeroElements = dscp.GetNumNonZeroElements();

        vector<vector<struct Element> > matr(NumRows);
        Element Elements;

        if (NumRows != NumCols) throw SCRMException("Non square matrix.");

        SparseMatrixDescriptor::ColMap::const_iterator itcol;
        int aux_NumNonZeroElements = 0;

        for (int row = 0; row < NumRows; row++) {
                const SparseMatrixDescriptor::ColMap &m = dscp.GetColMap(row);
                for (itcol = m.begin(); itcol != m.end(); itcol++) {
                        Elements.col = itcol->first;
                        Elements.value = itcol->second;
                        Elements.row = row;
                        matr[Elements.col].push_back(Elements);
                        ++aux_NumNonZeroElements;
                }  // end for
        }          // end for
        if (NumNonZeroElements != aux_NumNonZeroElements)
                throw SCRMException("wrong load matrix.");

        matrAF.resize(NumNonZeroElements);
        fill_arrays(matr, NumRows);
}

// -----------------------------------------------------------------------------

void SparseMatrixBlock::fill_arrays(vector<vector<Element> > &matr,
                                    int NumRows) {
        register int aux_MAX_CACHE_VETX, aux_subd_mtr;
        vector<int> aux_lin_subd(NumRows, 0);
        register int i = 0, l = 0;

        if (num_block >= NumRows)
                aux_MAX_CACHE_VETX = NumRows;
        else
                aux_MAX_CACHE_VETX = num_block;

        if ((NumRows % num_block) == 0)
                aux_subd_mtr = (NumRows / num_block);
        else
                aux_subd_mtr = (NumRows / num_block) + 1;

        for (int k = 0; k < aux_subd_mtr; ++k) {
                for (int j = 0; j < NumRows; ++j) {
                        i = 0;
                        for (; matr[j][i + aux_lin_subd[j]].row <
                                   aux_MAX_CACHE_VETX &&
                               (matr[j].size() > i + aux_lin_subd[j]);
                             ++i) {
                                matrAF[l].col =
                                    matr[j][i + aux_lin_subd[j]].col;
                                matrAF[l].row =
                                    matr[j][i + aux_lin_subd[j]].row;
                                matrAF[l].value =
                                    matr[j][i + aux_lin_subd[j]].value;
                                ++l;
                        }
                        aux_lin_subd[j] += i;
                }
                aux_MAX_CACHE_VETX += num_block;
                if (aux_MAX_CACHE_VETX >= NumRows) aux_MAX_CACHE_VETX = NumRows;
        }  // end for
}  // end method