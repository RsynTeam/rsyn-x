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

// Guilherme Flach 2016-03-27
// Added to ignore Wformat warning messages in this file.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#include <strings.h>

/* why doesn't libc have a nice hash table mechanism? */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "libhash.h"
#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include "mymalloc.h"

static si2drObjectIdT nulloid = {0, 0};
static unsigned int liberty_hash_name_hash(char *name, int size);
static unsigned int liberty_hash_name_hash_nocase(char *name, int size);

liberty_hash_table *liberty_hash_create_hash_table(int numels, int auto_resize,
                                                   int case_insensitive) {
        liberty_hash_table *ht;

        /* create a new hash table, with numels initial size */

        if (numels == 0) /* lets set an "auto" initial size of 53 els */
                numels = 53;

        ht = (liberty_hash_table *)my_calloc(sizeof(liberty_hash_table), 1);
        ht->auto_resize = auto_resize;
        ht->size = numels;
        ht->table = (liberty_hash_bucket **)my_calloc(
            sizeof(liberty_hash_bucket), numels);
        ht->counts = (int *)my_calloc(sizeof(int), numels);
        if (ht->auto_resize) ht->threshold = 8;
        ht->case_insensitive = case_insensitive;
        return ht;
}

int liberty_hash_get_next_higher_prime(int prime) {
        if (prime < 20) return 53;
        if (prime < 60) return 409;
        if (prime < 400) return 1019;
        if (prime < 1020) return 5009;
        if (prime < 5010) return 20011;
        return 99529; /* I hope we don't have to worry about tables any bigger
                         than this! */
}

void liberty_hash_destroy_hash_table(liberty_hash_table *ht) {
        /* traverse the all chain, and destroy all the buckets. */
        liberty_hash_bucket *hbptr, *hb_next;

        for (hbptr = ht->all_list; hbptr; hbptr = hb_next) {
                hb_next = hbptr->all_next;
                hbptr->next = (liberty_hash_bucket *)NULL;
                hbptr->all_next = (liberty_hash_bucket *)NULL;
                hbptr->all_prev = (liberty_hash_bucket *)NULL;
                hbptr->name = (char *)NULL;
                hbptr->item = nulloid;
                my_free(hbptr);
        }

        /* free the table */
        my_free(ht->table);

        /* zero out fields for the sake of memory debug */

        ht->table = 0;
        ht->all_list = 0;
        ht->all_last = 0;
        ht->size = 0;
        ht->longest_bucket_string = 0;
        ht->entry_count = 0;
        if (ht->counts) {
                free(ht->counts);
                ht->counts = 0;
        }
        free(ht);
}

void liberty_hash_resize_hash_table(liberty_hash_table *ht, int new_size) {
        liberty_hash_bucket *hb;
        int hash_num;

        /* safety valve: if the new size is the same as the old, just return! */

        if (new_size == ht->size) return; /* that was easy! */

        /* free the old table, my_calloc a new one */

        my_free(ht->table);
        my_free(ht->counts);

        ht->table = (liberty_hash_bucket **)my_calloc(
            sizeof(liberty_hash_bucket), new_size);
        ht->counts = (int *)my_calloc(sizeof(int), new_size);
        ht->size = new_size;
        ht->longest_bucket_string = 0;

        /* the all list is still valid, reform the next links around the new
         * table size */
        for (hb = ht->all_list; hb; hb = hb->all_next) {
                if (ht->case_insensitive)
                        hash_num =
                            liberty_hash_name_hash_nocase(hb->name, ht->size);
                else
                        hash_num = liberty_hash_name_hash(hb->name, ht->size);

                hb->next = ht->table[hash_num];
                ht->table[hash_num] = hb;

                ht->counts[hash_num]++;

                if (ht->longest_bucket_string < ht->counts[hash_num])
                        ht->longest_bucket_string = ht->counts[hash_num];
        }
}

static unsigned int liberty_hash_name_hash(char *name, int size) {
        unsigned int h = 0;
        for (; *name; name++) h = 13 * h + (int)(*name);
        h = (h % size);
        return h;
}

static unsigned int liberty_hash_name_hash_nocase(char *name, int size) {
        unsigned int h = 0;
        for (; *name; name++) h = 13 * h + (int)(toupper(*name));
        h = (h % size);
        return h;
}

