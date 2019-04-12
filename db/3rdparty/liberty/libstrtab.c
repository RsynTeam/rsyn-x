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

/* why doesn't libc have a nice hash table mechanism? */

// Guilherme Flach 2016-03-26
// Added to remove some implicit declaration related warnings.
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <ctype.h>
#include "libhash.h"
#include "libstrtab.h"
#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include "mymalloc.h"

int total_lookup_calls = 0;
int total_enter_calls = 0;
int total_strings_entered = 0;
int total_bytes_entered = 0;

void print_strtab_stats(void) {
        printf(
            "String Table Lookup called %d times\nString Table Enter called %d "
            "times\nNo. Strings entered into String Table: %d\n, Total Bytes "
            "entered: %d\n\n",
            total_lookup_calls, total_enter_calls, total_strings_entered,
            total_bytes_entered);
}

liberty_strtable *liberty_strtable_create_strtable(int numels, int strsize,
                                                   int case_insensitive) {
        liberty_strtable *ht;
        liberty_strtable_chunk *htc;

        ht = (liberty_strtable *)my_calloc(sizeof(liberty_strtable), 1);
        ht->hashtab = liberty_hash_create_hash_table(
            numels, 1 /* make it resizeable */, case_insensitive);
        htc = (liberty_strtable_chunk *)my_calloc(
            sizeof(liberty_strtable_chunk), 1);
        ht->chunklist = htc;
        htc->strtab = (char *)my_calloc(1, strsize);
        htc->strtab_allocated = strsize;
        htc->strtab_used = 0;
        return ht;
}

void liberty_strtable_destroy_strtable(liberty_strtable *ht) {
        liberty_strtable_chunk *htc, *htcn;

        /* traverse the all chain, and destroy all the buckets. */
        if (ht->hashtab) liberty_hash_destroy_hash_table(ht->hashtab);
        ht->hashtab = 0;
        htc = ht->chunklist;
        while (htc) {
                htcn = htc->next;
                if (htc->strtab) free(htc->strtab);
                htc->strtab_allocated = 0;
                htc->strtab_used = 0;
                htc->strtab = 0;
                htc->next = 0;
                free(htc);
                htc = htcn;
        }
        ht->chunklist = 0;
        total_lookup_calls = 0;
        total_enter_calls = 0;
        total_strings_entered = 0;
        total_bytes_entered = 0;
        free(ht);
}

void liberty_strtable_resize_strtable(liberty_strtable *ht, int new_size) {
        liberty_hash_resize_hash_table(ht->hashtab, new_size);

        /* there isn't anything to do about the string space */
}

char *liberty_strtable_enter_string(liberty_strtable *ht, char *str) {
        si2drObjectIdT t, ores;
        char *result, *strptr;
        int slen = strlen(str);

        total_enter_calls++;

        /* first, check the table for the string */

        liberty_hash_lookup(ht->hashtab, str, &ores);

        if (ores.v1 != 0) return ores.v1;

        /* first, add the string to the table */

        if (ht->chunklist->strtab_used + slen + 1 >
            ht->chunklist->strtab_allocated) {
                /* this string won't fit in the current chunk. realloc isn't an
                   option, all the string pointers would
                   be off. So create a new chunk, and put it at the top of the
                   list, and put the new string in there */
                liberty_strtable_chunk *htc =
                    (liberty_strtable_chunk *)my_calloc(
                        sizeof(liberty_strtable_chunk), 1);

                htc->strtab =
                    (char *)my_calloc(ht->chunklist->strtab_allocated, 1);
                htc->strtab_allocated = ht->chunklist->strtab_allocated;
                htc->strtab_used = 0;

                /* link in the new chunk to the head of the chunklist */
                htc->next = ht->chunklist;
                ht->chunklist = htc;
        }
        strcpy(ht->chunklist->strtab + ht->chunklist->strtab_used, str);
        strptr = ht->chunklist->strtab + ht->chunklist->strtab_used;
        total_strings_entered++;
        total_bytes_entered += slen + 1;
        ht->chunklist->strtab_used += slen + 1;

        t.v1 = strptr;
        t.v2 = 0;

        liberty_hash_enter_oid(ht->hashtab, strptr,
                               t); /* ToDo: make a generic hashtab, then somehow
                                      use it as a base for oid hashes,
                                                                     and then
                                      use it as a base for the string hash */
        return strptr;
}

void liberty_strtable_lookup(liberty_strtable *ht, char *str, char **result) {
        si2drObjectIdT t, ores;
        char *strptr;

        total_lookup_calls++;

        if (str == NULL || *str == 0) {
                *result = (char *)0;
                return;
        }

        /* first, check the table for the string */

        liberty_hash_lookup(ht->hashtab, str, &ores);

        if (ores.v1 != 0) {
                *result = ores.v1;
                return;
        }

        *result = 0;
        return;
}
