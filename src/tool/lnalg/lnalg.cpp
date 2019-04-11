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
using std::cout;
using std::cerr;
#include <fstream>
using std::ofstream;
#include <algorithm>
using std::swap;
#include <cstdlib>

#include "lnalg.h"
#include "scrm.h"

// -----------------------------------------------------------------------------

ostream &operator<<(ostream &out, vector<double> &v) {
        for (size_t i = 0; i < v.size(); i++) out << v[i] << "\n";
        return out;
}  // end function

// -----------------------------------------------------------------------------

void Cholesky(const SymmetricCompressedRowMatrix &A,
              SquareCompressedRowMatrix &U, SquareCompressedRowMatrix &L) {
        A.CopyUpper(U);
        U.IncompleteCholesky();
        Transpose(U, L);
}  // end method

// -----------------------------------------------------------------------------

void Mul(const SquareCompressedRowMatrix &A, const vector<double> &x,
         vector<double> &b) {
        // TIP FOR DEBUGGING:
        // - Try to debug using eigenvector and its respective eigenvalue.

        register int N = x.size();
        register int i, j, k;

        for (i = 0; i < N; i++) {
                double &bi = b[i];

                // Diagonal
                bi = A.clsDig[i] * x[i];

                // Off-Diagonal
                k = A.clsRow[i + 1];
                for (j = A.clsRow[i]; j < k; j++) {
                        const OffDiagonalElement &off = A.clsOff[j];
                        bi += off.val * x[off.col];
                }  // end for
        }          // end for
}  // end function
// -----------------------------------------------------------------------------

void Mul(const SymmetricCompressedRowMatrix &A, const vector<double> &d,
         const vector<double> &x, vector<double> &b) {
        // TIP FOR DEBUGGING:
        // - Try to debug using eigenvector and its respective eigenvalue.

        register int N = A.GetDimension();
        register int i, j, k;

        for (i = 0; i < N; i++) b[i] = 0;

        N--;  // -1 discounts last row which has only the diagonal element,
              // and therefore is handled separately.

        for (i = 0; i < N; i++) {
                const double xi = x[i];
                double &bi = b[i];

                // Diagonal
                bi += d[i] * x[i];

                // Off-Diagonal
                k = A.clsRow[i + 1];
                for (j = A.clsRow[i]; j < k; j++) {
                        const OffDiagonalElement &off = A.clsOff[j];
                        bi += off.val * x[off.col];
                        b[off.col] += off.val * xi;
                }  // end for
        }          // end for

        // Last element.
        b[i] += d[i] * x[i];
}  // end function

// -----------------------------------------------------------------------------

void Mul(const SymmetricCompressedRowMatrix &A, const vector<double> &x,
         vector<double> &b) {
        Mul(A, A.clsDig, x, b);
}  // end function

// -----------------------------------------------------------------------------

