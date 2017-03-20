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

#ifndef UIT_TIMER_H_
#define UIT_TIMER_H_

#include "uit_circuit.h"
#include "uit_node.h"
#include "uit_edge.h"
#include "uit_celllib.h"
#include "uit_verilog.h"
#include "uit_iterator.h"
#include "uit_command.h"
#include "uit_cppr.h"
#include "uit_debug.h"
#include "uit_spef.h"
#include "uit_pipeline.h"
#include "uit_functor.h"

namespace uit {

// Class: Timer
class Timer {

  public:

    Timer();                                                            // Constructor.
    ~Timer();                                                           // Destructor.
    
    inline void_t set_name(string_crt);                                 // Set the timer's name.
    inline void_t set_num_threads(int_ct);                              // Set the number of threads.

    net_pt insert_net(string_crt);                                      // Create a new net.
    instance_pt insert_instance(string_crt, string_crt);                // Create a new instance.
    primary_input_pt insert_primary_input(string_crt);                  // Create a new input port.
    primary_output_pt insert_primary_output(string_crt);                // Create a new output port.

    void_t init_tau2015_timer(int argc, char *argv[]);                  // Initialize the timer.
    void_t init_iccad2015_timer(int argc, char *argv[]);                // Initialize the timer.
    void_t report_tau2015(int argc, char *argv[]);                      // Main procedure of tau2015.
    void_t report_timer();                                              // Report the timer.
    void_t report_worst_paths(string_crt, size_t);                      // Report the worst paths.
    void_t report_worst_paths(pin_pt, size_t);                          // Report the worst paths.
    void_t report_worst_paths(string_crt, size_t, path_trace_vrt);      // Report the worst paths.
    void_t report_worst_paths(pin_pt, size_t, path_trace_vrt);          // Report the worst paths.
    void_t report_worst_testpoints(string_crt, size_t, testpoint_vrt);  // Report the worst testpoints.
    void_t report_worst_testpoints(pin_pt, size_t, testpoint_vrt);      // Report the worst testpoints.
    void_t report_all_slacks();                                         // Report all slacks.
    void_t read_spef(string_crt);                                       // Read a spef file.
    void_t repower_instance(string_crt, string_crt);                    // Resize an exisiting gate (instance).
    void_t remove_net(string_crt);                                      // Remove an exisiting net.
    void_t remove_instance(string_crt);                                 // Remove an exisiting gate (instance).
    void_t remove_primary_input(string_crt);                            // Remove the PI port.
    void_t disconnect_pin(string_crt);                                  // Disconnect the pin from its net.
    void_t disconnect_pin(pin_pt);                                      // Disconnect the pin from its net.
    void_t connect_pin(string_crt, string_crt);                         // Connect the pin to the net.
    void_t connect_pin(string_crt, net_pt);                             // Connect the pin to the net.
    void_t connect_pin(pin_pt, net_pt);                                 // Connect the pin to the net.
    void_t splice_driver(string_crt, string_crt, string_crt);           // Splice the driver.
    void_t update_timing(bool_t is_incremental = true);                 // Update the timing.
    void_t set_slew(string_crt, int, int, float_ct);                    // Assert the slew.
    void_t set_slew(char_cpt, int, int, float_ct);                      // Assert the slew.
    void_t set_slew(pin_pt, int, int, float_ct);                        // Assert the slew.
    void_t set_at(string_crt, int, int, float_ct);                      // Assert the at.
    void_t set_at(char_cpt, int, int, float_ct);                        // Assert the at.
    void_t set_at(pin_pt, int, int, float_ct);                          // Assert the at.

    float_t report_at(string_crt, int, int);                            // Report the at.
    float_t report_at(pin_pt, int, int);                                // Report the at.
    float_t report_rat(string_crt, int, int);                           // Report the rat.
    float_t report_rat(pin_pt, int, int);                               // Report the rat.
    float_t report_slack(string_crt, int, int);                         // Report the slack.
    float_t report_slack(pin_pt, int, int);                             // Report the slack.
    float_t report_TNS(int, int);                                       // Report the TNS.
    float_t report_TNS();                                               // Report the TNS.
    float_t report_WNS(int, int);                                       // Report the WNS.
    float_t report_WNS();                                               // Report the WNS.

    inline string_crt name() const;                                     // Query the timer name.
    inline celllib_pt celllib_ptr(int) const;                           // Query the celllib pointer.
    inline circuit_pt circuit_ptr() const;                              // Query the circuit pointer.
    inline command_pt command_ptr() const;                              // Query the command pointer.
    
    size_t num_pins() const;                                            // Query the pin count.
    size_t num_nets() const;                                            // Query the net count.
    size_t num_nodes() const;                                           // Query the node count.
    size_t num_edges() const;                                           // Query the edge count.
    size_t num_tests() const;                                           // Query the test count.
    size_t num_jumps() const;                                           // Query the jump count.
    size_t num_instances() const;                                       // Query the instance count.
    size_t num_primary_inputs() const;                                  // Query the number of primary inputs.
    size_t num_primary_outputs() const;                                 // Query the number of primary outputs.
    
