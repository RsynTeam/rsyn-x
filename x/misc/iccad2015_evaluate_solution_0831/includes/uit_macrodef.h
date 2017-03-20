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

#ifndef UIT_MACRODEF_H_
#define UIT_MACRODEF_H_

#define UIT_MAX_NUM_THREADS 8

#define UIT_INT_INF INT_MAX
#define UIT_INT_NINF INT_MIN
#define UIT_INT_MAX UIT_INT_INF-1
#define UIT_INT_MIN UIT_INT_NINF+1

#define UIT_SIZE_INF SIZE_MAX
#define UIT_SIZE_MAX UIT_SIZE_INF-1
#define UIT_SIZE_MIN 0

#define UIT_FLT_INF FLT_MAX
#define UIT_FLT_NINF -FLT_MAX
#define UIT_FLT_MAX 1e16f
#define UIT_FLT_MIN -1e16f
#define UIT_FLT_ZERO 0.0f
#define UIT_FLT_ONE 1.0f
#define UIT_FLT_TWO 2.0f
#define TAU2015_FLT_MAX 987654.0f
#define TAU2015_FLT_MIN -987654.0f

#define UIT_DEFAULT_PIN_DICTIONARY_SIZE 786433
#define UIT_DEFAULT_INSTANCE_DICTIONARY_SIZE 393241
#define UIT_DEFAULT_NET_DICTIONARY_SIZE 393241

#define UIT_UNDEFINED_INT UIT_INT_INF
#define UIT_UNDEFINED_FLT UIT_FLT_INF
#define UIT_UNDEFINED_IDX -1
#define UIT_UNDEFINED_LEVEL -1

#define UIT_DEFAULT_NODE_LEVEL 0

#define UIT_DEFAULT_CELL_LEAKAGE_POWER UIT_FLT_ZERO
#define UIT_DEFAULT_CELL_AREA UIT_FLT_ZERO
#define UIT_DEFAULT_CELL_TYPE COMBINATIONAL_CELL_TYPE

#define UIT_DEFAULT_CELLPIN_CAPACITANCE UIT_FLT_ZERO
#define UIT_DEFAULT_CELLPIN_MAX_CAPACITANCE UIT_FLT_MAX
#define UIT_DEFAULT_CELLPIN_MIN_CAPACITANCE UIT_FLT_ZERO
#define UIT_DEFAULT_CELLPIN_IS_CLOCK false

#define UIT_DEFAULT_NET_LCAP UIT_FLT_ZERO

#define UIT_DEFAULT_EDGE_DELAY UIT_FLT_ZERO
#define UIT_DEFAULT_TEST_SLACK UIT_FLT_ZERO

#define UIT_DEFAULT_RCTREE_NODE_CAP UIT_FLT_ZERO
#define UIT_DEFAULT_RCTREE_NODE_LCAP UIT_FLT_ZERO
#define UIT_DEFAULT_RCTREE_NODE_BETA UIT_FLT_ZERO
#define UIT_DEFAULT_RCTREE_NODE_DELAY UIT_FLT_ZERO
#define UIT_DEFAULT_RCTREE_NODE_LDELAY UIT_FLT_ZERO
#define UIT_DEFAULT_RCTREE_NODE_IMPULSE UIT_FLT_ZERO
#define UIT_DEFAULT_RCTREE_EDGE_RES UIT_FLT_ZERO

#define UIT_DEFAULT_PRIMARY_OUTPUT_LCAP UIT_FLT_ZERO
#define UIT_DEFAULT_PRIMARY_OUTPUT_RAT UIT_UNDEFINED_FLT
#define UIT_DEFAULT_PRIMARY_INPUT_AT UIT_FLT_ZERO
#define UIT_DEFAULT_PRIMARY_INPUT_SLEW UIT_FLT_ZERO

#define UIT_DEFAULT_SPEFCAP UIT_FLT_ZERO
#define UIT_DEFAULT_SPEFRES UIT_FLT_ZERO
#define UIT_DEFAULT_SPEFLCAP UIT_FLT_ZERO

#define UIT_DEFAULT_CLOCK_SOURCE_PERIOD UIT_FLT_ZERO

#define UIT_SQUARE(X) (X)*(X)

#define EARLY 0
#define LATE  1
#define RISE  0
#define FALL  1
#define UIT_UNDEFINED_TRANS -1
#define UIT_UNDEFINED_SPLIT -1

#define UIT_EL_ITER(el) for(int el=0; el<2; ++el) 
#define UIT_RF_ITER(rf) for(int rf=0; rf<2; ++rf) 
#define UIT_RF_RF_ITER(irf, orf) for(int irf=0; irf<2; ++irf) \
                                   for(int orf=0; orf<2; ++orf)
#define UIT_EL_RF_ITER(el, rf) for(int el=0; el<2; ++el) \
                                 for(int rf=0; rf<2; ++rf)
#define UIT_EL_RF_RF_ITER(el, rf1, rf2) for(int el=0; el<2; ++el) \
                                          for(int rf1=0; rf1<2; ++rf1) \
                                            for(int rf2=0; rf2<2; ++rf2)


#define FAST_LOG2_FLOOR(x) (sizeof(unsigned int)*8 - 1 - __builtin_clz((unsigned int)(x)))

#endif