int liberty_hash_enter_oid(liberty_hash_table *ht, char *name,
                           si2drObjectIdT oid) {
        liberty_hash_bucket *hb;
        int hash_num;

        if (name == NULL || *name == 0) {
                return -1;
        }
        if (ht->case_insensitive)
                hash_num = liberty_hash_name_hash_nocase(name, ht->size);
        else
                hash_num = liberty_hash_name_hash(name, ht->size);

        for (hb = ht->table[hash_num]; hb; hb = hb->next) {
                if (ht->case_insensitive) {
                        if (strcasecmp(hb->name, name) == 0) {
                                return -2;
                        }
                } else {
                        if (strcmp(hb->name, name) == 0) {
                                return -2;
                        }
                }
        }

        hb = (liberty_hash_bucket *)my_calloc(sizeof(liberty_hash_bucket), 1);
        hb->name = name;
        hb->item = oid;

        hb->next = ht->table[hash_num];
        ht->table[hash_num] = hb;

        if (ht->all_last == (liberty_hash_bucket *)NULL) {
                ht->all_last = hb;
                ht->all_list = hb;
        } else {
                ht->all_list->all_prev = hb;
                hb->all_next = ht->all_list;
                ht->all_list = hb;
        }

        ht->entry_count++;
        ht->counts[hash_num]++;

        if (ht->longest_bucket_string < ht->counts[hash_num])
                ht->longest_bucket_string = ht->counts[hash_num];

        if (ht->auto_resize && ht->longest_bucket_string > ht->threshold) {
                int biggest_size =
                    (ht->size > ht->entry_count ? ht->size : ht->entry_count);
                int next_size =
                    liberty_hash_get_next_higher_prime(biggest_size);
                if (next_size > ht->size) {
                        liberty_hash_resize_hash_table(ht, next_size);
                        if (ht->longest_bucket_string > ht->threshold) {
                                ht->threshold =
                                    ht->longest_bucket_string +
                                    3; /* we don't want to get too radical! */
                        }
                }
        }

        return 0;
}

void liberty_hash_delete_elem(liberty_hash_table *ht, char *name) {
        liberty_hash_bucket *hb, *hb_last;
        int hash_num;
        int count;

        if (name == NULL || *name == 0) {
                return;
        }
        if (ht->case_insensitive)
                hash_num = liberty_hash_name_hash_nocase(name, ht->size);
        else
                hash_num = liberty_hash_name_hash(name, ht->size);

        hb_last = 0;
        for (hb = ht->table[hash_num]; hb; hb = hb->next) {
                int res;

                if (ht->case_insensitive)
                        res = strcasecmp(hb->name, name);
                else
                        res = strcmp(hb->name, name);

                if (res == 0) {
                        /* unlink it, by stuffing the "next" into the last's
                         * next */
                        if (hb_last == (liberty_hash_bucket *)NULL) {
                                ht->table[hash_num] = hb->next;
                        } else {
                                hb_last->next = hb->next;
                        }

                        /* relink the all's */

                        if (ht->all_last == hb && ht->all_list == hb) {
                                ht->all_last = 0;
                                ht->all_list = 0;
                        } else if (ht->all_last == hb) {
                                hb->all_prev->all_next = 0;
                                ht->all_last = hb->all_prev;
                        } else if (hb->all_prev ==
                                   (liberty_hash_bucket *)NULL) {
                                /* first guy in the list */

                                ht->all_list = hb->all_next;
                                hb->all_next->all_prev =
                                    (liberty_hash_bucket *)NULL;
                        } else {
                                hb->all_next->all_prev = hb->all_prev;
                                hb->all_prev->all_next = hb->all_next;
                        }

                        /* zero out fields and free the bucket */
                        hb->next = (liberty_hash_bucket *)0;
                        hb->all_next = (liberty_hash_bucket *)0;
                        hb->all_prev = (liberty_hash_bucket *)0;
                        hb->name = (char *)0;
                        hb->item = nulloid;
                        free(hb);

                        ht->counts[hash_num]--;
                        ht->entry_count--;

                        return;
                }
                hb_last = hb;
        }
        return;
}

void liberty_hash_lookup(liberty_hash_table *ht, char *name,
                         si2drObjectIdT *oidptr) {
        liberty_hash_bucket *hb;
        int hash_num;

        if (name == NULL || *name == 0) {
                *oidptr = nulloid;
                return;
        }
        if (ht->case_insensitive)
                hash_num = liberty_hash_name_hash_nocase(name, ht->size);
        else
                hash_num = liberty_hash_name_hash(name, ht->size);

        for (hb = ht->table[hash_num]; hb; hb = hb->next) {
                if (ht->case_insensitive) {
                        if (strcasecmp(hb->name, name) == 0) {
                                *oidptr = hb->item;
                                return;
                        }
                } else {
                        if (strcmp(hb->name, name) == 0) {
                                *oidptr = hb->item;
                                return;
                        }
                }
        }
        *oidptr = nulloid;
        return;
}

