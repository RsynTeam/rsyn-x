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

#ifndef UIT_NODE_H_
#define UIT_NODE_H_

#include "uit_utility.h"
#include "uit_datadef.h"
#include "uit_typedef.h"
#include "uit_classdef.h"
#include "uit_edge.h"
#include "uit_pin.h"

namespace uit {

// Class: Node
class Node {

  public:

    Node();                                                 // Constructor.
    ~Node();                                                // Destructor.

    void_t relax_rat(int, int, float_ct);                   // Update the rat.
    void_t relax_slew(int, int, float_ct);                  // Update the slew.
    void_t relax_at(int, int, int, edge_pt);                // Relax the AT.
    void_t insert_fanin(edge_pt);                           // Insert a fanin edge.
    void_t insert_fanout(edge_pt);                          // Insert a fanout edge.
    void_t insert_jumpin(jump_pt);                          // Insert a fanin jump.
    void_t insert_jumpout(jump_pt);                         // Insert a fanout jump.
    void_t remove_timing();                                 // Remove the timing on nodes.
    void_t remove_slew();                                   // Remove the slew on nodes.
    void_t remove_at();                                     // Remove the at on nodes.
    void_t remove_rat();                                    // Remove the rat on nodes.
    void_t remove_fanin(edge_pt);                           // Remove the fanin edge of a node.
    void_t remove_fanout(edge_pt);                          // Remove the fanout edge of a node.
    void_t remove_jumpin(jump_pt);                          // Remove the jump of a node.
    void_t remove_jumpout(jump_pt);                         // Remove the jump of a node.
    
    bool_t is_in_clock_tree() const;                        // Query if the node is in a clock tree.
    bool_t is_constrained() const;                          // Query if the node is constrained.
    bool_t is_clock_sink() const;                           // Query the clock sink node status.
    bool_t is_primary_input() const;                        // Query the io port status.
    bool_t is_primary_output() const;                       // Query the io port status.
    bool_t is_sequential() const;                           // Query the sequential status.
    bool_t is_rctree_root() const;                          // Query the rctree root status.
    bool_t is_data_path_source() const;                     // Query the data path source status.
    bool_t is_jump_head() const;                            // Query the jump status.
    bool_t is_jump_tail() const;                            // Query the jump status.

    size_t num_fanins() const;                              // Query the fanin size.
    size_t num_fanouts() const;                             // Query the fanout size.
    size_t num_jumpins() const;                             // Query the fanin jump size.
    size_t num_jumpouts() const;                            // Query the fanout jump size.

    inline float_t slew(int, int) const;                    // Query the slew.
    inline float_t at(int, int) const;                      // Query the arrival time.
    inline float_t rat(int, int) const;                     // Query the required arrival time.
    inline float_t slack(int, int) const;                   // Query the slack.
    inline float_t at_diff(int, int, int, int) const;       // Query the arrival time difference.
    
    inline int_t idx() const;                               // Query the node idx.
    inline int_t level() const;                             // Query the level.
    inline int_t clock_tree_node_idx() const;               // Query the clock tree idx.

    inline pin_pt pin_ptr() const;                          // Query the pin pointer.

    inline int at_parent_rf(int, int) const;                // Query the parent transition.
    inline node_pt at_parent_ptr(int, int) const;           // Query the parent pointer.
    inline nodeset_pt nodeset_ptr() const;                  // Query the nodeset pointer.

    inline edgelist_pt fanin_ptr() const;                   // Query the fanin pointer.
    inline edgelist_pt fanout_ptr() const;                  // Query the fanout pointer.
    inline jumplist_pt jumpin_ptr() const;                  // Query the jumpin pointer.
    inline jumplist_pt jumpout_ptr() const;                 // Query the jumpout pointer.

    inline nodelist_iter_t pipeline_satellite() const;      // Query the pipeline satellite.
    
    inline bool_t is_in_pipeline() const;                   // Query the satellite status.
    inline bool_t is_idx_set() const;                       // Query the index status.
    inline bool_t is_at_clocked(int, int) const;            // Query the at status.
    inline bool_t is_slew_defined(int, int) const;          // Query the slew status.
    inline bool_t is_at_defined(int, int) const;            // Query the at status.
    inline bool_t is_rat_defined(int, int) const;           // Query the rat status.
    inline bool_t is_slack_defined(int, int) const;         // Query the slack status.
    
