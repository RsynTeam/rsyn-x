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

#ifndef UIT_SPEF_H_
#define UIT_SPEF_H_

#include "uit_macrodef.h"
#include "uit_typedef.h"

namespace uit {

// Class: Spef
class Spef {

  friend class timer;

  public:

    Spef();                                                                     // Constructor.
    ~Spef();                                                                    // Destructor.

    void_t read_spef(string_crt);                                               // Read the parasitics.

    spefnet_pt insert_spefnet(string_crt, float_ct);                            // Insert the spef net.
    inline spefnet_ptr_vpt spefnets_ptr() const;                                // Query the vector pointer.

  private:

    void_t _read_file_to_tokens(string_crt, string_1D_vrt);                     // Read the file into tokens.
    void_t _read_file_to_buffer(string_crt, char_rpt, char_rpt);                // Read the file into buffer.
    bool_t _read_line_to_tokens(istream &, string_1D_vrt);                      // Read the line into tokens.
    bool_t _read_next_token(char_cpt, char_cpt, char_rpt, char_rpt, char_pt);   // Read the next token.

    spefnet_ptr_vpt _spefnets_ptr;                                              // Spef nets.
};

// Function: spefnets_ptr
inline spefnet_ptr_vpt Spef::spefnets_ptr() const {
  return _spefnets_ptr;
}

// Class: SpefPin
class SpefPin {
  public:

    SpefPin();                                              // Constructor.
    ~SpefPin();                                             // Destructor.
    
    inline string_crt name() const;                         // Query the name.

    inline void_t set_name(string_crt);                     // Set the name.

  private:
    string_t _name;                                         // Pin name.
};

// Procedure: set_name
inline void_t SpefPin::set_name(string_crt name) {
  _name = name;
}

// Function: name
inline string_crt SpefPin::name() const {
  return _name;
}

// Class: SpefCap
class SpefCap {
  public:

    SpefCap();                                              // Constructor.
    ~SpefCap();                                             // Destructor.
    
    inline float_t cap() const;                             // Query the capacitance.

    inline string_crt name() const;                         // Query the name.

    inline void_t set_name(string_crt);                     // Set the name.
    inline void_t set_cap(float_ct);                        // Set the capacitance.

  private:
    string_t _name;                                         // Pin name.

    float_t _cap;                                           // Pin capacitance.
};

// Procedure: set_name
inline void_t SpefCap::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_cap
inline void_t SpefCap::set_cap(float_ct cap) {
  _cap = cap;
}

// Function: name
inline string_crt SpefCap::name() const {
  return _name;
}

// Function: cap
inline float_t SpefCap::cap() const {
  return _cap;
}

// Class: SpefRes
class SpefRes {

  public:
    SpefRes();
    ~SpefRes();

    inline void_t set_res(float_ct);                        // Set the resistance.
    inline void_t set_endpoint_name_1(string_crt);          // Set the name 1.
    inline void_t set_endpoint_name_2(string_crt);          // Set the name 2.
    
    inline float_t res() const;                             // Query the resistance.
    inline string_crt endpoint_name_1() const;              // Query the name 1.
    inline string_crt endpoint_name_2() const;              // Query the name 2.

  private:

    string_t _endpoint_name_1;                              // Endpoint name 1.
    string_t _endpoint_name_2;                              // Endpoint name 2.

    float_t _res;                                           // Resistance.
};

// Procedure: set_res
inline void_t SpefRes::set_res(float_ct res) {
  _res = res;
}

// Procedure: set_endpoint_name_1
inline void_t SpefRes::set_endpoint_name_1(string_crt name) {
  _endpoint_name_1 = name;
}

// Procedure: set_endpoint_name_2
inline void_t SpefRes::set_endpoint_name_2(string_crt name) {
  _endpoint_name_2 = name;
}

// Function: res
inline float_t SpefRes::res() const {
  return _res;
}

// Function: endpoint_name_1
inline string_crt SpefRes::endpoint_name_1() const {
  return _endpoint_name_1;
}

// Function: endpoint_name_2
inline string_crt SpefRes::endpoint_name_2() const {
  return _endpoint_name_2;
}

// Class: SpefNet
class SpefNet {
  
  public:
    SpefNet();                                              // Constructor.
    ~SpefNet();                                             // Destructor.

    inline string_crt name() const;                         // Query the net name.

    inline float_t lcap() const;                            // Query the load capacitance.
    
    spefcap_pt insert_spefcap(string_crt name, float_ct);        // Insert a spef cap object.
    spefpin_pt insert_spefpin(string_crt name);                  // Insert a spef pin object.
    spefres_pt insert_spefres(string_crt, string_crt, float_ct); // Insert a spef res object.

    inline void_t set_name(string_crt);                     // Set the net name.
    inline void_t set_lcap(float_ct);                       // Set the load capacitance.

    inline spefpin_ptr_vpt spefpins_ptr() const;            // Query the spefpins pointer.
    inline spefcap_ptr_vpt spefcaps_ptr() const;            // Query the spefcaps pointer.
    inline spefres_ptr_vpt spefreses_ptr() const;           // Query the spefreses pointer.
  
  private:

    string_t _name;                                         // Net name.

    float_t _lcap;                                          // Load capacitance.

    spefpin_ptr_vpt _spefpins_ptr;                          // Pins.
    spefcap_ptr_vpt _spefcaps_ptr;                          // Capacitances.
    spefres_ptr_vpt _spefreses_ptr;                         // Resistance.

};

// Function: name
inline string_crt SpefNet::name() const {                   
  return _name;
}

// Function: lcap
inline float_t SpefNet::lcap() const {
  return _lcap;
}

// Procedure: set_name
inline void_t SpefNet::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_lcap
inline void_t SpefNet::set_lcap(float_ct lcap) {
  _lcap = lcap;
}

// Function: spefpins_ptr
inline spefpin_ptr_vpt SpefNet::spefpins_ptr() const {
  return _spefpins_ptr;
}

// Function: spefcaps_ptr
inline spefcap_ptr_vpt SpefNet::spefcaps_ptr() const {
  return _spefcaps_ptr;
}

// Function: spefreses_ptr
inline spefres_ptr_vpt SpefNet::spefreses_ptr() const {
  return _spefreses_ptr;
}

};


#endif
