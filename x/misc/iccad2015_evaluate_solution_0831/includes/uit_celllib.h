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

#ifndef UIT_CELLLIB_H_
#define UIT_CELLLIB_H_

#include "uit_macrodef.h"
#include "uit_utility.h"
#include "uit_enumdef.h"
#include "uit_typedef.h"
#include "uit_iterator.h"

namespace uit {

// Class: CellLib
// This class defines the characteristics of a library. The library contains the descriptions
// of cells, pins, timing arcs, lookup table, delay, slew, and so on.
// For more information, visit: https://www.opensourcelibrary.org
class CellLib {

  public:

    CellLib();                                                          // Constructor.
    ~CellLib();                                                         // Destructor.
    
    void_t read(string_crt);                                            // Read the cell library.

    cell_pt insert_cell(string_crt);                                    // Insert a cell into the library.

    lut_template_pt insert_lut_template(string_crt);                    // Insert a look_up_table

    inline string_crt name() const;                                     // Query the library name.

    inline lut_template_dict_pt lut_template_dict_ptr();                // Lut template dictionary pointer.
    
    inline void_t set_name(string_crt);                                 // Set the library name.
    inline void_t set_library_type(library_type_e);                     // Set the library type.

    library_type_e library_type() const;                                // Query the library type.

    inline size_t num_cells() const;                                    // Query the number of cells.
    inline size_t num_lut_templates() const;                            // Query the number of lut templates.
    
    inline cell_pt cell_ptr(string_crt);                                // Query the cell pointer.
    inline cell_cpt cell_ptr(string_crt) const;                         // Query the cell pointer.
    inline cell_dict_pt cell_dict_ptr();                                // Cell dictionary pointer.

  private:

    string_t _name;                                                     // Library name.

    library_type_e _library_type;                                       // Library type.

    lut_template_dict_t _lut_template_dict;                             // Lut template dictionary.

    cell_dict_t _cell_dict;                                             // Cell dictionary.

    bool_t _is_delimiter(char_ct) const;                                // Query the string delimiter.
    bool_t _is_beg_group_delimiter(char_ct) const;                      // Query the left group delimiter.
    bool_t _is_end_group_delimiter(char_ct) const;                      // Query the right group delimiter.
    bool_t _is_group_delimiter(char_ct) const;                          // Query the group delimiter.
    bool_t _is_dummy_timing(timing_pt) const;                           // Query the timing status.

    void_t _read_file_to_tokens(string_crt, string_1D_vrt);             // Read the file into tokens.
    void_t _read_lut(string_1D_vrt, unsigned_rt, lut_template_pt);      // Read the lut temmplate.
    void_t _read_lut(string_1D_vrt, unsigned_rt, timing_lut_pt);        // Read the lut.
    void_t _read_cell(string_1D_vrt, unsigned_rt, cell_pt);             // Read the cell group.
    void_t _read_cellpin(string_1D_vrt, unsigned_rt, cellpin_pt);       // Read the pin group.
    void_t _read_timing_arc(string_1D_vrt, unsigned_rt, timing_arc_pt); // Readthe timing arc group.
    void_t _read_timing(string_1D_vrt, unsigned_rt, timing_pt);         // Read the timing group.
};

// Procedure: set_name
// Set the library name.
inline void_t CellLib::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_library_type
// Set the library type.
inline void_t CellLib::set_library_type(library_type_e library_type) {
  _library_type = library_type;
}

// Function: name
// Query the library name.
inline string_crt CellLib::name() const {
  return _name;
}

// Function: library_type
// Query the library type.
inline library_type_e CellLib::library_type() const {
  return _library_type;
}

// Function: num_cells
// Return the number of cells in the library.
inline size_t CellLib::num_cells() const {
  return _cell_dict.size();
}

// Function: num_lut_templates
// Return the number of cells in the library.
inline size_t CellLib::num_lut_templates() const {
  return _lut_template_dict.size();
}

// Function: cell_ptr
// Return the pointer to the cell of a given name.
inline cell_pt CellLib::cell_ptr(string_crt name) {
  return _cell_dict[name];
}

// Function: cell_ptr
// Return the pointer to the cell of a given name.
inline cell_cpt CellLib::cell_ptr(string_crt name) const {
  return _cell_dict[name];
}

// Function: cell_dict_ptr
// Return the pointer to the cell dictionary.
inline cell_dict_pt CellLib::cell_dict_ptr() {
  return &_cell_dict;
}

// Function: lut_template_dict_ptr
// Return the pointer to the lut template dictionary.
inline lut_template_dict_pt CellLib::lut_template_dict_ptr() {
  return &_lut_template_dict;
}

//-------------------------------------------------------------------------------------------------

// Class: Cell
// This class defines the core cells, a major part of a technology library. They provide
// information about the area, function, and timing of each component in an ASIC technology.
class Cell {

