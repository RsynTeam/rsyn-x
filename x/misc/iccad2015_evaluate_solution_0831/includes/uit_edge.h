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

#ifndef UIT_EDGE_H_
#define UIT_EDGE_H_

#include "uit_utility.h"
#include "uit_datadef.h"
#include "uit_typedef.h"
#include "uit_classdef.h"
#include "uit_celllib.h"

namespace uit {

// Class: Edge
class Edge {

  public:

    Edge();                                                 // Constructor.
    ~Edge();                                                // Destructor.

    inline float_t delay(int, int, int) const;              // Query the delay.

    inline edge_type_e edge_type() const;                   // Query the edge type.
    inline timing_sense_e timing_sense() const;             // Query the timing sense.

    inline node_pt from_node_ptr() const;                   // Query the from-node pointer.
    inline node_pt to_node_ptr() const;                     // Query the to-node pointer.

    inline timing_arc_pt timing_arc_ptr(int) const;         // Query the timing-arc pointer.

    inline net_pt net_ptr() const;                          // Query the net pointer.

    inline edgelist_iter_t fanin_satellite() const;         // Query the fanin satellite.
    inline edgelist_iter_t fanout_satellite() const;        // Query the fanout satellite.
    inline edgelist_iter_t edgelist_satellite() const;      // Query the edgelist satellite. 
    
    inline void_t set_edge_type(edge_type_ce);              // Set the edge type.
    inline void_t set_timing_sense(timing_sense_ce);        // Set the timing sense.
    inline void_t set_delay(int, int, int, float_ct);       // Set the delay
    inline void_t set_from_node_ptr(node_pt);               // Set the from-node pointer.
    inline void_t set_to_node_ptr(node_pt);                 // Set the to-node pointer.
    inline void_t set_timing_arc_ptr(int, timing_arc_pt);   // Set the timing-arc pointer.
    inline void_t set_net_ptr(net_pt);                      // Set the net pointer (net arc).
    inline void_t set_fanin_satellite(edgelist_iter_t);     // Set the fanin satellite.
    inline void_t set_fanout_satellite(edgelist_iter_t);    // Set the fanout satellite.
    inline void_t set_edgelist_satellite(edgelist_iter_t);  // Set the edgelist satellite.

  private:

    edge_type_e _edge_type;                                 // Edge type.
    timing_sense_e _timing_sense;                           // Timing sense.

    float_t _delay[2][2][2];                                // Delay (el/irf/orf).

    net_pt _net_ptr;                                        // Net pointer (net arc).

    node_pt _from_node_ptr;                                 // From-node pointer.
    node_pt _to_node_ptr;                                   // To-node pointer.
    
    edgelist_iter_t _fanin_satellite;                       // Fanin satellite.
    edgelist_iter_t _fanout_satellite;                      // Fanout satellite.
    edgelist_iter_t _edgelist_satellite;                    // Edgelist satellite.

    timing_arc_pt _timing_arc_ptr[2];                       // Timing-arc pointer.
};

// Procedure: set_edge_type
inline void_t Edge::set_edge_type(edge_type_ce edge_type) {
  _edge_type = edge_type;
}

// Procedure: set_timing_sense
inline void_t Edge::set_timing_sense(timing_sense_ce timing_sense) {
  _timing_sense = timing_sense;
}

// Procedure: set_delay
inline void_t Edge::set_delay(int el, int irf, int orf, float_ct delay) {
  numeric_guard(_delay[el][irf][orf] = delay);
}

// Procedure: set_from_node_ptr
// Set the from node pointer.
inline void_t Edge::set_from_node_ptr(node_pt node_ptr) {
  _from_node_ptr = node_ptr;
}

// Procedure: set_to_node_ptr
// Set the to node pointer.
inline void_t Edge::set_to_node_ptr(node_pt node_ptr) {
  _to_node_ptr = node_ptr;
}

// Procedure: set_net_ptr
// Set the net pointer.
inline void_t Edge::set_net_ptr(net_pt net_ptr) {
  _net_ptr = net_ptr;
}

// Procedure: set_fanin_satellite
// Set the fanin satellite.
inline void_t Edge::set_fanin_satellite(edgelist_iter_t satellite) {
  _fanin_satellite = satellite;
}

// Procedure: set_fanout_satellite
// Set the fanout satellite.
inline void_t Edge::set_fanout_satellite(edgelist_iter_t satellite) {
  _fanout_satellite = satellite;
}

// Procedure: set_edgelist_satellite
// Set the satellite.
inline void_t Edge::set_edgelist_satellite(edgelist_iter_t satellite) {
  _edgelist_satellite = satellite;
}

// Procedure: set_timing_arc_ptr
// Set the timing arc pointer.
inline void_t Edge::set_timing_arc_ptr(int el, timing_arc_pt timing_arc_ptr) {
  _timing_arc_ptr[el] = timing_arc_ptr;
}

// Function: edge_type
// Return the edge type.
inline edge_type_e Edge::edge_type() const {
  return _edge_type;
}

// Function: timing_sense
// Return the timing sense.
inline timing_sense_e Edge::timing_sense() const {
  return _timing_sense;
}

// Function: delay
// Return the delay value.
inline float_t Edge::delay(int el, int irf, int orf) const {
  return _delay[el][irf][orf];
}

// Function: from_node_ptr
// Return the from node pointer.
inline node_pt Edge::from_node_ptr() const {
  return _from_node_ptr;
}

// Function: to_node_ptr
// Return the to node pointer.
inline node_pt Edge::to_node_ptr() const {
  return _to_node_ptr;
}

// Function: timing_arc_ptr
// Return the timing arc pointer.
inline timing_arc_pt Edge::timing_arc_ptr(int el) const {
  return _timing_arc_ptr[el];
}

// Function: net_ptr
// Return the net pointer.
inline net_pt Edge::net_ptr() const {
  return _net_ptr;
}

// Function: fanin_satellite
// Return the satellite on the fanin list.
inline edgelist_iter_t Edge::fanin_satellite() const {
  return _fanin_satellite;
}

// Function: fanout_satellite
// Return the satellite on the fanout list.
inline edgelist_iter_t Edge::fanout_satellite() const {
  return _fanout_satellite;
}

// Function: edgelist_satellite
// Return the satellite on the edgelist.
inline edgelist_iter_t Edge::edgelist_satellite() const {
  return _edgelist_satellite;
}

//-------------------------------------------------------------------------------------------------

class Jump{
  
