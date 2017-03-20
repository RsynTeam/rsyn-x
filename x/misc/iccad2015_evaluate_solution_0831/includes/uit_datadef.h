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

#ifndef UIT_DATADEF_H_
#define UIT_DATADEF_H_

#include "uit_typedef.h"

namespace uit{

// Class: Data
// This class is the basic definition of the data that are used through the ui-timer.
// The data is stored in "_data" and the flag "_is_set" is used to check if the data
// is valid in current usage.
template <class T>
class Data {
  
  public:
    
    Data();                                                 // Constructor.
    ~Data();                                                // Destructor.
    
    //inline T & data();                                      // Query the data.
    inline const T &data() const;                           // Query the data.
    inline bool_t is_set() const;                           // Query if the data is set.
    inline void set(const T&);                              // Set the data.
    inline void set();                                      // Set the data.
    inline void unset(const T&);                            // Unset/clear the data.
    inline void unset();                                    // Unset/clear the data.

  private:
    
    T _data;                                                // Data storage.
    bool_t _is_set;                                         // Set/unset flag.
};

// Constructor
template <class T>
Data<T>::Data() {
  _is_set = false;
}

// Destructor.
template <class T>
Data<T>::~Data() {
}

/*// Function: data
template <class T>
inline T & Data<T>::data() {
  return _data;
}*/

// Function: data
template <class T>
inline const T & Data<T>::data() const{
  return _data;
}

// Function: is_set
// Return true if the data is valid or false otherwise.
template <class T>
inline bool_t Data<T>::is_set () const {
  return _is_set;
}

// Function: set
// Set the data and flag.
template <class T>
inline void Data<T>::set (const T &data) {
  _data = data;
  _is_set = true;
}

// Function: set
// Set the data and flag.
template <class T>
inline void Data<T>::set () {
  _is_set = true;
}

// Function: unset
// Clear the data.
template <class T>
inline void Data<T>::unset (const T &data) {
  _data = data;
  _is_set = false;
}

// Function: unset
// Clear the data.
template <class T>
inline void Data<T>::unset () {
  _is_set = false;
}



};

#endif