  public:
    
    Cell();                                                 // Constructor.
    ~Cell();                                                // Destructor.
    
    inline size_t num_cellpins() const;                     // Query the cellpin count.

    inline string_crt name() const;                         // Query the cell name.
    inline celllib_pt celllib_ptr() const;                  // Query the pointer to the celllib.
    inline cellpin_dict_pt cellpin_dict_ptr();              // Query the cellpin dictionary pointer.
    inline cellpin_pt cellpin_ptr(string_crt);              // Query the cellpin pointer.
    inline cell_type_e cell_type() const;                   // Query the cell type.

    inline void_t set_name(string_crt);                     // Set the cell name.
    inline void_t set_footprint(string_crt);                // Set the cell footprint.
    inline void_t set_leakage_power(float_ct);              // Set the leakage power.
    inline void_t set_area(float_ct);                       // Set the area.
    inline void_t set_celllib_ptr(celllib_pt);              // Set the pointer to celllib.
    inline void_t set_cell_type(cell_type_ce);              // Set the cell type.

    cellpin_pt insert_cellpin(string_crt);                  // Insert a cellpin.

  private:
    
    string_t _name;                                         // Cell name.
    string_t _footprint;                                    // Cell foot print.

    cell_type_e _cell_type;                                 // Cell type.

    float_t _leakage_power;                                 // Cell leakage power.
    float_t _area;                                          // Cell area.

    celllib_pt _celllib_ptr;                                // Pointer to the celllib.

    cellpin_dict_t _cellpin_dict;                           // Cellpin dictionary.
};

// Function: name
// Return the name of this cell.
inline string_crt Cell::name() const {
  return _name;
}

// Function: num_cellpins
// Return the number of cellpins.
inline size_t Cell::num_cellpins() const {
  return _cellpin_dict.size();
}


// Function: cellpin_ptr
// Return the pointer to the cellpin of a give name.
inline cellpin_pt Cell::cellpin_ptr(string_crt name) {
  return _cellpin_dict[name];
}

// Procedure: set_name
// Set the name of this cell.
inline void_t Cell::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_footprint
inline void_t Cell::set_footprint(string_crt footprint) {
  _footprint = footprint;
}

// Procedure: set_cell_type
inline void_t Cell::set_cell_type(cell_type_ce cell_type) {
  _cell_type = cell_type;
}

// Function: cell_type
// Return the cell type.
inline cell_type_e Cell::cell_type() const {
  return _cell_type;
}

// Procedure: set_leakage_power
inline void_t Cell::set_leakage_power(float_ct leakage_power) {
  _leakage_power = leakage_power;
}

// Procedure: set_area
// Set the area.
inline void_t Cell::set_area(float_ct area) {
  _area = area;
}

// Procedure: set_celllib_ptr
// Set the celllib pointer.
inline void_t Cell::set_celllib_ptr(celllib_pt celllib_ptr) {
  _celllib_ptr = celllib_ptr;
}

// Function: celllib_ptr
// Return the pointer to the celllib.
inline celllib_pt Cell::celllib_ptr() const {
  return _celllib_ptr;
}

// Function: cellpin_dict_ptr
// Return the pointer to the cellpin dictionary.
inline cellpin_dict_pt Cell::cellpin_dict_ptr() {
  return &_cellpin_dict;
}

//-------------------------------------------------------------------------------------------------

// Class: Cellpin
// This class defines each pin in a cell. The cell group must contain a descriptiong of
// the pin characteristics, which is defined in this class.
class Cellpin {