void Backward(const SquareCompressedRowMatrix &A, const vector<double> &b,
              vector<double> &x) {
        register int n = b.size() - 1;
        x[n] = b[n] / A.clsDig[n];
        for (n = n - 1; n >= 0; n--) {
                x[n] = b[n];
                for (int i = A.clsRow[n]; i < A.clsRow[n + 1]; i++) {
                        const OffDiagonalElement &off = A.clsOff[i];
                        x[n] -= off.val * x[off.col];
                }  // end for
                x[n] /= A.clsDig[n];
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Forward(const SquareCompressedRowMatrix &A, const vector<double> &b,
             vector<double> &x) {
        x[0] = b[0] / A.clsDig[0];

        const int N = A.GetDimension();
        register int n;
        for (n = 1; n < N; n++) {
                x[n] = b[n];
                for (int i = A.clsRow[n]; i < A.clsRow[n + 1]; i++) {
                        const OffDiagonalElement &off = A.clsOff[i];
                        x[n] -= off.val * x[off.col];
                }  // end for
                x[n] /= A.clsDig[n];
        }  // end for
}  // end method

// -----------------------------------------------------------------------------

void Transpose(const SquareCompressedRowMatrix &A,
               SquareCompressedRowMatrix &B) {
        B.clsOff.resize(A.clsOff.size());
        B.clsRow.resize(A.clsRow.size());

        B.clsDig = A.clsDig;

        vector<int> aux(A.clsRow.size(), 0);

        for (size_t j = 0; j < A.clsOff.size(); j++) aux[A.clsOff[j].col]++;

        B.clsRow[0] = 0;
        for (size_t b = 1; b < aux.size(); b++)
                B.clsRow[b] = B.clsRow[b - 1] + aux[b - 1];

        aux = B.clsRow;

        const int N = A.clsRow.size() - 1;
        for (int i = 0; i < N; i++) {
                const int beg = A.clsRow[i];
                const int end = A.clsRow[i + 1];

                for (int j = beg; j < end; j++) {
                        const int index = aux[A.clsOff[j].col]++;
                        B.clsOff[index].val = A.clsOff[j].val;
                        B.clsOff[index].col = i;
                }  // end for
        }          // end for
}  // end method

// -----------------------------------------------------------------------------

void SecondSmallestLaplacianEigenvector(const SymmetricCompressedRowMatrix &L,
                                        const double epsilon,
                                        vector<double> &eigenvector) {
        const int n = L.GetDimension();

        // Pingponged variables.
        vector<double> x[2];
        vector<double> h[2];
        vector<double> g[2];
        double F[2];
        double rho[2];
        double beta[2];

        // Variables.
        vector<double> y(n);
        vector<double> e(n, 1.0 / sqrt(double(n)));
        vector<double> aux_a(n);
        vector<double> aux_b(n);
        double q1;
        double u1;
        double s1;
        double t1;
        double p1;
        double r1;
        double alpha;
        double alpha_2;

        // Initializations.
        for (int i = 0; i < 2; i++) {
                x[i].resize(n);
                h[i].resize(n);
                g[i].resize(n);
        }  // end for

        for (int i = 0; i < n / 2; i++) x[0][i] = -(rand() / double(RAND_MAX));

        Scale(e, Dot(e, x[0]) / Dot(e, e), aux_a);
        Sub(x[0], aux_a, x[0]);

        // cerr << "Dot=" << Dot(e, x[0]) << "\n";

        rho[0] = Pow(x[0]);
        Mul(L, x[0], y);
        F[0] = Dot(x[0], y) / rho[0];
        Interpolation(x[0], y, (2 * F[0]) / rho[0], -2 / rho[0], g[0]);
        h[0] = g[0];
        beta[0] = Pow(g[0]);

        int ping = 0;
        int pong = 1;

        int iteration = 1;

        while (true) {
                // previous iteration: k-1
                const vector<double> &x0 = x[ping];
                const vector<double> &h0 = h[ping];
                const vector<double> &g0 = g[ping];
                const double F0 = F[ping];
                const double rho0 = rho[ping];
                const double beta0 = beta[ping];

                // current iteration: k
                vector<double> &x1 = x[pong];
                vector<double> &h1 = h[pong];
                vector<double> &g1 = g[pong];
                double &F1 = F[pong];
                double &rho1 = rho[pong];
                double &beta1 = beta[pong];

                Mul(L, h0, y);
                p1 = rho0;  // ok, this seems not be necessary
                q1 = 2 * Dot(x0, h0);
                r1 = Pow(h0);
                s1 = rho0 * F0;
                t1 = 2 * Dot(x0, y);
                u1 = Dot(h0, y);

                alpha = (-(u1 * p1 - s1 * r1) +
                         sqrt(pow(u1 * p1 - s1 * r1, 2.0) -
                              (u1 * q1 - t1 * r1) * (t1 * p1 - s1 * q1))) /
                        (u1 * q1 - t1 * r1);

                alpha_2 = pow(alpha, 2.0);

                F1 = (s1 + t1 * alpha + u1 * alpha_2) /
                     (p1 + q1 * alpha + r1 * alpha_2);

                Add(x0, h0, alpha, x1);
                rho1 = Pow(x1);

                // g1 <- (2/p1)*( F1*x1 - F0*x0 + (1/2)p0*g0 - alpha*y1 )
                Interpolation(x1, x0, F1, -F0, aux_a);
                Interpolation(g0, y, 0.5 * rho0, -alpha, aux_b);
                Add(aux_a, aux_b, aux_a);
                Scale(aux_a, 2 / rho1, g1);

                beta1 = Pow(g1);
                Add(g1, h0, beta1 / beta0, h1);

                const double error = Norm(g1);
                if (error < epsilon) break;

                iteration++;
                swap(ping, pong);

                cout << "\r" << iteration << ": " << error;
        }  // end while
        cout << "\n";

        // Normalize
        const vector<double> &result = x[ping];

        double norm = 0;
        for (int i = 0; i < L.GetDimension(); i++) norm += pow(result[i], 2.0);
        norm = sqrt(norm);

        eigenvector.resize(n);
        for (int i = 0; i < L.GetDimension(); i++)
                eigenvector[i] = result[i] / norm;
}  // end method

// -----------------------------------------------------------------------------

void LargestEigenvector(const SquareCompressedRowMatrix &M,
                        const double epsilon, vector<double> &eigenvector) {
        vector<double> aux[2];
        for (int i = 0; i < 2; i++) aux[i].resize(M.GetDimension(), 1);

        int ping = 0;
        int pong = 1;

        while (true) {
                const vector<double> &p0 = aux[ping];
                vector<double> &p1 = aux[pong];

                Mul(M, p0, p1);  // p1 <- M x p0
                if (Diff(p0, p1) < epsilon) break;
                swap(ping, pong);
        }  // end while

        eigenvector = aux[ping];
}  // end method
