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
#include "si2dr_liberty.h"


#ifndef LIBERTY_STRTAB_LIB_
#define LIBERTY_STRTAB_LIB_

struct liberty_strtable_chunk
{
	char *strtab;
	struct liberty_strtable_chunk *next;
	int strtab_allocated;
	int strtab_used;
};

typedef struct liberty_strtable_chunk liberty_strtable_chunk;


struct liberty_strtable
{
	liberty_hash_table *hashtab; /* a ptr to an array of ptrs */
	liberty_strtable_chunk *chunklist;
};

typedef struct liberty_strtable liberty_strtable;


liberty_strtable *liberty_strtable_create_strtable(int numels, int strsize, int case_insensitive);

void liberty_strtable_destroy_strtable(liberty_strtable *ht);

void liberty_strtable_resize_strtable(liberty_strtable *ht, int new_size);

char *liberty_strtable_enter_string(liberty_strtable *ht, char *str);
 
/* there's no element delete here, because it wouldn't be very useful */

void liberty_strtable_lookup(liberty_strtable *ht, char *str, char **result);
void print_strtab_stats(void);

#endif