    inline void_t set_idx(int_t);                           // Set the node idx.
    inline void_t set_level(int_t);                         // Set the level.
    inline void_t set_pin_ptr(pin_pt);                      // Set the pin pointer.
    inline void_t set_clock_tree_node_idx(int_t);           // Set the clock tree idx.
    inline void_t set_slew(int, int, float_ct);             // Set the slew.
    inline void_t set_at(int, int, float_ct);               // Set the arrival time.
    inline void_t set_rat(int, int, float_ct);              // Set the required arrival time.
    inline void_t set_nodelist_satellite(nodelist_iter_t);  // Set the nodelist satellite. 
    inline void_t set_pipeline_satellite(nodelist_iter_t);  // Set the pipeline satellite.
    inline void_t set_is_at_clocked(int, int, bool_ct);     // Set the at direction.
    inline void_t set_at_parent_rf(int, int, int);          // Set the at parent transition.
    inline void_t set_at_parent_ptr(int, int, node_pt);     // Set the at parent transition.
    inline void_t set_nodeset_ptr(nodeset_pt);              // Set the nodeset pointer.
    inline void_t remove_slew(int, int);                    // Unset the slew.
    inline void_t remove_at(int, int);                      // Unset the at.
    inline void_t remove_rat(int, int);                     // Unset the rat.
    inline void_t add_rat(int, int, float_ct);              // Add the rat.

  private:

    float_t _slew[2][2];                                    // Slew.
    float_t _at[2][2];                                      // Arrival time.
    float_t _rat[2][2];                                     // Required arrival time.

    bool_t _is_at_clocked[2][2];                            // Clock tree propagated.

    pin_pt _pin_ptr;                                        // Pin pointer.
    
    int_t _idx;                                             // Node idx.
    int_t _level;                                           // Level in the pipeline.
    int_t _clock_tree_node_idx;                             // Clock tree occurrence idx.

    edgelist_pt _fanin_ptr;                                 // Fanin edgelist.
    edgelist_pt _fanout_ptr;                                // Fanout edgelist.

    jumplist_pt _jumpin_ptr;                                // Fanin jumplist.
    jumplist_pt _jumpout_ptr;                               // Fanout jumplist.

    int _at_parent_rf[2][2];                                // Parent node transition.

    node_pt _at_parent_ptr[2][2];                           // Parent node pointer.

    nodeset_pt _nodeset_ptr;                                // Nodeset pointer.

