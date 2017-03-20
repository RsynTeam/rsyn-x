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

#ifndef UIT_CLOCK_H_
#define UIT_CLOCK_H_

#include "uit_macrodef.h"
#include "uit_typedef.h"
#include "uit_node.h"
#include "uit_io_port.h"

namespace uit {

// Class: ClockTree
class ClockTree {

  public:

    ClockTree();                                                    // Constructor.
    ~ClockTree();                                                   // Destructor.

    void_t update_clock_tree();                                     // Initialize the clock tree.
    
    pin_pt root_pin_ptr() const;                                    // Query the root pin pointer.

    node_pt root_node_ptr() const;                                  // Query the root node pointer.
    node_pt lca_node_ptr(node_pt, node_pt) const;                   // Query the LCA of two nodes.
    node_pt lca_node_ptr(int, int, node_pt, node_pt) const;         // Query the LCA of two nodes.

    inline void_t set_primary_input_ptr(primary_input_pt);          // Set the pi pointer.
    inline void_t set_period(float_ct);                             // Set the clock period.
    inline void_t set_is_clock_tree_updated(bool_ct);               // Set the update flag.
    inline void_t enable_clock_tree_update();                       // Enable the clock tree update.

    inline int_t level() const;                                     // Level of the clock tree.
    
    int_t level(pin_pt) const;                                      // Level of a clock tree pin.
    int_t level(node_pt) const;                                     // Level of a clock tree node.
    int_t num_negations(node_pt) const;                             // Negation count to a clock tree node.

    inline size_t size() const;                                     // Size of the clock tree.

    inline float_t period() const;                                  // Query the clock period.

    float_t cppr_credit(int, int, int, node_pt, node_pt) const;     // Query the cppr credit.

    inline primary_input_pt primary_input_ptr() const;              // Query the pi pointer.

    inline bool_t is_clock_tree_updated() const;                    // Query the update status.
    inline bool_t is_clock_tree_unupdated() const;                  // Query the update status.
    
  private:
    
    int_t _level;                                                   // Clock tree level.
    int_vt _L;                                                      // Level of the ith visited node.
    int_vt _N;                                                      // Negation count of the ith visited node.
    int_mt _ST;                                                     // Sparse table.

    bool_t _is_clock_tree_updated;                                  // Update flag.

    node_ptr_vt _E;                                                 // The ith visited node.

    size_t _size;                                                   // Clock tree size.

    float_t _period;                                                // Clock period.
    primary_input_pt _primary_input_ptr;                            // Primary input pointer (root).
    
    void_t _traverse(node_pt, node_pt, int_ct, int_ct);             // Traverse the clock tree.
    void_t _update_sparse_table();                                  // Update the sparse table.
    void_t _insert_ELHN(node_pt, int_ct, int_ct);                   // Insert new entries to E/L/H/N table.
};

// Function: primary_input_ptr
inline primary_input_pt ClockTree::primary_input_ptr() const {
  return _primary_input_ptr;
}

// Function: period
inline float_t ClockTree::period() const {
  return _period;
}

// Function: size
inline size_t ClockTree::size() const {
  return _size;
}

// Function: level
inline int_t ClockTree::level() const {
  return _level;
}

// Function: is_clock_tree_updated
inline bool_t ClockTree::is_clock_tree_updated() const {
  return _is_clock_tree_updated;
}

// Function: is_clock_tree_updated
inline bool_t ClockTree::is_clock_tree_unupdated() const {
  return !is_clock_tree_updated();
}

// Procedure: set_is_clock_tree_updated
inline void_t ClockTree::set_is_clock_tree_updated(bool_ct flag) {
  _is_clock_tree_updated = flag;
}

// Procedure: set_primary_input_ptr
inline void_t ClockTree::set_primary_input_ptr(primary_input_pt pi_ptr) {
  _primary_input_ptr = pi_ptr;
}

// Procedure: set_period
inline void_t ClockTree::set_period(float_ct period) {
  _period = period;
}

// Procedure: enable_clock_tree_update
inline void_t ClockTree::enable_clock_tree_update() {
  set_is_clock_tree_updated(false);
}

};

#endif









