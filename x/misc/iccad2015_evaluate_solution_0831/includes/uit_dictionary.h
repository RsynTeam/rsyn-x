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

#ifndef UIT_DICTIONARY_H_
#define UIT_DICTIONARY_H_

#include "uit_headerdef.h"
#include "uit_classdef.h"

namespace uit {

#define UIT_PRIME32_1 2654435761U
#define UIT_PRIME32_2 2246822519U
#define UIT_PRIME32_3 3266489917U
#define UIT_PRIME32_4  668265263U
#define UIT_PRIME32_5  374761393U

#define UIT_PRIME64_1 11400714785074694791ULL
#define UIT_PRIME64_2 14029467366897019727ULL
#define UIT_PRIME64_3  1609587929392839161ULL
#define UIT_PRIME64_4  9650029242287828579ULL
#define UIT_PRIME64_5  2870177450012600261ULL

#define UIT_HASH_SEED 196613

// Class: Dictionary
// K: key type
// T: obj type
// Dictionary class stores the mapping of an object's key (name) into its physical memory
// address location. This class will automatically create the memory for the given object
// and will automatically delete the memory by its destructor.
template <class K, class V>
class Dictionary {
  
  public:
    
    typedef typename unordered_map<K, V*, xxHash64>::iterator iter_t;

    Dictionary();                                         // Constructor.
    Dictionary(const size_t);                             // Constructor.
    ~Dictionary();                                        // Destructor.
    
    inline void clear();                                  // Clear the hash table.
    inline void rehash(const size_t);                     // Rehash the table.
    inline void remove(const K &);                        // Erase an item.
    inline size_t size() const;                           // Return the array size.

    inline iter_t begin();                                // Beg iterator region.
    inline iter_t end();                                  // End iterator region.

    inline V* insert(const K &);                          // Insert an new item.
    inline V* rehash(const K &, const K &);               // Rehash a key.
    inline void insert(const K &, const V*);              // Insert a given item.
    inline V* operator [] (const K &);                    // Operator [].
    inline const V* operator [] (const K &) const;        // Operator [].

  private:
    
    unordered_map <K, V*, xxHash64> _dictionary;          // Dictionary table.
};

// Constructor
template <class K, class V>
Dictionary<K, V>::Dictionary() {
  //_dictionary.set_empty_key("-1");
  //_dictionary.set_deleted_key("-2");
}

// Constructor
template <class K, class V>
Dictionary<K, V>::Dictionary(const size_t prime) {
  //_dictionary.set_empty_key("-1");
  //_dictionary.set_deleted_key("-2");
  rehash(prime);
}

// Destructor
template <class K, class V>
Dictionary<K, V>::~Dictionary() {
  clear();
}

// Procedure: clear
// Clear the object.
template <class K, class V>
inline void Dictionary<K, V>::clear() {
  for(auto itr=_dictionary.begin(); itr!=_dictionary.end(); ++itr) {
    delete itr->second;
  }
  _dictionary.clear();
}

// Procedure: rehash
// Rehash the dictionary.
template <class K, class V>
inline void Dictionary<K, V>::rehash(const size_t prime) {
  _dictionary.rehash(prime);
}

// Function: begin
// Return the beginning iterator of the dictionary.
template <class K, class V>
inline typename unordered_map<K, V*, xxHash64>::iterator Dictionary<K, V>::begin() {
  return _dictionary.begin();
}

// Function: end
// Return the ending iterator of the dictionary.
template <class K, class V>
inline typename unordered_map<K, V*, xxHash64>::iterator Dictionary<K, V>::end() {
  return _dictionary.end();
}

// Operator: []
template <class K, class V>
inline const V* Dictionary<K, V>::operator [] (const K &key) const {
  auto itr = _dictionary.find(key);
  return (itr == _dictionary.end()) ? NULL : itr->second;
}

// Operator: []
template <class K, class V>
inline V* Dictionary<K, V>::operator [] (const K &key) {
  auto itr = _dictionary.find(key);
  return (itr == _dictionary.end()) ? NULL : itr->second;
}

// Function: size
template <class K, class V>
inline size_t Dictionary<K, V>::size() const {
  return _dictionary.size();
}

// Function: insert
template <class K, class V>
inline void Dictionary<K, V>::insert(const K &key, const V* v_ptr) {
  auto itr = _dictionary.find(key);
  if(itr == _dictionary.end()) {
    _dictionary[key] = v_ptr;
  }
  else {
    delete itr->second;
    itr->second = v_ptr;
  }
  return v_ptr;
}

// Function: insert
template <class K, class V>
inline V* Dictionary<K, V>::insert(const K &key) {
  auto itr = _dictionary.find(key);
  if(itr == _dictionary.end()) {
    return _dictionary[key] = new V();
  }
  return itr->second;
}

// Procedure: remove
template <class K, class V>
inline void Dictionary<K, V>::remove(const K &key) {
  auto itr = _dictionary.find(key);
  if(itr != _dictionary.end()) {
    delete itr->second;
    _dictionary.erase(itr);
  }
}

// Function: rehash
// Rehash an item with a new key value. Notice that it is user's responsibility to change 
// the key value of the given object.
template <class K, class V>
inline V* Dictionary<K, V>::rehash(const K &old_key, const K &new_key) {
  auto itr = _dictionary.find(old_key);
  if(itr == _dictionary.end()) return insert(new_key);
  remove(new_key);
  V* v = _dictionary[new_key] = itr->second;
  _dictionary.erase(itr);
  return v;
}

//-------------------------------------------------------------------------------------------------

// Class: MurMurHash32
// MurmurHash3 was written by Austin Appleby, and is placed in the public domain. 
// The author already disclaims copyright to this source code.
class MurMurHash32 {

  public:

    unsigned long operator() (const string &) const;
    unsigned long operator() (const int) const;

};

//-------------------------------------------------------------------------------------------------

// Class: xxHash32 
//
//   xxHash - Extremely Fast Hash algorithm
//   Header File
//   Copyright (C) 2012-2015, Yann Collet.
//   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are
//   met:

//       * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//       * Redistributions in binary form must reproduce the above
//   copyright notice, this list of conditions and the following disclaimer
//   in the documentation and/or other materials provided with the
//   distribution.

//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   You can contact the author at :
//   - xxHash source repository : http://code.google.com/p/xxhash/
//   
//   The class wrapper is made by Tsung-Wei Huang.
class xxHash32 {
  
  public:
    unsigned int operator() (const string &) const;
};

//-------------------------------------------------------------------------------------------------

// Class: xxHash64
//
//   xxHash - Extremely Fast Hash algorithm
//   Header File
//   Copyright (C) 2012-2015, Yann Collet.
//   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are
//   met:

//       * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//       * Redistributions in binary form must reproduce the above
//   copyright notice, this list of conditions and the following disclaimer
//   in the documentation and/or other materials provided with the
//   distribution.

//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   You can contact the author at :
//   - xxHash source repository : http://code.google.com/p/xxhash/
//   
//   The class wrapper is made by Tsung-Wei Huang.
class xxHash64 {
  
  public:
    size_t operator() (const string &) const;
};

};    // End of UIT namespace.

#endif

