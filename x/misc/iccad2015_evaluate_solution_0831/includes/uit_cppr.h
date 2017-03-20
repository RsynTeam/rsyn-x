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

#ifndef UIT_CPPR_H_
#define UIT_CPPR_H_

#include "uit_pin.h"
#include "uit_node.h"
#include "uit_edge.h"
#include "uit_circuit.h"

namespace uit {

//-------------------------------------------------------------------------------------------------

// Class: CPPR
class CPPR {
  
  public:

    CPPR(int, int, test_pt, pin_pt, clock_tree_pt, bool_t);                   // Constructor.
    ~CPPR();                                                                  // Destructor.

    inline int D_el() const;                                                  // Early/Late of D.
    inline int D_rf() const;                                                  // Rise/Fall of D.
    inline int S_rf() const;                                                  // Query the super source rf.
    inline int CK_rf() const;                                                 // Query the clock transition.
    inline int_t S_idx() const;                                               // Query the super source idx.

    inline node_pt through_node_ptr() const;                                  // Query the through node pointer.
    inline node_pt D() const;                                                 // Query the D pin node.
    inline node_pt CK() const;                                                // Query the CK pin node.
    inline nodeset_pt nodeset_ptr() const;                                    // Query the node set pointer.
    inline nodeset_rt nodeset() const;                                        // Query the node set reference.

    inline test_pt test_ptr() const;                                          // Query the test pointer.
    inline clock_tree_pt clock_tree_ptr() const;                              // Query the circuit pointer.

    inline size_t num_paths() const;                                          // Query the number of ranked paths.
    
    inline void_t set_cutoff_slack(float_ct);                                 // Set the cutoff slack.

    void_t report_worst_path(path_trace_rt);                                  // Report the worst path.
    void_t report_worst_paths(size_t, path_trace_vrt);                        // Report the K worst paths.
    void_t recover_path(path_prefix_pt, path_trace_rt);                       // Recover the path.
    void_t update_worst_paths(size_t, path_trace_min_max_heap_rt);            // Update the worst paths.
    
    float_t cutoff_slack() const;                                             // Query the cutoff slack.
    float_t report_worst_slack(size_t K = 1);                                 // Query the K'th worst path slack.
    float_t report_path_slack(path_prefix_pt) const;                          // Query the path slack.

    inline path_prefix_pt path_prefix_ptr(size_t K) const;                    // Query the K'th path prefix.

  private:
    
    int _D_el;                                                                // Test timing split.
    int _D_rf;                                                                // Test timing trans.
    int _CK_rf;                                                               // Clock transition status.

    int_t _S_idx;                                                             // Super source idx.
    int_t _num_indices;                                                       // Number of indices;
    int_mpt _bucketlist;                                                      // Levellist.
    
    bool_t _is_prefix_tree_needed;
    bool_pt _body;                                                            // Fanout cone of the through pin.
    bool_pt _is_in_list;                                                      // Query if resides in bucketlist.
    
    test_pt _test_ptr;                                                        // Test pointer.
    clock_tree_pt _clock_tree_ptr;                                            // Circuit pointer.

    node_pt _D;                                                               // D pin.
    node_pt _CK;                                                              // CK pin.
    node_pt _through_node_ptr;                                                // Through node pointer.
    nodeset_pt _nodeset_ptr;                                                  // Pointer to the node set.

    float_t _cutoff_slack;                                                    // The cutoff value for slack.
    float_pt _dist;                                                           // Distance array type.

    int_pt _sffx;                                                             // Suffix array type.
    int_vt _path_src;                                                         // Data path sources.

    path_prefix_ptr_vpt _prfx;                                                // Prefix path array.
    prefix_heap_pt _heap;                                                     // Heap for path ordering.

    int_t _clock_tree_parent(int_ct) const;                                   // Query the parent in clock tree.

    inline int _decode_rf(int_t) const;                                       // Query the transition of an idx.
    inline int_t _encode_idx(int, int_t) const;                               // Query the idx with rf encoded.
    inline int_t _decode_idx(int_t) const;                                    // Query the node idx.
    inline node_pt _decode_node_ptr(int_t) const;                             // Query the node pointer.
    
    bool_t _is_in_body(node_pt) const;                                        // Query if feasibly through.
    bool_t _is_in_body(int_t) const;                                          // Query if feasibly through.
    bool_t _is_prunable_from_suffix_tree(int_t) const;                        // Query if prunable.