    nodelist_iter_t _pipeline_satellite;                    // Pipeline satellite.
};

// Function: idx
// Query the node index.
inline int_t Node::idx() const {
  return _idx;
}

// Function: slew
// Return the slew
inline float_t Node::slew(int el, int rf) const {
  return _slew[el][rf];
}

// Function: at
// Return the at
inline float_t Node::at(int el, int rf) const {
  return _at[el][rf];
}

// Function: rat
// Return the rat
inline float_t Node::rat(int el, int rf) const {
  return _rat[el][rf];
}

// Function: is_slew_defined
inline bool_t Node::is_slew_defined(int el, int rf) const {
  return !(_slew[el][rf] >= UIT_FLT_MAX || _slew[el][rf] <= UIT_FLT_MIN);
}

// Function: is_at_defined
inline bool_t Node::is_at_defined(int el, int rf) const {
  return !(_at[el][rf] >= UIT_FLT_MAX || _at[el][rf] <= UIT_FLT_MIN);
}

// Function: is_rat_defined
inline bool_t Node::is_rat_defined(int el, int rf) const {
  return !(_rat[el][rf] >= UIT_FLT_MAX || _rat[el][rf] <= UIT_FLT_MIN);
}

// Function: slack
// Return the slack.
inline float_t Node::slack(int el, int rf) const {
  return (el == EARLY) ? at(el, rf) - rat(el, rf) : rat(el, rf) - at(el, rf);
}

// Function: at_diff
inline float_t Node::at_diff(int lhs_el, int lhs_rf, int rhs_el, int rhs_rf) const {
  return at(lhs_el, lhs_rf) - at(rhs_el, rhs_rf);
}

// Function: pin_ptr
// Return the pin pointer.
inline pin_pt Node::pin_ptr() const {
  return _pin_ptr;
}

// Function: nodeset_ptr
// Return the nodeset pointer.
inline nodeset_pt Node::nodeset_ptr() const {
  return _nodeset_ptr;
}

// Procedure: set_nodeset_ptr
// Set the nodeset pointer.
inline void_t Node::set_nodeset_ptr(nodeset_pt ptr) {
  _nodeset_ptr = ptr;
}

// Procedure: set_at_parent_rf
inline void_t Node::set_at_parent_rf(int el, int rf, int prf) {
  _at_parent_rf[el][rf] = prf;
}

// Procedure: set_at_parent_ptr
inline void_t Node::set_at_parent_ptr(int el, int rf, node_pt ptr) {
  _at_parent_ptr[el][rf] = ptr;
}

// Function: at_parent_rf
// return the node transition of the AT parent.
inline int Node::at_parent_rf(int el, int rf) const {
  return _at_parent_rf[el][rf];
}

// Function: at_parent_ptr
// Return the node pointer to the AT parent.
inline node_pt Node::at_parent_ptr(int el, int rf) const {
  return _at_parent_ptr[el][rf];
}

// Function: fanin_ptr
// Return the pointer to the fanin list.
inline edgelist_pt Node::fanin_ptr() const {
  return _fanin_ptr;
}

// Function: fanout_ptr
// Return the pointer to the fanin list.
inline edgelist_pt Node::fanout_ptr() const {
  return _fanout_ptr;
}

// Function: jumpin_ptr
// Return the pointer to the jumpin list.
inline jumplist_pt Node::jumpin_ptr() const {
  return _jumpin_ptr;
}

// Function: jumpout_ptr
// Return the pointer to the jumpout list.
inline jumplist_pt Node::jumpout_ptr() const {
  return _jumpout_ptr;
}

// Function: pipeline_satellite
// Return the satellite on the pipeline.
inline nodelist_iter_t Node::pipeline_satellite() const {
  return _pipeline_satellite;
}

// Procedure: set_pipeline_satellite
// Set the pipeline satellite
inline void_t Node::set_pipeline_satellite(nodelist_iter_t satellite) {
  _pipeline_satellite = satellite;
}

// Procedure: set_idx
// Set the node index.
inline void_t Node::set_idx(int_t idx) {
  _idx = idx;
}

// Procedure: set_slew
// Set the slew.
inline void_t Node::set_slew(int el, int rf, float_ct slew) {
  numeric_guard(_slew[el][rf] = slew);
}

// Procedure: set_at
// Set the arrival time.
inline void_t Node::set_at(int el, int rf, float_ct at) {
  numeric_guard(_at[el][rf] = at);
}

// Procedure: set_rat
// Set the required arrival time.
inline void_t Node::set_rat(int el, int rf, float_ct rat) {
  numeric_guard(_rat[el][rf] = rat);
}

// Procedure: add_rat
// Add the required arrival time.
inline void_t Node::add_rat(int el, int rf, float_ct delata) {
  set_rat(el, rf, rat(el, rf) + delata);
}

// Procedure: remove_slew
// Unset the slew.
inline void_t Node::remove_slew(int el, int rf) {
  _slew[el][rf] = (el == EARLY) ? UIT_FLT_MAX : UIT_FLT_MIN;
}

// Procedure: remove_at
// Unset the at.
inline void_t Node::remove_at(int el, int rf) {
  _at[el][rf] = (el == EARLY) ? UIT_FLT_MAX : UIT_FLT_MIN;
  _is_at_clocked[el][rf] = false;
  _at_parent_ptr[el][rf] = NULL;
  _at_parent_rf[el][rf] = UIT_UNDEFINED_TRANS;
}

// Procedure: remove_rat
// Unset the rat.
inline void_t Node::remove_rat(int el, int rf) {
  _rat[el][rf] = (el == EARLY) ? UIT_FLT_MIN : UIT_FLT_MAX;
}

// Procedure: set_pin_ptr
// Set the pin pointer.
inline void_t Node::set_pin_ptr(pin_pt pin_ptr) {
  _pin_ptr = pin_ptr;
}

// Procedure: set_clock_tree_node_idx
// Set the clock tree index.
inline void_t Node::set_clock_tree_node_idx(int_t idx) {
  _clock_tree_node_idx = idx;
}

// Procedure: set_level
// Set the level.
inline void_t Node::set_level(int_t level) {
  #ifndef NDEBUG
  assert(!is_in_pipeline());
  #endif
  _level = level;
}

// Procedure: set_is_at_clocked
inline void_t Node::set_is_at_clocked(int el, int rf, bool_ct flag) {
  _is_at_clocked[el][rf] = flag;
}

// Function: clock_tree_node_idx
inline int_t Node::clock_tree_node_idx() const {
  return _clock_tree_node_idx;
}

// Function: level
// Query the level.
inline int_t Node::level() const {
  return _level;
}

// Function: is_in_pipeline
// Query the satellite status
inline bool_t Node::is_in_pipeline() const {
  return _pipeline_satellite != NULL;
}

// Function: is_idx_set
// Query the index status.
inline bool_t Node::is_idx_set() const {
  return _idx != UIT_UNDEFINED_IDX;
}

// Function: is_at_clocked
inline bool_t Node::is_at_clocked(int el, int rf) const {
  return _is_at_clocked[el][rf];
}


};

#endif
