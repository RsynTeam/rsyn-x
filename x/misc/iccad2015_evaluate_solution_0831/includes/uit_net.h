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

#ifndef UIT_NET_H_
#define UIT_NET_H_

#include "uit_typedef.h"
#include "uit_pin.h"
#include "uit_rctree.h"

namespace uit {

// Net class.
class Net {

  public:
    
    Net();                                                  // Constructor.
    ~Net();                                                 // Destructor.

    void_t create_new_rctree();                             // Create a rctree.
    void_t destroy_rctree();                                // Destroy the rctree.
    void_t insert_pin_ptr(pin_pt);                          // Insert a pin pointer into the list.
    void_t remove_pin_ptr(pin_pt);                          // Remove a pin pointer from the list.
    void_t enable_rc_timing_update();                       // Enable the rc timing update. 
    void_t disable_rc_timing_update();                      // Disable the rc timing update.

    inline float_t load_leave_cap(int, int) const;          // Query the load capacitance.

    inline string_crt name() const;                         // Query the net name.
    inline pinlist_pt pinlist_ptr() const;                  // Query the pinlist pointer.
    inline rctree_pt rctree_ptr() const;                    // Query the rctree pointer.
    inline pin_pt root_pin_ptr() const;                     // Query the root pin pointer.

    inline size_t num_pins() const;                         // Query the number of pins.
    
    inline void_t add_load_leave_cap(int, int, float_ct);   // Add the load capacitance.
    inline void_t set_name(string_crt);                     // Set the pin name.
    inline void_t set_load_leave_cap(int, int, float_ct);   // Set the load capacitance.
    inline void_t set_root_pin_ptr(pin_pt);                 // Set the root pin pointer.
    inline void_t set_rctree_ptr(rctree_pt);                // Set the rctree pointer.

  private:
    
    string_t _name;                                         // Net name.
    
    float_t _load_leave_cap[2][2];                          // Load capacitance of the net.

    pin_pt _root_pin_ptr;                                   // Root pin pointer of the rctree.
    rctree_pt _rctree_ptr;                                  // RC-tree pointer.
    pinlist_pt _pinlist_ptr;                                // Pin ptr list type.
};

// Function: name
// Return the name of this net.
inline string_crt Net::name() const {
  return _name;
}

// Function: num_pins
inline size_t Net::num_pins() const {
  return _pinlist_ptr->size();
}

// Function: lcap
// Query the load capacitance.
inline float_t Net::load_leave_cap(int el, int rf) const {
  return _load_leave_cap[el][rf];
}

// Procedure: set_load_leave_cap
// Set the load capacitance of this net.
inline void_t Net::set_load_leave_cap(int el, int rf, float_ct lcap) {
  _load_leave_cap[el][rf] = lcap;
}

// Procedure: add_load_leave_cap
// Add capacitance to the load capacitance of this net.
inline void_t Net::add_load_leave_cap(int el, int rf, float_ct cap) {
  _load_leave_cap[el][rf] += cap;
}

// Procedure: set_name
// Set the name of this net.
inline void Net::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_root_pin_ptr
// Set the root pin pointer.
inline void_t Net::set_root_pin_ptr(pin_pt root_pin_ptr) {
  _root_pin_ptr = root_pin_ptr;
}

// Procedure: set_rctree_ptr
// Set the rctree pointer.
inline void_t Net::set_rctree_ptr(rctree_pt rctree_ptr) {
  _rctree_ptr = rctree_ptr;
}

// Function: root_pin_ptr
// Query the root pint pointer.
inline pin_pt Net::root_pin_ptr() const {
  return _root_pin_ptr;
}

// Function: pinlist_ptr
// Return the pointer to the pinlist.
inline pinlist_pt Net::pinlist_ptr() const {
  return _pinlist_ptr;
}

// Function: rctree_ptr
// Return the rctree pointer.
inline rctree_pt Net::rctree_ptr() const {
  return _rctree_ptr;
}

};                                                          // End of uit namespace.

#endif