    inline nodeset_rt nodeset() const;                                  // Query the nodeset reference.
    inline nodeset_pt nodeset_ptr() const;                              // Query the nodeset pointer.
    inline jumpset_rt jumpset() const;                                  // Query the jumpset reference.
    inline jumpset_pt jumpset_ptr() const;                              // Query the jumpset pointer.
    inline edgelist_pt edgelist_ptr() const;                            // Query the edgelist pointer.
    inline pipeline_rt pipeline() const;                                // Query the pipeline reference.
    inline pipeline_pt pipeline_ptr() const;                            // Query the pipeline pointer.
    
  private:

    string_t _name;                                                     // Timer name.

    command_pt _command_ptr;                                            // Command object.

    float_t _TNS[2][2];                                                 // Total negative slack.
    float_t _WNS[2][2];                                                 // Worst negative slack.
    bool_t _is_tns_updated[2][2];                                       // TNS update flag.
    bool_t _is_wns_updated[2][2];                                       // WNS update flag.

    file_pt _ofp;                                                       // Output file pointer.

    pipeline_pt _pipeline_ptr;                                          // Pipeline pointer.

    celllib_pt _celllib_ptr[2];                                         // Early/Late celllib pointer.
    circuit_pt _circuit_ptr;                                            // Circuit object pointer.
     
    nodeset_pt _nodeset_ptr;                                            // Nodeset.
    jumpset_pt _jumpset_ptr;                                            // Jumpset.
    edgelist_pt _edgelist_ptr;                                          // Edgelist.

    bool_t _is_test_affected(test_pt) const;                            // Query if the test is affected. 
    bool_t _is_test_unaffected(test_pt) const;                          // Query if the test is unaffected.

    node_pt _insert_node(pin_pt);                                       // Insert a node.
    node_pt _clock_tree_root_node_ptr() const;                          // Return the clock tree root node.

    edge_pt _insert_edge(string_crt, string_crt, net_pt);               // Insert an edge (net arc).
    edge_pt _insert_edge(node_pt, node_pt, net_pt);                     // Insert an edge (net arc).
    edge_pt _insert_edge(pin_pt, pin_pt, net_pt);                       // Insert an edge (net arc).
    edge_pt _insert_edge(string_crt, string_crt, timing_arc_pt[]);      // Insert an edge (cell arc).
    edge_pt _insert_edge(pin_pt, pin_pt, timing_arc_pt[]);              // Insert an edge (cell arc).
    edge_pt _insert_edge(node_pt, node_pt, timing_arc_pt[]);            // Insert an edge (cell arc).
    edge_pt _insert_edge(string_crt, string_crt, test_pt);              // Insert an edge (cell arc).
    edge_pt _insert_edge(pin_pt, pin_pt, test_pt);                      // Insert an edge (cell arc);
    edge_pt _insert_edge(node_pt, node_pt, test_pt);                    // Insert an edge (cell arc);
    edge_pt _find_edge_ptr(string_crt, string_crt);                     // Find an edge.
    edge_pt _find_edge_ptr(pin_pt, pin_pt);                             // Find an edge.
    edge_pt _find_edge_ptr(node_pt, node_pt);                           // Find an edge.
    
    jump_pt _insert_jump(pin_pt, pin_pt, int_t, float_t [][2]);         // Insert a jump.
    jump_pt _insert_jump(node_pt, node_pt, int_t, float_t [][2]);       // Insert a jump.

