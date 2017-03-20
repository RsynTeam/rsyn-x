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

#ifndef UIT_PIN_H_
#define UIT_PIN_H_

#include "uit_utility.h"
#include "uit_typedef.h"
#include "uit_celllib.h"
#include "uit_test.h"

namespace uit {

// Pin class.
class Pin {

  public:
    
    Pin();                                                  // Constructor.
    ~Pin();                                                 // Destructor.

    inline string_crt name() const;                         // Return the pin name.
    inline pin_direction_e direction() const;               // Query the pin direction.
    inline net_pt net_ptr() const;                          // Query the net pointer.
    inline node_pt node_ptr() const;                        // Query the node pointer.
    inline cellpin_pt cellpin_ptr(int) const;               // Query the cellpin pointer.
    inline instance_pt instance_ptr() const;                // Query the instance pointer.
    inline rctree_node_pt rctree_node_ptr() const;          // Query the rctree node pointer.
    inline primary_input_pt primary_input_ptr() const;      // Query the primary input pointer.
    inline primary_output_pt primary_output_ptr() const;    // Query the primary output pointer.
    inline pinlist_iter_t pinlist_satellite() const;        // Query the satellite on the plist.
    inline test_pt test_ptr() const;                        // Query the test pointer.

    inline void_t set_name(string_crt);                     // Set the pin name.
    inline void_t set_direction(pin_direction_ce);          // Set the pin direction.
    inline void_t set_cellpin_ptr(int, cellpin_pt);         // Set the cell pin pointer.
    inline void_t set_instance_ptr(instance_pt);            // Set the instance pointer.
    inline void_t set_net_ptr(net_pt);                      // Set the net pointer.
    inline void_t set_node_ptr(node_pt);                    // Set the node pointer.
    inline void_t set_rctree_node_ptr(rctree_node_pt);      // Set the rctree node pointer.
    inline void_t set_primary_input_ptr(primary_input_pt);  // Set the primary input pointer.
    inline void_t set_primary_output_ptr(primary_output_pt);// Set the primary output pointer.
    inline void_t set_pinlist_satellite(pinlist_iter_t);    // Set the satellite.
    inline void_t set_test_ptr(test_pt);                    // Set the test pointer.

    float_t cap(int, int) const;                            // Query the pin capacitance.
    float_t lcap(int, int) const;                           // Query the load capacitance.

    bool_t is_rctree_root() const;                          // Query if the pin is a rc root.
    bool_t is_sequential() const;                           // Query if the pin is a sequential pin.
    bool_t is_constrained() const;                          // Query if the pin is a constrained pin.
    bool_t is_clock_sink() const;                           // Query if the pin is a clock sink pin.
    bool_t is_primary_input() const;                        // Query if the pin is a primary input pin.
    bool_t is_primary_output() const;                       // Query if the pin is a primary output pin.

  private:
    
    string_t _name;                                         // Pin name.
    
    pin_direction_e _direction;                             // Pin direction.
    
    net_pt _net_ptr;                                        // Net pointer.
    node_pt _node_ptr;                                      // Node pointer.
    cellpin_pt _cellpin_ptr[2];                             // Cellpin pointer.
    instance_pt _instance_ptr;                              // Instance pointer.
    rctree_node_pt _rctree_node_ptr;                        // RC tree node pointer.
    primary_input_pt _primary_input_ptr;                    // Primary input port.
    primary_output_pt _primary_output_ptr;                  // Primary output port.
    pinlist_iter_t _pinlist_satellite;                      // Satellite pointer.
    test_pt _test_ptr;                                      // Constraint pointer.
};

// Function: name
// Return the pin name.
inline string_crt Pin::name() const {
  return _name;
}

// Procedure: set_name
// Set the pin name.
inline void Pin::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_cellpin_ptr
// Set the cellpin pointer.
inline void_t Pin::set_cellpin_ptr(int el, cellpin_pt cellpin_ptr) {
  _cellpin_ptr[el] = cellpin_ptr;
}

// Procedure: set_instance_ptr
// Set the instance pointer.
inline void_t Pin::set_instance_ptr(instance_pt instance_ptr) {
  _instance_ptr = instance_ptr;
}

// Procedure: set_direction
// Set the pin direction.
inline void_t Pin::set_direction(pin_direction_ce direction) {
  _direction = direction;
}

// Procedure: set_rctree_node_ptr
// Set the rctree node pointer.
inline void_t Pin::set_rctree_node_ptr(rctree_node_pt rctree_node_ptr) {
  _rctree_node_ptr = rctree_node_ptr;
}

// Procedure: set_primary_input_ptr
// Set the primary input pointer.
inline void_t Pin::set_primary_input_ptr(primary_input_pt primary_input_ptr) {
  _primary_input_ptr = primary_input_ptr;
}

// Procedure: set_primary_output_ptr
// Set the primary output pointer.
inline void_t Pin::set_primary_output_ptr(primary_output_pt primary_output_ptr) {
  _primary_output_ptr = primary_output_ptr;
}

// Procedure: set_test_ptr
// Set the test pointer.
inline void_t Pin::set_test_ptr(test_pt test_ptr) {
  _test_ptr = test_ptr;
}

// Procedure: set_net_ptr
// Set the net pointer.
inline void_t Pin::set_net_ptr(net_pt net_ptr) {
  _net_ptr = net_ptr;
}

// Procedure: set_node_ptr
// Set the node pointer.
inline void_t Pin::set_node_ptr(node_pt node_ptr) {
  _node_ptr = node_ptr;
}

// Procedure: set_pinlist_satellite
// Set the satellite.
inline void_t Pin::set_pinlist_satellite(pinlist_iter_t pinlist_satellite) {
  _pinlist_satellite = pinlist_satellite;
}

// Function: direction
// Return the pin direction
inline pin_direction_e Pin::direction() const {
  return _direction;
}

// Function: cellpin_ptr
// Return the cellpin pointer.
inline cellpin_pt Pin::cellpin_ptr(int el) const {
  return _cellpin_ptr[el];
}

// Function: rctree_node_ptr
// Return the pointer to a rctree node.
inline rctree_node_pt Pin::rctree_node_ptr() const {
  return _rctree_node_ptr;
}

// Function: node_ptr
// Return the node pointer.
inline node_pt Pin::node_ptr() const {
  return _node_ptr;
}

// Function: instance_ptr
// Return the instance pointer.
inline instance_pt Pin::instance_ptr() const {
  return _instance_ptr;
}

// Function: pinlist_satellite
// Return the pinlist satellite
inline pinlist_iter_t Pin::pinlist_satellite() const {
  return _pinlist_satellite;
}

// Function: primary_input_ptr
// Return the primary input pointer.
inline primary_input_pt Pin::primary_input_ptr() const {
  return _primary_input_ptr;
}

// Function: primary_output_ptr
// Return the primary output pointer.
inline primary_output_pt Pin::primary_output_ptr() const {
  return _primary_output_ptr;
}

// Function: net_ptr
// Return the net pointer
inline net_pt Pin::net_ptr() const {
  return _net_ptr;
}

// Function: test_ptr
// Return the test pointer.
inline test_pt Pin::test_ptr() const {
  return _test_ptr;
}

};

#endif


