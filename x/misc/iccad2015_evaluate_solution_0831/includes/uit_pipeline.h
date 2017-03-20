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

#ifndef UIT_PIPELINE_H_
#define UIT_PIPELINE_H_

#include "uit_circuit.h"
#include "uit_node.h"
#include "uit_edge.h"
#include "uit_celllib.h"
#include "uit_iterator.h"

namespace uit {
  
// Class: Pipeline
class Pipeline {
  
  public:

    Pipeline();                                             // Constructor.
    ~Pipeline();                                            // Destructor.
    
    inline size_t num_nodes() const;                        // Query the number of nodes.
    inline size_t num_levels() const;                       // Query the level of pipeline.

    void_t remove_all_nodes();                              // Remove all nodes from the pipeline.
    void_t resize(size_t);                                  // Resize the level of pipeline.
    void_t remove(node_pt);                                 // Remove the node from the pipeline.
    void_t insert(node_pt);                                 // Insert a node into the pipeline.
    void_t node_ptrs(int_t, node_ptr_vrt);                  // Obtain the node pointers.

    bool_t empty();                                         // Query the pipeline size.
    bool_t empty(int_t);                                    // Query the pipeline size in a level.

    inline int_t min_level() const;                         // Query the leftmost non-empty level.
    inline int_t max_level() const;                         // Query the rightmost non-empty level.

    nodelist_pt nodelist_ptr(int_t);                        // Query the nodelist in a given level.

    inline node_bucketlist_pt bucketlist_ptr() const;       // Query the pointer to the bucketlist.
    inline node_bucketlist_rt bucketlist();                 // Query the reference to the bucketlist.

  private:

    node_bucketlist_pt _bucketlist_ptr;                     // Pointer to the pipeline bucketlist.

    size_t _num_nodes;                                      // Number of nodes.

    int_t _min_level;                                       // Minimum nonempty level.
    int_t _max_level;                                       // Maximum nonempty level.
};

// Function: num_levels
// Query the level.
inline size_t Pipeline::num_levels() const {
  return _bucketlist_ptr->size();
}

// Function: num_nodes
// Query the number of nodes.
inline size_t Pipeline::num_nodes() const {
  return _num_nodes;
}

// Function: bucketlist_ptr
inline node_bucketlist_pt Pipeline::bucketlist_ptr() const {
  return _bucketlist_ptr;
}

// Function: bucketlist
inline node_bucketlist_rt Pipeline::bucketlist() {
  return *_bucketlist_ptr;
}

// Function: min_level
inline int_t Pipeline::min_level() const {
  return _min_level;
}

// Function: max_level
inline int_t Pipeline::max_level() const {
  return _max_level;
}


};

#endif



