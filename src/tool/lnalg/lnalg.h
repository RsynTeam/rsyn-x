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

#ifndef LNALG_H
#define LNALG_H

#include <cmath>
#include <vector>
using std::vector;
#include <iostream>
using std::cerr;

// -----------------------------------------------------------------------------

class SquareCompressedRowMatrix;
class SymmetricCompressedRowMatrix;

// -----------------------------------------------------------------------------
// Description: b <- Ax

void Mul(const SquareCompressedRowMatrix &A, const vector<double> &x,
         vector<double> &b);

// -----------------------------------------------------------------------------
// Description: b <- Ax

void Mul(const SymmetricCompressedRowMatrix &A, const vector<double> &d,
         const vector<double> &x, vector<double> &b);

// -----------------------------------------------------------------------------
// Description: b <- Ax where using vector d as diagonal values instead of those
//              in matrix A.

void Mul(const SymmetricCompressedRowMatrix &A, const vector<double> &x,
         vector<double> &b);

// -----------------------------------------------------------------------------
// Description: scalar <- u.v

inline double Dot(const vector<double> &u, const vector<double> &v) {
        register double scalar = 0;
        const register int N = u.size();
        for (register int i = 0; i < N; i++) scalar += u[i] * v[i];
        return scalar;
}  // end function

// -----------------------------------------------------------------------------
// Description: scalar <- u.1

inline double Sum(const vector<double> &u) {
        register double scalar = 0;
        const register int N = u.size();
        for (register int i = 0; i < N; i++) scalar += u[i];
        return scalar;
}  // end function

// -----------------------------------------------------------------------------
// Description: scalar <- u.u

inline double Pow(const vector<double> &u) {
        register double scalar = 0;
        const register int N = u.size();
        for (register int i = 0; i < N; i++)
                scalar +=
                    u[i] * u[i];  // we hope compiler will optimize this :)
        return scalar;
}  // end function

// -----------------------------------------------------------------------------
// Description: scalar <- sqrt(u.u)

inline double Norm(const vector<double> &u) {
        register double scalar = 0;
        const register int N = u.size();
        for (register int i = 0; i < N; i++)
                scalar +=
                    u[i] * u[i];  // we hope compiler will optimize this :)
        return sqrt(scalar);
}  // end function

// -----------------------------------------------------------------------------
// Description:

inline double Diff(const vector<double> &v, const vector<double> &u) {
        const double normv = Norm(v);
        const double normu = Norm(u);

        double sum = 0;
        const int n = v.size();
        for (int i = 0; i < n; i++)
                sum += pow(v[i] / normv - u[i] / normu, 2.0);

        return sqrt(sum);
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- alphau*u + alphav*v

inline void Interpolation(const vector<double> &u, const vector<double> &v,
                          const double alphau, const double alphav,
                          vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++)
                r[i] = alphau * u[i] + alphav * v[i];
}  // end function

// -----------------------------------------------------------------------------
// Description: v <- alpha*u

inline void Scale(const vector<double> &u, const double alpha,
                  vector<double> &v) {
        const register int N = u.size();
        for (register int i = 0; i < N; i++) v[i] = alpha * u[i];
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- alpha*u + beta*v

inline void Add(const vector<double> &u, const double alpha,
                const vector<double> &v, const double beta, vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++) r[i] = alpha * u[i] + beta * v[i];
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- u + alpha*v

inline void Add(const vector<double> &u, const vector<double> &v,
                const double alpha, vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++) r[i] = u[i] + alpha * v[i];
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- u + v

inline void Add(const vector<double> &u, const vector<double> &v,
                vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++) r[i] = u[i] + v[i];
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- u - alpha*v

inline void Sub(const vector<double> &u, const vector<double> &v,
                const double alpha, vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++) r[i] = u[i] - alpha * v[i];
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- u - v

inline void Sub(const vector<double> &u, const vector<double> &v,
                vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++) r[i] = u[i] - v[i];
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- u / alpha

inline void Div(const vector<double> &u, const double alpha,
                vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++) r[i] = u[i] / alpha;
}  // end function

