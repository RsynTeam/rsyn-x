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

#ifndef UIT_UNITDEF_H_
#define UIT_UNITDEF_H_

#include "uit_enumdef.h"

namespace uit {

// Class: Unit
// An unit is composed of 1) magnitued and 2) base.
template <class M, class B>
class Unit {

  public:
    inline M magnitude() const;                             // Query the unit magnitude.
    inline B base() const;                                  // Query the unit base.
    inline void set_magnitude(const M &);                   // Set the unit magnitude.
    inline void set_base(const B &);                        // Set the unit base.
     
  private:
    M _magnitude;                                           // magnitude of the unit.
    B _base;                                                // base of the unit.
};

// Function: base
template <class M, class B>
inline B Unit<M, B>::base() const {
  return _base;
}

// Function: magnitude
template <class M, class B>
inline M Unit<M, B>::magnitude() const {
  return _magnitude;
}

// Procedure: set_magnitude
template <class M, class B>
inline void Unit<M, B>::set_magnitude(const M &magnitude) {
  _magnitude = magnitude;
}

// Procedure: set_base
template <class M, class B>
inline void Unit<M, B>::set_base(const B &base) {
  _base = base;
}



};

#endif
