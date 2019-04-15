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

#ifndef SCRM_H
#define SCRM_H

#include <iostream>
using std::ostream;
using std::cerr;
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <tuple>
#include <string>
using std::string;

#include "util/Double2.h"

// -----------------------------------------------------------------------------

class SCRMException : public std::exception {
       private:
        string clsMsg;

       public:
        SCRMException(const string &msg) : clsMsg(msg) {}
        virtual ~SCRMException() throw() {}

        virtual const char *what() const throw() {
                return clsMsg.c_str();
        }  // end method
};         // end class

// -----------------------------------------------------------------------------

class SparseMatrixDescriptor {
       private:
        int clsMaxRow;
        int clsMaxCol;

        int clsNumNonZeroElements;

       public:
        SparseMatrixDescriptor();

        // Auxiliary structure used when build the matrix (before calling
        // Consolidate()).
        typedef map<int, double> ColMap;

        // Add elements to the matrix:
        // - If we are going to add an element in the (row,column) that already
        //   exists, the (row, column) element is set to the old value plus
        //   the new one.
        void AddElement(const int row, const int col, const double value);
        void AddDiagonalElement(const int row, const double value);

        // Gets.
        int GetNumCol() const { return clsMaxCol + 1; }
        int GetNumRow() const { return clsRows.size(); }
        int GetNumNonZeroElements() const { return clsNumNonZeroElements; }

        const ColMap &GetColMap(const int row) const { return clsRows[row]; }

        // Returns the element at (row,col) position.
        double ElementAt(const int row, const int col) const;

        // Return true if the matrix has only numbers.
        // Otherwise returns false. If the matrix has a position that is
        // infinity (inf) or has a
        // position that is not a number (nan), the parameter (row, col)
        // indicates the first position with the problem.
        bool CheckIsNumber(int &row, int &col) const;

        // Returns true if this descriptor stores a symmetric matrix and false
        // otherwise. If the matrix is not symmetric (row, col) indicates the
        // first element non-symmetric element found.
        bool CheckSymmetry(int &row, int &col) const;

        // Returns true if this descriptor stores a upper triangular matrix and
        // false otherwise. If matrix is not lower, (row, col) indicates the
        // first lower element found.
        bool CheckUpper(int &row, int &col) const;

        // Returns true if this descriptor stores a lower triangular matrix and
        // false otherwise. If matrix is not lower, (row, col) indicates the
        // first upper element found.
        bool CheckLower(int &row, int &col) const;

        // Print matrix.
        void Print(ostream &out) const;

        // Clear
        void Clear();

       private:
        vector<ColMap> clsRows;
};  // end class

// -----------------------------------------------------------------------------

class UpperSparseMatrixDescriptor : public SparseMatrixDescriptor {
       public:
        void AddElement(const int row, const int col, const double value);
};  // end class

// -----------------------------------------------------------------------------

class LowerSparseMatrixDescriptor : public SparseMatrixDescriptor {
       public:
        void AddElement(const int row, const int col, const double value);
};  // end class

// -----------------------------------------------------------------------------

struct OffDiagonalElement {
        double val;
        int col;
};  // end struct

// -----------------------------------------------------------------------------

class RawSparseMatrixDescriptor {
       public:
        vector<OffDiagonalElement> off;  // off-diagonal values
        vector<double> dig;              // diagonal values
        vector<int> row;                 // row pointers
};                                       // end class

// -----------------------------------------------------------------------------

class AbstractSquareCompressedRowMatrix {
       protected:
        // Matrix
        vector<OffDiagonalElement> clsOff;  // off-diagonal elements
        vector<double> clsDig;              // diagonal values
        vector<int> clsRow;                 // row pointers

        enum ConsolidationMode {
                CONSOLIDATION_MODE_DISCARD_NONE,
                CONSOLIDATION_MODE_DISCARD_LOWER,
                CONSOLIDATION_MODE_DISCARD_UPPER
        };

        void Consolidate(const SparseMatrixDescriptor &dscp,
                         const ConsolidationMode mode);

       public:
        AbstractSquareCompressedRowMatrix();

        // Resets the matrix.
        void Reset();

        // Gets
        int GetDimension() const { return clsDig.size(); }
        int GetNumNonZeroEntries() const {
                return clsDig.size() + clsOff.size();
        }
        int GetNummNonZeroOffDiagonalEntries() const { return clsOff.size(); }

        double GetDiagonalElement(const int index) const {
                return clsDig[index];
        }

        const vector<double> &GetDiagonalVector() const { return clsDig; }
        vector<double> &GetDiagonalVector() { return clsDig; }

        const vector<OffDiagonalElement> &GetOffDiagonalVector() const {
                return clsOff;
        }
        const vector<int> &GetRowPointerVector() const { return clsRow; }

        // Set the diagonal element value.
        void SetDiagonalElement(const int row, const double value) {
                clsDig[row] = value;
        }
        void SetDiagonalVector(const vector<double> &diagonal) {
                clsDig = diagonal;
        }

        // Add a value to an diagonal entry.
        void AddDiagonalElement(const int row, const double value) {
                clsDig[row] += value;
        }

        // Set a value to an existent position.
        void SetElement(const int col, const int row, const double value) {
                if (col == row) {
                        SetDiagonalElement(row, value);
                } else {
                        const int i0 = clsRow[row];
                        const int i1 = clsRow[row + 1];
                        for (int i = i0; i < i1; i++) {
                                OffDiagonalElement &off = clsOff[i];
                                if (off.col == col) {
                                        off.val = value;
                                        return;
                                }  // end if
                        }          // end for
                        throw SCRMException(
                            "Element must already be initialized in order to "
                            "be updated.");
                }  // end else
        }          // end method

