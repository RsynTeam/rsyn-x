
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
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include "liberty_structs.h"
#include "si2dr_liberty.h"
#include "group_enum.h"
#include "attr_enum.h"
#include "libhash.h"
#include "libstrtab.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include "mymalloc.h" /* meant to override the my_*alloc calls if dmalloc defined*/

   static si2drGroupIdT gs[1000];
   static int gsindex = 0;

   static si2drErrorT   err;
   static si2drAttrTypeT atype;
   static si2drAttrIdT curr_attr;
   static si2drDefineIdT curr_def;
   void push_group(liberty_head *h );
   void pop_group(liberty_head *h);
   si2drValueTypeT convert_vt(char *type);
   int lineno;
   int syntax_errors;
   static char PB[8000]; /* so as not to have a bunch of local buffers */
   extern int tight_colon_ok;
   extern char token_comment_buf[SI2DR_MAX_STRING_LEN]; 
   extern char token_comment_buf2[SI2DR_MAX_STRING_LEN];
   static char token_comment_buft[SI2DR_MAX_STRING_LEN];
   extern int tok_encountered;
   extern char *curr_file;
   extern liberty_strtable *master_string_table;

   struct xnumber
   {
      int type; /* 0=int, 1=float */
      int intnum;
      double floatnum;
   };
   typedef struct xnumber xnumber;

   void make_complex(liberty_head *h);
   void make_simple(char *name, liberty_attribute_value *v);


   %}

   %union {
      char *str;
      xnumber num;
      liberty_group *group;
      liberty_attribute *attr;
      liberty_attribute_value *val;
      liberty_define *def;
      liberty_head *head;
      si2drExprT *expr;
   }


%token COMMA SEMI LPAR RPAR LCURLY RCURLY COLON KW_DEFINE KW_DEFINE_GROUP KW_TRUE KW_FALSE PLUS MINUS MULT DIV EQ
%token UNARY

%token <num> NUM
%token <str> STRING IDENT

%left PLUS MINUS 
%left MULT DIV
%right UNARY
%left LPAR RPAR

%type <group>     group file statements statement
%type <def>       define define_group
%type <val> param_list attr_val attr_val_expr
%type <str>	s_or_i
%type <head> head
%type <expr> expr
%%

file	: {lineno = 1; syntax_errors= 0;} group {}
;

group	: head LCURLY {push_group($1);} statements RCURLY {pop_group($1);}
| head LCURLY {push_group($1);} RCURLY {pop_group($1);}
;


statements 	: statement {}
| statements statement  {}
;


statement 	: simple_attr {}
| complex_attr {}
| define {}
| define_group {}
| group  {}
;

simple_attr	: IDENT COLON attr_val_expr { make_simple($1,$3);} SEMI
| IDENT COLON attr_val_expr { make_simple($1,$3);}
| IDENT EQ    attr_val_expr { make_simple($1,$3);si2drSimpleAttrSetIsVar(curr_attr,&err); } SEMI
;

complex_attr 	: head  SEMI  {make_complex($1);}
| head  {make_complex($1);}
;

head	: IDENT LPAR {tight_colon_ok =1;} param_list RPAR { $$ = (liberty_head*)my_calloc(sizeof(liberty_head),1); $$->name = $1; $$->list = $4; $$->lineno = lineno;$$->filename = curr_file; tight_colon_ok =0;}
| IDENT LPAR RPAR            { $$ = (liberty_head*)my_calloc(sizeof(liberty_head),1); $$->name = $1; $$->list = 0; $$->lineno = lineno;$$->filename = curr_file;}
;


param_list  : attr_val {$$=$1;}
| param_list COMMA attr_val
{
   liberty_attribute_value *v;
   for(v=$1; v; v=v->next)
   {
      if(!v->next)
      {
         v->next = $3;
         break;
      }
   }
   $$ = $1;
}
| param_list attr_val
{
   liberty_attribute_value *v;
   for(v=$1; v; v=v->next)
   {
      if(!v->next)
      {
         v->next = $2;
         break;
      }
   }
   $$ = $1;
}
;

