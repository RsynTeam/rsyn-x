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

#ifndef UIT_TYPEDEF_H_
#define UIT_TYPEDEF_H_

#include "uit_unitdef.h"
#include "uit_headerdef.h"
#include "uit_enumdef.h"
#include "uit_classdef.h"
#include "uit_dictionary.h"
#include "uit_list.h"

namespace uit {

// Typedef definition field.
// suffix 't': generic type.
// suffix 'e': enum type.
// prefix 'p': pointer.
// prefix 'c': const.
// prefix 'r': reference.
// prefix 'v': vector.
// prefix 's': static.
// prefix 'm': matrix type.
// prefix 'dict': dictionary type.

// Typedef of integer variable.
typedef int          int_t;                                 // Integer type.
typedef int*         int_pt;                                // Integer pointer type.
typedef int&         int_rt;                                // Integer reference type.
typedef const int    int_ct;                                // Constant integer type.
typedef const int*   int_cpt;                               // Constant integer pointer type.
typedef const int&   int_crt;                               // Constant integer reference type.
typedef vector < int > int_vt;                              // Int 1D vector type.
typedef vector < int >* int_vpt;                            // Int 1D vector pointer type.
typedef vector < int >& int_vrt;                            // Int 1D vector reference type.
typedef vector < vector<int> > int_mt;                      // Int 2D vector type.
typedef vector < vector<int> >* int_mpt;                    // Int 2D vector pointer type.
typedef vector < vector<int> >& int_mrt;                    // Int 2D vector reference type.
typedef struct IntPair int_pair_t;                          // Int pair type.
typedef struct IntPair& int_pair_rt;                        // Int pair reference type.
typedef struct IntPair* int_pair_pt;                        // Int pair pointer type.
typedef const struct IntPair int_pair_ct;                   // Constant int pair type.
typedef const struct IntPair* int_pair_cpt;                 // Constant int pair pointer type.
typedef const struct IntPair& int_pair_crt;                 // Constant int pair reference type.
typedef vector < int_pair_t > int_pair_vt;                  // Int pair vector type.
typedef set < int_t > intset_t;                             // Int set type.
typedef set < int_t >* intset_pt;                           // Int set poitner type.
typedef set < int_t >& intset_rt;                           // Int set reference type.
typedef intset_t::iterator intset_iter_t;                   // Int set iterator.
typedef queue < int_t > int_que_t;                          // Int queue type.

// Typedef of unsigned variable.
typedef unsigned  unsigned_t;                               // Unsigned type.
typedef unsigned* unsigned_pt;                              // Unsigned pointer type.
typedef unsigned& unsigned_rt;                              // Unsigned reference type.
typedef const unsigned  unsigned_ct;                        // Constant unsigned type.
typedef const unsigned* unsigned_cpt;                       // Constant unsigned pointer type.
typedef const unsigned& unsigned_crt;                       // Constant unsigned reference type.

// Typedef of floating variable.
typedef float  float_t;                                     // Numeric type.
typedef float* float_pt;                                    // Numeric pointer type.
typedef float& float_rt;                                    // Numeric reference type.
typedef const float  float_ct;                              // Constant numeric type.
typedef const float* float_cpt;                             // Constant numeric pointer type.
typedef const float& float_crt;                             // Constant numeric reference type.
typedef vector < float > float_vt;                          // Numeric 1D vector type.
typedef vector < float >* float_vpt;                        // Numeric 1D vector pointer type.
typedef vector < float >& float_vrt;                        // Numeric 1D vector reference type.
typedef vector < vector<float> > float_mt;                  // Numeric 2D matrix type.
typedef vector < vector<float> >* float_mpt;                // Numeric 2D matrix type.
typedef vector < vector<float> >& float_mrt;                // Numeric 2D matrix reference type.

// Typedef of char variable.
typedef char              char_t;                           // Char type.
typedef char*             char_pt;                          // Char pointer type.
typedef char&             char_rt;                          // Char reference type.
typedef char*&            char_rpt;                         // Char reference pointer type.
typedef const char        char_ct;                          // Constant char type.
typedef const char*       char_cpt;                         // Constant char pointer type.
typedef const char&       char_crt;                         // Constant char reference type.
typedef vector < char > char_1D_vt;                         // Char 1D vector type.
typedef vector < char >& char_1D_vrt;                       // Char 1D reference vector type.

// Typedef of void variable.
typedef void              void_t;                           // Void type.
typedef void*             void_pt;                          // Void pointer type.
typedef const void        void_ct;                          // Constant har void.
typedef const void*       void_cpt;                         // Constant void pointer type.

// Typedef of boolean variable.
typedef bool          bool_t;                               // Bool type.
typedef bool*         bool_pt;                              // Bool pointer type.
typedef bool&         bool_rt;                              // Bool reference type.
typedef const bool    bool_ct;                              // Constant bool type.
typedef const bool*   bool_cpt;                             // Constant bool pointer type.
typedef const bool&   bool_crt;                             // Constant bool reference type.
typedef vector < bool > bool_vt;                            // Bool vector type.
typedef vector < bool >* bool_vpt;                          // Bool vector type.
typedef vector < bool >& bool_vrt;                          // Bool vector type.
typedef vector < vector<bool> > bool_mt;                    // Bool matrix type.
typedef vector < vector<bool> >* bool_mpt;                  // Bool matrix type.
typedef vector < vector<bool> >& bool_mrt;                  // Bool matrix type.

// Typedef of the rc-tree variable.
typedef RCTree rctree_t;                                    // RCTree type.
typedef RCTree* rctree_pt;                                  // RCTree point type.
typedef RCTree& rctree_rt;                                  // RCTree reference type.
typedef const RCTree rctree_ct;                             // Constant RCTree type.
typedef const RCTree* rctree_cpt;                           // Constant point RCTree type.
typedef const RCTree& rctree_crt;                           // Constant reference RCTree type.

// Typedef of the rc-tree node variable.
typedef RCTreeNode rctree_node_t;                           // RCTreeNode type.
typedef RCTreeNode* rctree_node_pt;                         // RCTreeNode point type.
typedef RCTreeNode& rctree_node_rt;                         // RCTreeNode reference type.
typedef const RCTreeNode rctree_node_ct;                    // Constant RCTreeNode type.
typedef const RCTreeNode* rctree_node_cpt;                  // Constant point RCTreeNode type.
typedef const RCTreeNode& rctree_node_crt;                  // Constant reference RCTreeNode type.

// Typedef of the rc-tree edge variable.
typedef RCTreeEdge rctree_edge_t;                           // RCTreeEdge type.
typedef RCTreeEdge* rctree_edge_pt;                         // RCTreeEdge point type.
typedef RCTreeEdge& rctree_edge_rt;                         // RCTreeEdge reference type.
typedef const RCTreeEdge rctree_edge_ct;                    // Constant RCTreeEdge type.
typedef const RCTreeEdge* rctree_edge_cpt;                  // Constant point RCTreeEdge type.
typedef const RCTreeEdge& rctree_edge_crt;                  // Constant reference RCTreeEdge type.

// Typedef of string variable.
typedef string  string_t;                                   // String type.
typedef string* string_pt;                                  // String pointer type.
typedef string& string_rt;                                  // String reference type.
typedef const string    string_ct;                          // Constant string type.
typedef const string*   string_cpt;                         // Constant string pointer type.
typedef const string&   string_crt;                         // Constant string reference type.
typedef vector < string > string_1D_vt;                     // String vector type.
typedef vector < string >* string_1D_vpt;                   // String vector pointer type.
typedef vector < string >& string_1D_vrt;                   // String vector reference type.
typedef string_1D_vt::iterator string_1D_vec_iter_t;        // String vector iterator type.

// Typedef of cell variable.
typedef Cell        cell_t;                                 // Cell type.
typedef Cell*       cell_pt;                                // Cell pointer type.
typedef Cell&       cell_rt;                                // Cell reference type.
typedef const Cell  cell_ct;                                // Constant cell type.
typedef const Cell* cell_cpt;                               // Constant cell pointer type.
typedef const Cell& cell_crt;                               // Constant cell reference type.

// Typedef of cellpin variable.
typedef Cellpin  cellpin_t;                                 // Cellpin type.
typedef Cellpin* cellpin_pt;                                // Cellpin pointer type.
typedef Cellpin& cellpin_rt;                                // Cellpin reference type.
typedef const Cellpin  cellpin_ct;                          // Constant cellpin type.
typedef const Cellpin* cellpin_cpt;                         // Constant cellpin pointer type.
typedef const Cellpin& cellpin_crt;                         // Constant cellpin reference type.

// Typedef of celllib variable.
typedef CellLib        celllib_t;                           // Celllib type.
typedef CellLib*       celllib_pt;                          // Celllib pointer type.
typedef CellLib&       celllib_rt;                          // Celllib reference type.
typedef const CellLib  celllib_ct;                          // Constant celllib type.
typedef const CellLib* celllib_cpt;                         // Constant celllib pointer type.
typedef const CellLib& celllib_crt;                         // Constant celllib reference type.

// Typedef of instance variable.
typedef Instance        instance_t;                         // Instance type.
typedef Instance*       instance_pt;                        // Instance pointer type.
typedef Instance&       instance_rt;                        // Instance reference type.
typedef const Instance  instance_ct;                        // Constant instance type.
typedef const Instance* instance_cpt;                       // Constant instance pointer type.
typedef const Instance& instance_crt;                       // Constant instance reference type.

// Typedef of primary input port variable.
typedef PrimaryInput        primary_input_t;                // PrimaryInput type.
typedef PrimaryInput*       primary_input_pt;               // PrimaryInput pointer type.
typedef PrimaryInput&       primary_input_rt;               // PrimaryInput reference type.
typedef const PrimaryInput  primary_input_ct;               // Constant primary input port type.
typedef const PrimaryInput* primary_input_cpt;              // Constant primary input port pointer type.
typedef const PrimaryInput& primary_input_crt;              // Constant instance reference type.

// Typedef of primary output port variable.
typedef PrimaryOutput        primary_output_t;              // PrimaryOutput type.
typedef PrimaryOutput*       primary_output_pt;             // PrimaryOutput pointer type.
typedef PrimaryOutput&       primary_output_rt;             // PrimaryOutput reference type.
typedef const PrimaryOutput  primary_output_ct;             // Constant primary output port type.
typedef const PrimaryOutput* primary_output_cpt;            // Constant primary output port pointer type.
typedef const PrimaryOutput& primary_output_crt;            // Constant instance reference type.

// Typedef of net variable.
typedef Net        net_t;                                   // Net type.
typedef Net*       net_pt;                                  // Net pointer type.
typedef Net&       net_rt;                                  // Net reference type.
typedef const Net  net_ct;                                  // Constant net type.
typedef const Net* net_cpt;                                 // Constant net pointer type.
typedef const Net& net_crt;                                 // Constant net reference type.

// Typedef of pin variable.
typedef Pin           pin_t;                                // Pin type.
typedef Pin*          pin_pt;                               // Pin pointer type.
typedef Pin&          pin_rt;                               // Pin reference type.
typedef const Pin     pin_ct;                               // Constant pin type.
typedef const Pin*    pin_cpt;                              // Constant pin pointer type.
typedef const Pin&    pin_crt;                              // Constant pin reference type.

// Typedef of node variable.
typedef Node           node_t;                              // Node type.
typedef Node*          node_pt;                             // Node pointer type.
typedef Node&          node_rt;                             // Node reference type.
typedef const Node     node_ct;                             // Constant node type.
typedef const Node*    node_cpt;                            // Constant node pointer type.
typedef const Node&    node_crt;                            // Constant node reference type.
typedef vector < Node* > node_ptr_vt;                       // Node pointer vector type.
typedef vector < Node* >& node_ptr_vrt;                     // Node pointer vector reference type.
typedef vector < Node* >* node_ptr_vpt;                     // Node pointer vector pointer type.
typedef vector < Node* >::iterator node_ptrs_iter_t;        // Node pointer vector iterator type.
typedef IndexedSet < Node > nodeset_t;                       // Nodeset type.
typedef IndexedSet < Node >& nodeset_rt;                     // Nodeset reference type.
typedef IndexedSet < Node >* nodeset_pt;                     // Nodeset pointer type.

// Typedef of explorer.
typedef struct Explorer explorer_t;                         // Explorer type.
typedef struct Explorer* explorer_pt;                       // Explorer type pointer type.
typedef struct Explorer& explorer_rt;                       // Explorer type reference type.
typedef const struct Explorer& explorer_crt;                // Constant explorer reference type.
typedef vector < explorer_t > explorer_vt;                  // Explorer vector type.
typedef vector < explorer_t >* explorer_vpt;                // Explorer vector pointer type.
typedef vector < explorer_t >& explorer_vrt;                // Explorer vector reference type.
typedef vector < vector < explorer_t > > explorer_mt;       // Explorer matrix type.
typedef vector < vector < explorer_t > >* explorer_mpt;     // Explorer matrix type.
typedef vector < vector < explorer_t > >& explorer_mrt;     // Explorer matrix type.
typedef deque < explorer_t > explorer_qt;                   // Explorer deque type.
typedef deque < explorer_t >* explorer_qpt;                 // Explorer deque pointer type.
typedef deque < explorer_t >& explorer_qrt;                 // Explorer deque reference type.

// Typedef of edge variable.
typedef Edge           edge_t;                              // Edge type.
typedef Edge*          edge_pt;                             // Edge pointer type.
typedef Edge&          edge_rt;                             // Edge reference type.
typedef const Edge     edge_ct;                             // Constant edge type.
typedef const Edge*    edge_cpt;                            // Constant edge pointer type.
typedef const Edge&    edge_crt;                            // Constant edge reference type.
typedef vector < Edge* > edge_ptr_vt;                       // Edge vector type.

// Typedef of jump variable.
typedef Jump jump_t;                                        // Jump type.
typedef Jump* jump_pt;                                      // Jump pointer type.
typedef Jump& jump_rt;                                      // Jump reference type.
typedef const Jump jump_ct;                                 // Constant jump type.
typedef const Jump* jump_cpt;                               // Constant jump pointer type.
typedef const Jump& jump_crt;                               // Constant jump reference type.
typedef vector < Jump* > jump_ptr_vt;                       // Jump vector type.
typedef IndexedSet < Jump > jumpset_t;                       // Jumpset type.
typedef IndexedSet < Jump >& jumpset_rt;                     // Jumpset reference type.
typedef IndexedSet < Jump >* jumpset_pt;                     // Jumpset pointer type.

// Typedef of circuit variable.
typedef Circuit           circuit_t;                        // Circuit type.
typedef Circuit*          circuit_pt;                       // Circuit pointer type.
typedef Circuit&          circuit_rt;                       // Circuit reference type.
typedef const Circuit     circuit_ct;                       // Constant circuit type.
typedef const Circuit*    circuit_cpt;                      // Constant circuit pointer type.
typedef const Circuit&    circuit_crt;                      // Constant circuit reference type.

// Typedef of the clock variable.
typedef ClockTree clock_tree_t;                             // Clock type.
typedef ClockTree* clock_tree_pt;                           // Clock point type.
typedef ClockTree& clock_tree_rt;                           // Clock reference type.
typedef const ClockTree clock_tree_ct;                      // Constant Clock type.
typedef const ClockTree* clock_tree_cpt;                    // Constant point Clock type.
typedef const ClockTree& clock_tree_crt;                    // Constant reference Clock type.

// Typedef of timing arc variable.
typedef TimingArc        timing_arc_t;                      // Timing-arc type.
typedef TimingArc*       timing_arc_pt;                     // Timing-arc pointer type.
typedef TimingArc&       timing_arc_rt;                     // Timing-arc reference type.
typedef const TimingArc  timing_arc_ct;                     // Constant timing-arc type.
typedef const TimingArc* timing_arc_cpt;                    // Constant timing-arc pointer type.
typedef const TimingArc& timing_arc_crt;                    // Constant timing-arc reference type.
typedef vector < TimingArc* > timing_arcs_vt;               // Timing-arc pointer vector type.
typedef vector < TimingArc* >& timing_arcs_vrt;             // Timing-arc pointer vector reference type.
typedef vector < TimingArc* >* timing_arcs_vpt;             // Timing-arc pointer vector pointer type.
typedef timing_arcs_vt::iterator timing_arcs_iter_t;        // Iterator.

// Typedef of timing variable.
typedef Timing        timing_t;                             // Timing-arc type.
typedef Timing*       timing_pt;                            // Timing-arc pointer type.
typedef Timing&       timing_rt;                            // Timing-arc reference type.
typedef const Timing  timing_ct;                            // Constant timing-arc type.
typedef const Timing* timing_cpt;                           // Constant timing-arc pointer type.
typedef const Timing& timing_crt;                           // Constant timing-arc reference type.
typedef vector < Timing* > timing_ptr_vt;                   // Timing-arc pointer vector type.
typedef vector < Timing* >& timing_ptr_vrt;                 // Timing-arc pointer vector reference type.
typedef vector < Timing* >* timing_ptr_vpt;                 // Timing-arc pointer vector pointer type.
typedef timing_ptr_vt::iterator timing_ptrs_iter_t;         // Iterator.
typedef IndexedSet < Timing > timingset_t;                   // Timingset type.
typedef IndexedSet < Timing >& timingset_rt;                 // Timingset reference type.
typedef IndexedSet < Timing >* timingset_pt;                 // Timingset pointer type.
typedef IndexedSet < Timing >::iter_t timingset_iter_t;      // Timingset iter.

// Typedef of the test variable.
typedef Test test_t;                                        // Test type.
typedef Test* test_pt;                                      // Test point type.
typedef Test& test_rt;                                      // Test reference type.
typedef const Test test_ct;                                 // Constant onstraint type.
typedef const Test* test_cpt;                               // Constant test pointer type.
typedef const Test& test_crt;                               // Constant test reference type.
typedef vector < Test* > test_ptr_vt;                       // Test vector type.
typedef vector < Test* >* test_ptr_vpt;                     // Test vector pointer type.
typedef vector < Test* >& test_ptr_vrt;                     // Test vector reference type.
typedef IndexedSet < Test > testset_t;                      // Testset type.
typedef IndexedSet < Test >& testset_rt;                    // Testset reference type.
typedef IndexedSet < Test >* testset_pt;                    // Testset pointer type.
typedef IndexedSet < Test >::iter_t testset_iter_t;         // Teseset iter.
typedef List < Test* > testlist_t;                          // Test ptr list type.
typedef List < Test* >* testlist_pt;                        // Test ptr list type.
typedef ListNode < Test* >* testlist_iter_t;                // Test ptr list edge pointer type.

// Typedef of the testpoint variable.
typedef Testpoint testpoint_t;                              // Testpoint type.
typedef Testpoint* testpoint_pt;                            // Testpoint point type.
typedef Testpoint& testpoint_rt;                            // Testpoint reference type.
typedef const Testpoint testpoint_ct;                       // Constant onstraint type.
typedef const Testpoint* testpoint_cpt;                     // Constant test pointer type.
typedef const Testpoint& testpoint_crt;                     // Constant test reference type.
typedef vector < Testpoint > testpoint_vt;                  // Testpoint vector type.
typedef vector < Testpoint >* testpoint_vpt;                // Testpoint vector pointer type.
typedef vector < Testpoint >& testpoint_vrt;                // Testpoint vector reference type.
typedef const vector < Testpoint >& testpoint_cvrt;         // Testpoint vector reference type.

// Typedef of parasitics variable.
typedef Spef spef_t;                                        // Spef type.
typedef Spef* spef_pt;                                      // Spef pointer type.
typedef Spef& spef_rt;                                      // Spef reference type.
typedef const Spef spef_ct;                                 // Constant spef type.
typedef const Spef* spef_cpt;                               // Constant spef pointer type.
typedef const Spef& spef_crt;                               // Constant spef reference type.

// Typedef of parasitics net variable.
typedef SpefNet spefnet_t;                                  // SpefNet type.
typedef SpefNet* spefnet_pt;                                // SpefNet pointer type.
typedef SpefNet& spefnet_rt;                                // SpefNet reference type.
typedef const SpefNet spefnet_ct;                           // Constant spef net type.
typedef const SpefNet* spefnet_cpt;                         // Constant spef net pointer type.
typedef const SpefNet& spefnet_crt;                         // Constant spef net reference type.
typedef vector < SpefNet* > spefnet_ptr_vt;                 // Spef pin pointer vector type.
typedef vector < SpefNet* >& spefnet_ptr_vrt;               // Spef pin pointer vector reference type.
typedef vector < SpefNet* >* spefnet_ptr_vpt;               // Spef pin pointer vector pointer type.
typedef spefnet_ptr_vt::iterator spefnet_ptr_vec_iter_t;    // Iterator.

// Typedef of parasitics pin variable.
typedef SpefPin spefpin_t;                                  // SpefPin type.
typedef SpefPin* spefpin_pt;                                // SpefPin pointer type.
typedef SpefPin& spefpin_rt;                                // SpefPin reference type.
typedef const SpefPin spefpin_ct;                           // Constant spef pin type.
typedef const SpefPin* spefpin_cpt;                         // Constant spef pin pointer type.
typedef const SpefPin& spefpin_crt;                         // Constant spef pin reference type.
typedef vector < SpefPin* > spefpin_ptr_vt;                 // Spef pin pointer vector type.
typedef vector < SpefPin* >* spefpin_ptr_vpt;               // Spef pin pointer vector pointer type.
typedef spefpin_ptr_vt::iterator spefpin_ptr_vec_iter_t;    // Iterator.

// Typedef of parasitics cap variable.
typedef SpefCap spefcap_t;                                  // SpefCap type.
typedef SpefCap* spefcap_pt;                                // SpefCap pointer type.
typedef SpefCap& spefcap_rt;                                // SpefCap reference type.
typedef const SpefCap spefcap_ct;                           // Constant spef cap type.
typedef const SpefCap* spefcap_cpt;                         // Constant spef cap pointer type.
typedef const SpefCap& spefcap_crt;                         // Constant spef cap reference type.
typedef vector < SpefCap* > spefcap_ptr_vt;                 // Spef cap pointer vector type.
typedef vector < SpefCap* >* spefcap_ptr_vpt;               // Spef cap pointer vector pointer type.
typedef spefcap_ptr_vt::iterator spefcap_ptr_vec_iter_t;    // Iterator.

// Typedef of parasitics res variable.
typedef SpefRes spefres_t;                                  // SpefRes type.
typedef SpefRes* spefres_pt;                                // SpefRes pointer type.
typedef SpefRes& spefres_rt;                                // SpefRes reference type.
typedef const SpefRes spefres_ct;                           // Constant spef res type.
typedef const SpefRes* spefres_cpt;                         // Constant spef res pointer type.
typedef const SpefRes& spefres_crt;                         // Constant spef res reference type.
typedef vector < SpefRes* > spefres_ptr_vt;                 // Spef res pointer vector type.
typedef vector < SpefRes* >* spefres_ptr_vpt;               // Spef res pointer vector pointer type.
typedef spefres_ptr_vt::iterator spefres_ptr_vec_iter_t;    // Iterator type.

// Typedef of path finder variable.
typedef CPPR  cppr_t;                                       // CPPR type.
typedef CPPR* cppr_pt;                                      // CPPR pointer type.
typedef CPPR& cppr_rt;                                      // CPPR reference type.
typedef const CPPR  cppr_ct;                                // Constant path finder type.
typedef const CPPR* cppr_cpt;                               // Constant path finder pointer type.
typedef const CPPR& cppr_crt;                               // Constant path finder reference type.

// Typedef of path prefix variable.
typedef PathPrefix path_prefix_t;                           // PathPrefix type.
typedef PathPrefix* path_prefix_pt;                         // PathPrefix pointer type.
typedef PathPrefix& path_prefix_rt;                         // PathPrefix reference type.
typedef vector <PathPrefix*> path_prefix_ptr_vt;            // PathPrefix vector type.
typedef vector <PathPrefix*>* path_prefix_ptr_vpt;          // PathPrefix vector pointer type.
typedef vector <PathPrefix*>& path_prefix_ptr_vrt;          // PathPrefix vector reference type.

// Typedef of path trace.
typedef PathTrace path_trace_t;                             // Path trace type.
typedef PathTrace* path_trace_pt;                           // Path trace pointer type.
typedef PathTrace& path_trace_rt;                           // Path trace reference type.
typedef const PathTrace& path_trace_crt;                    // Path trace constant reference type.
typedef vector <PathTrace> path_trace_vt;                   // Path trace vector type.
typedef vector <PathTrace>* path_trace_vpt;                 // Path trace vector pointer type.
typedef vector <PathTrace>& path_trace_vrt;                 // Path trace vector reference type.
typedef vector <PathTrace*> path_trace_ptr_vt;              // Path trace pointer vector type.
typedef vector <PathTrace*>& path_trace_ptr_vrt;            // Path trace pointer vector reference type.
typedef explorer_vt::iterator path_trace_iter_t;            // Path trace iterator type.
typedef explorer_vt::reverse_iterator path_trace_riter_t;   // Path trace reversed iterator type.
typedef vector < path_trace_vt > path_trace_mt;             // Path trace matrix type.
typedef vector < path_trace_vt >* path_trace_mpt;           // Path trace matrix pointer type.
typedef vector < path_trace_vt >& path_trace_mrt;           // Path trace matrix reference type.
typedef const vector < path_trace_vt > path_trace_cmt;      // Constant path trace matrix type.
typedef const vector < path_trace_vt >* path_trace_cmpt;    // Constant path trace matrix pointer type.
typedef const vector < path_trace_vt >& path_trace_cmrt;    // Constant path trace matrix reference type.

// Typedef of path heap variable.
typedef PrefixHeap prefix_heap_t;                                     // Path heap type.
typedef PrefixHeap* prefix_heap_pt;                                   // Path heap pointer type.
typedef PrefixHeap& prefix_heap_rt;                                   // Path heap reference type.
typedef PathTraceMinMaxHeap path_trace_min_max_heap_t;                // Path trace heap type.
typedef vector<PathTraceMinMaxHeap> path_trace_min_max_heap_vt;       // Path trace heap vector type.
typedef PathTraceMinMaxHeap* path_trace_min_max_heap_pt;              // Path trace heap pointer type.
typedef PathTraceMinMaxHeap& path_trace_min_max_heap_rt;              // Path trace heap reference type.

// Typedef of a timer variable.
typedef Timer        timer_t;                               // timer type.
typedef Timer*       timer_pt;                              // timer pointer type.
typedef Timer&       timer_rt;                              // timer reference type.
typedef const Timer  timer_ct;                              // Constant timer type.
typedef const Timer* timer_cpt;                             // Constant timer pointer type.
typedef const Timer& timer_crt;                             // Constant timer reference type.

// Typedef of a command variable.
typedef Command command_t;                                  // Command type.
typedef Command* command_pt;                                // Command pointer type.

// Typedef of verilog.
typedef Verilog verilog_t;                                  // Verilog type.
typedef Verilog* verilog_pt;                                // Verilog pointer type.
typedef Verilog& verilog_rt;                                // Verilog reference type.
typedef const Verilog verilog_ct;                           // Constant verilog type.
typedef const Verilog* verilog_cpt;                         // Constant verilog pointer type.

// Typedef of pipeline
typedef Pipeline pipeline_t;                                // Pipeline type.
typedef Pipeline* pipeline_pt;                              // Pipeline pointer type.
typedef Pipeline& pipeline_rt;                              // Pipeline reference type.
typedef const Pipeline pipeline_ct;                         // Constant pipeline type.
typedef const Pipeline& pipeline_crt;                       // Constant pipeline reference type.
typedef const Pipeline* pipeline_cpt;                       // Constant pipeline pointer type.

// Typedef of verilog module.
typedef VerilogModule verilog_module_t;                     // Verilog module type.
typedef VerilogModule* verilog_module_pt;                   // Verilog module pointer type.
typedef VerilogModule& verilog_module_rt;                   // Verilog module reference type.
typedef const VerilogModule verilog_module_ct;              // Constant verilog module type.
typedef const VerilogModule* verilog_module_cpt;            // Constant verilog module pointer type.

// Typedef of verilog wire.
typedef VerilogWire verilog_wire_t;                         // Verilog wire type.
typedef VerilogWire* verilog_wire_pt;                       // Verilog wire pointer type.
typedef VerilogWire& verilog_wire_rt;                       // Verilog wire reference type.
typedef const VerilogWire verilog_wire_ct;                  // Constant verilog wire type.
typedef const VerilogWire* verilog_wire_cpt;                // Constant verilog wire pointer type.
typedef vector < VerilogWire* > verilog_wire_ptr_vt;        // Verilog wire pointer vector type.
typedef vector < VerilogWire* >* verilog_wire_ptr_vpt;      // Verilog wire pointer vector pointer type.
typedef verilog_wire_ptr_vt::iterator verilog_wire_ptr_vec_iter_t;

// Typedef of verilog input.
typedef VerilogInput verilog_input_t;                       // Verilog input type.
typedef VerilogInput* verilog_input_pt;                     // Verilog input pointer type.
typedef VerilogInput& verilog_input_rt;                     // Verilog input reference type.
typedef const VerilogInput verilog_input_ct;                // Constant verilog input type.
typedef const VerilogInput* verilog_input_cpt;              // Constant verilog input pointer type.
typedef vector < VerilogInput* > verilog_input_ptr_vt;      // Verilog input pointer vector type.
typedef vector < VerilogInput* >* verilog_input_ptr_vpt;    // Verilog input pointer vector pointer type.
typedef verilog_input_ptr_vt::iterator verilog_input_ptr_vec_iter_t;

// Typedef of verilog output.
typedef VerilogOutput verilog_output_t;                     // Verilog output type.
typedef VerilogOutput* verilog_output_pt;                   // Verilog output pointer type.
typedef VerilogOutput& verilog_output_rt;                   // Verilog output reference type.
typedef const VerilogOutput verilog_output_ct;              // Constant verilog output type.
typedef const VerilogOutput* verilog_output_cpt;            // Constant verilog output pointer type.
typedef vector < VerilogOutput* > verilog_output_ptr_vt;    // Verilog wire pointer vector type.
typedef vector < VerilogOutput* >* verilog_output_ptr_vpt;  // Verilog wire pointer vector pointer type.
typedef verilog_output_ptr_vt::iterator verilog_output_ptr_vec_iter_t;

// Typedef of verilog instance.
typedef VerilogInstance verilog_instance_t;                 // Verilog instance type.
typedef VerilogInstance* verilog_instance_pt;               // Verilog instance pointer type.
typedef VerilogInstance& verilog_instance_rt;               // Verilog instance reference type.
typedef const VerilogInstance verilog_instance_ct;          // Constant verilog instance type.
typedef const VerilogInstance* verilog_instance_cpt;        // Constant verilog instance pointer type.
typedef vector < VerilogInstance* > verilog_instance_ptr_vt;    
typedef vector < VerilogInstance* >* verilog_instance_ptr_vpt;
typedef verilog_instance_ptr_vt::iterator verilog_instance_ptr_vec_iter_t;

// Typedef of timing look-up table.
typedef LUTTemplate lut_template_t;                         // Timing lut template type.
typedef LUTTemplate* lut_template_pt;                       // Timing lut template pointer type.
typedef LUTTemplate& lut_template_rt;                       // Timing lut template reference type.
typedef TimingLUT timing_lut_t;                             // Timing lut type.
typedef TimingLUT* timing_lut_pt;                           // Timing lut pointer type.
typedef TimingLUT& timing_lut_rt;                           // Timing lut reference type.

// Typedef of list type.
typedef List < Pin* > pinlist_t;                            // Pin ptr list type.
typedef List < Pin* >* pinlist_pt;                          // Pin ptr list type.
typedef ListNode < Pin* >* pinlist_iter_t;                  // Pin ptr list node pointer type.
typedef List < Node* > nodelist_t;                          // Node ptr list type.
typedef List < Node* >& nodelist_rt;                        // Node ptr list reference type.
typedef List < Node* >* nodelist_pt;                        // Node ptr list pointer type.
typedef vector < List <Node*>* > node_bucketlist_t;         // Node bucket list vector type.
typedef vector < List <Node*>* >& node_bucketlist_rt;       // Node bucket list vector reference type.
typedef vector < List <Node*>* >* node_bucketlist_pt;       // Node bucket list vector pointer type.
typedef ListNode < Node* >* nodelist_iter_t;                // Node ptr list node pointer type.
typedef List < RCTreeNode* > rctree_nodelist_t;             // RCTree node ptr list type.
typedef List < RCTreeNode* >* rctree_nodelist_pt;           // RCTree node ptr list type.
typedef ListNode < RCTreeNode* >* rctree_nodelist_iter_t;   // RCTree node ptr list node pointer type.
typedef List < Edge* > edgelist_t;                          // Edge ptr list type.
typedef List < Edge* >* edgelist_pt;                        // Edge ptr list type.
typedef ListNode < Edge* >* edgelist_iter_t;                // Edge ptr list edge pointer type.
typedef List < Jump* > jumplist_t;                          // Jump ptr list type.
typedef List < Jump* >* jumplist_pt;                        // jump ptr list type.
typedef ListNode < Jump* >* jumplist_iter_t;                // jump ptr list edge pointer type.
typedef List < RCTreeEdge* > rctree_edgelist_t;             // RC-tree edge ptr list type.
typedef List < RCTreeEdge* >* rctree_edgelist_pt;           // RC-tree edge ptr list type.
typedef ListNode < RCTreeEdge* >* rctree_edgelist_iter_t;   // RC-tree edge ptr list edge pointer type.

// Typedef of dictionary type.
typedef Dictionary < string_t, Pin > pin_dict_t;                            // Pin dictionary type.
typedef Dictionary < string_t, Pin >* pin_dict_pt;                          // Pin dictionary pointer type.
typedef Dictionary < string_t, Net > net_dict_t;                            // Net dictionary type.
typedef Dictionary < string_t, Net >* net_dict_pt;                          // Net dictionary pointer type.
typedef Dictionary < string_t, Cell > cell_dict_t;                          // Cell dictionary type.
typedef Dictionary < string_t, Cell >* cell_dict_pt;                        // Cell dictionary pointer type.
typedef Dictionary < string_t, Node > node_dict_t;                          // Node dictionary type.
typedef Dictionary < string_t, Node >* node_dict_pt;                        // Node dictionary pointer type.
typedef Dictionary < string_t, Edge > edge_dict_t;                          // Edge dictionary type.
typedef Dictionary < string_t, Edge >* edge_dict_pt;                        // Edge dictionary pointer type.
typedef Dictionary < string_t, Cellpin > cellpin_dict_t;                    // Cellpin dictionary type.
typedef Dictionary < string_t, Cellpin >* cellpin_dict_pt;                  // Cellpin dictionary pointer type.
typedef Dictionary < string_t, Instance > instance_dict_t;                  // Instance dictionary type.
typedef Dictionary < string_t, Instance >* instance_dict_pt;                // Instance dictionary pointer type.
typedef Dictionary < string_t, TimingArc > timing_arc_dict_t;               // Timing arc dictionary type.
typedef Dictionary < string_t, TimingArc >* timing_arc_dict_pt;             // Timing arc dictionary pointer type.
typedef Dictionary < string_t, TimingArc >& timing_arc_dict_rt;             // Timing arc dictionary reference type.
typedef Dictionary < string_t, LUTTemplate > lut_template_dict_t;           // LUT template dictionary type.
typedef Dictionary < string_t, LUTTemplate >* lut_template_dict_pt;         // LUT template dictionary pointer type.
typedef Dictionary < string_t, RCTreeNode > rctree_node_dict_t;             // LUT template dictionary type.
typedef Dictionary < string_t, RCTreeNode >* rctree_node_dict_pt;           // LUT template dictionary pointer type.
typedef Dictionary < string_t, PrimaryInput > primary_input_dict_t;         // Primary input port dictionary type.
typedef Dictionary < string_t, PrimaryInput >* primary_input_dict_pt;       // Primary input port dictionary pointer type.
typedef Dictionary < string_t, PrimaryOutput > primary_output_dict_t;       // Primary input port dictionary type.
typedef Dictionary < string_t, PrimaryOutput >* primary_output_dict_pt;     // Primary input port dictionary pointer type.
typedef Dictionary < string_t, VerilogModule > verilog_module_dict_t;       // Verilog module dictionary type.
typedef Dictionary < string_t, VerilogModule >* verilog_module_dict_pt;     // Verilog module dictionary pointer type.
typedef Dictionary < string_t, VerilogInstance > verilog_instance_dict_t;   // Verilog instance dictionary type.
typedef Dictionary < string_t, VerilogInstance >* verilog_instance_dict_pt; // Verilog instance dictionary pointer type.

// Typedef of dictionary iterator type.
typedef pin_dict_t::iter_t pin_dict_iter_t;                                 // Pin dictionary iterator type.
typedef net_dict_t::iter_t net_dict_iter_t;                                 // Net dictionary iterator type.
typedef cell_dict_t::iter_t cell_dict_iter_t;                               // Cell dictionary iterator type.
typedef node_dict_t::iter_t node_dict_iter_t;                               // Node dictionary iterator type.
typedef nodeset_t::iter_t nodeset_iter_t;                                   // Node set iterator.
typedef edge_dict_t::iter_t edge_dict_iter_t;                               // Edge dictionary iterator type.
typedef cellpin_dict_t::iter_t cellpin_dict_iter_t;                         // Cellpin dictionary iterator type.
typedef instance_dict_t::iter_t instance_dict_iter_t;                       // Instance dictionary iterator type.
typedef timing_arc_dict_t::iter_t timing_arc_dict_iter_t;                   // Timing arc dictionary iterator type.
typedef rctree_node_dict_t::iter_t rctree_node_dict_iter_t;                 // RC tree node dictionary iterator type.
typedef verilog_module_dict_t::iter_t verilog_module_dict_iter_t;           // Verilog module dictionary iterator type.
typedef verilog_instance_dict_t::iter_t verilog_instance_dict_iter_t;       // Verilog instance dictionary iterator type.
typedef primary_input_dict_t::iter_t primary_input_dict_iter_t;             // Primary input port dictionary iterator type.
typedef primary_output_dict_t::iter_t primary_output_dict_iter_t;           // Primary output port dictionary iterator type.

// Typedef of map type.
typedef map < string_t, string_t > string_to_string_map_t;                  // String to string map type.
typedef map < string_t, string_t >* string_to_string_map_pt;                // String to string map pointer type.
typedef map < string_t, int_t > string_to_int_map_t;                        // String to int map type.

// Typedef of map iterator type.
typedef string_to_string_map_t::iterator string_to_string_iter_t;
typedef string_to_int_map_t::iterator string_to_int_iter_t;

// Typedef of iterator
typedef CircuitInstanceIterator circuit_instance_iter_t;
typedef CircuitNetIterator circuit_net_iter_t;
typedef CircuitPinIterator circuit_pin_iter_t;
typedef CircuitPrimaryInputIterator circuit_primary_input_iter_t;
typedef CircuitPrimaryOutputIterator circuit_primary_output_iter_t;
typedef CellpinIterator cellpin_iter_t;
typedef CellTimingArcIterator cell_timing_arc_iter_t;
typedef LibraryCellIterator library_cell_iter_t;
typedef NetPinlistIterator net_pinlist_iter_t;
typedef VerilogModuleIterator verilog_module_iter_t;
typedef VerilogInstanceCellpinToNetMappingIterator verilog_instance_cellpin_to_net_mapping_iter_t;
typedef TimerNodeIterator timer_node_iter_t;
typedef TimerEdgeIterator timer_edge_iter_t;

// Typedef of io stream type.
typedef ifstream ifstream_t;                                // Input file stream type.
typedef ifstream& ifstream_rt;                              // Input file stream type.
typedef ofstream ofstream_t;                                // Output file stream type.
typedef ofstream& ofstream_rt;                              // Output file stream type.
typedef istream istream_t;                                  // Input stream type.
typedef istream& istream_rt;                                // Input stream reference type.
typedef FILE* file_pt;                                      // File pointer type.

// Typedef of enum type.
typedef CapacitanceUnitBase capacitance_unit_base_e;        // Capacitance unit base enum.
typedef CellType cell_type_e;                               // Cell type enum.
typedef CurrentUnitBase current_unit_base_e;                // Current unit base enum.
typedef LibraryDelayModel library_delay_model_e;            // Library delay model enum.
typedef LibraryTechnology library_technology_e;             // Library technology enum.
typedef LookupTableVariable lut_variable_e;                 // Lut variable enum.
typedef NodeTimingType node_timing_type_e;                  // Node timing enum.
typedef PinDirection pin_direction_e;                       // Pin direction enum.
typedef PowerUnitBase power_unit_base_e;                    // Power unit base enum.
typedef ResistanceUnitBase resistance_unit_base_e;          // Resistance unit base enum.
typedef TimingSense timing_sense_e;                         // Timing sense enum.
typedef TimingType timing_type_e;                           // Timing arc type enum.
typedef TimeUnitBase time_unit_base_e;                      // Timing unit base enum.
typedef VoltageUnitBase voltage_unit_base_e;                // Voltage unit base enum.
typedef EdgeType edge_type_e;                               // Edge type enum.
typedef LibraryType library_type_e;                         // Library type enum.
typedef PathType path_type_e;                               // Path type.

// Typedef of constant enum type.
typedef const CapacitanceUnitBase capacitance_unit_base_ce; // Const capacitance unit base enum.
typedef const CellType cell_type_ce;                        // Const cell type enum.
typedef const CurrentUnitBase current_unit_base_ce;         // Const current unit base enum.
typedef const LibraryDelayModel library_delay_model_ce;     // Const library delay model enum.
typedef const LibraryTechnology library_technology_ce;      // Const library technology enum.
typedef const LookupTableVariable lut_variable_ce;          // Const lut variable enum.  
typedef const NodeTimingType node_timing_type_ce;           // Const node timing type enum.
typedef const PinDirection pin_direction_ce;                // Const pin direction enum.
typedef const PowerUnitBase power_unit_base_ce;             // Const power unit base enum.
typedef const ResistanceUnitBase resistance_unit_base_ce;   // Const resistance unit base enum.
typedef const TimingSense timing_sense_ce;                  // Const timing sense enum.
typedef const TimingType timing_type_ce;                    // Const timing arc type enum.
typedef const TimeUnitBase time_unit_base_ce;               // Const timing unit base enum.
typedef const VoltageUnitBase voltage_unit_base_ce;         // Const voltage unit base enum.
typedef const EdgeType edge_type_ce;                        // Const edge type enum.
typedef const PathType path_type_ce;                        // Const path type enum.

// Typedef of unit measurement.
typedef Unit < float_t, time_unit_base_e        > time_unit_t;         // Time unit. 
typedef Unit < float_t, power_unit_base_e       > power_unit_t;        // Power unit. 
typedef Unit < float_t, resistance_unit_base_e  > resistance_unit_t;   // Time unit. 
typedef Unit < float_t, current_unit_base_e     > current_unit_t;      // Time unit. 
typedef Unit < float_t, capacitance_unit_base_e > capacitance_unit_t;  // Time unit. 
typedef Unit < float_t, voltage_unit_base_e     > voltage_unit_t;      // Time unit. 

};

#endif

