
/* bison input file */
%{
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
#include "syntax.h"
#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include "mymalloc.h"
	
static libsynt_group_info *gs[100];
static int gsindex = 0;

 liberty_hash_table *libsynt_groups;
 liberty_hash_table *libsynt_allgroups;
 liberty_hash_table *libsynt_attrs;

 libsynt_technology libsynt_techs[20];
 int libsynt_tech_count = 0;
 
void push_group(libsynt_head *h);
void pop_group(void);

extern int synt_lineno;

libsynt_attribute_info *make_complex(libsynt_head *h);
 
libsynt_attribute_info *make_simple(char *name, libsynt_attr_type type, void *constraint_ptr);
 

 static char *enumlist[100]; /* temps used in the process of building structs */
 static int enumcount;
 
%}

%union {
	char *str;
	double num;
	void *voidptr;
	libsynt_attr_type attrtype;
	libsynt_float_constraint *float_constr;
	libsynt_int_constraint *int_constr;
	libsynt_string_enum *str_enum;
	libsynt_argument *arg;
	libsynt_attribute_info *attr;
	libsynt_name_constraints nameconstr;
	libsynt_group_info *group;
	libsynt_head *head;
	libsynt_technology *tech;
}


%token COMMA SEMI LPAR RPAR LCURLY RCURLY COLON LBRACK RBRACK
%token KW_FLOAT KW_STRING KW_ENUM KW_UNK_ARGS KW_INTEGER KW_VIRTATTR KW_SHORT KW_OR KW_AND KW_BOOLEAN COLONEQ KW_LIST GREATERTHAN LESSTHAN DOTS

%token <num> NUM
%token <str> STRING IDENT

%type <nameconstr> namelist
%type <arg> arglist
%type <head> arg_or_namelist head
%type <str_enum> string_enum
%type <attr> simple_attr complex_attr
%type <float_constr> float_constraint
%type <int_constr> int_constraint
%type <group> group
%type <tech> file

%type <str>	s_or_i

%%

file	: group { libsynt_techs[libsynt_tech_count++].lib = $1; }
		;

group	: head LCURLY {push_group($1);} statements RCURLY {$$ = gs[gsindex-1];pop_group();}
        | head LCURLY {push_group($1);} RCURLY {si2drObjectIdT toid;  $$ = gs[gsindex-1]; liberty_hash_lookup(libsynt_groups, gs[gsindex-1]->type, &toid);
                                                if( toid.v1 != (void*)NULL )$$->ref =(libsynt_group_info*)toid.v1;
                                                else {printf("Error: line %d: Couldn't find group %s\n",$1->lineno,$1->ident); } pop_group();}
		;


statements 	: statement {}
		 	| statements statement {}
			;


statement 	: simple_attr {si2drObjectIdT toid; toid.v1 = (void*)$1;liberty_hash_enter_oid(gs[gsindex-1]->attr_hash, $1->name, toid);$1->next = gs[gsindex-1]->attr_list; gs[gsindex-1]->attr_list = $1; }
			| complex_attr {si2drObjectIdT toid;toid.v1 = (void*)$1;liberty_hash_enter_oid(gs[gsindex-1]->attr_hash, $1->name, toid);$1->next = gs[gsindex-1]->attr_list; gs[gsindex-1]->attr_list = $1;}
			| group  {si2drObjectIdT toid;toid.v1 = (void*)$1;liberty_hash_enter_oid(gs[gsindex-1]->group_hash, $1->type,toid);$1->next = gs[gsindex-1]->group_list; gs[gsindex-1]->group_list = $1;}
			;

simple_attr	: IDENT COLON KW_VIRTATTR SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_VIRTUAL,0);} 
            | IDENT COLON KW_STRING SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_STRING,0);} 
            | IDENT COLON KW_STRING string_enum SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_ENUM, $4);} 
            | IDENT COLON KW_FLOAT float_constraint  SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_FLOAT,$4);}
            | IDENT COLON KW_FLOAT SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_FLOAT,0);}
            | IDENT COLON KW_ENUM string_enum SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_ENUM, $4);} 
            | IDENT COLON KW_SHORT SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_INT,0);} 
            | IDENT COLON KW_SHORT int_constraint SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_INT,$4);} 
            | IDENT COLON KW_INTEGER SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_INT,0);} 
            | IDENT COLON KW_INTEGER int_constraint SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_INT,$4);} 
            | IDENT COLON KW_BOOLEAN SEMI{ $$ = make_simple($1,SYNTAX_ATTRTYPE_BOOLEAN,0);} 
            ;

