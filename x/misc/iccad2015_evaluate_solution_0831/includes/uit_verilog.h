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

#ifndef UIT_VERILOG_H_
#define UIT_VERILOG_H_

#include "uit_macrodef.h"
#include "uit_utility.h"
#include "uit_datadef.h"
#include "uit_enumdef.h"
#include "uit_typedef.h"
#include "uit_iterator.h"

namespace uit {

// Class: Verilog
// The verilog file specifies the top-level hierarchy of the design. The class defins a small
// set of keywords with the Verilog language defines as follows.
//
// module simple (
// ...
// );
//
// input inp1;
// ...
//
// output out;
// ...
//
// wire n1;
// ...
//
// NAND2_X1 u1 ( .a(inp1), .b(inp2), .o(n1) );
// ...
class Verilog {

  public:

    Verilog();                                              // Constructor.
    ~Verilog();                                             // Destructor.

    void_t report();                                        // Report the verilog object.
    void_t read_module(string_crt);                         // Read the verilog file.

    inline verilog_module_dict_pt module_dict_ptr();        // Query the module dictionary pointer.

    verilog_module_pt insert_module(string_crt);            // Insert a module.

  private:

    string_t _name;                                         // Verilog name.

    verilog_module_dict_pt _module_dict_ptr;                // Module dictionary.

    bool_t _is_beg_group_delimiter(char_ct) const;          // Query the group beg delimiter.
    bool_t _is_end_group_delimiter(char_ct) const;          // Query the group end delimiter.
    bool_t _is_group_delimiter(char_ct) const;              // Query the group delimiter.

    void_t _read_file_to_buffer(string_crt, char_rpt, char_rpt); 
    bool_t _read_next_token(char_cpt, char_cpt, char_rpt, char_pt);
};

// Function: module_dict_ptr
// Return the module dictionary pointer.
inline verilog_module_dict_pt Verilog::module_dict_ptr() {
  return _module_dict_ptr;
}

// Class: VerilogWire
class VerilogWire {
  
  public:
    
    inline string_crt name() const;                         // Query the name.

    inline void_t set_name(string_crt);                     // Set the name.

  private:

    string_t _name;                                         // Name.
};

// Procedure: set_name
inline void_t VerilogWire::set_name(string_crt name) {
  _name = name;
}

// Function: name
inline string_crt VerilogWire::name() const {
  return _name;
}

// Class: VerilogInput
class VerilogInput {
  
  public:
    
    inline string_crt name() const;                         // Query the name.

    inline void_t set_name(string_crt);                     // Set the name.

  private:

    string_t _name;                                         // Name.
};

// Procedure: set_name
inline void_t VerilogInput::set_name(string_crt name) {
  _name = name;
}

// Function: name
inline string_crt VerilogInput::name() const {
  return _name;
}

// Class: VerilogOutput
class VerilogOutput {
  
  public:
    
    inline string_crt name() const;                         // Query the name.

    inline void_t set_name(string_crt);                     // Set the name.

  private:

    string_t _name;                                         // Name.
};

// Procedure: set_name
inline void_t VerilogOutput::set_name(string_crt name) {
  _name = name;
}

// Function: name
inline string_crt VerilogOutput::name() const {
  return _name;
}

// Class: VerilogModule
// This class defines the module in a verilog file.
class VerilogModule {
  
  public:

    VerilogModule();                                                    // Constructor.
    ~VerilogModule();                                                   // Destructor.
    
    inline string_crt name() const;                                     // Query the module name.

    inline void_t set_name(string_crt);                                 // Set the module name.    

    inline size_t num_wires() const;                                    // Query the wire count.
    inline size_t num_inputs() const;                                   // Query the input count.
    inline size_t num_outputs() const;                                  // Query the output count.
    inline size_t num_instances() const;                                // Query the instance count.

    verilog_input_pt insert_input(string_crt);                          // Insert an input pin.
    verilog_output_pt insert_output(string_crt);                        // Insert a output pin.
    verilog_wire_pt insert_wire(string_crt);                            // Insert a wire net.
    verilog_instance_pt insert_instance(string_crt, string_crt);        // Insert an instance.

    inline verilog_wire_ptr_vpt wires_ptr() const;                      // Query the wires pointer.
    inline verilog_input_ptr_vpt inputs_ptr() const;                    // Query the inputs pointer.
    inline verilog_output_ptr_vpt outputs_ptr() const;                  // Query the Outputs pointer.
    inline verilog_instance_ptr_vpt instances_ptr() const;              // Query the instances pointer.
  
  private:
    
    string_t _name;                                                     // Module name.

