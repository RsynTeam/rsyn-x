/******************************************************************************
 *                                                                            *
 * Copyright (c) 2014, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef UIT_FUNCTOR_H_
#define UIT_FUNCTOR_H_

#include "uit_cppr.h"
#include "uit_test.h"

namespace uit{

// Functor: ComparePathMatrix
class ComparePathMatrix {
  
  public:

    ComparePathMatrix(path_trace_mrt in);
    ~ComparePathMatrix();

    bool_t operator () (int_pair_crt, int_pair_crt) const;

  private:
    
    path_trace_mrt _path_matrix;
};

//-------------------------------------------------------------------------------------------------

// Class: CompareTestpoint
class CompareTestpoint {
  
  public:

    bool_t operator () (testpoint_crt, testpoint_crt) const;
};

//-------------------------------------------------------------------------------------------------

// Class: ComparePathTrace
class ComparePathTrace {
  
  public:

    bool_t operator () (path_trace_crt, path_trace_crt) const;
    bool_t operator () (path_trace_pt, path_trace_pt) const;

};

//-------------------------------------------------------------------------------------------------


};

#endif

