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

/* declarations for structures to store the contents of liberty files */
#include "libhash.h"

typedef struct liberty_name_list
{
		char *name;
		int prefix_len;
		struct liberty_name_list *next;
} liberty_name_list;

typedef struct liberty_head
{
		char *name;
		int lineno;
		char *filename;
		struct liberty_attribute_value *list;
} liberty_head;

struct liberty_group
{
		liberty_name_list *names;
		liberty_name_list *last_name;
		
		char *type;
		
		int  EVAL;

	    char *comment;

		struct liberty_attribute *attr_list;
		struct liberty_attribute *attr_last;
		liberty_hash_table *attr_hash;
		
		struct liberty_define *define_list;
		struct liberty_define *define_last;
		liberty_hash_table *define_hash;
		
		struct liberty_group *group_list;
		struct liberty_group *group_last;
		liberty_hash_table *group_hash;
		
		struct liberty_group *next;
		int lineno;
		char *filename;
		struct liberty_group *owner;
};

typedef struct liberty_group liberty_group;

typedef enum liberty_attribute_type 
{
	LIBERTY__SIMPLE,
	LIBERTY__COMPLEX
} liberty_attribute_type;

typedef enum liberty_attribute_value_type 
{
	LIBERTY__VAL_STRING,
	LIBERTY__VAL_DOUBLE,
	LIBERTY__VAL_BOOLEAN,
	LIBERTY__VAL_INT,
	LIBERTY__VAL_EXPR,
	LIBERTY__VAL_UNDEFINED
} liberty_attribute_value_type;

struct liberty_attribute_value
{
		liberty_attribute_value_type type;
		union
		{
				char *string_val;
				int int_val;
				double double_val;
				int bool_val;
			    void *expr_val;
		} u;
		struct liberty_attribute_value *next; /* for lists */
};

typedef struct liberty_attribute_value liberty_attribute_value;


struct liberty_attribute
{
		liberty_attribute_type type;
		char *name;
	    char *comment;
		liberty_attribute_value *value;
		liberty_attribute_value *last_value;
		
		struct liberty_attribute *next;
		int EVAL;
		int is_var;
		int lineno;
		char *filename;
		liberty_group *owner;
};

typedef struct liberty_attribute liberty_attribute;


struct liberty_define
{
		char *name;
		char *group_type;
		liberty_attribute_value_type valtype;
	    char *comment;

		struct liberty_define *next;
		int lineno;
		char *filename;
		liberty_group *owner;
};


typedef struct liberty_define liberty_define;


