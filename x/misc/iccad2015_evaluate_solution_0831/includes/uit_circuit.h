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

#ifndef UIT_CIRCUIT_H_
#define UIT_CIRCUIT_H_

#include "uit_headerdef.h"
#include "uit_utility.h"
#include "uit_classdef.h"
#include "uit_macrodef.h"
#include "uit_typedef.h"
#include "uit_pin.h"
#include "uit_net.h"
#include "uit_instance.h"
#include "uit_io_port.h"
#include "uit_clock.h"
#include "uit_dictionary.h"

namespace uit{

// Class: Circuit
class Circuit {

  public:

    Circuit();                                                                  // Constructor.
    ~Circuit();                                                                 // Destructor.

    void_t clear();                                                             // Clear the object.

    inline string_crt name() const;                                             // Query the circuit name.

    inline void_t set_name(string_crt);                                         // Set the circuit name.

    inline pin_pt pin_ptr(string_crt);                                          // Query the pin pointer.
    inline pin_dict_pt pin_dict_ptr() const;                                    // Query the pin dictionary pointer.
    inline net_pt net_ptr(string_crt);                                          // Query the net pointer.
    inline net_dict_pt net_dict_ptr() const;                                    // Query the net dictionary pointer.
    inline instance_pt instance_ptr(string_crt);                                // Query the instance pointer.
    inline instance_dict_pt instance_dict_ptr() const;                          // Query the instance dictionary pointer.
    inline clock_tree_pt clock_tree_ptr() const;                                // Query the clock source pointer.
    inline primary_input_pt primary_input_ptr(string_crt);                      // Query the primary input pointer.
    inline primary_output_pt primary_output_ptr(string_crt);                    // Query the primary input pointer.
    inline primary_input_dict_pt primary_input_dict_ptr() const;                // Query the pi dictionary pointer.
    inline primary_output_dict_pt primary_output_dict_ptr() const;              // Query the po dictionary pointer.
    inline testset_pt testset_ptr() const;                                      // Query the testset pointer.
    inline testset_rt testset() const;                                          // Query the testset reference.

    inline size_t num_pins() const;                                             // Query the pin count.
    inline size_t num_nets() const;                                             // Query the net count.
    inline size_t num_instances() const;                                        // Query the instance count.
    inline size_t num_primary_inputs() const;                                   // Query the number of primary inputs.
    inline size_t num_primary_outputs() const;                                  // Query the number of primary outputs.
    inline size_t num_tests() const;                                            // Query the number of tests.
  
    void_t remove_pin(string_crt);                                              // Remove a pin.
    void_t remove_net(string_crt);                                              // Remove a net.
    void_t remove_instance(string_crt);                                         // Remove an instance.
    void_t remove_primary_input(string_crt);                                    // Remove a primary input.
    void_t remove_primary_output(string_crt);                                   // Remove a primary output.
    void_t remove_test(test_pt);                                                // Remove a test.

    pin_pt insert_pin(string_crt);                                              // Insert a pin.
    pin_pt clock_tree_root_pin_ptr() const;                                     // Query the pin of the clock tree root.

    net_pt insert_net(string_crt);                                              // Insert a net.

    instance_pt insert_instance(string_crt);                                    // Insert an instance.

    primary_input_pt insert_primary_input(string_crt);                          // Insert a primary input.

    primary_output_pt insert_primary_output(string_crt);                        // Insert a primary output.

    test_pt insert_test(string_crt, string_crt, timing_arc_pt[]);               // Insert a test.
    test_pt insert_test(pin_pt, pin_pt, timing_arc_pt[]);                       // Insert a test.
    test_pt insert_test(pin_pt);                                                // Insert a test.

  private:
    
    string_t _name;                                                             // Name of the circuit.
    
    pin_dict_pt _pin_dict_ptr;                                                  // Pin dictionary.
    net_dict_pt _net_dict_ptr;                                                  // Net dictionary.
    instance_dict_pt _instance_dict_ptr;                                        // Instance dictionary.
    primary_input_dict_pt _primary_input_dict_ptr;                              // Primary input dictionary.
    primary_output_dict_pt _primary_output_dict_ptr;                            // Primary output dictionary.
    clock_tree_pt _clock_tree_ptr;                                              // Clock source.
    testset_pt _testset_ptr;                                                    // Constraint set.
};

// Function: name
// Return the pointer to the circuit name.
inline string_crt Circuit::name() const {
  return _name;
}

// Procedure: set_name
// Set the circuit name.
inline void_t Circuit::set_name(string_crt name) {
  _name = name;
}

// Function: pin_ptr
// Query the pin pointer.
inline pin_pt Circuit::pin_ptr(string_crt name) {
  return (*_pin_dict_ptr)[name];
}

// Function: net_ptr
// Query the net pointer.
inline net_pt Circuit::net_ptr(string_crt name) {
  return (*_net_dict_ptr)[name];
}

// Function: instance_ptr
inline instance_pt Circuit::instance_ptr(string_crt name) {
  return (*_instance_dict_ptr)[name];
}

// Function: primary_input_ptr
inline primary_input_pt Circuit::primary_input_ptr(string_crt name) {
  return (*_primary_input_dict_ptr)[name];
}

// Function: primary_output_ptr
inline primary_output_pt Circuit::primary_output_ptr(string_crt name) {
  return (*_primary_output_dict_ptr)[name];
}

// Function: primary_input_dict_ptr
inline primary_input_dict_pt Circuit::primary_input_dict_ptr() const {
  return _primary_input_dict_ptr;
}

// Function: primary_output_dict_ptr
inline primary_output_dict_pt Circuit::primary_output_dict_ptr() const {
  return _primary_output_dict_ptr;
}

// Function: pin_dict_ptr
// Query the pin dictionary pointer.
inline pin_dict_pt Circuit::pin_dict_ptr() const {
  return _pin_dict_ptr;
}

// Function: net_dict_ptr
// Query the net dictionary pointer.
inline net_dict_pt Circuit::net_dict_ptr() const {
  return _net_dict_ptr;
}

// Function: isntance_dict_ptr
// Query the instance dictionary pointer.
inline instance_dict_pt Circuit::instance_dict_ptr() const {
  return _instance_dict_ptr;
}

// Function: clock_tree_ptr
inline clock_tree_pt Circuit::clock_tree_ptr() const {
  return _clock_tree_ptr;
}

// Function: num_pins
// Query the number of pins.
inline size_t Circuit::num_pins() const {
  return pin_dict_ptr()->size();
}

// Function: num_nets
// Query the number of nets.
inline size_t Circuit::num_nets() const {
  return net_dict_ptr()->size();
}

// Function: num_instances
// Query the number of instances.
inline size_t Circuit::num_instances() const {
  return instance_dict_ptr()->size();
}

// Function: num_primary_inputs
// Query the number of primary inputs.
inline size_t Circuit::num_primary_inputs() const {
  return primary_input_dict_ptr()->size();
}

// Function: num_primary_outputs
// Query the number of primary outputs
inline size_t Circuit::num_primary_outputs() const {
  return primary_output_dict_ptr()->size();
}

// Function: num_tests
// Query the number of tests.
inline size_t Circuit::num_tests() const {
  return testset_ptr()->size();
}

// Function: testset_ptr
// Return the pointer to the test set.
inline testset_pt Circuit::testset_ptr() const {
  return _testset_ptr;
}

// Function: testset
// Return the reference to the test set.
inline testset_rt Circuit::testset() const {
  return *_testset_ptr;
}
  

};  // end of uit namespace.

#endif


