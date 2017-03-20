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


#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

static int my_malloc_totals = 0;
static int  calls_calloc = 0, calls_malloc = 0;

#ifdef EXPERIMENTAL

#define SLOTS 200
#define BYTES_PER_SLOT 1048576

static void *storage[SLOTS]; /* a limit of 200Mbytes of storage */
static int  storage_allocd[SLOTS];
static int  storage_left[SLOTS];
static int  storage_at = 0;


static void *my_get_mem(int size)
{
	void * ret;
	
	if( size > 1048576 )
	{
		printf("Sorry, I can't give you more than 1Mb at a time using this mechanism!\n");
		exit(1);
	}
	
	if( storage_allocd[storage_at] == 0 )
	{
		printf("allocing 1M of space initially\n");
		
		storage[storage_at] = (void*)calloc(BYTES_PER_SLOT,1);
		storage_allocd[storage_at] = BYTES_PER_SLOT;
		storage_left[storage_at] = BYTES_PER_SLOT;
	}

	if( storage_left[storage_at] >= size )
	{
		ret = storage[storage_at]+( storage_allocd[storage_at]-storage_left[storage_at]);
		
		storage_left[storage_at] -= size;
		return ret;
	}
	else
	{
		printf("Allocing another 1M of space\n");

		storage_at++;
		storage[storage_at] = (void*)calloc(BYTES_PER_SLOT,1);
		storage_allocd[storage_at] = BYTES_PER_SLOT;
		storage_left[storage_at] = BYTES_PER_SLOT;
		
		ret = storage[storage_at]+( storage_allocd[storage_at]-storage_left[storage_at]);
		
		storage_left[storage_at] -= size;
		return ret;
	}
}

#endif

void *my_malloc(int size)
{
	void *ret;
	
#ifdef EXPERIMENTAL
	int size2 = (((size)+7)&0xfffffff8);
				 
	return my_get_mem(size2);
#endif
	calls_malloc++;
	my_malloc_totals +=	size;
	ret = (void*)malloc(size);
	if( !ret )
	{
		printf("Failure to fetch %d bytes of memory!\n", size);
	}
	return ret;
}

void my_free(void *ptr)
{
	/* do all the freeing in one big burp later */
	free(ptr);
}


void *my_calloc(int size, int numels)
{
	void *ret;
#ifdef EXPERIMENTAL
	int size2 = (((size*numels)+7)&0xfffffff8);
	
	
	return my_get_mem(size2);
#endif
	calls_calloc++;
	my_malloc_totals +=	size*numels;
	ret = (void*)calloc(size, numels);
	if( !ret )
	{
		printf("Failure to fetch %d bytes of memory!\n", size*numels);
	}
	return ret;
}

#ifdef EXPERIMENTAL
void *my_free_all(void)
{
	int  at;
	for(at=0;at< storage_at; at++)
	{
		free(storage[at]);
		storage[at] = 0;
	}
	storage_at = 0;
	storage_allocd[at] = 0;
	storage_left[at] = 0;
}
#endif

void my_malloc_print_totals(void)
{
	printf("Totals:\n Calls to malloc = %d\n Calls to calloc = %d\n\n",
		   calls_malloc, calls_calloc);
}