    void_t _parse_verilog(string_crt, verilog_pt);                      // Parse the verilog.
    void_t _parse_verilog(char_cpt, verilog_pt);                        // Parse the verilog.
    void_t _parse_spef(string_crt, spef_pt);                            // Parse the spef file.
    void_t _parse_spef(char_cpt, spef_pt);                              // Parse the spef file.
    void_t _parse_celllib(string_crt, celllib_pt);                      // Parse the library.
    void_t _parse_celllib(char_cpt, celllib_pt);                        // Parse the library.
    void_t _update_TNS();                                               // Update the TNS.
    void_t _update_TNS(int, int);                                       // Update the TNS.
    void_t _update_WNS();                                               // Update the WNS.
    void_t _update_WNS(int, int);                                       // Update the WNS.
    void_t _update_spef(spef_pt);                                       // Update the spef object.
    void_t _update_forward_propagate_pipeline();                        // Update the forward pipeline.
    void_t _update_affected_tests();                                    // Update the affected tests.
    void_t _update_testpoint_slacks(pin_pt, testpoint_vrt);             // Update the testpoint slacks.
    void_t _update_clock_tree();                                        // Update the clock tree.
    void_t _update_node_level(node_pt, int_t) const;                    // Update the node level.
    void_t _update_all_node_levels() const;                             // Update all node levels.
    void_t _obtain_fanout_test_ptrs(pin_pt, test_ptr_vrt);              // Update the pin fanout tests.
    void_t _obtain_testpoints(test_ptr_vrt, testpoint_vrt);             // Update the testpoints.
    void_t _induce_jump(node_pt, node_pt, int_t, float_t dv[][2]);      // Induce the jump
    void_t _init_timer();                                               // Initialize the timer.
    void_t _init_circuit_from_verilog(verilog_pt);                      // Initialize circuit connection.
    void_t _init_io_timing(string_crt);                                 // Initialize the io timing.
    void_t _init_full_timing_update();                                  // Initialize the full timing update.
    void_t _enable_lazy_evaluation();                                   // Enable the lazy evaluation.
    void_t _clear_edgelist();                                           // Clear the edgelist.
    void_t _remap_cellpin(int, pin_pt, cellpin_pt);                     // Remap the cellpin.
    void_t _remove_node(node_pt);                                       // Remove the node. 
    void_t _remove_edge(edge_pt);                                       // Remove the edge.
    void_t _remove_jump(node_pt);                                       // Remove the jump.
    void_t _remove_jump(jump_pt);                                       // Remove the jump.
    void_t _insert_frontier(node_pt) const;                             // Insert a frontier node.
    void_t _remove_frontier(node_pt) const;                             // Remove a frontier node.
    void_t _remove_all_frontiers();                                     // Clear the pipeline.
    void_t _forward_propagate_fanout(node_pt);                          // Forward propagate fanout.
    void_t _forward_propagate_fanout(int_t);                            // Forward propagate fanout.
    void_t _forward_propagate_at(node_pt);                              // Forward propagate at.
    void_t _forward_propagate_at(int_t);                                // Forward propagate at.
    void_t _forward_propagate_slew(node_pt);                            // Forward propagate slew.
    void_t _forward_propagate_slew(int_t);                              // Forward propagate slew.
    void_t _forward_propagate_delay(node_pt);                           // Forward propagate delay.
    void_t _forward_propagate_delay(int_t);                             // Forward propagate delay.
    void_t _forward_propagate_rc_timing(int_t);                         // Forward propagate the rc delay.
    void_t _forward_propagate_rc_timing(node_pt);                       // Forward propagate the rc delay.
    void_t _forward_propagate_jump(int_t);                              // Forward propagate the jump.
    void_t _forward_propagate_jump(node_pt);                            // Forward propagate the jump.
    void_t _forward_propagate_test(int_t);                              // Forward propagate the test.
    void_t _forward_propagate_test(test_pt);                            // Forward propagate the test.
    void_t _backward_propagate_fanin(node_pt);                          // Backward propagate the level.
    void_t _backward_propagate_fanin(int_t);                            // Backward propagate the level.
    void_t _backward_propagate_rat(node_pt);                            // Backward propagate the rat.
    void_t _backward_propagate_rat(int_t);                              // Backward propagate the rat.
};

// Function: name
// Return the timer's name
inline string_crt Timer::name() const {
  return _name;
}

// Procedure: set_name
// Set the timer's name.
inline void_t Timer::set_name(string_crt name) {
  _name = name;
}

// Procedure: set_num_threads
inline void_t Timer::set_num_threads(int_ct num_threads) {
  omp_set_num_threads(min(num_threads, UIT_MAX_NUM_THREADS));
}

// Function: command_ptr
// Return the pointer to the command object.
inline command_pt Timer::command_ptr() const {
  return _command_ptr;
}

// Function: celllib_ptr
// Return the pointer to the celllib.
inline celllib_pt Timer::celllib_ptr(int el) const {
  return _celllib_ptr[el];
}

// Function: circuit_ptr
// Return the circuit pointer.
inline circuit_pt Timer::circuit_ptr() const {
  return _circuit_ptr;
}

// Function: nodeset_ptr
inline nodeset_pt Timer::nodeset_ptr() const {
  return _nodeset_ptr;
}

// Function: nodeset
inline nodeset_rt Timer::nodeset() const {
  return *_nodeset_ptr;
}

// Function: jumpset_ptr
inline jumpset_pt Timer::jumpset_ptr() const {
  return _jumpset_ptr;
}

// Function: jumpset
inline jumpset_rt Timer::jumpset() const {
  return *_jumpset_ptr;
}

// Function: edgelist_ptr
// Return the pointer to the edgelist.
inline edgelist_pt Timer::edgelist_ptr() const {
  return _edgelist_ptr;
}

// Function: pipeline_ptr
// Return the pointer to the pipeline.
inline pipeline_pt Timer::pipeline_ptr() const {
  return _pipeline_ptr;
}

// Function: pipeline
// Return the reference to the pipeline.
inline pipeline_rt Timer::pipeline() const {
  return *_pipeline_ptr;
}



};

#endif