define 	: KW_DEFINE LPAR s_or_i COMMA s_or_i COMMA s_or_i RPAR SEMI  
{curr_def = si2drGroupCreateDefine(gs[gsindex-1],$3,$5,convert_vt($7),&err);si2drObjectSetLineNo(curr_def,lineno,&err);si2drObjectSetFileName(curr_def,curr_file,&err);
   if( token_comment_buf[0] ) { si2drDefineSetComment(curr_def, token_comment_buf,&err); token_comment_buf[0]=0;} 
   if( token_comment_buf2[0] )	{strcpy(token_comment_buf, token_comment_buf2);token_comment_buf2[0] = 0;}
   tok_encountered = 0;
}
;


define_group : KW_DEFINE_GROUP LPAR s_or_i COMMA s_or_i RPAR SEMI
{curr_def = si2drGroupCreateDefine(gs[gsindex-1],$3,$5,SI2DR_UNDEFINED_VALUETYPE,&err);si2drObjectSetLineNo(curr_def,lineno,&err);si2drObjectSetFileName(curr_def,curr_file,&err);
   if( token_comment_buf[0] ) { si2drDefineSetComment(curr_def, token_comment_buf,&err); token_comment_buf[0]=0;} 
   if( token_comment_buf2[0] )	{strcpy(token_comment_buf, token_comment_buf2);token_comment_buf2[0] = 0;}
   tok_encountered = 0;
}
;

s_or_i  : STRING {$$ = $1;}
| IDENT {$$=$1;}
;

attr_val : NUM { $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
              /* I get back a floating point number... not a string, and I have to 
                 tell if it's an integer, without using any math lib funcs? */
              if( $1.type == 0 )
              {
                 $$->type = LIBERTY__VAL_INT;
                 $$->u.int_val = (int) $1.intnum;
              }
              else
              {
                 $$->type = LIBERTY__VAL_DOUBLE;
                 $$->u.double_val = $1.floatnum;
              }
           }
|  s_or_i
{
   $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
   $$->type = LIBERTY__VAL_STRING;
   $$->u.string_val = $1;
}
|  s_or_i  COLON s_or_i 
{
   char *x;
   $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
   $$->type = LIBERTY__VAL_STRING;
   x = (char*)alloca(strlen($1) + strlen($3) + 2); /* get a scratchpad */
   sprintf(x, "%s:%s", $1,$3);
   $$->u.string_val = liberty_strtable_enter_string(master_string_table, x); /* scratchpad goes away after this */
}
| KW_TRUE
{
   $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
   $$->type = LIBERTY__VAL_BOOLEAN;
   $$->u.bool_val = 1;
}
| KW_FALSE
{
   $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
   $$->type = LIBERTY__VAL_BOOLEAN;
   $$->u.bool_val = 0;
}
;