        // Add a value to an existent position.
        void AddElement(const int col, const int row, const double value) {
                if (col == row) {
                        AddDiagonalElement(row, value);
                } else {
                        const int i0 = clsRow[row];
                        const int i1 = clsRow[row + 1];
                        for (int i = i0; i < i1; i++) {
                                OffDiagonalElement &off = clsOff[i];
                                if (off.col == col) {
                                        off.val += value;
                                        return;
                                }  // end if
                        }          // end for
                        throw SCRMException(
                            "Element must already be initialized in order to "
                            "be updated.");
                }  // end else
        }          // end method

        // Build the matrix using a raw descriptor. Basically the vectors inside
        // the descriptor are swapped with the ones inside this class and then
        // are directly used to represent the matrix. User must provide data in
        // the right format (CRS).
        void Build(RawSparseMatrixDescriptor &dscp);

        // Initialize the matrix.
        virtual void Initialize(const SparseMatrixDescriptor &dscp,
                                const bool checkSymmetry = true) = 0;

        // Check if some diagonal element is zero: |value| <= epsilon.
        bool HasNullDiagonalElement(int &row, const double epsilon = 0) const;

        // Check if some row has no off-diagonal element.
        // Note that for symmetric matrixes, which stores only the upper part,
        // this method does not check for "implicit" off-diagonal elements.
        bool HasEmptyRow(int &row) const;

};  // end class

// -----------------------------------------------------------------------------

class SquareCompressedRowMatrix : public AbstractSquareCompressedRowMatrix {
        friend class SymmetricCompressedRowMatrix;

        friend void Mul(const SquareCompressedRowMatrix &A,
                        const vector<double> &x, vector<double> &b);
        friend void Transpose(const SquareCompressedRowMatrix &A,
                              SquareCompressedRowMatrix &B);
        friend void Forward(const SquareCompressedRowMatrix &A,
                            const vector<double> &x, vector<double> &b);
        friend void Backward(const SquareCompressedRowMatrix &A,
                             const vector<double> &x, vector<double> &b);

        friend void Mul(const SquareCompressedRowMatrix &A,
                        const vector<double2> &x, vector<double2> &b);
        friend void Forward(const SquareCompressedRowMatrix &A,
                            const vector<double2> &x, vector<double2> &b);
        friend void Backward(const SquareCompressedRowMatrix &A,
                             const vector<double2> &x, vector<double2> &b);

       public:
        // Normalize the matriz columns such that their sum is one.
        void MarkovNormalization();

        // Incomplete Cholescky Factorization.
        void IncompleteCholesky();

        // Printing.
        void Print(ostream &out) const;
        void PrintMatlab(ostream &out) const;

        // Initialize.
        virtual void Initialize(const SparseMatrixDescriptor &dscp,
                                const bool checkSymmetry = true);
};  // end class

// -----------------------------------------------------------------------------

class SymmetricCompressedRowMatrix : public AbstractSquareCompressedRowMatrix {
        friend void Mul(const SymmetricCompressedRowMatrix &A,
                        const vector<double> &d, const vector<double> &x,
                        vector<double> &b);
        friend void Mul(const SymmetricCompressedRowMatrix &A,
                        const vector<double> &x, vector<double> &b);

        friend void Mul(const SymmetricCompressedRowMatrix &A,
                        const vector<double> &d, const vector<double2> &x,
                        vector<double2> &b);
        friend void Mul(const SymmetricCompressedRowMatrix &A,
                        const vector<double2> &x, vector<double2> &b);

       public:
        // Print.
        void Print(ostream &out) const;
        void PrintMatlab(ostream &out) const;

        // Copy
        void CopyUpper(SquareCompressedRowMatrix &M) const;

        // Set a value to an existent position.
        void SetElement(const int col, const int row, const double value) {
                if (row > col) {
                        throw SCRMException(
                            "Symmetric matrices only store the elements in the "
                            "upper part.");
                } else {
                        AbstractSquareCompressedRowMatrix::SetElement(col, row,
                                                                      value);
                }  // end else
        }          // end method

        // Add a value to an existent position.
        void AddElement(const int col, const int row, const double value) {
                if (row > col) {
                        throw SCRMException(
                            "Symmetric matrices only store the elements in the "
                            "upper part.");
                } else {
                        AbstractSquareCompressedRowMatrix::AddElement(col, row,
                                                                      value);
                }  // end else
        }          // end method

        // Initialize.
        virtual void Initialize(const SparseMatrixDescriptor &dscp,
                                const bool checkMatrix = true);
        void Initialize(const UpperSparseMatrixDescriptor &dscp);
};  // end class

// -----------------------------------------------------------------------------

class SparseMatrixBlock {
        friend void Mul(const SparseMatrixBlock &A, const vector<double> &x,
                        vector<double> &y);
        friend void Mul(const SparseMatrixBlock &A, const vector<double2> &x,
                        vector<double2> &y);

       private:
        struct Element {
                int row;
                int col;
                double value;
        };

        vector<Element> matrAF;
        int num_block;
        int dimension;
        void fill_arrays(vector<vector<Element> > &mat, int NumRows);

       public:
        SparseMatrixBlock() { num_block = 20000; }
        void Initialize(const SparseMatrixDescriptor &dscp);
        void mult_matriz_esparsa(const vector<double> &x,
                                 vector<double> &y) const;  // a.(*x)= y

        int GetDimension() const { return dimension; }
};  // end class

#endif