// -----------------------------------------------------------------------------
// Description: r <- u

inline void Cpy(const vector<double> &u, vector<double> &r) {
        const register int N = r.size();
        for (register int i = 0; i < N; i++) r[i] = u[i];
}  // end function

// -----------------------------------------------------------------------------

void Backward(const SquareCompressedRowMatrix &A, const vector<double> &b,
              vector<double> &x);

// -----------------------------------------------------------------------------

void Forward(const SquareCompressedRowMatrix &A, const vector<double> &b,
             vector<double> &x);

// -----------------------------------------------------------------------------
// Description: Solve the system Ax = b using Conjugate Gradient method.

template <typename Matrix>
void SolveByConjugateGradient(const Matrix &A, const vector<double> &b,
                              vector<double> &x,
                              const int maxNumIterations = 500,
                              const double tolerance = 1e-5) {
        vector<double> r(x.size());  // residual
        vector<double> d(x.size());
        vector<double> v(x.size());  // auxiliary vector to perform b - Ax
        vector<double> q(x.size());

        double normf, rho, rhoNew, alpha;

        // r <- b - Ax
        Mul(A, x, v);
        Sub(b, v, r);
        Cpy(r, d);

        // Compute norm of right-hand side to take relative residuum as
        // break condition.
        normf = Norm(b);

        // if the norm is very close to zero, take the
        // absolute residuum instead as break condition
        // ( norm(r) > tol ), since the relative
        // residuum will not work (division by zero).
        if (normf < 2.2204e-016) {
                cerr << "norm(f) is very close to zero, taking absolute "
                        "residuum... as break condition.\n";
                normf = 1;
        }  // end if

        rho = Pow(r);

        int niter = 0;
        while (Norm(r) / normf > tolerance) {
                Mul(A, d, q);  // q = A*d;

                alpha = rho / Dot(q, d);     // alpha = rho/(q'*d);
                Add(x, d, alpha, x);         // x = x + alpha*d;
                Sub(r, q, alpha, r);         // r = r - alpha*q;
                rhoNew = Pow(r);             // rho_new = r'*r;
                Add(r, d, rhoNew / rho, d);  // d = r + rho_new/rho * d;
                rho = rhoNew;                // rho = rho_new;

                if (++niter == maxNumIterations) break;

        }  // end for

        std::cout << "#iterations = " << niter << "\n";
}  // end function

// -----------------------------------------------------------------------------

template <typename Matrix>
void SolveByConjugateGradientWithPCond(const Matrix &A, const vector<double> &b,
                                       vector<double> &x,
                                       const int maxNumIterations,
                                       const double tolerance,
                                       const SquareCompressedRowMatrix &U,
                                       const SquareCompressedRowMatrix &L) {
        vector<double> r(x.size());  // residual
        vector<double> d(x.size());
        vector<double> v(x.size());  // auxiliary vector to perform b - Ax
        vector<double> q(x.size());

        vector<double> y(x.size());  // auxiliary
        vector<double> s(x.size());

        double normf, rho, rhoNew, alpha;

        // r <- b - Ax
        Mul(A, x, v);
        Sub(b, v, r);

        // d <- M-1*r = solve Md = r
        Forward(L, r, y);
        Backward(U, y, d);

        // Compute norm of right-hand side to take relative residuum as
        // break condition.
        normf = Norm(b);

        // if the norm is very close to zero, take the
        // absolute residuum instead as break condition
        // ( norm(r) > tol ), since the relative
        // residuum will not work (division by zero).
        if (normf < 2.2204e-016) {
                cerr << "norm(f) is very close to zero, taking absolute "
                        "residuum... as break condition.\n";
                normf = 1;
        }  // end if

        // rho = r'*d
        rho = Dot(r, d);

        int niter = 0;
        while (Norm(r) / normf > tolerance) {
                Mul(A, d, q);  // q = A*d;

                alpha = rho / Dot(q, d);  // alpha = rhoNew/(q'*d);
                Add(x, d, alpha, x);      // x = x + alpha*d;
                Sub(r, q, alpha, r);      // r = r - alpha*q;

                // s <- M-1*r
                Forward(L, r, y);
                Backward(U, y, s);

                rhoNew = Dot(r, s);          // rho_new = r'*s;
                Add(s, d, rhoNew / rho, d);  // d = s + rho_new/rho * d;
                rho = rhoNew;

                niter++;
                // cerr << "#iterations = " << niter << "\t" << Norm( r )/normf
                // << "\n";

                if (niter >= maxNumIterations)

                        // if ( ++niter == maxNumIterations )
                        break;
        }  // end while

        // std::cout << "#iterations = " << niter  << "\n";
}  // end function