    verilog_wire_ptr_vpt _wires_ptr;
    verilog_input_ptr_vpt _inputs_ptr;
    verilog_output_ptr_vpt _outputs_ptr;
    verilog_instance_ptr_vpt _instances_ptr;
};

// Function: name
// Return the name pointer.
inline string_crt VerilogModule::name() const {
  return _name;
}

// Procedure: set_name
// Set the module name.
inline void_t VerilogModule::set_name(string_crt name) {
  _name = name;
}

// Function: num_instances
inline size_t VerilogModule::num_instances() const {
  return _instances_ptr->size();
}

// Function: num_outputs
inline size_t VerilogModule::num_outputs() const {
  return _outputs_ptr->size();
}

// Function: num_inputs
inline size_t VerilogModule::num_inputs() const {
  return _inputs_ptr->size();
}

// Function: num_wires
inline size_t VerilogModule::num_wires() const {
  return _wires_ptr->size();
}

// Function: instances_ptr
// Query the pointer to the instance dictionary.
inline verilog_instance_ptr_vpt VerilogModule::instances_ptr() const {
  return _instances_ptr;
}

// Function: inputs_ptr
// Query the pointer to the string vector of inputs.
inline verilog_input_ptr_vpt VerilogModule::inputs_ptr() const {
  return _inputs_ptr;
}

// Function: outputs_ptr
// Query the pointer to the string vector of outputs.
inline verilog_output_ptr_vpt VerilogModule::outputs_ptr() const {
  return _outputs_ptr;
}

// Function: wires_ptr
// Query the pointer to the string vector of wires.
inline verilog_wire_ptr_vpt VerilogModule::wires_ptr() const {
  return _wires_ptr;
}

// Class: VerilogInstance
// This class defines the instance in a verilog file.
class VerilogInstance {

  public:

    VerilogInstance();                                            // Constructor.
    ~VerilogInstance();                                           // Destructor.

    inline string_crt name() const;                               // Query instance name.
    inline string_crt cell_name() const;                          // Query the cell name.
    
    inline void_t set_name(string_crt);                           // Set the instance name.
    inline void_t set_cell_name(string_crt);                      // Set the cell name.
    inline void_t insert_cellpin_to_net(string_crt, string_crt);  // Insert a cellpin-net mapping.
    inline void_t insert_net_to_cellpin(string_crt, string_crt);  // Insert a net-cellpin mapping.

    inline size_t num_mappings() const;                           // Query the mapping count.
    inline string_to_string_map_pt cellpin_to_net_ptr();          // Query the mapper pointer.
    inline string_to_string_map_pt net_to_cellpin_ptr();          // Query the mapper pointer.

  private:

    string_t _name;                                               // Instance name.
    string_t _cell_name;                                          // Cell name.
    
    string_to_string_map_t _cellpin_to_net;                       // Mapping from cellpin to net.
    string_to_string_map_t _net_to_cellpin;                       // Mapping from net to cellpin.  
};

// Function: name
// Return the inst name.
inline string_crt VerilogInstance::name() const {
  return _name;
}

// Function: cell_name
// Return the cell name.
inline string_crt VerilogInstance::cell_name() const {
  return _cell_name;
}

// Procedure: set_name
// Set the inst name.
inline void_t VerilogInstance::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_cell_name
// Set the cell name.
inline void_t VerilogInstance::set_cell_name(string_crt name) {
  _cell_name = name;
}

// Function: num_mappings
// Return the number of mappings.
inline size_t VerilogInstance::num_mappings() const {
  return _cellpin_to_net.size();
}

// Function: cellpin_to_net_ptr
// Return the mapper pointer.
inline string_to_string_map_pt VerilogInstance::cellpin_to_net_ptr() {
  return &_cellpin_to_net;
}

// Function: net_to_cellpin_ptr
// Return the mapper pointer.
inline string_to_string_map_pt VerilogInstance::net_to_cellpin_ptr() {
  return &_net_to_cellpin;;
}

// Procedure: insert_cellpin_to_net
// Insert a mapping from a cell pin to a net.
inline void_t VerilogInstance::insert_cellpin_to_net(string_crt cellpin_name, string_crt net_name) {
  _cellpin_to_net[cellpin_name] = net_name;
  _net_to_cellpin[net_name] = cellpin_name;
}

// Procedure: insert_net_to_cellpin
// Insert a mapping from a cell pin to a net.
inline void_t VerilogInstance::insert_net_to_cellpin(string_crt net_name, string_crt cellpin_name) {
  _cellpin_to_net[cellpin_name] = net_name;
  _net_to_cellpin[net_name] = cellpin_name;
}

};

#endif