string_enum : LPAR s_or_i_list RPAR { int i; $$ = (libsynt_string_enum*)my_calloc(sizeof(libsynt_string_enum),1); $$->array = (char**)my_calloc(sizeof(char*),enumcount+1);
                                            for(i=0;i<enumcount; i++){$$->array[i] = enumlist[i];} $$->size = enumcount;}
            ;

s_or_i_list : s_or_i { enumcount=0; enumlist[enumcount++] = $1;}
            | s_or_i_list COMMA s_or_i { enumlist[enumcount++] = $3;}
            | s_or_i_list COMMA KW_OR s_or_i { enumlist[enumcount++] = $4;}
            ;


float_constraint : LPAR GREATERTHAN NUM RPAR {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->greater_than = $3; $$->greater_specd =1; }
                 | LPAR LESSTHAN NUM RPAR {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->less_than = $3; $$->less_specd = 1; }
                 | LPAR GREATERTHAN NUM KW_AND LESSTHAN NUM RPAR {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1);
                                                 $$->less_than = $6; $$->less_specd = 1;$$->greater_than = $3; $$->greater_specd =1;}
                 | COLONEQ NUM {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->default_value = $2; $$->default_specd =1;}
                 | LPAR GREATERTHAN NUM RPAR COLONEQ NUM {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->default_value = $6; $$->default_specd =1; $$->greater_than = $3; $$->greater_specd =1; }
                 | LPAR LESSTHAN NUM RPAR COLONEQ NUM {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->default_value = $6; $$->default_specd =1;$$->less_than = $3; $$->less_specd = 1; }
                 | LPAR GREATERTHAN NUM KW_AND LESSTHAN NUM RPAR COLONEQ NUM {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->default_value = $9;
                                                 $$->less_than = $6; $$->less_specd = 1;$$->greater_than = $3; $$->greater_specd =1; $$->default_specd =1;}
                 | COLONEQ s_or_i {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->refd_default = $2; $$->ref_default_specd =1;}
                 | LPAR GREATERTHAN NUM RPAR COLONEQ s_or_i {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->refd_default = $6;  $$->ref_default_specd =1;$$->greater_than = $3; $$->greater_specd =1; }
                 | LPAR LESSTHAN NUM RPAR COLONEQ s_or_i {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->refd_default = $6; $$->ref_default_specd =1;$$->less_than = $3; $$->less_specd = 1; }
                 | LPAR GREATERTHAN NUM KW_AND LESSTHAN NUM RPAR COLONEQ s_or_i {$$ = (libsynt_float_constraint*)my_calloc(sizeof(libsynt_float_constraint),1); $$->refd_default = $9;
                                                 $$->less_than = $6; $$->less_specd = 1;$$->greater_than = $3; $$->greater_specd =1; $$->ref_default_specd =1;}
                 ;

