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

#ifndef UIT_LIST_H
#define UIT_LIST_H_

#include "uit_headerdef.h"

namespace uit {

// Class: ListNode
// This class defines the structure of the list node.
template <class T>
class ListNode
{
  public:
    
    inline T &item();                                       // Query the item.
    inline ListNode *next() const;                          // Query the next item.
    inline ListNode *prev() const;                          // Query the previous item.
    inline void set_next(ListNode*);                        // Set the next pointer.
    inline void set_prev(ListNode*);                        // Set the previous pointer.

    ListNode(const T &t): _item(t), _next(NULL), _prev(NULL) {}

    T _item;                                                // _item.
    ListNode *_next;                                        // Pointer to the next item.
    ListNode *_prev;                                        // Pointer to the prev item.  
};

// Function: item
// Return the reference to the item.
template <class T>
inline T& ListNode<T>::item() {
  return _item;
}

// Function: next
// Return the next pointer.
template <class T>
inline ListNode<T>* ListNode<T>::next() const {
  return _next;
}

// Function: prev
// Return the prev pointer.
template <class T>
inline ListNode<T>* ListNode<T>::prev() const {
  return _prev;
}

// Procedure: set_next
template <class T>
inline void ListNode<T>::set_next(ListNode<T>* next) {
  _next = next;
}

// Procedure: set_prev
template <class T>
inline void ListNode<T>::set_prev(ListNode<T>* prev) {
  _prev = prev;
}

// Class: List
// This class defines the general structure of the list.
template <class T>
class List
{
  public:

    List();
    ~List();

    void clear();
    void pop_back();
    void pop_front();

    void remove(ListNode<T> *);
    ListNode<T>* push_front(const T &);
    ListNode<T>* push_back (const T &);
        
    inline size_t size() const;
    inline bool empty() const;
    inline ListNode <T> *head() const;
    inline ListNode <T> *tail() const;

  private:

    ListNode <T> *_head;
    ListNode <T> *_tail;
        
    size_t _size;  
};

// Procedure: List
// Constructor of the class.
template<class T>
List<T>::List():
  _head(NULL),
  _tail(NULL),
  _size(0)
{
}

// Procedure: List
// Destructor of the class.
template<class T>
List<T>::~List() {
  clear();
}

// Procedure: clear
// Clear the list.
template<class T>
void List<T>::clear() {
  while(!empty()) pop_back();
}

// Function: size
// Return the number of items in the list.
template<class T>
inline size_t List<T>::size() const {
  return _size;
}

// Function: empty
// Return true for empty list, otherwise false.
template<class T>
inline bool List<T>::empty() const {
  return !_size;
}

// Function: head
// Return the head pointer of the list.
template<class T>
inline ListNode <T>* List<T>::head() const {
  return _head;
}

// Function: tail
// Return the tail pointer of the list.
template<class T>
inline ListNode <T>* List<T>::tail() const {
  return _tail;
}

// Function: push_back
// Inserting a new item from the back of list, and returning the address of the
// corresponding list node.
template<class T>
ListNode <T>* List<T>::push_back(const T &item) {
  ListNode <T> *node_ptr = new ListNode <T> (item); 
  if(empty()) {
    _head = node_ptr;
    _tail = node_ptr;
  }
  else {
    _tail->set_next(node_ptr);
    node_ptr->set_prev(_tail);
    _tail = node_ptr;
  }
  ++_size;
  return node_ptr;
}

// Function: push_front
// Inserting a new item from the front of the list, then returning the address
// of the list node containing the item.
template<class T>
ListNode <T>* List<T>::push_front(const T &item) {
  ListNode <T> *node_ptr = new ListNode <T> (item);
  if(empty())
  {
    set_head(node_ptr);
    set_tail(node_ptr);
  }
  else
  {
    _head->set_prev(node_ptr);
    node_ptr->set_next(_head);
    set_head(node_ptr);
  }
  ++_size;
  return node_ptr;
}


// Procedure: pop_back
// Pop out a list node from the back with clearing its memory.
template<class T>
void List<T>::pop_back() {
  if(empty()) return;
  ListNode <T> *ptr = _tail;    
  _tail = _tail->prev();
  if(_tail) _tail->set_next(NULL);
  else _head = NULL;
  delete ptr;
  --_size;
}

// Procedure: pop_front
// Pop out a list node from the beginning with clearing its memory.
template<class T>
void List<T>::pop_front() {
  if(empty()) return;
  ListNode <T> *tmp = _head;
  _head = _head->next();
  if(_head) _head->set_prev(NULL);
  else _tail = NULL;
  delete tmp;
  --_size;
}

// Procedure: remove
// Erase a list node and clear its memory.
template<class T>
void List<T>::remove(ListNode<T> *node_ptr) {
  
  // Unchecked, for the case node_ptr not in List.
  #ifndef NDEBUG
  assert(node_ptr != NULL);
  #endif
  
  if(node_ptr == _head) pop_front();
  else if(node_ptr==_tail) pop_back();
  else {
    node_ptr->prev()->set_next(node_ptr->next());
    node_ptr->next()->set_prev(node_ptr->prev());
    delete node_ptr;
    --_size;
  }
}

// Class: IndexedSet
template <class T>
class IndexedSet {
  
  public:

    typedef typename vector < T* >::iterator iter_t;

    IndexedSet();
    ~IndexedSet();
    
    inline size_t size() const;
    inline size_t num_indices() const;

    void remove(size_t);

    size_t insert();

    inline iter_t begin();
    inline iter_t end();

    inline vector < T* > &data();
    
    inline T* operator [] (const size_t);
    
  private:

    size_t _size;

    vector < T* > _data;
    vector < size_t > _recycled_indices;
};  

// Constructor.
template <class T>
IndexedSet<T>::IndexedSet() {
  _size = 0;
}

// Destructor
template <class T>
IndexedSet<T>::~IndexedSet() {
  for(size_t i=0; i<_data.size(); i++) {
    if(_data[i]) {
      delete _data[i];
    }
  }
}

// Function: begin
template <class T>
inline typename vector <T*>::iterator IndexedSet<T>::begin() {
  return _data.begin();
}

// Function: end
template <class T>
inline typename vector <T*>::iterator IndexedSet<T>::end() {
  return _data.end();
}

// Function: data
template <class T>
inline vector < T* > & IndexedSet<T>::data() {
  return _data;
}

// Operator
template <class T>
inline T* IndexedSet<T>::operator [] (const size_t idx) {
  return idx < num_indices() ? _data[idx] : NULL;
}

// Function: num_indices
template <class T>
inline size_t IndexedSet<T>::num_indices() const {
  return _data.size();
}

// Function: size
template <class T>
inline size_t IndexedSet<T>::size() const {
  return _size;
}

// Procedure: remove
// Remove an item that is specified by the idx.
template <class T>
void IndexedSet<T>::remove(size_t idx) {

  #ifndef NDEBUG
  assert(idx < num_indices());
  #endif

  if(_data[idx] != NULL) {
    delete _data[idx];
    _data[idx] = NULL;
    --_size;
    _recycled_indices.push_back(idx);
  }
}

// Procedure: insert
// Insert a new item.
template <class T>
size_t IndexedSet<T>::insert() {

  size_t idx;

  if(!_recycled_indices.empty()) {
    idx = _recycled_indices.back();
    _recycled_indices.pop_back();

    #ifndef NDEBUG
    assert(idx < num_indices() && _data[idx] == NULL);
    #endif

    _data[idx] = new T();
  }
  else {
    idx = _data.size();
    _data.push_back(new T());
  }

  ++_size;

  return idx;
}


};

#endif