// -----------------------------------------------------------------------------

template <typename Matrix>
void SolveByConjugateGradientWithDiagonalPreConditioner(
    const Matrix &A, const vector<double> &b, vector<double> &x,
    const int maxNumIterations = 500, const double tolerance = 1e-5) {
        vector<double> r(x.size());  // residual
        vector<double> d(x.size());
        vector<double> v(x.size());  // auxiliary vector to perform b - Ax
        vector<double> q(x.size());

        vector<double> y(x.size());  // auxiliary
        vector<double> s(x.size());

        double normf, rho, rhoNew, alpha;

        // r <- b - Ax
        Mul(A, x, v);
        Sub(b, v, r);

        // d <- M-1*r = solve Md = r
        for (int i = 0; i < A.GetDimension(); i++)
                d[i] = (1.0 / A.GetDiagonalVector()[i]) * r[i];

        // Compute norm of right-hand side to take relative residuum as
        // break condition.
        normf = Norm(b);

        // if the norm is very close to zero, take the
        // absolute residuum instead as break condition
        // ( norm(r) > tol ), since the relative
        // residuum will not work (division by zero).
        if (normf < 2.2204e-016) {
                cerr << "norm(f) is very close to zero, taking absolute "
                        "residuum... as break condition.\n";
                normf = 1;
        }  // end if

        // rho = r'*d
        rho = Dot(r, d);

        int niter = 0;
        while (Norm(r) / normf > tolerance) {
                Mul(A, d, q);  // q = A*d;

                alpha = rho / Dot(q, d);  // alpha = rhoNew/(q'*d);
                Add(x, d, alpha, x);      // x = x + alpha*d;
                Sub(r, q, alpha, r);      // r = r - alpha*q;

                // s <- M-1*r
                for (int i = 0; i < A.GetDimension(); i++)
                        s[i] = (1.0 / A.GetDiagonalVector()[i]) * r[i];

                rhoNew = Dot(r, s);          // rho_new = r'*s;
                Add(s, d, rhoNew / rho, d);  // d = s + rho_new/rho * d;
                rho = rhoNew;

                niter++;
                // cerr << "#iterations = " << niter << "\t" << Norm( r )/normf
                // << "\n";

                if (niter >= maxNumIterations) break;
        }  // end while

        std::cout << "#iterations = " << niter << "\n";
}  // end function

// -----------------------------------------------------------------------------

void Cholesky(const SymmetricCompressedRowMatrix &A,
              SquareCompressedRowMatrix &U, SquareCompressedRowMatrix &L);

// -----------------------------------------------------------------------------
// Computes the second smallest eigenvector (the one associated with the second
// smallest eigenvalue) of a laplacian matrix.

void SecondSmallestLaplacianEigenvector(const SymmetricCompressedRowMatrix &L,
                                        const double epsilon,
                                        vector<double> &eigenvector);

// -----------------------------------------------------------------------------
// Computes the largest eigenvector (the one associates with de largest
// eigenvalue) through power method.

void LargestEigenvector(const SquareCompressedRowMatrix &M,
                        const double epsilon, vector<double> &eigenvector);

#endif