    void_t _clear_paths();                                                    // Clear all paths.
    void_t _clear_thread_storage();                                           // Clear thread storage.
    void_t _relax(int_t, int_t, edge_pt, int_mrt);                            // Relaxation procedure.
    void_t _relax(int_t, int_t, jump_pt, int_mrt);                            // Relaxation procedure.
    void_t _relax_jumpin(int_t, int_mrt);                                     // Relaxation procedure.
    void_t _relax_fanin(int_t, int_mrt);                                      // Relaxation procedure.
    void_t _insert_path(path_prefix_pt);                                      // Insert a path.
    void_t _spur(path_prefix_pt);                                             // Spur the search space.
    void_t _spur(path_prefix_pt, int_t, int_t, edge_pt);                      // Spur the search space.
    void_t _update_body();                                                    // Update the body table.
    void_t _update_suffix_tree();                                             // Update the suffix tree.
    void_t _update_prefix_tree(size_t);                                       // Update the prefix tree. 
    void_t _update_prefix_tree(size_t, path_trace_min_max_heap_rt);           // Update the prefix tree. 
    void_t _update_worst_sources();                                           // Update the worst slack source.
    void_t _update_worst_source(int_t);                                       // Update the worst slack source.
    void_t _prefetch_cutoff_slack();                                          // Prefetch the cutoff slack.
    void_t _recover_worst_path(path_trace_rt);                                // Recover the first worst path.
    void_t _recover_jump_path(int_t, int_t, path_trace_rt);                   // Recover the jump path.
    void_t _recover_data_path(path_prefix_pt, int_t, path_trace_rt);          // Recover the data path.
    void_t _recover_clock_path(path_prefix_pt, int_t, path_trace_rt);         // Recover the clock path.

    float_t _cppr_offset(int_t) const;                                        // Query the cppr offset.
    float_t _cppr_offset(int, node_pt) const;                                 // Query the cppr offset.
    float_t _cppr_credit(int_t) const;                                        // Query the cppr credit.
    float_t _cppr_credit(int, int, int, node_pt, node_pt) const;              // Query the cppr credit.
    float_t _devi_cost(int_t, int_t, edge_pt) const;                          // Query the deviation cost. 
};

// Function: D_el
// Return the el status of the test.
inline int CPPR::D_el() const {
  return _D_el;
}

// Function: D_rf
// Return the rf status of the test.
inline int CPPR::D_rf() const {
  return _D_rf;
}

// Function: _decode_rf
// Return the transition of the given idx.
inline int CPPR::_decode_rf(int_t idx) const {
  return idx < _num_indices ? RISE : FALL;
}

// Function: _encode_idx
inline int_t CPPR::_encode_idx(int rf, int_t idx) const {
  return rf == RISE ? idx : idx + _num_indices;
}

// Function: _decode_idx
inline int_t CPPR::_decode_idx(int_t idx) const {
  return idx % _num_indices;
}

// Function: _decode_node_ptr
inline node_pt CPPR::_decode_node_ptr(int_t idx) const {
  return (*_nodeset_ptr)[_decode_idx(idx)];
}

// Function: CK_rf
// Query the clock transition status.
inline int_t CPPR::CK_rf() const {
  return _CK_rf;
}

// Function: test_ptr
// Return the pointer to the test.
inline test_pt CPPR::test_ptr() const {
  return _test_ptr;
}

// Procedure: set_cutoff_slack
// Set the cutoff slack.
inline void_t CPPR::set_cutoff_slack(float_ct val) {
  _cutoff_slack = val;
}

// Function: num_paths
// Return the number of ranked paths.
inline size_t CPPR::num_paths() const {
  return _prfx->size();
}

// Function: path_prefix_ptr
inline path_prefix_pt CPPR::path_prefix_ptr(size_t K) const {
  return (num_paths() && K) ? (*_prfx)[min(num_paths(), K)-1] : NULL;
}

// Function: nect
// Return the next node pointer.
inline node_pt CPPR::through_node_ptr() const {
  return _through_node_ptr;
}

// Function: D
// Return the D pin node.
inline node_pt CPPR::D() const {
  return _D;
}

// Function: CK
// Return the CK pin node.
inline node_pt CPPR::CK() const {
  return _CK;
}

// Function: S_idx
// Return the idx of the super source.
inline int_t CPPR::S_idx() const {
  return _S_idx;
}

// Function: nodeset_ptr
// Return the pointer to the node set.
inline nodeset_pt CPPR::nodeset_ptr() const {
  return _nodeset_ptr;
}

// Function: nodeset
// Return the reference to the node set.
inline nodeset_rt CPPR::nodeset() const {
  return *_nodeset_ptr;
}

// Function: clock_tree_ptr
// Return the pointer to the clock tree.
inline clock_tree_pt CPPR::clock_tree_ptr() const {
  return _clock_tree_ptr;
}

// Function: cutoff_slack
// Return the cutoff slack.
inline float_t CPPR::cutoff_slack() const {
  return _cutoff_slack;
}

//-------------------------------------------------------------------------------------------------

// Class: PathPrefix 
class PathPrefix {