attr_val_expr : /* NUM { $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
                   $$->type = LIBERTY__VAL_DOUBLE;
                   $$->u.double_val = $1;
                   }  I'm going to put nums thru the expr stuff

                   |  */ STRING
{
   $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
   $$->type = LIBERTY__VAL_STRING;
   $$->u.string_val = $1;
}
| KW_TRUE
{
   $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
   $$->type = LIBERTY__VAL_BOOLEAN;
   $$->u.bool_val = 1;
}
| KW_FALSE
{
   $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
   $$->type = LIBERTY__VAL_BOOLEAN;
   $$->u.bool_val = 0;
}
| expr
{
   /* all the if/else if's are to reduce the total number of exprs to a minimum */
   if( $1->type == SI2DR_EXPR_VAL && $1->valuetype == SI2DR_FLOAT64 && !$1->left && !$1->right )
   {
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_DOUBLE;
      $$->u.double_val = $1->u.d;
      /* printf("EXPR->double %g \n", $1->u.d); */
      si2drExprDestroy($1,&err);
   }
   else if( $1->type == SI2DR_EXPR_VAL && $1->valuetype == SI2DR_INT32 && !$1->left && !$1->right )
   {
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_INT;
      $$->u.int_val = $1->u.i;
      /* printf("EXPR->int - %d \n", $1->u.i); */
      si2drExprDestroy($1,&err);
   }
   else if( $1->type == SI2DR_EXPR_OP_SUB && $1->left && !$1->right 
         && $1->left->valuetype == SI2DR_FLOAT64 && !$1->left->left && !$1->left->right )
   {
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_DOUBLE;
      $$->u.double_val = -$1->left->u.d;
      /* printf("EXPR->double - %g \n", $1->u.d); */
      si2drExprDestroy($1,&err);
   }
   else if( $1->type == SI2DR_EXPR_OP_SUB && $1->left && !$1->right 
         && $1->left->valuetype == SI2DR_INT32 && !$1->left->left && !$1->left->right )
   {
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_INT;
      $$->u.int_val = -$1->left->u.i;
      /* printf("EXPR->double - %g \n", $1->u.d); */
      si2drExprDestroy($1,&err);
   }
   else if( $1->type == SI2DR_EXPR_OP_ADD && $1->left && !$1->right 
         && $1->left->valuetype == SI2DR_FLOAT64 && !$1->left->left && !$1->left->right )
   {
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_DOUBLE;
      $$->u.double_val = $1->left->u.d;
      /* printf("EXPR->double + %g \n", $1->u.d); */
      si2drExprDestroy($1,&err);
   }
   else if( $1->type == SI2DR_EXPR_OP_ADD && $1->left && !$1->right 
         && $1->left->valuetype == SI2DR_INT32 && !$1->left->left && !$1->left->right )
   {
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_INT;
      $$->u.int_val = $1->left->u.i;
      /* printf("EXPR->double + %g \n", $1->u.d); */
      si2drExprDestroy($1,&err);
   }
   else if( $1->type == SI2DR_EXPR_VAL && $1->valuetype == SI2DR_STRING && !$1->left && !$1->right 
         /* && ( strcmp($1->u.s,"VDD") && strcmp($1->u.s,"VSS")  )  I'm getting complaints about excluding VSS and VDD, so.... they'll not be exprs any more it they are all alone */ )
   {  /* uh, do we need to exclude all but VSS and VDD ? no! */
      /* The only way a string would turned into an expr, is if it were parsed
         as an IDENT -- so no quotes will ever be seen... */
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_STRING;
      $$->u.string_val = $1->u.s;
      /* printf("EXPR->string = %s \n", $1->u.s); */
      si2drExprDestroy($1,&err);
   }
   else
   {
      $$= (liberty_attribute_value*)my_calloc(sizeof(liberty_attribute_value),1);
      $$->type = LIBERTY__VAL_EXPR;
      $$->u.expr_val = $1;
      /* printf("left EXPR alone\n"); */
   }
}
;

expr     : expr PLUS expr
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_OP_ADD,&err);
   $$->left = $1;
   $$->right = $3;
}
| expr MINUS expr
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_OP_SUB,&err);
   $$->left = $1;
   $$->right = $3;
}
| expr MULT expr
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_OP_MUL,&err);
   $$->left = $1;
   $$->right = $3;
}
| expr DIV  expr
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_OP_DIV,&err);
   $$->left = $1;
   $$->right = $3;
}
| LPAR expr RPAR
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_OP_PAREN,&err);
   $$->left = $2;
   $$->right = 0;
}
| MINUS expr %prec UNARY
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_OP_SUB,&err);
   $$->left = $2;
   $$->right = 0;
}
| PLUS  expr %prec UNARY
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_OP_ADD,&err);
   $$->left = $2;
   $$->right = 0;
}
| NUM
{
   si2drErrorT err;
   $$ = si2drCreateExpr(SI2DR_EXPR_VAL,&err);
   $$->left = 0;
   $$->right = 0;
   if( $1.type == 0 )
   {
      $$->valuetype = SI2DR_INT32;
      $$->u.i = $1.intnum;
   }
   else
   {
      $$->valuetype = SI2DR_FLOAT64;
      $$->u.d = $1.floatnum;
   }

}
| IDENT
{
   si2drErrorT err;

   $$ = si2drCreateExpr(SI2DR_EXPR_VAL,&err);
   $$->valuetype = SI2DR_STRING;
   $$->u.s = $1;
   $$->left = 0;
   $$->right = 0;
}
;

%%

