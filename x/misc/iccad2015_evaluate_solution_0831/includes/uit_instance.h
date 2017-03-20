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

#ifndef UIT_INSTANCE_H_
#define UIT_INSTANCE_H_

#include "uit_utility.h"
#include "uit_typedef.h"

namespace uit {

// Instance class.
class Instance {

  public:
    
    Instance();                                             // Constructor.
    ~Instance();                                            // Destructor.
    
    inline string_crt name() const;                         // Return the pin name.

    inline cell_pt cell_ptr() const;                        // Query the default cell pointer.
    inline cell_pt cell_ptr(int) const;                     // Query the cell pointer.

    inline void_t set_name(string_crt);                     // Set the pin name.
    inline void_t set_cell_ptr(int, cell_pt);               // Set the cell pointer.

  private:

    string_t _name;                                         // Instance name.
    cell_pt _cell_ptr[2];                                   // Cell pointer.
};

// Function: name
// Return the name of this cell.
inline string_crt Instance::name() const {
  return _name;
}

// Procedure: set_name
// Set the name of this cell.
inline void_t Instance::set_name(string_crt name) {
  _name = name;
}

// Function: cell_ptr
// Return the cell pointer.
inline cell_pt Instance::cell_ptr(int el) const {
  return _cell_ptr[el];
}

// Function: cell_ptr
// Return the default cell pointer.
inline cell_pt Instance::cell_ptr() const {
  return cell_ptr(EARLY);
}

// Procedure: set_cell_ptr
// Set the cell pointer.
inline void_t Instance::set_cell_ptr(int el, cell_pt cell_ptr) {
  _cell_ptr[el] = cell_ptr;
}

};                                                          // End of uit namespace.

#endif