  public:
    
    Jump();                                                           // Constructor.
    ~Jump();                                                          // Destructor.
    
    inline void_t set_idx(int_ct);                                    // Set the index.
    inline void_t set_from_node_ptr(node_pt);                         // Set the from node pointer.
    inline void_t set_to_node_ptr(node_pt);                           // Set the to node pointer.
    inline void_t set_delay(int, int, int, float_ct);                 // Set the delay value.
    inline void_t set_timing_sense(timing_sense_ce);                  // Set the timing sense.
    inline void_t set_jumpin_satellite(jumplist_iter_t);              // Set the satellite.
    inline void_t set_jumpout_satellite(jumplist_iter_t);             // Set the satellite.

    inline int_t idx() const;                                         // Query the index.
    inline node_pt from_node_ptr() const;                             // Query the from node pointer.
    inline node_pt to_node_ptr() const;                               // Query the to node pointer.
    inline timing_sense_e timing_sense() const;                       // Query the timing sense.
    inline float_t delay(int, int, int) const;                        // Query the delay.
    inline jumplist_iter_t jumpin_satellite() const;                  // Query the in satellite.
    inline jumplist_iter_t jumpout_satellite() const;                 // Query the out satellite.

  private:

    int_t _idx;                                                       // Index.

    float_t _delay[2][2][2];                                          // Delay (el/irf/orf).

    timing_sense_e _timing_sense;                                     // Timing sense.

    node_pt _from_node_ptr;                                           // From node pointer.
    node_pt _to_node_ptr;                                             // To node pointr.

    jumplist_iter_t _jumpin_satellite;                                // Jumpin satellite.
    jumplist_iter_t _jumpout_satellite;                               // Jumpout satellite.
};

// Procedure: set_from_node_ptr
inline void_t Jump::set_from_node_ptr(node_pt node_ptr) {
  _from_node_ptr = node_ptr;
}

// Procedure: set_to_node_ptr
inline void_t Jump::set_to_node_ptr(node_pt node_ptr) {
  _to_node_ptr = node_ptr;
}

// Procedure: set_delay
inline void_t Jump::set_delay(int el, int irf, int orf, float_ct delay) {
  _delay[el][irf][orf] = delay;
}

// Procedure: set_timing_sense
inline void_t Jump::set_timing_sense(timing_sense_ce sense) {
  #ifndef NDEBUG
  assert(sense == POSITIVE_UNATE || sense == NEGATIVE_UNATE);
  #endif
  _timing_sense = sense;
}

// Procedure: set_jumpin_satellite
inline void_t Jump::set_jumpin_satellite(jumplist_iter_t satellite) {
  _jumpin_satellite = satellite;
}

// Procedure: set_jumpout_satellite
inline void_t Jump::set_jumpout_satellite(jumplist_iter_t satellite) {
  _jumpout_satellite = satellite;
}

// Procedure: set_idx
inline void_t Jump::set_idx(int_ct idx) {
  _idx = idx;
}

// Function: from_node_ptr
inline node_pt Jump::from_node_ptr() const {
  return _from_node_ptr;
}

// Function: to_node_ptr
inline node_pt Jump::to_node_ptr() const {
  return _to_node_ptr;
}

// Function: timing_sense
inline timing_sense_e Jump::timing_sense() const {
  return _timing_sense;
}

// Function: delay
inline float_t Jump::delay(int el, int irf, int orf) const {
  return _delay[el][irf][orf];
}

// Function: idx
inline int_t Jump::idx() const {
  return _idx;
}

// Function: jumpin_satellite
inline jumplist_iter_t Jump::jumpin_satellite() const {
  return _jumpin_satellite;
}

// Function: jumpout_satellite
inline jumplist_iter_t Jump::jumpout_satellite() const {
  return _jumpout_satellite;
}


};

#endif