void push_group(liberty_head *h )
{
   liberty_attribute_value *v,*vn;
   extern group_enum si2drGroupGetID(si2drGroupIdT group, 
         si2drErrorT   *err);
   si2drErrorT err;

   si2drMessageHandlerT MsgPrinter;

   group_enum ge;
   MsgPrinter = si2drPIGetMessageHandler(&err); /* the printer is in another file! */


   if( gsindex == 0 )
   {

      gs[gsindex] = si2drPICreateGroup(0,h->name,&err);
   }

   else
   {
      gs[gsindex] = si2drGroupCreateGroup(gs[gsindex-1],0,h->name,&err);
   }

   if( token_comment_buf[0] )
   {
      si2drGroupSetComment(gs[gsindex], token_comment_buf, &err);  /* heaven help us if there's more than 100K of comment! */
      token_comment_buf[0] = 0;
      tok_encountered = 0;
      if( token_comment_buf2[0] )
         strcpy(token_comment_buf,token_comment_buf2);
      token_comment_buf2[0] = 0;
   }

   ge = si2drGroupGetID(gs[gsindex],&err);

   if( err == SI2DR_OBJECT_ALREADY_EXISTS && ( ge != LIBERTY_GROUPENUM_internal_power ) )
   {
      sprintf(PB,"%s:%d: The group name %s is already being used in this context.",curr_file,lineno,h->name);
      (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
            PB, 
            &err);
   }
   if( err == SI2DR_INVALID_NAME )
   {
      sprintf(PB,"%s:%d: The group name \"%s\" is invalid.",curr_file,lineno,h->name);
      (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
            PB, 
            &err);
   }
   gsindex++;

   si2drObjectSetLineNo(gs[gsindex-1],h->lineno, &err);
   si2drObjectSetFileName(gs[gsindex-1],h->filename, &err);
   for(v=h->list;v;v=vn)
   {
      if( v->type != LIBERTY__VAL_STRING )
      {
         char buf[1000],*buf2;

         if( v->type == LIBERTY__VAL_INT )
         {
            sprintf(buf,"%d",v->u.int_val);
         }
         else
         {
            sprintf(buf,"%.12g",v->u.double_val);
         }
         si2drGroupAddName(gs[gsindex-1],buf,&err);

         if( err == SI2DR_OBJECT_ALREADY_EXISTS && ( ge != LIBERTY_GROUPENUM_internal_power ) && (ge != LIBERTY_GROUPENUM_vector) && (ge != LIBERTY_GROUPENUM_ccs_timing_base_curve))
         {
            sprintf(PB,"%s:%d: The group name %s is already being used in this context.",
                  curr_file,lineno,buf);
            (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
                  PB, 
                  &err);
         }
         else if( err == SI2DR_INVALID_NAME )
         {
            sprintf(PB,"%s:%d: The group name \"%s\" is invalid.",curr_file,lineno,buf);
            (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
                  PB, 
                  &err);
         }
         else  if( err != SI2DR_NO_ERROR) {
             sprintf(PB,"%s:%d: Adding group name \"%s\" failed.",curr_file,lineno,buf);
            (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
                  PB, 
                  &err);
         }
      }
      else
      {
         si2drGroupAddName(gs[gsindex-1],v->u.string_val,&err);

         if( err == SI2DR_OBJECT_ALREADY_EXISTS && ( ge != LIBERTY_GROUPENUM_internal_power ) && (ge != LIBERTY_GROUPENUM_vector) && (ge != LIBERTY_GROUPENUM_library) && (ge != LIBERTY_GROUPENUM_intrinsic_resistance) && 
               (ge != LIBERTY_GROUPENUM_va_compact_ccs_rise) && (ge != LIBERTY_GROUPENUM_va_compact_ccs_fall) &&
               (ge != LIBERTY_GROUPENUM_va_receiver_capacitance1_fall) &&
               (ge != LIBERTY_GROUPENUM_va_receiver_capacitance2_fall) &&
               (ge != LIBERTY_GROUPENUM_va_receiver_capacitance1_rise) &&
               (ge != LIBERTY_GROUPENUM_va_receiver_capacitance2_rise) &&
               (ge != LIBERTY_GROUPENUM_normalized_driver_waveform) &&
               (ge != LIBERTY_GROUPENUM_dc_current) &&
               (ge != LIBERTY_GROUPENUM_va_rise_constraint) && (ge != LIBERTY_GROUPENUM_va_fall_constraint) && (ge != 0) )
         {
            sprintf(PB,"%s:%d: The group name %s is already being used in this context.",
                  curr_file,lineno,v->u.string_val);
            (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
                  PB, 
                  &err);
         }
         if( err == SI2DR_INVALID_NAME )
         {
            sprintf(PB,"%s:%d: The group name \"%s\" is invalid. It will not be added to the database\n",
                  curr_file,lineno,v->u.string_val);
            (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
                  PB, 
                  &err);
         }
      }

      vn = v->next;
      my_free(v);
   }
}

