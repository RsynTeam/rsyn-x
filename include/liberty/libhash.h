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


#ifndef LIBERTY_HASH_LIB_
#define LIBERTY_HASH_LIB_

struct liberty_hash_bucket
{
		struct liberty_hash_bucket *next;
		struct liberty_hash_bucket *all_next;
		struct liberty_hash_bucket *all_prev;
		char *name;
		si2drObjectIdT item;
};

typedef struct liberty_hash_bucket liberty_hash_bucket;

struct liberty_hash_table
{
		liberty_hash_bucket **table; /* a ptr to an array of ptrs */
		int *counts;
		liberty_hash_bucket *all_list;
		liberty_hash_bucket *all_last;
		int auto_resize;
		int threshold;
		int size;
		int longest_bucket_string;
		int entry_count;
		int case_insensitive;
};

typedef struct liberty_hash_table liberty_hash_table;



liberty_hash_table *liberty_hash_create_hash_table(int numels, int auto_resize, int case_insensitive);

int liberty_hash_get_next_higher_prime(int prime);

void liberty_hash_destroy_hash_table(liberty_hash_table *ht);

void liberty_hash_resize_hash_table(liberty_hash_table *ht, int new_size);

int liberty_hash_enter_oid(liberty_hash_table *ht, char *name, si2drObjectIdT oid);
 
void liberty_hash_delete_elem(liberty_hash_table *ht, char *name);
 
void liberty_hash_lookup(liberty_hash_table *ht, char *name, si2drObjectIdT *oid);

void dump_attr_hash(liberty_hash_table *ht, FILE *outc, FILE *outh);

void dump_group_hash(liberty_hash_table *ht, FILE *outc, FILE *outh);

char *make_rep(char *prefix, void *x);

#endif