  public:
    
    Cellpin();                                                            // Constructor.
    ~Cellpin();                                                           // Destructor.
    
    inline size_t num_timing_arcs() const;                                // Query the timing-arc count.
    inline size_t num_timings() const;                                    // Query the timing count.

    inline void_t set_name(string_crt);                                   // Set the pin name.
    inline void_t set_capacitance(float_ct);                              // Set the pin capacitance.
    inline void_t set_is_clock(bool_ct);                                  // Set the clock attribute.
    inline void_t set_max_capacitance(float_ct);                          // Set the max pin capacitance.
    inline void_t set_min_capacitance(float_ct);                          // Set the min pin capacitance.
    inline void_t set_direction(pin_direction_ce);                        // Set the pin direction.
    inline void_t set_cell_ptr(cell_pt);                                  // Set the cell pointer.

    inline string_crt name() const;                                       // Return the pin name.

    inline pin_direction_e direction() const;                             // Return the direction.

    inline float_t capacitance() const;                                   // Return the pin capacitance.
    inline float_t max_capacitance() const;                               // Return the pin max capacitance.
    inline float_t min_capacitance() const;                               // Return the pin min capacitance.

    inline bool_t is_clock() const;                                       // Return the clock status.

    inline cell_pt cell_ptr() const;                                      // Return the cell pointer.

    inline timingset_pt timingset_ptr() const;                            // Query the timingset pointer.
    inline timingset_rt timingset() const;                                // Query the timingset reference.

    inline timing_arc_dict_pt timing_arc_dict_ptr() const;                // Query the timing arc dict pointer.
    inline timing_arc_dict_rt timing_arc_dict() const;                    // Query the timing arc dict reference.

    timing_pt insert_timing();                                            // Insert a new timing.
    timing_arc_pt insert_timing_arc(string_crt);                          // Insert a new timing arc.

  private:
    
    string_t _name;                                                       // Cell pin name.

    pin_direction_e _direction;                                           // Cell pin direction.

    float_t _capacitance;                                                 // Pin capacitance.
    float_t _max_capacitance;                                             // Max pin load capacitance (output pin).
    float_t _min_capacitance;                                             // Min pin load capacitance (output pin).

    bool_t _is_clock;                                                     // Is clock pin.

    cell_pt _cell_ptr;                                                    // Pointer to the cell.
    
