/******************************************************************************
    Copyright (c) 1996-2005 Synopsys, Inc.    ALL RIGHTS RESERVED

  The contents of this file are subject to the restrictions and limitations
  set forth in the SYNOPSYS Open Source License Version 1.0  (the "License"); 
  you may not use this file except in compliance with such restrictions 
  and limitations. You may obtain instructions on how to receive a copy of 
  the License at

  http://www.synopsys.com/partners/tapin/tapinprogram.html. 

  Software distributed by Original Contributor under the License is 
  distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
  expressed or implied. See the License for the specific language governing 
  rights and limitations under the License.

******************************************************************************/
#ifdef DMALLOC

#define my_malloc(size) dmalloc_malloc(__FILE__, __LINE__, (size), DMALLOC_FUNC_MALLOC, 0, 0)
#define my_free(ptr)    dmalloc_free(__FILE__, __LINE__, (ptr), DMALLOC_FUNC_FREE)
#define my_calloc(count,size)  dmalloc_malloc(__FILE__, __LINE__, (count)*(size), DMALLOC_FUNC_CALLOC, 0, 0)

#else

void *my_malloc(int size);

void my_free(void *ptr);

void *my_calloc(int size, int numels);

#endif
void *my_free_all(void);

void my_malloc_print_totals(void);