int_constraint :  LPAR GREATERTHAN NUM RPAR {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->greater_than = $3; $$->greater_specd =1; }
               | LPAR LESSTHAN NUM RPAR {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->less_than = $3; $$->less_specd = 1; }
               | LPAR GREATERTHAN NUM KW_AND LESSTHAN NUM RPAR {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1);
                                                                $$->less_than = $6; $$->less_specd = 1;$$->greater_than = $3; $$->greater_specd =1;}
               | COLONEQ NUM {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->default_value = $2; $$->default_specd =1;}
               | LPAR GREATERTHAN NUM RPAR COLONEQ NUM {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->default_value = $6; $$->default_specd =1; $$->greater_than = $3; $$->greater_specd =1; }
               | LPAR LESSTHAN NUM RPAR COLONEQ NUM {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->default_value = $6; $$->default_specd =1;$$->less_than = $3; $$->less_specd = 1; }
               | LPAR GREATERTHAN NUM KW_AND LESSTHAN NUM RPAR COLONEQ NUM {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->default_value = $9;
                                                                            $$->less_than = $6; $$->less_specd = 1;$$->greater_than = $3; $$->greater_specd =1; $$->default_specd =1;}
               | COLONEQ s_or_i {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->refd_default = $2; $$->ref_default_specd =1;}
               | LPAR GREATERTHAN NUM RPAR COLONEQ s_or_i {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->refd_default = $6;  $$->ref_default_specd =1;$$->greater_than = $3; $$->greater_specd =1; }
               | LPAR LESSTHAN NUM RPAR COLONEQ s_or_i {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->refd_default = $6; $$->ref_default_specd =1;$$->less_than = $3; $$->less_specd = 1; }
               | LPAR GREATERTHAN NUM KW_AND LESSTHAN NUM RPAR COLONEQ s_or_i {$$ = (libsynt_int_constraint*)my_calloc(sizeof(libsynt_int_constraint),1); $$->refd_default = $9;
                                                                      $$->less_than = $6; $$->less_specd = 1;$$->greater_than = $3; $$->greater_specd =1; $$->ref_default_specd =1;}
;



complex_attr 	: head  SEMI  { $$= make_complex($1);}
				;

head	: IDENT LPAR arg_or_namelist RPAR { $$ = $3; $$->ident = $1; $$->lineno = synt_lineno;}
        | KW_SHORT LPAR arg_or_namelist RPAR { $$ = $3; $$->ident = "short"; $$->lineno = synt_lineno;}
		;


arg_or_namelist : namelist {$$ = my_calloc(sizeof(libsynt_head),1); $$->namecons = $1;}
				| arglist {$$ = my_calloc(sizeof(libsynt_head),1); $$->arglist = $1;}
				;

namelist :  {$$ = SYNTAX_GNAME_NONE;}
		 | IDENT {$$ = SYNTAX_GNAME_ONE;}
		 | LBRACK IDENT RBRACK {$$ = SYNTAX_GNAME_NONE_OR_ONE;}
		 | IDENT LBRACK COMMA DOTS COMMA IDENT RBRACK {$$ = SYNTAX_GNAME_ONE_OR_MORE;}
		 | IDENT COMMA IDENT {$$ = SYNTAX_GNAME_TWO;}
		 | IDENT COMMA IDENT COMMA IDENT {$$ = SYNTAX_GNAME_THREE;}
		 ;

arglist : KW_STRING { $$=my_calloc(sizeof(libsynt_argument),1); $$->type = SYNTAX_ATTRTYPE_STRING;}
        | KW_INTEGER {$$=my_calloc(sizeof(libsynt_argument),1); $$->type = SYNTAX_ATTRTYPE_INT;}
        | KW_FLOAT {$$=my_calloc(sizeof(libsynt_argument),1); $$->type = SYNTAX_ATTRTYPE_FLOAT;}
        | KW_UNK_ARGS {$$=my_calloc(sizeof(libsynt_argument),1); $$->type = SYNTAX_ATTRTYPE_COMPLEX_UNKNOWN;}
        | KW_LIST {$$=my_calloc(sizeof(libsynt_argument),1); $$->type = SYNTAX_ATTRTYPE_COMPLEX_LIST;}
        | arglist COMMA KW_STRING {libsynt_argument *x= my_calloc(sizeof(libsynt_argument),1),*y; x->type = SYNTAX_ATTRTYPE_STRING; for(y=$$;y;y=y->next){if( !y->next){y->next=x;break;}}$$=$1;}
        | arglist COMMA KW_INTEGER {libsynt_argument *x= my_calloc(sizeof(libsynt_argument),1),*y; x->type = SYNTAX_ATTRTYPE_INT;for(y=$$;y;y=y->next){if( !y->next){y->next=x;break;}}$$=$1;}
        | arglist COMMA KW_FLOAT {libsynt_argument *x= my_calloc(sizeof(libsynt_argument),1),*y; x->type = SYNTAX_ATTRTYPE_FLOAT;for(y=$$;y;y=y->next){if( !y->next){y->next=x;break;}}$$=$1;}
        | arglist COMMA KW_LIST {libsynt_argument *x= my_calloc(sizeof(libsynt_argument),1),*y; x->type = SYNTAX_ATTRTYPE_COMPLEX_LIST;for(y=$$;y;y=y->next){if( !y->next){y->next=x;break;}}$$=$1;}
        ;