    timingset_pt _timingset_ptr;                                          // Timing set pointer.
    timing_arc_dict_pt _timing_arc_dict_ptr;                              // Timing arc dictionary pointer.
};

// Function: name
// Return the name of this cell pin.
inline string_crt Cellpin::name() const {
  return _name;
}

// Function: num_timing_arcs
// Return the number of timing arcs.
inline size_t Cellpin::num_timing_arcs() const {
  return _timing_arc_dict_ptr->size();
}

// Function: num_timings
// Return the number of timings.
inline size_t Cellpin::num_timings() const {
  return _timingset_ptr->size();
}

// Procedure: set_name
// Set the name of this cell pin.
inline void_t Cellpin::set_name(string_crt name) {
  _name = name;
}

// Function: direction
// Set the direction of this cell pin.
inline pin_direction_e Cellpin::direction() const {
  return _direction;
}

// Procedure: set_direction
// Set the direction of this cell pin.
inline void_t Cellpin::set_direction(pin_direction_ce direction) {
  _direction = direction;
}

// Procedure: set_is_clock
// Set the clock status.
inline void_t Cellpin::set_is_clock(bool_ct is_clock) {
  _is_clock = is_clock;
}

// Procedure: capacitance
// Return the pin capacitance
inline float_t Cellpin::capacitance() const {
  return _capacitance;
}

// Function: max_capacitance
// Return the max pin capacitance
inline float_t Cellpin::max_capacitance() const {
  return _max_capacitance;
}

// Function: min_capacitance
// Return the min pin capacitance.
inline float_t Cellpin::min_capacitance() const {
  return _min_capacitance;
}

// Function: is_clock
// Return the clock status
inline bool_t Cellpin::is_clock() const {
  return _is_clock;
}

// Procedure: set_capacitance
// Set the capacitance of this cell pin.
inline void_t Cellpin::set_capacitance(float_ct capacitance) {
  _capacitance = capacitance;
}

// Procedure: set_max_capacitance
// Set the max capacitance of this cell pin.
inline void_t Cellpin::set_max_capacitance(float_ct max_capacitance) {
  _max_capacitance = max_capacitance;
}

// Procedure: set_min_capacitance
// Set the min capacitance of this cell pin.
inline void_t Cellpin::set_min_capacitance(float_ct min_capacitance) {
  _min_capacitance = min_capacitance;
}

// Procedure: set_cell_ptr
// Set the cell pointer.
inline void_t Cellpin::set_cell_ptr(cell_pt cell_ptr) {
  _cell_ptr = cell_ptr;
}

// Function: cell_ptr
// Return the pointer to the cell of this cellpin.
inline cell_pt Cellpin::cell_ptr() const {
  return _cell_ptr;
}

// Function: timingset_ptr
// Query the timingset pointer.
inline timingset_pt Cellpin::timingset_ptr() const {
  return _timingset_ptr;
}

// Function: timingset
// Query the timingset reference.
inline timingset_rt Cellpin::timingset() const {
  return *_timingset_ptr;
}

// Function: timing_arc_dict_ptr
// Query the pointer to the timing arc dictionary.
inline timing_arc_dict_pt Cellpin::timing_arc_dict_ptr() const {
  return _timing_arc_dict_ptr;
}

// Function: timing_arc_dict
// Query the reference to the timing arc dictionary.
inline timing_arc_dict_rt Cellpin::timing_arc_dict() const {
  return *_timing_arc_dict_ptr;
}

//-------------------------------------------------------------------------------------------------

// Class: TemplateLUT
class LUTTemplate {
  
  public:

    LUTTemplate();                                          // Constructor.
    ~LUTTemplate();                                         // Destructor.
    
    void_t clear();                                         // Clear this object.

    inline void_t set_name(string_crt);                     // Set the name.
    inline void_t set_celllib_ptr(celllib_pt);              // Set the celllib pointer.
    inline void_t set_variable1(lut_variable_ce);           // Set the variable 1.
    inline void_t set_variable2(lut_variable_ce);           // Set the variable 2.
    inline void_t insert_index1(float_ct);                  // Add an index.
    inline void_t insert_index2(float_ct);                  // Add an index.

    inline string_crt name() const;                         // Query the name.
    inline celllib_pt celllib_ptr() const;                  // Query the celllib pointer. 
    inline size_t size1() const;                            // Query the size of variable 1.
    inline size_t size2() const;                            // Query the size of variable 2. 
    inline lut_variable_e variable1() const;                // Query the variable 1.
    inline lut_variable_e variable2() const;                // Query the variable 2.

  private:
    
    string_t _name;                                         // Name.
    
    lut_variable_e _variable1;                              // Variable for index 1.
    lut_variable_e _variable2;                              // Variable for index 2.

    float_vt _indices1;                                     // Indices for index 1.
    float_vt _indices2;                                     // Indices for index 2.