char *make_rep(char *prefix, void *x) {
        static char buf[30][50];
        static int bufpos = 0;

        if (bufpos == 30) bufpos = 0;

        if (x == NULL) {
                strcpy(buf[bufpos], "0");
        } else
                sprintf(buf[bufpos], "&%s_%lx", prefix, x);
        return buf[bufpos++];
}

void dump_attr_hash(liberty_hash_table *ht, FILE *outc, FILE *outh) {
        /* go thru all the hash buckets and print out forward references to the
         * h file */
        liberty_hash_bucket *b;
        liberty_hash_table *t;
        int i;

        for (b = ht->all_list; b; b = b->all_next) {
                fprintf(outh, "extern liberty_hash_bucket hbuck_%lx;\n", b);
                fprintf(outc,
                        "liberty_hash_bucket hbuck_%lx = { %s, %s, %s, \"%s\", "
                        "{%s,(void*)0x%lx}  };\n",
                        b, make_rep("hbuck", b->next),
                        make_rep("hbuck", b->all_next),
                        make_rep("hbuck", b->all_prev), b->name,
                        make_rep("attr", b->item.v1), b->item.v2);
        }

        /* print the table */
        fprintf(outc, "liberty_hash_bucket *ht_table_%lx[%d] = {", ht->table,
                ht->size);
        for (i = 0; i < ht->size; i++) {
                if (ht->table[i])
                        fprintf(outc, "&hbuck_%lx,", ht->table[i]);
                else
                        fprintf(outc, "0,");
        }
        fprintf(outc, "};\n");

        /* print the counts */
        fprintf(outc, "int ht_counts_%lx[%d] = {", ht->counts, ht->size);
        for (i = 0; i < ht->size; i++) {
                fprintf(outc, "%d,", ht->counts[i]);
        }
        fprintf(outc, "};\n");

        /* print the struct */
        fprintf(outc,
                "liberty_hash_table ht_%lx = {ht_table_%lx, ht_counts_%lx, %s, "
                "%s, %d, %d, %d, %d, %d, %d};\n",
                ht, ht->table, ht->counts, make_rep("hbuck", ht->all_list),
                make_rep("hbuck", ht->all_last), ht->auto_resize, ht->threshold,
                ht->size, ht->longest_bucket_string, ht->entry_count,
                ht->case_insensitive);
}

void dump_group_hash(liberty_hash_table *ht, FILE *outc, FILE *outh) {
        /* go thru all the hash buckets and print out forward references to the
         * h file */
        liberty_hash_bucket *b;
        liberty_hash_table *t;
        int i;

        for (b = ht->all_list; b; b = b->all_next) {
                fprintf(outh, "extern liberty_hash_bucket hbuck_%lx;\n", b);
                fprintf(outc,
                        "liberty_hash_bucket hbuck_%lx = { %s, %s, %s, \"%s\", "
                        "{%s,(void*)0x%lx}  };\n",
                        b, make_rep("hbuck", b->next),
                        make_rep("hbuck", b->all_next),
                        make_rep("hbuck", b->all_prev), b->name,
                        make_rep("group", b->item.v1), b->item.v2);
        }

        /* print the table */
        fprintf(outc, "liberty_hash_bucket *ht_table_%lx[%d] = {", ht->table,
                ht->size);
        for (i = 0; i < ht->size; i++) {
                if (ht->table[i])
                        fprintf(outc, "&hbuck_%lx,", ht->table[i]);
                else
                        fprintf(outc, "0,");
        }
        fprintf(outc, "};\n");

        /* print the counts */
        fprintf(outc, "int ht_counts_%lx[%d] = {", ht->counts, ht->size);
        for (i = 0; i < ht->size; i++) {
                fprintf(outc, "%d,", ht->counts[i]);
        }
        fprintf(outc, "};\n");

        /* print the struct */
        fprintf(outc,
                "liberty_hash_table ht_%lx = {ht_table_%lx, ht_counts_%lx, %s, "
                "%s, %d, %d, %d, %d, %d, %d};\n",
                ht, ht->table, ht->counts, make_rep("hbuck", ht->all_list),
                make_rep("hbuck", ht->all_last), ht->auto_resize, ht->threshold,
                ht->size, ht->longest_bucket_string, ht->entry_count,
                ht->case_insensitive);
}

// Guilherme Flach 2016-03-27
// Added to ignore Wformat warning messages in this file.
#pragma GCC diagnostic pop
