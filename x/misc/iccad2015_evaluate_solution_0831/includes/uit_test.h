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

#ifndef UIT_TEST_H_
#define UIT_TEST_H_

#include "uit_utility.h"
#include "uit_datadef.h"
#include "uit_typedef.h"
#include "uit_classdef.h"
#include "uit_pin.h"
#include "uit_node.h"

namespace uit {

// Class: Test
// This class specifies the timing test (i.e., regular PO rat and sequential setup/hold rat).
// Hold test is the early test (i.e., latest rat and earliest at) and setup test
// is the late test (i.e., earliest rat and latest at).
class Test {
  
  public:
    
    Test();                                                             // Constructor.
    ~Test();                                                            // Destructor.
    
    void_t update_rat(clock_tree_pt);                                   // Update the RAT time.
    void_t update_rat(int, int, clock_tree_pt);                         // Update the RAT time.
    void_t update_slack();                                              // Update the slack.
    void_t update_slack(int, int);                                      // Update the slack.
    void_t update_cppr_dominator(clock_tree_pt, nodeset_pt);            // Update the cppr dominator.
    void_t update_cppr_dominator(int, clock_tree_pt, nodeset_pt);       // Update the cppr dominator.

    inline bool_t is_sequential_constraint() const;                     // Query the constraint status.

    inline void_t set_idx(int_t);                                       // Set the idx.
    inline void_t set_rat(int, int, float_ct);                          // Set the rat value.
    inline void_t set_timing_arc_ptr(int, timing_arc_pt);               // Set the timing arc.
    inline void_t set_related_pin_ptr(pin_pt);                          // Set the pin pointer.
    inline void_t set_constrained_pin_ptr(pin_pt);                      // Set the pin pointer.
    inline void_t set_slack(int, int, float_ct);                        // Set the slack.
    inline void_t add_rat(int, int, float_ct);                          // Add the rat value.

    inline pin_pt related_pin_ptr() const;                              // Query the related pin pointer.
    inline pin_pt constrained_pin_ptr() const;                          // Query the constrained pin pointer.

    inline int_t idx() const;                                           // Query the idx.

    inline float_t rat(int, int) const;                                 // Query the rat value.
    inline float_t slack(int, int) const;                               // Query the slack.

    inline timing_arc_pt timing_arc_ptr(int) const;                     // Query the timing arc pointer. 

    node_pt constrained_node_ptr() const;
    node_pt related_node_ptr() const;

  private:

    int_t _idx;                                                         // Test index.

    float_t _rat[2][2];                                                 // Required arrival time.
    float_t _slack[2][2];                                               // Worst slack of this test.

    pin_pt _constrained_pin_ptr;                                        // Constrained pin pointer.
    pin_pt _related_pin_ptr;                                            // Related pin pointer.

    timing_arc_pt _timing_arc_ptr[2];                                   // Test timing arc pointer.
};

// Procedure: set_constrained_pin_ptr
// Set the constrained pin pointer.
inline void_t Test::set_constrained_pin_ptr(pin_pt pin_ptr) {
  _constrained_pin_ptr = pin_ptr;
}

// Procedure: set_related_pin_ptr
// Set the related pin pointer.
inline void_t Test::set_related_pin_ptr(pin_pt pin_ptr) {
  _related_pin_ptr = pin_ptr;
}

// Procedure: set_timing_arc_ptr
// Set the test timing arc.
inline void_t Test::set_timing_arc_ptr(int el, timing_arc_pt timing_arc_ptr) {
  _timing_arc_ptr[el] = timing_arc_ptr;
}

// Procedure: set_idx
// Set the test satellite pointer.
inline void_t Test::set_idx(int_t idx) {
  _idx = idx;
}

// Procedure: set_slack
// Set the slack.
inline void_t Test::set_slack(int el, int rf, float_ct slack) {
  _slack[el][rf] = slack;
}

// Procedure: set_rat
// Set the rat.
inline void_t Test::set_rat(int el, int rf, float_ct rat) {
  _rat[el][rf] = rat;
}

// Procedure: add_rat
// Add the rat value.
inline void_t Test::add_rat(int el, int rf, float_ct rat) {
  _rat[el][rf] += rat;
}

// Function: is_sequential_constraint
// Query the constraint status.
inline bool_t Test::is_sequential_constraint() const {
  return related_pin_ptr() != NULL;
}

// Function: timing_arc_ptr
// Query the timing arc pointer.
inline timing_arc_pt Test::timing_arc_ptr(int el) const {
  return _timing_arc_ptr[el];
}

// Function: related_pin_ptr
// Query the related pin pointer.
inline pin_pt Test::related_pin_ptr() const {
  return _related_pin_ptr;
}

// Function: constrained_pin_ptr
// Query the constrained pin pointer.
inline pin_pt Test::constrained_pin_ptr() const {
  return _constrained_pin_ptr;
}

// Function: idx
// Query the idx
inline int_t Test::idx() const {
  return _idx;
}

// Function: rat
// Query the rat value.
inline float_t Test::rat(int el, int rf) const {
  return _rat[el][rf];
}

// Function: slack
// Query the slack value.
inline float_t Test::slack(int el, int rf) const {
  return _slack[el][rf];
}

//-------------------------------------------------------------------------------------------------

// Class: Testpoint
class Testpoint {

  public:

    Testpoint(int, int, test_pt, float_ct);                 // Constructor.
    ~Testpoint();                                           // Destructor.

    inline int el() const;                                  // Query the timing split.
    inline int rf() const;                                  // Query the transition status. 

    inline test_pt test_ptr() const;                        // Query the test pointer.

    inline void_t set_slack(float_ct);                      // Set the slack.

    inline float_t slack() const;                           // Query the slack value.

  private:

    int _el;                                                // Early/Late.
    int _rf;                                                // Rise/Fall.

    float_t _slack;                                         // Slack.

    test_pt _test_ptr;                                      // Test pointer.
};

// Function: el
// Query the timing split.
inline int Testpoint::el() const {
  return _el;
}

// Function: rf
// Query the timing transition.
inline int Testpoint::rf() const {
  return _rf;
}

// Function: test_ptr
// Query the test pointer.
inline test_pt Testpoint::test_ptr() const {
  return _test_ptr;
}

// Procedure: set_slack
// Set the slack of the testpoint.
inline void_t Testpoint::set_slack(float_ct slack) {
  _slack = slack;
}

// Function: slack
// Query the slack of the testpoint.
inline float_t Testpoint::slack() const {
  return _slack;
}




};


#endif