    celllib_pt _celllib_ptr;                                // Pointer to library.
};

// Function: variable1
inline lut_variable_e LUTTemplate::variable1() const {
  return _variable1;
}

// Function: variable1
inline lut_variable_e LUTTemplate::variable2() const {
  return _variable2;
}

// Procedure: insert_index1
inline void_t LUTTemplate::insert_index1(float_ct index) {
  _indices1.push_back(index);
}

// Procedure: insert_index2
inline void_t LUTTemplate::insert_index2(float_ct index) {
  _indices2.push_back(index);
}

// Procedure: set_variable1
inline void_t LUTTemplate::set_variable1(lut_variable_ce variable1) {
  _variable1 = variable1;
}

// Procedure: set_variable2
inline void_t LUTTemplate::set_variable2(lut_variable_ce variable2) {
  _variable2 = variable2;
}

// Function: name
// Return the name.
inline string_crt LUTTemplate::name() const {
  return _name;
}

// Procedure: set_celllib_ptr
// Set the celllib pointer.
inline void_t LUTTemplate::set_celllib_ptr(celllib_pt celllib_ptr) {
  _celllib_ptr = celllib_ptr;
}

// Function: celllib_ptr
inline celllib_pt LUTTemplate::celllib_ptr() const {
  return _celllib_ptr;
}

// Procedure: set_name
inline void_t LUTTemplate::set_name(string_crt name) {
  _name = name;
}

// Function: size1
inline size_t LUTTemplate::size1() const {
  return _indices1.size();
}

// Function: size2
inline size_t LUTTemplate::size2() const {
  return _indices2.size();
}

//-------------------------------------------------------------------------------------------------

// Class: TimingLUT
// This class defines the look up table to store delay or slew functions.
// Look up table is indexed by the output load capacitance and the input transition slew.
// Let L = _output_load_indices[i] and T = _input_slew_indices[j], the table value corresponding 
// to  L and T will be: table[i][j]
class TimingLUT {
  
  public:
    
    TimingLUT();                                                                     // Constructor.
    ~TimingLUT();                                                                    // Destructor.

    void_t clear();                                                                  // Clear this object.
    void_t resize(int_ct, int_ct);                                                   // Resize the table.
    
    inline void_t set_lut_template_ptr(lut_template_pt);                             // Set the lut template pointer.
    inline void_t assign_index_1(int_ct, float_ct);                                  // Add an index value.
    inline void_t assign_index_2(int_ct, float_ct);                                  // Add an index value.
    inline void_t assign_value(int_ct, int_ct, float_ct);                            // Add a value to the table.

    inline float_t indices1(int_ct) const;                                           // Query the indices 1.
    inline float_t indices2(int_ct) const;                                           // Query the indices 2.
    inline float_t table(int_ct, int_ct) const;                                      // Query the table value.
    
    inline lut_template_pt lut_template_ptr() const;                                 // Query the lut template pointer.

    inline size_t size1() const;                                                     // Query the table size 1.
    inline size_t size2() const;                                                     // Query the table size 2.

    inline bool_t is_scalar() const;                                                 // Query the scalar table.
    inline bool_t empty() const;                                                     // Query the existence of the LUT.

    float_t lut_polation(float_ct, float_ct) const;                                  // Query the linear numerics.
    float_t linear_polation(float_ct, float_ct, float_ct, float_ct, float_ct) const; // Inter/extra polation

  private:
  
    float_vt _indices1;                                                              // Indices 1.
    float_vt _indices2;                                                              // Indices 2.
    float_mt _table;                                                                 // Lookup table.