  public:
    
    PathPrefix(int_t, int_t, edge_pt, PathPrefix*, float_t, float_t);                 // Constructor.
    ~PathPrefix();                                                                    // Destructor.

    inline int_t from_idx() const;                                                    // Query the from explorer.
    inline int_t to_idx() const;                                                      // Query the to explorer.

    inline float_t cppr_credit() const;                                               // Query the cppr credit.
    inline float_t devi_credit() const;                                               // Query the path devi_credit.

    inline edge_pt deviator() const;                                                  // Query the deviator.
    inline PathPrefix* parent() const;                                                // Query the parent.

    inline void_t set_from_idx(int_t);                                                // Set the explorer.
    inline void_t set_to_idx(int_t);                                                  // Set the explorer.
    inline void_t set_cppr_credit(float_ct);                                          // Set the credit.
    inline void_t set_devi_credit(float_ct);                                          // Set the post-cppr devi_credit.
    inline void_t set_parent(path_prefix_pt);                                         // Set parent.
    inline void_t set_deviator(edge_pt);                                              // Set the deviator.

  private:

    int_t _from_idx;                                                                  // The from explorer.
    int_t _to_idx;                                                                    // The to explorer.
    
    float_t _cppr_credit;                                                             // CPPR credit.
    float_t _devi_credit;                                                             // Post-CPPR devi_credit.

    edge_pt _deviator;                                                                // Deviator pointer.
    PathPrefix* _parent;                                                              // Parent.
};

// Function: from_explorer
inline int_t PathPrefix::from_idx() const {
  return _from_idx;
}

// Function: to_explorer
inline int_t PathPrefix::to_idx() const {
  return _to_idx;
}

// Function: cppr_credit
// Query the cppr credit.
inline float_t PathPrefix::cppr_credit() const {
  return _cppr_credit;
}

// Function: devi_credit
// Query the post cppr devi_credit.
inline float_t PathPrefix::devi_credit() const {
  return _devi_credit;
}

// Function: deviator
// Query the deviator.
inline edge_pt PathPrefix::deviator() const {
  return _deviator;
}

// Function: parent
// Query the parent of the path prefix.
inline path_prefix_pt PathPrefix::parent() const {
  return _parent;
}

// Procedure: set_from_idx
// Set the from explorer
inline void_t PathPrefix::set_from_idx(int_t idx) {
  _from_idx = idx;
}

// Procedure: set_to_idx
// Set the to explorer
inline void_t PathPrefix::set_to_idx(int_t idx) {
  _to_idx = idx;
}

// Procedure: set_cppr_credit
// Set the cppr credit.
inline void_t PathPrefix::set_cppr_credit(float_ct credit) {
  _cppr_credit = credit;
}

// Procedure: set_devi_credit
// Set the post cppr devi_credit.
inline void_t PathPrefix::set_devi_credit(float_ct devi_credit) {
  _devi_credit = devi_credit;
}

// Procedure: set_parent
// Set the parent.
inline void_t PathPrefix::set_parent(path_prefix_pt parent) {
  _parent = parent;
}

// Procedure: set_deviator
// Set the deviator.
inline void_t PathPrefix::set_deviator(edge_pt e) {
  _deviator = e;
}

//-------------------------------------------------------------------------------------------------

class PrefixHeap {

  public:

    PrefixHeap();                                             // Constructor.
    ~PrefixHeap();                                            // Destructor.

    inline size_t size() const;                               // Return the size of PQ.

    void insert(path_prefix_pt);                              // Insert an path prefix pointer.
    void pop();                                               // Extract the minimum key item.
    void clear(bool_t delete_path = false);                   // Clear the heap.

    path_prefix_pt top() const;                               // The minimum key item.

  private:
    
    size_t _N;                                                // Size of the heap.
    path_prefix_ptr_vt _heap;                                 // Heap.
};

// Function: size
// Query the size of the heap.
inline size_t PrefixHeap::size() const {
  return _N;
}

//-------------------------------------------------------------------------------------------------

// Class: PathTrace
class PathTrace {

  public:
    
    PathTrace();                                            // Constructor.
    ~PathTrace();                                           // Destructor.
    
    inline int el() const;                                  // Query the split.
    inline int rf() const;                                  // Query the transition.

    inline float_t slack() const;                           // Query the slack.

    inline path_type_e path_type() const;                   // Query the path type.

    inline size_t size() const;                             // Trace length.