void pop_group(liberty_head *h)
{
   gsindex--;
   my_free(h);
}

void make_complex(liberty_head *h)
{
   liberty_attribute_value *v,*vn;

   curr_attr=si2drGroupCreateAttr(gs[gsindex-1],h->name,SI2DR_COMPLEX,&err);
   if( token_comment_buf[0] ) { si2drAttrSetComment(curr_attr, token_comment_buf,&err); token_comment_buf[0]=0; tok_encountered = 0;}

   si2drObjectSetLineNo(curr_attr,h->lineno, &err);
   si2drObjectSetFileName(curr_attr,h->filename, &err);
   for(v=h->list;v;v=vn)
   {
      if( v->type == LIBERTY__VAL_BOOLEAN )
         si2drComplexAttrAddBooleanValue(curr_attr,v->u.bool_val,&err);
      else if( v->type == LIBERTY__VAL_STRING )
         si2drComplexAttrAddStringValue(curr_attr,v->u.string_val,&err);		
      else if( v->type == LIBERTY__VAL_DOUBLE )
         si2drComplexAttrAddFloat64Value(curr_attr,v->u.double_val,&err);
      else if(v->type ==LIBERTY__VAL_INT)
         si2drComplexAttrAddInt32Value(curr_attr,v->u.int_val,&err);
      else 
         si2drComplexAttrAddStringValue(curr_attr,v->u.string_val,&err);		
      vn = v->next;
      my_free(v);
   }
   my_free (h);
}

void make_simple(char *name, liberty_attribute_value *v)
{
   curr_attr=si2drGroupCreateAttr(gs[gsindex-1],name,SI2DR_SIMPLE,&err);
   if( token_comment_buf[0] ) { si2drAttrSetComment(curr_attr, token_comment_buf,&err); token_comment_buf[0]=0; tok_encountered = 0;}

   si2drObjectSetLineNo(curr_attr,lineno, &err);
   si2drObjectSetFileName(curr_attr,curr_file, &err);
   if( v->type == LIBERTY__VAL_BOOLEAN )
      si2drSimpleAttrSetBooleanValue(curr_attr,v->u.bool_val,&err);
   else if( v->type == LIBERTY__VAL_EXPR )
      si2drSimpleAttrSetExprValue(curr_attr,v->u.expr_val,&err);
   else if( v->type == LIBERTY__VAL_STRING )
      si2drSimpleAttrSetStringValue(curr_attr,v->u.string_val,&err);
   else if( v->type == LIBERTY__VAL_DOUBLE )
      si2drSimpleAttrSetFloat64Value(curr_attr,v->u.double_val,&err);
   else if( v->type == LIBERTY__VAL_INT)
      si2drSimpleAttrSetInt32Value(curr_attr,v->u.int_val,&err);
   else 
      si2drSimpleAttrSetStringValue(curr_attr,v->u.string_val,&err);

   my_free(v);
}



si2drValueTypeT convert_vt(char *type)
{
   if( !strcmp(type,"string") )
      return SI2DR_STRING;
   if( !strcmp(type,"integer") )
      return SI2DR_INT32;
   if( !strcmp(type,"float") )
      return SI2DR_FLOAT64;
   if( !strcmp(type,"boolean") )
      return SI2DR_BOOLEAN;
   return SI2DR_UNDEFINED_VALUETYPE;
}

yyerror(char *s)
{
   si2drErrorT err;

   si2drMessageHandlerT MsgPrinter;

   MsgPrinter = si2drPIGetMessageHandler(&err); /* the printer is in another file! */

   sprintf(PB,"===\nERROR === %s file: %s, line number %d\nERROR ===", s, curr_file, lineno);
   (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, 
         PB, 
         &err);

   syntax_errors++;
}