    lut_template_pt _lut_template_ptr;                                               // LUT template pointer.
};

// Function: lut_template_ptr
// Return the pointer to the lut template.
inline lut_template_pt TimingLUT::lut_template_ptr() const {
  return _lut_template_ptr;
}

// Function: size1
inline size_t TimingLUT::size1() const {
  return _indices1.size();
}

// Function: size2
inline size_t TimingLUT::size2() const {
  return _indices2.size();
}

// Function: is_scalar
inline bool_t TimingLUT::is_scalar() const {
  return (size1() == 1 && size2() == 1);
}

// Function: empty
inline bool_t TimingLUT::empty() const {
  return size1() == 0 && size2() == 0;
}

// Procedure: set_lut_template_ptr
// Set the lut template pointer.
inline void_t TimingLUT::set_lut_template_ptr(lut_template_pt lut_template_ptr) {
  _lut_template_ptr = lut_template_ptr;
}

// Procedure: assign_value
inline void_t TimingLUT::assign_value(int_ct i, int_ct j, float_ct val) {
  _table[i][j] = val;
}

// Procedure: assign_index_1
// Add an index 1.
inline void_t TimingLUT::assign_index_1(int_ct i, float_ct val) {
  _indices1[i] = val;
}

// Procedure: assign_index_2
// Add an index 2.
inline void_t TimingLUT::assign_index_2(int_ct i, float_ct val) {
  _indices2[i] = val;
}

// Function: indices1
// Return the value of indices 1
inline float_t TimingLUT::indices1(int_ct i) const {
  return _indices1[i];
}

// Function: indices2
// Return the value of indices 2
inline float_t TimingLUT::indices2(int_ct j) const {
  return _indices2[j];
}

// Function: table
inline float_t TimingLUT::table(int_ct i, int_ct j) const {
  return _table[i][j];
}

//-------------------------------------------------------------------------------------------------

// Class: Timing
class Timing {

  public:
    
    Timing();                                                           // Constructor.
    ~Timing();                                                          // Destructor.

    inline int_t idx() const;                                           // Query the idx.

    inline string_crt from_cellpin_name() const;                        // Query the from cellpin name.
    inline string_crt to_cellpin_name() const;                          // Query the to cellpin name.
    
    inline void_t set_idx(int_ct);                                      // Set the idx.
    inline void_t set_name(string_crt);                                 // Set the timing arc name.
    inline void_t set_timing_sense(timing_sense_ce);                    // Set the timing sense.
    inline void_t set_timing_type(timing_type_ce);                      // Set the timing type.
    inline void_t set_from_cellpin_name(string_crt);                    // Set the from cellpin name.
    inline void_t set_to_cellpin_name(string_crt);                      // Set the to cellpin name.

    inline timing_sense_e timing_sense() const;                         // Query the timing sense.
    inline timing_type_e timing_type() const;                           // Query the timing arc type.

    inline timing_lut_pt cell_rise_ptr() const;                         // Query the lut pointer (delay).
    inline timing_lut_pt cell_fall_ptr() const;                         // Query the lut pointer (delay).
    inline timing_lut_pt rise_transition_ptr() const;                   // Query the lut pointer (slew).
    inline timing_lut_pt fall_transition_ptr() const;                   // Query the lut pointer (slew).
    inline timing_lut_pt rise_constraint_ptr() const;                   // Query the lut pointer (constraint).
    inline timing_lut_pt fall_constraint_ptr() const;                   // Query the lut pointer (constraint).
    inline timing_lut_rt cell_rise() const;                             // Query the lut pointer (delay).
    inline timing_lut_rt cell_fall() const;                             // Query the lut pointer (delay).
    inline timing_lut_rt rise_transition() const;                       // Query the lut pointer (slew).
    inline timing_lut_rt fall_transition() const;                       // Query the lut pointer (slew).
    inline timing_lut_rt rise_constraint() const;                       // Query the lut pointer (constraint).
    inline timing_lut_rt fall_constraint() const;                       // Query the lut pointer (constraint).

    bool_t is_constraint() const;                                       // Query the constraint status.
    bool_t is_hold_constraint() const;                                  // Query the constraint status.
    bool_t is_setup_constraint() const;                                 // Query the constraint status.
    bool_t is_rising_edge_triggered() const;                            // Query the rising edge status.
    bool_t is_falling_edge_triggered() const;                           // Query the falling edge status.
    bool_t is_input_transition_defined() const;                         // Query the input transition status.
    bool_t is_input_transition_defined(int) const;                      // Query the input transition status.
    bool_t is_output_transition_defined(int) const;                     // Query the output transition status.
    bool_t is_transition_defined(int, int) const;                       // Query the timing transition status.

  private:
     
    int_t _idx;

    string_t _from_cellpin_name;                                        // Name of from cellpin name.
    string_t _to_cellpin_name;                                          // Name of to cellpin name.