    inline void_t push_back(explorer_crt);                  // Insert an explorer into the trace.
    inline void_t clear();                                  // Clear the trace.
    inline void_t set_slack(float_ct);                      // Set the slack.
    inline void_t set_path_type(path_type_ce);              // Set the path type.
    inline void_t set_el(int);                              // Set the data path split.
    inline void_t set_rf(int);                              // Set the data path trans.

    inline path_trace_iter_t begin();                       // Beginning position of the trace.
    inline path_trace_iter_t end();                         // Ending position of the trace.
    inline path_trace_riter_t rbegin();                     // Reversed beginning position.
    inline path_trace_riter_t rend();                       // Reversed ending posision.


  private:

    int _el;                                                // Early/Late split.
    int _rf;                                                // Rise/Fall split.

    float_t _slack;                                         // Path slack.

    path_type_e _path_type;                                 // Path type.

    explorer_vt _trace;                                     // Path trace.
};

// Function: el
// Query the timing split.
inline int PathTrace::el() const {
  return _el;
}

// Function: rf
// Query the timing trans.
inline int PathTrace::rf() const {
  return _rf;
}

// Function: size
// Return the size of the trace.
inline size_t PathTrace::size() const {
  return _trace.size();
}

// Function: path_type
// Return the path type.
inline path_type_e PathTrace::path_type() const {
  return _path_type;
}

// Procedure: set_el
// Set the timing split.
inline void_t PathTrace::set_el(int el) {
  _el = el;
}

// Procedure: set_rf
// Set the timing trans.
inline void_t PathTrace::set_rf(int rf) {
  _rf = rf;
}

// Procedure: set_slack
// Set the path slack.
inline void_t PathTrace::set_slack(float_ct slack) {
  _slack = slack;
}

// Procedure: set_path_type
// Set the path type.
inline void_t PathTrace::set_path_type(path_type_ce type) {
  _path_type = type;
}

// Procedure: clear
// Clear the trace.
inline void_t PathTrace::clear() {
  _trace.clear();
}

// Procedure: push_back
// Insert an idx into the trace.
inline void_t PathTrace::push_back(explorer_crt explorer) {
  _trace.push_back(explorer);
}

// Function: slack
// Query the slack value.
inline float_t PathTrace::slack() const {
  return _slack;
}

// Function: begin
// Return the begin position.
inline path_trace_iter_t PathTrace::begin() {
  return _trace.begin();
}

// Function: end
// Return the end position.
inline path_trace_iter_t PathTrace::end() {
  return _trace.end();
}

// Function: rbegin
// Return the reversed beging position.
inline path_trace_riter_t PathTrace::rbegin() {
  return _trace.rbegin();
}

// Function: rend
// Return the reversed ending position.
inline path_trace_riter_t PathTrace::rend() {
  return _trace.rend();
}

//-------------------------------------------------------------------------------------------------

// Class: PathTraceMinMaxHeap
class PathTraceMinMaxHeap {

  public:

    PathTraceMinMaxHeap();                                  // Constructor.
    ~PathTraceMinMaxHeap();                                 // Destructor.
    
    inline size_t size() const;                             // Return the size of PQ.
    
    void_t clear();                                         // Clear the heap.
    void_t insert(path_trace_pt);                           // Insert an item into the PQ.
    void_t pop_min();                                       // Extract the minimum element.
    void_t pop_max();                                       // Extract the maximum element.
    void_t maintain_k_min(size_t);                          // Maintain only the top k min items.
    void_t maintain_k_max(size_t);                          // Maintain only the top k max items.
    void_t append_path_trace_ptrs(path_trace_ptr_vrt);      // Append the path traces.

    inline path_trace_pt top_min() const;                   // Return the minimum element.
    inline path_trace_pt top_max() const;                   // Return the maximum element.

  private:

    size_t _N;                                              // Heap size.
    path_trace_ptr_vt _heap;                                // Trace pointers.
    
    void_t _trickle_down(int_ct);                           // Heapify down (all).
    void_t _trickle_down_min(int_t);                        // Heapify down (min).
    void_t _trickle_down_max(int_t);                        // Heapify down (max).
    void_t _bubble_up(int_ct);                              // Bubble up (all).
    void_t _bubble_up_min(int_t);                           // Bubble up (min).
    void_t _bubble_up_max(int_t);                           // Bubble up (min).
};

// Function: size
// Return the size of min-max heap.
inline size_t PathTraceMinMaxHeap::size() const {
  return _N;
}

// Function: top_min
inline path_trace_pt PathTraceMinMaxHeap::top_min() const {
  return (_N >= 1) ? _heap[1] : NULL;
}

// Function: top_max
inline path_trace_pt PathTraceMinMaxHeap::top_max() const {
  if(_N>=3) return (_heap[2]->slack() > _heap[3]->slack()) ? _heap[2] : _heap[3];
  return _heap[_N];
}


};

#endif