s_or_i  : STRING {$$ = $1;}
		| IDENT {$$=$1;}
		;

%%

void push_group(libsynt_head *h )
{
	si2drObjectIdT toid,noid;
	
	gs[gsindex] = (libsynt_group_info*)my_calloc(sizeof(libsynt_group_info),1);

	toid.v1 = (void*)gs[gsindex];

	gs[gsindex]->lineno = h->lineno;

	gs[gsindex]->type = h->ident;

	gs[gsindex]->name_constraints = h->namecons;

	gs[gsindex]->attr_hash = liberty_hash_create_hash_table(41, 1, 0);

	gs[gsindex]->group_hash = liberty_hash_create_hash_table(41, 1, 0);

	liberty_hash_lookup(libsynt_groups, h->ident, &noid);
	if( noid.v1 == (void*)NULL )
		liberty_hash_enter_oid(libsynt_groups, h->ident, toid);
 
	liberty_hash_lookup(libsynt_allgroups, h->ident, &noid);
	if( noid.v1 == (void*)NULL )
		liberty_hash_enter_oid(libsynt_allgroups, h->ident, toid);
 
	gsindex++;
}

void pop_group(void)
{
	gsindex--;
}

libsynt_attribute_info *make_complex(libsynt_head *h)
{
	si2drObjectIdT toid,noid;
	libsynt_attribute_info *x = my_calloc(sizeof(libsynt_attribute_info),1);
	x->name = h->ident;
	toid.v1 = (void*)x;
	
	if( h->arglist->type == SYNTAX_ATTRTYPE_COMPLEX_UNKNOWN )
		x->type = SYNTAX_ATTRTYPE_COMPLEX_UNKNOWN;
	else if( h->arglist->type == SYNTAX_ATTRTYPE_COMPLEX_LIST )
		x->type = SYNTAX_ATTRTYPE_COMPLEX_LIST;
	else
		x->type = SYNTAX_ATTRTYPE_COMPLEX;
	x->u.args = h->arglist;
	liberty_hash_lookup(libsynt_attrs, h->ident, &noid);
	if( noid.v1 == (void*)NULL )
		liberty_hash_enter_oid(libsynt_attrs, h->ident, toid);
	return x;
}

libsynt_attribute_info *make_simple(char *name, libsynt_attr_type type, void *constraint_ptr)
{
	si2drObjectIdT toid,noid;
	libsynt_attribute_info *x = my_calloc(sizeof(libsynt_attribute_info),1);
	x->name = name;
	x->type = type;
	toid.v1 = (void*)x;
	switch( type )
	{
	case SYNTAX_ATTRTYPE_STRING:
		break;
	case SYNTAX_ATTRTYPE_VIRTUAL:
		break;
	case SYNTAX_ATTRTYPE_ENUM:
		x->u.stringenum = (libsynt_string_enum*)constraint_ptr;
		break;
	case SYNTAX_ATTRTYPE_FLOAT:
		x->u.floatcon = (libsynt_float_constraint*)constraint_ptr;
		break;
	case SYNTAX_ATTRTYPE_INT:
		x->u.intcon = (libsynt_int_constraint*)constraint_ptr;
		break;
	case SYNTAX_ATTRTYPE_BOOLEAN:
		break;
	default:
		break;
	}
	liberty_hash_lookup(libsynt_attrs, x->name, &noid);
	if( noid.v1 == (void*)NULL )
		liberty_hash_enter_oid(libsynt_attrs, x->name, toid);
	return x;
}

int syntax_parser_error(char *);

int yyerror(char *s)
{
	fprintf(stderr,"%s line number %d\n", s, synt_lineno);
	return 0;
}