    timing_sense_e _timing_sense;                                       // Timing sense of this timing arc.
    timing_type_e _timing_type;                                         // Type of this timing arc.

    timing_lut_pt _cell_rise_ptr;                                       // Delay LUT rise (sink).
    timing_lut_pt _cell_fall_ptr;                                       // Delay LUT fall (sink).
    timing_lut_pt _rise_transition_ptr;                                 // Slew LUT rise (sink).
    timing_lut_pt _fall_transition_ptr;                                 // Slew LUT fall (sink).
    timing_lut_pt _rise_constraint_ptr;                                 // Rise constraint (sink).
    timing_lut_pt _fall_constraint_ptr;                                 // Fall constraint (sink).
};

// Function: idx
inline int_t Timing::idx() const {
  return _idx;
}

// Procedure: set_idx
inline void_t Timing::set_idx(int_ct idx) {
  _idx = idx;
}

// Function: from_cellpin_name
inline string_crt Timing::from_cellpin_name() const {
  return _from_cellpin_name;
}

// Function: to_cellpin_name
inline string_crt Timing::to_cellpin_name() const {
  return _to_cellpin_name;
}

// Procedure: set_from_cellpin_name
inline void_t Timing::set_from_cellpin_name(string_crt name) {
  _from_cellpin_name = name;
}

// Procedure: set_from_cellpin_name
inline void_t Timing::set_to_cellpin_name(string_crt name) {
  _to_cellpin_name = name;
}

// Procedure: set_timing_sense
inline void_t Timing::set_timing_sense(timing_sense_ce timing_sense) {
  _timing_sense = timing_sense;
}

// Procedure: set_timing_type
inline void_t Timing::set_timing_type(timing_type_ce timing_type) {
  _timing_type = timing_type;
}

// Function: timing_sense
// Return the timing sense of this timing arc.
inline timing_sense_e Timing::timing_sense() const {
  return _timing_sense;
}

// Function: timing_type
// Return the timing type of this timing arc.
inline timing_type_e Timing::timing_type() const {
  return _timing_type;
}

// Function: cell_rise_ptr
// Return the lut of rise delay.
inline timing_lut_pt Timing::cell_rise_ptr() const{
  return _cell_rise_ptr;
}

// Function: cell_fall_ptr
// Return the lut of fall delay.
inline timing_lut_pt Timing::cell_fall_ptr() const {
  return _cell_fall_ptr;
}

// Function: rise_transition_ptr
// Return the lut pointer of rise slew.
inline timing_lut_pt Timing::rise_transition_ptr() const {
  return _rise_transition_ptr;
}

// Function: fall_transition_ptr
// Return the lut pointer of fall slew.
inline timing_lut_pt Timing::fall_transition_ptr() const {
  return _fall_transition_ptr;
}

// Function: rise_constraint_ptr
inline timing_lut_pt Timing::rise_constraint_ptr() const {
  return _rise_constraint_ptr;
}

// Function: fall_constraint_ptr
inline timing_lut_pt Timing::fall_constraint_ptr() const {
  return _fall_constraint_ptr;
}

// Function: cell_rise
// Return the lut of rise delay.
inline timing_lut_rt Timing::cell_rise() const {
  return *_cell_rise_ptr;
}

// Function: cell_fall
// Return the lut of fall delay.
inline timing_lut_rt Timing::cell_fall() const {
  return *_cell_fall_ptr;
}

// Function: rise_transition
// Return the lut pointer of rise slew.
inline timing_lut_rt Timing::rise_transition() const {
  return *_rise_transition_ptr;
}

// Function: fall_transition
// Return the lut pointer of fall slew.
inline timing_lut_rt Timing::fall_transition() const {
  return *_fall_transition_ptr;
}

// Function: rise_constraint
inline timing_lut_rt Timing::rise_constraint() const {
  return *_rise_constraint_ptr;
}

// Function: fall_constraint
inline timing_lut_rt Timing::fall_constraint() const {
  return *_fall_constraint_ptr;
}

// Class: TimingArc
// This class defines the characteristics of timing arcs. Timing arcs are divided into two
// major areas: 1) timing delay (the actual circuit timing) and 2) timing constraints (at
// the boundaries). Timing arcs along with netlist interconnect information are the paths
// followed by the path tracer during path analysis. Each timing arc has a startpoint and 
// an endpoint. The startpoint can be an input, output, or I/O pin. The endpoint is always
// an output pin or an I/O pin. The only "exception" is a constraint timing arc, such as a
// setup or hold constraint between two input pins.
class TimingArc {

