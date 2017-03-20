/******************************************************************************
 *                                                                            *
 * Copyright (c) 2015, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef UIT_UTILITY_H_
#define UIT_UTILITY_H_

#include "uit_headerdef.h"
#include "uit_macrodef.h"
#include "uit_typedef.h"

namespace uit {

// Integer struct.
struct IntPair {
  int_t first;
  int_t second;
};

//-------------------------------------------------------------------------------------------------

// Explorer struct.
struct Explorer {
  int first;
  node_pt second;
};

// Function: make_explorer
explorer_t make_explorer(int_t, node_pt);

// Procedure: make_explorer
void_t make_explorer(explorer_rt, int_t, node_pt);

//-------------------------------------------------------------------------------------------------

// Function: to_numeric
float_t to_numeric(string_crt);

// Function: numeric_guard
void_t numeric_guard(float_rt);

// Function: tau2015_numeric_guard
void_t tau2015_numeric_guard(float_rt);

// Procedure: resize
void_t resize(float_mrt, size_t, size_t);

// Procedure: resize
void_t resize(float_mrt, size_t, size_t, float_ct);

// Function: file_max_line_size.
size_t file_max_line_size(const char file_path[]);

//-------------------------------------------------------------------------------------------------

// Procedure: clear_vector
template <class T>
void_t clear_vector(vector <T> & vec) {
  vec.clear();
}

// Procedure: clear_matrix
template <class T>
void_t clear_matrix(vector < vector<T> > &mat) {
  for(unsigned_t i=0; i<mat.size(); ++i) {
    mat[i].clear();
  }
  mat.clear();
}




};



#endif


