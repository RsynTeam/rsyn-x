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
#include "libhash.h"

/* structures needed to store & use syntax & constraint info */

typedef enum libsynt_attr_type
{
	SYNTAX_ATTRTYPE_STRING,
	SYNTAX_ATTRTYPE_VIRTUAL,
	SYNTAX_ATTRTYPE_ENUM,
	SYNTAX_ATTRTYPE_FLOAT,
	SYNTAX_ATTRTYPE_INT,
	SYNTAX_ATTRTYPE_BOOLEAN,
	SYNTAX_ATTRTYPE_COMPLEX,
	SYNTAX_ATTRTYPE_COMPLEX_UNKNOWN,
	SYNTAX_ATTRTYPE_COMPLEX_LIST
} libsynt_attr_type;


typedef struct libsynt_float_constraint
{
	double greater_than;
	double less_than;
	double default_value;
	char *refd_default;
		
	int greater_specd, less_specd, default_specd, ref_default_specd;
} libsynt_float_constraint;


typedef struct libsynt_int_constraint
{
	int greater_than;
	int less_than;
	int default_value;
	char *refd_default;
	int greater_specd, less_specd,default_specd, ref_default_specd;
} libsynt_int_constraint;

typedef struct libsynt_string_enum
{
		char **array; /* sorted for binary search */
		int size;
} libsynt_string_enum;

typedef struct libsynt_argument
{
		libsynt_attr_type type;
		struct libsynt_argument *next;
}libsynt_argument;

typedef struct libsynt_attribute_info
{
	char *name;
	libsynt_attr_type type;
	int lineno;
		
	union
	{
		libsynt_float_constraint *floatcon;
		libsynt_int_constraint   *intcon;
		libsynt_string_enum *stringenum;
		libsynt_argument *args;
	} u;
		
	struct libsynt_attribute_info *next;
		
} libsynt_attribute_info;

typedef enum libsynt_name_constraints 
{
	SYNTAX_GNAME_NONE,
	SYNTAX_GNAME_ONE,
	SYNTAX_GNAME_TWO,
	SYNTAX_GNAME_THREE,
	SYNTAX_GNAME_NONE_OR_ONE,
	SYNTAX_GNAME_ONE_OR_MORE
} libsynt_name_constraints;

typedef struct libsynt_head /* used in the parser */
{
		libsynt_name_constraints namecons;
		libsynt_argument *arglist;
		char *ident;
		int lineno;
}libsynt_head;

typedef struct libsynt_group_info
{
		int mark;
		libsynt_name_constraints name_constraints;
		
		char *type;
		int ID;
		int lineno;
		
		libsynt_attribute_info *attr_list;
		struct libsynt_group_info *group_list;
		
		liberty_hash_table *attr_hash;
		liberty_hash_table *group_hash;
		
		struct libsynt_group_info *next;
		struct libsynt_group_info *ref; /* if this group is really defined elsewheres */
		
} libsynt_group_info;

typedef struct libsynt_technology
{
		char *name;
		libsynt_group_info *lib;
		liberty_hash_table *group_hash;
		
} libsynt_technology;