  public:

    TimingArc();                                                        // Constructor.
    ~TimingArc();                                                       // Destructor.
    
    inline string_crt from_cellpin_name() const;                        // Query the from cellpin name.
    inline string_crt to_cellpin_name() const;                          // Query the to cellpin name.

    inline void_t set_name(string_crt);                                 // Set the timing arc name.
    inline void_t set_timing_type(timing_type_ce);                      // Set the timing type.
    inline void_t set_from_cellpin_name(string_crt);                    // Set the from cellpin name.
    inline void_t set_to_cellpin_name(string_crt);                      // Set the to cellpin name.
    inline void_t set_timing_ptr(int, int, timing_pt);                  // Set the timing ptr.

    inline timing_pt timing_ptr(int, int) const;                        // Query the timing ptr.

    timing_sense_e timing_sense() const;                                // Query the timing sense.
    timing_type_e timing_type() const;                                  // Query the timing arc type.
    
    bool_t is_constraint() const;                                       // Query the constraint status.
    bool_t is_hold_constraint() const;                                  // Query the constraint status.
    bool_t is_setup_constraint() const;                                 // Query the constraint status.
    bool_t is_rising_edge_triggered() const;                            // Query rising edge status.
    bool_t is_falling_edge_triggered() const;                           // Query falling edge status.
    bool_t is_transition_defined(int, int) const;                       // Query the transition status.

    float_t slew(int, int, float_ct, float_ct) const;                   // Query the slew.
    float_t delay(int, int, float_ct, float_ct) const;                  // Query the delay.
    float_t constraint(int, int, float_ct, float_ct) const;             // Query the constraint.

    bool_t is_positive_unate() const;                                   // Query the transition status.
    bool_t is_negative_unate() const;                                   // Query the transition status.
    bool_t is_non_unate() const;                                        // Query the transition status.

  private:

    string_t _from_cellpin_name;                                        // Name of from cellpin name.
    string_t _to_cellpin_name;                                          // Name of to cellpin name.

    timing_pt _timing_ptr[2][2];                                        // Transition timing piece.

    timing_type_e _timing_type;                                         // Type of this timing arc.
};

// Function: from_cellpin_name
inline string_crt TimingArc::from_cellpin_name() const {
  return _from_cellpin_name;
}

// Function: to_cellpin_name
inline string_crt TimingArc::to_cellpin_name() const {
  return _to_cellpin_name;
}

// Procedure: set_from_cellpin_name
inline void_t TimingArc::set_from_cellpin_name(string_crt name) {
  _from_cellpin_name = name;
}

// Procedure: set_from_cellpin_name
inline void_t TimingArc::set_to_cellpin_name(string_crt name) {
  _to_cellpin_name = name;
}

// Procedure: set_timing_type
inline void_t TimingArc::set_timing_type(timing_type_ce timing_type) {
  _timing_type = timing_type;
}

// Function: timing_type
// Return the timing type of this timing arc.
inline timing_type_e TimingArc::timing_type() const {
  return _timing_type;
}

// Procedure: set_timing_ptr
inline void_t TimingArc::set_timing_ptr(int irf, int orf, timing_pt timing_ptr) {
  _timing_ptr[irf][orf] = timing_ptr;
}

// Function: timing_ptr
inline timing_pt TimingArc::timing_ptr(int irf, int orf) const {
  return _timing_ptr[irf][orf];
}


};                                                          // End of uit namespace.

#endif

