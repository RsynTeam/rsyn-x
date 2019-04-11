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
// Added to ignore some warning messages in this file.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wint-conversion"

#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdarg.h>
#include <math.h>
#include "liberty_structs.h"
#include "libhash.h"
#include "libstrtab.h"
#include "si2dr_liberty.h"
#include "string.h"
#include "group_enum.h"
#include "attr_enum.h"
typedef struct libGroupMap {
        char *name;
        group_enum type;
} libGroupMap;
typedef struct libAttrMap {
        char *name;
        attr_enum type;
} libAttrMap;

#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include "mymalloc.h"

static si2drMessageHandlerT si2ErrMsg;

const struct libGroupMap *lookup_group_name(register const char *str,
                                            register unsigned int len);

const struct libAttrMap *lookup_attr_name(register const char *str,
                                          register unsigned int len);

extern si2drErrorT syntax_check(si2drGroupIdT);
extern int liberty_parser_parse(void);

static liberty_group *master_group_list;
static liberty_hash_table *master_group_hash;
static liberty_hash_table *master_define_hash;
/* not static, because everyone needs to access the... */
liberty_strtable *master_string_table;

static si2drObjectIdT nulloid = {0, 0};

typedef enum {
        ITER_NAME,
        ITER_VAL,
        ITER_GROUP,
        ITER_ATTR,
        ITER_DEF,
        ITER_NOTHING
} liberty_iter_type;

typedef struct iterat {
        void *next;
        liberty_iter_type owner;
} iterat;

/* Function declarations */
attr_enum si2drAttrGetID(si2drAttrIdT attr, si2drErrorT *err);

group_enum si2drGroupGetID(si2drGroupIdT group, si2drErrorT *err);

int lib__name_needs_to_be_quoted(char *name);

char *expr_string(si2drExprT *e);

void lib__write_group(FILE *of, si2drGroupIdT group, char *indent,
                      char *cellname);

void create_floating_define_for_cell_area(si2drStringT string);

static int l__iter_group_count = 0;
static int l__iter_name_count = 0;
static int l__iter_val_count = 0;
static int l__iter_attr_count = 0;
static int l__iter_def_count = 0;
static int liberty___debug_mode = 0;
static int liberty___nocheck_mode = 0;
static FILE *liberty___trace_mode_CFP = 0;
static FILE *liberty___trace_mode_HFP = 0;

/* ==========================================================================================================================
 */
/*     TRACE ROUTINES */

/* TRACE globals */
static int trace = 0;
static int tracecount = 0;
static int tracefilecount = 0;
static int tracefunc = 0;
static int trace_grand_total = 0;
static char tracefbase[500];
static FILE *tracefile1, *tracefile2;

/* enum strings */

static char *ot_string(si2drObjectTypeT ot) {
        switch (ot) {
                case SI2DR_UNDEFINED_OBJECTTYPE:
                        return "SI2DR_UNDEFINED_OBJECTTYPE";
                case SI2DR_GROUP:
                        return "SI2DR_GROUP";
                case SI2DR_ATTR:
                        return "SI2DR_ATTR";
                case SI2DR_DEFINE:
                        return "SI2DR_DEFINE";
                case SI2DR_MAX_OBJECTTYPE:
                        return "SI2DR_MAX_OBJECTTYPE";
        }
        return "UNKNOWN";
}

static char *otype_string(si2drObjectTypeT ot) {
        switch (ot) {
                case SI2DR_UNDEFINED_OBJECTTYPE:
                        return "si2drObjectIdT";
                case SI2DR_GROUP:
                        return "si2drGroupIdT";
                case SI2DR_ATTR:
                        return "si2drAttrIdT";
                case SI2DR_DEFINE:
                        return "si2drDefineIdT";
                case SI2DR_MAX_OBJECTTYPE:
                        return "si2drObjectIdT";
        }
        return "si2drObjectIdT";
}

static char *vt_string(si2drValueTypeT vt) {
        switch (vt) {
                case SI2DR_UNDEFINED_VALUETYPE:
                        return "SI2DR_UNDEFINED_VALUETYPE";
                case SI2DR_INT32:
                        return "SI2DR_INT32";
                case SI2DR_STRING:
                        return "SI2DR_STRING";
                case SI2DR_FLOAT64:
                        return "SI2DR_FLOAT32";
                case SI2DR_BOOLEAN:
                        return "SI2DR_BOOLEAN";
                case SI2DR_MAX_VALUETYPE:
                        return "SI2DR_MAX_VALUETYPE";
                default:
                        break;
        }
        return "SI2DR_UNDEFINED_VALUETYPE";
}

static char *at_string(si2drAttrTypeT vt) {
        switch (vt) {
                case SI2DR_SIMPLE:
                        return "SI2DR_SIMPLE";
                case SI2DR_COMPLEX:
                        return "SI2DR_COMPLEX";
        }
        return "SI2DR_SIMPLE";
}

static char *oid_string(si2drObjectIdT oid) {
        static int index = 0;
        static char buf[10][500];
        if (index == 10) index = 0;
        if (oid.v1 == 0 && oid.v2 == 0)
                sprintf(buf[index], "nulloid");
        else
                sprintf(buf[index], "OID_%s_%x",
                        ot_string((si2drObjectTypeT)(oid.v1)), (oid.v2));

        return buf[index++];
}

static char *iter_string(si2drIterIdT iter) {
        iterat *y = (iterat *)iter;
        static int index = 0;
        static char buf[10][500];
        char *itype = 0;

        if (index == 10) index = 0;
        switch (y->owner) {
                case ITER_NAME:
                        itype = "NAME";
                        break;

                case ITER_VAL:
                        itype = "VAL";
                        break;

                case ITER_GROUP:
                        itype = "GROUP";
                        break;

                case ITER_ATTR:
                        itype = "ATTR";
                        break;

                case ITER_DEF:
                        itype = "DEFINE";
                        break;

                case ITER_NOTHING:
                        itype = "NOTHING";
                        break;
        }

        sprintf(buf[index], "ITER_%s_%x", itype, (unsigned long)(iter));
        /*sprintf(buf[index], "ITER_%s_%x", itype, (unsigned int)(iter));*/
        return buf[index++];
}

static char *itype_string(si2drIterIdT iter) {
        iterat *y = (iterat *)iter;
        static int index = 0;
        char *itype;

        if (index == 10) index = 0;
        switch (y->owner) {
                case ITER_NAME:
                        itype = "si2drNamesIdT";
                        break;

                case ITER_VAL:
                        itype = "si2drValuesIdT";
                        break;

                case ITER_GROUP:
                        itype = "si2drGroupsIdT";
                        break;

                case ITER_ATTR:
                        itype = "si2drAttrsIdT";
                        break;

                case ITER_DEF:
                        itype = "si2drDefinesIdT";
                        break;
                default:
                        itype = "si2drObjectsIdT";
                        break;
        }

        return itype;
}

static void trace_check(si2drObjectIdT oid) {
        fprintf(tracefile1, "\t\tcheck_results(%s, err, __FILE__,__LINE__);\n",
                oid_string(oid));
        fflush(tracefile1);
}

static void outinit_oid(si2drObjectIdT oid) {
        fprintf(tracefile2, "%s %s;\n",
                otype_string((si2drObjectTypeT)(oid.v2)), oid_string(oid));
}

static void outinit_iter(si2drIterIdT iter) {
        fprintf(tracefile2, "%s %s;\n", itype_string(iter), iter_string(iter));
}

static void inc_tracecount(void) {
        tracecount++;
        trace_grand_total++;

        if (tracecount > 1200) {
                tracefunc++;

                fprintf(tracefile1, "}\n\n\n");

                if ((tracefunc % 6) == 0) {
                        char buf[100];

                        tracefilecount++;

                        fclose(tracefile1);

                        sprintf(buf, "%s%02d.c", tracefbase, tracefilecount);

                        tracefile1 = fopen(buf, "w");

                        if (!tracefile1) {
                                printf(
                                    "Tracing turned off due to file open "
                                    "problems\n");
                                trace = 0;
                                return;
                        } else {
                                fprintf(tracefile1, "#include <stdio.h>\n\n");
                                fprintf(tracefile1,
                                        "#include \"si2dr_liberty.h\"\n");
                                fprintf(tracefile1,
                                        "#include \"oiddecls.h\"\n\n\n");
                                fprintf(tracefile1,
                                        "extern void "
                                        "check_results(si2drObjectIdT result, "
                                        "si2drErrorT err, char *fname, int "
                                        "lineno);\n\n");
                        }
                }

                fprintf(tracefile1, "\n\nvoid func%d(void)\n{\n", tracefunc);
                fprintf(tracefile1, "\t/* output decls used later */\n");
                fprintf(tracefile1, "\tsi2drValueTypeT type;\n");
                fprintf(tracefile1, "\tsi2drInt32T     intgr;\n");
                fprintf(tracefile1, "\tsi2drFloat64T   float64;\n");
                fprintf(tracefile1, "\tsi2drStringT    string;\n");
                fprintf(tracefile1, "\tsi2drStringT    name_string;\n");
                fprintf(tracefile1, "\tsi2drStringT    allowed_groups;\n");
                fprintf(tracefile1, "\tsi2drStringT    valtype;\n");
                fprintf(tracefile1, "\tsi2drBooleanT   boolval;\n");
                fprintf(tracefile1, "\tsi2drErrorT     err;\n");
                fprintf(
                    tracefile1,
                    "\tsi2drObjectIdT  nulloid = si2drPIGetNullId(&err);\n");
                tracecount = 0;
        }
}

si2drVoidT si2drPISetDebugMode(si2drErrorT *err) {
        liberty___debug_mode = 1;
        *err = SI2DR_NO_ERROR;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPISetDebugMode(&err);\n\n");
                inc_tracecount();
        }
}
si2drVoidT si2drPIUnSetDebugMode(si2drErrorT *err) {
        liberty___debug_mode = 0;
        *err = SI2DR_NO_ERROR;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPIUnSetDebugMode(&err);\n\n");
                inc_tracecount();
        }
}

si2drBooleanT si2drPIGetDebugMode(si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPIGetDebugMode(&err);\n\n");
                inc_tracecount();
        }
        if (liberty___debug_mode != 0)
                return SI2DR_TRUE;
        else
                return SI2DR_FALSE;
}

si2drVoidT si2drPISetNocheckMode(si2drErrorT *err) {
        liberty___nocheck_mode = 1;
        *err = SI2DR_NO_ERROR;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPISetNocheckMode(&err);\n\n");
                inc_tracecount();
        }
}

si2drVoidT si2drPIUnSetNocheckMode(si2drErrorT *err) {
        liberty___nocheck_mode = 0;
        *err = SI2DR_NO_ERROR;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPIUnSetNocheckMode(&err);\n\n");
                inc_tracecount();
        }
}

si2drBooleanT si2drPIGetNocheckMode(si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPIGetNocheckMode(&err);\n\n");
                inc_tracecount();
        }
        if (liberty___nocheck_mode != 0)
                return SI2DR_TRUE;
        else
                return SI2DR_FALSE;
}

si2drVoidT si2drDefaultMessageHandler(si2drSeverityT sev,
                                      si2drErrorT errToPrint,
                                      si2drStringT auxText, si2drErrorT *err) {
        si2drErrorT err1;

        char *sevstr, *errt = si2drPIGetErrorText(errToPrint, &err1);

        switch (sev) {
                case SI2DR_SEVERITY_NOTE:
                        sevstr = "NOTE";
                        printf("%s: %s\n", sevstr, auxText);
                        break;

                case SI2DR_SEVERITY_WARN:
                        sevstr = "WARNING";
                        printf("%s: %s\n", sevstr, auxText);
                        break;

                case SI2DR_SEVERITY_ERR:
                        sevstr = "ERROR";
                        if (auxText)
                                printf("%s: %s (%s)\n", sevstr, auxText, errt);
                        else
                                printf("%s: %s\n", sevstr, errt);
                        break;
        }
        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drPISetMessageHandler(si2drMessageHandlerT handler,
                                    si2drErrorT *err) {
        si2ErrMsg = handler;
        *err = SI2DR_NO_ERROR;
}

si2drMessageHandlerT si2drPIGetMessageHandler(si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        return si2ErrMsg;
}

si2drVoidT si2drPISetTraceMode(si2drStringT fname, si2drErrorT *err) {
        char fnamet[1000];
        char fnameth[1000];

        strcpy(tracefbase, fname);

        sprintf(fnamet, "%s00.c", fname);
        sprintf(fnameth, "%s_oiddecls.h", fname);

        tracefile1 = fopen(fnamet, "w");
        tracefile2 = fopen(fnameth, "w");

        if (!tracefile1 || !tracefile2) {
                trace = 0;
                *err = SI2DR_TRACE_FILES_CANNOT_BE_OPENED;
        } else {
                fprintf(tracefile1, "#include <stdio.h>\n\n");
                fprintf(tracefile1, "#include \"si2dr_liberty.h\"\n");
                fprintf(tracefile1, "#include \"%s_oiddecls.h\"\n\n\n", fname);
                fprintf(tracefile1, "si2drObjectIdT nulloid;\n\n");
                fprintf(tracefile1,
                        "void check_results(si2drObjectIdT result, si2drErrorT "
                        "err, char *fname, int lineno)\n");
                fprintf(tracefile1, "{\n");
                fprintf(tracefile1, "    si2drErrorT err3;\n");
                fprintf(tracefile1, "    \n");
                fprintf(tracefile1, "    if( err == SI2DR_NO_ERROR )\n");
                fprintf(tracefile1, "    {\n");
                fprintf(tracefile1,
                        "        if( !si2drObjectIsUsable(result,&err3) )\n");
                fprintf(tracefile1, "        {\n");
                fprintf(tracefile1,
                        "            printf(\"Unusable OID returned from "
                        "function at \\\"%%s\\\", line %%d\\n\", fname, "
                        "lineno);\n");
                fprintf(tracefile1, "        }\n");
                fprintf(tracefile1, "    }\n");
                fprintf(tracefile1, "}\n\n");

                fprintf(tracefile1, "void func0(void)\n{\n");
                fprintf(tracefile1, "\t/* output decls used later */\n");
                fprintf(tracefile1, "\tsi2drErrorT     err;\n");
                fprintf(tracefile1, "\tsi2drValueTypeT type;\n");
                fprintf(tracefile1, "\tsi2drInt32T     intgr;\n");
                fprintf(tracefile1, "\tsi2drFloat64T   float64;\n");
                fprintf(tracefile1, "\tsi2drStringT    string;\n");
                fprintf(tracefile1, "\tsi2drStringT    name_string;\n");
                fprintf(tracefile1, "\tsi2drStringT    allowed_groups;\n");
                fprintf(tracefile1, "\tsi2drStringT    valtype;\n");
                fprintf(tracefile1, "\tsi2drBooleanT   boolval;\n");
                fprintf(tracefile1, "\tsi2drObjectIdT  nulloid;\n");

                fprintf(tracefile1, "\tnulloid = si2drPIGetNullId(&err);\n\n");
                trace = 1;
                *err = SI2DR_NO_ERROR;
        }
}

si2drVoidT si2drPIUnSetTraceMode(si2drErrorT *err) {
        int i, j, k;

        for (i = k = 0; i < tracefilecount; i++) {
                for (j = 0; j < 6; j++) {
                        fprintf(tracefile1, "extern void func%d(void);\n", k++);
                }
        }

        fprintf(tracefile1, "}\n\n\nint main(int argc, char **argv)\n{\n");
        for (i = 0; i <= tracefunc; i++) {
                fprintf(tracefile1, "\tfunc%d();\n", i);
        }

        fprintf(tracefile1, "\treturn 0;\n}\n");
        fclose(tracefile1);
        fclose(tracefile2);

        *err = SI2DR_NO_ERROR;
}

si2drBooleanT si2drPIGetTraceMode(si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (liberty___trace_mode_CFP != 0)
                return SI2DR_TRUE;
        else
                return SI2DR_FALSE;
}

/* here are the implementations of the functions described in si2dr_liberty.h */

si2drGroupIdT si2drPICreateGroup(si2drStringT name, si2drStringT group_type,
                                 si2drErrorT *err) {
        si2drGroupIdT retoid, toid;
        const libGroupMap *lgm;
        liberty_group *g;
        char nameb[1500], *nnb = 0;
        int nbl = 0;
        int npl = strlen(group_type) + 4;

        if (name) {
                sprintf(nameb, "%s||||%s", group_type, name);
                nbl = strlen(nameb);
        }

        if (!liberty___nocheck_mode) {
                if (!master_group_hash) {
                        *err = SI2DR_PIINIT_NOT_CALLED;
                        return nulloid;
                }

                if (name && *name) {
                        liberty_hash_lookup(master_group_hash, name, &toid);
                        if (toid.v1 != (void *)0) {
                                *err = SI2DR_OBJECT_ALREADY_EXISTS;
                                if (liberty___debug_mode) {
                                        si2drErrorT err2;

                                        (*si2ErrMsg)(
                                            SI2DR_SEVERITY_ERR,
                                            SI2DR_OBJECT_ALREADY_EXISTS,
                                            "si2drPICreateGroup: ", &err2);
                                }
                                return nulloid;
                        }
                }
        }

        g = (liberty_group *)my_calloc(sizeof(liberty_group), 1);
        retoid.v1 = (void *)SI2DR_GROUP;
        retoid.v2 = (void *)g;

        if (trace) {
                outinit_oid(retoid);

                fprintf(
                    tracefile1,
                    "\n\t%s = si2drPICreateGroup(\"%s\", \"%s\", &err);\n\n",
                    oid_string(retoid), name, group_type);
                trace_check(retoid);
                inc_tracecount();
        }

        g->names = (liberty_name_list *)my_calloc(sizeof(liberty_name_list), 1);
        g->last_name = g->names;
        g->type =
            liberty_strtable_enter_string(master_string_table, group_type);
        g->names->prefix_len = npl;
        lgm = lookup_group_name(group_type, strlen(group_type));
        if (lgm) g->EVAL = lgm->type;

        if (name && *name) {
                nnb = g->names->name =
                    liberty_strtable_enter_string(master_string_table, nameb);
        } else
                g->names->name = 0;

        /* set up hash tables */

        g->attr_hash = liberty_hash_create_hash_table(503, 1, 0);

        g->define_hash = liberty_hash_create_hash_table(53, 1, 0);

        g->group_hash = liberty_hash_create_hash_table(2011, 1, 0);

        /* link into the master list */

        g->next = master_group_list;

        master_group_list = g;

        if (name && *name) {
                /* put this name(s) into the master hash tab */

                liberty_hash_enter_oid(master_group_hash, nnb, retoid);
        }
        *err = SI2DR_NO_ERROR;
        return retoid;
}

si2drAttrIdT si2drGroupCreateAttr(si2drGroupIdT group, si2drStringT name,
                                  si2drAttrTypeT type, si2drErrorT *err) {
        si2drAttrIdT retoid, toid;
        const libAttrMap *lam;
        liberty_group *g = (liberty_group *)group.v2;
        liberty_attribute *attr;

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupCreateAttr:", &err2);
                        }
                        return nulloid;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupCreateAttr:", &err2);
                        }
                        return nulloid;
                }

                if (name && *name) {
                        liberty_hash_lookup(g->attr_hash, name, &toid);
                        if (toid.v1 != (void *)0 &&
                            strcmp(name, "define_group") &&
                            strcmp(name, "curve_y") &&
                            strcmp(name, "library_features") &&
                            strcmp(name, "vector") &&
                            strcmp(name, "power_rail") &&
                            strcmp(name, "voltage_map")) {
                                *err = SI2DR_OBJECT_ALREADY_EXISTS;
                                if (liberty___debug_mode) {
                                        si2drErrorT err2;

                                        (*si2ErrMsg)(
                                            SI2DR_SEVERITY_ERR,
                                            SI2DR_OBJECT_ALREADY_EXISTS,
                                            "si2drGroupCreateAttr:", &err2);
                                }
                                return nulloid;
                        }
                } else {
                        *err = SI2DR_INVALID_NAME;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_NAME,
                                             "si2drGroupCreateAttr:", &err2);
                        }
                        return nulloid;
                }
                if (type != SI2DR_SIMPLE && type != SI2DR_COMPLEX) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_ATTRTYPE,
                                             "si2drGroupCreateAttr:", &err2);
                        }
                        return nulloid;
                }
        }

        attr = (liberty_attribute *)my_calloc(sizeof(liberty_attribute), 1);
        retoid.v1 = (void *)SI2DR_ATTR;
        retoid.v2 = (void *)attr;

        if (trace) {
                outinit_oid(retoid);

                fprintf(
                    tracefile1,
                    "\n\t%s = si2drGroupCreateAttr(%s,\"%s\", %s, &err);\n\n",
                    oid_string(retoid), oid_string(group), name,
                    at_string(type));
                trace_check(retoid);
                inc_tracecount();
        }
        attr->owner = g;

        attr->name = liberty_strtable_enter_string(master_string_table, name);
        lam = lookup_attr_name(name, strlen(name));
        if (lam) attr->EVAL = lam->type;

        switch (type) {
                case SI2DR_SIMPLE:
                        attr->type = LIBERTY__SIMPLE;

                        break;

                case SI2DR_COMPLEX:
                        attr->type = LIBERTY__COMPLEX;

                        break;
        }
        liberty_hash_enter_oid(g->attr_hash, attr->name, retoid);

        if (g->attr_last) {
                g->attr_last->next = attr;
                g->attr_last = attr;
        } else {
                g->attr_last = attr;
                g->attr_list = attr;
        }

        *err = SI2DR_NO_ERROR;
        return retoid;
}

/* Functions to handle comments */

si2drStringT si2drGroupGetComment(si2drGroupIdT group, si2drErrorT *err) {
        liberty_group *g = (liberty_group *)group.v2;
        return (si2drStringT)g->comment;
}

si2drVoidT si2drGroupSetComment(si2drGroupIdT group, si2drStringT comment,
                                si2drErrorT *err) {
        liberty_group *g = (liberty_group *)group.v2;
        g->comment =
            liberty_strtable_enter_string(master_string_table, comment);
}

si2drStringT si2drAttrGetComment(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *a = (liberty_attribute *)attr.v2;
        return (si2drStringT)a->comment;
}

si2drVoidT si2drAttrSetComment(si2drAttrIdT attr, si2drStringT comment,
                               si2drErrorT *err) {
        liberty_attribute *a = (liberty_attribute *)attr.v2;
        a->comment =
            liberty_strtable_enter_string(master_string_table, comment);
}

si2drStringT si2drDefineGetComment(si2drDefineIdT def, si2drErrorT *err) {
        liberty_define *d = (liberty_define *)def.v2;
        return (si2drStringT)d->comment;
}

si2drVoidT si2drDefineSetComment(si2drDefineIdT def, si2drStringT comment,
                                 si2drErrorT *err) {
        liberty_define *d = (liberty_define *)def.v2;
        d->comment =
            liberty_strtable_enter_string(master_string_table, comment);
}

group_enum si2drGroupGetID(si2drGroupIdT group, si2drErrorT *err) {
        liberty_group *g = (liberty_group *)group.v2;
        return g->EVAL;
}

attr_enum si2drAttrGetID(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *a = (liberty_attribute *)attr.v2;
        return a->EVAL;
}

si2drAttrTypeT si2drAttrGetAttrType(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drAttrGetAttrType(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }

        if (!liberty___nocheck_mode &&
            (si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                     SI2DR_INVALID_OBJECTTYPE,
                                     "si2drAttrGetAttrType:", &err2);
                }
                return 0;
        }
        *err = SI2DR_NO_ERROR;
        switch (x->type) {
                case LIBERTY__SIMPLE:
                        return SI2DR_SIMPLE;

                case LIBERTY__COMPLEX:
                        return SI2DR_COMPLEX;
        }
        *err = SI2DR_INVALID_OBJECTTYPE;
        return 0;
}

si2drVoidT si2drComplexAttrAddInt32Value(si2drAttrIdT attr, si2drInt32T intgr,
                                         si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        liberty_attribute_value *y;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drComplexAttrAddInt32Value(%s,%d,&err);\n\n",
                        oid_string(attr), (int)intgr);
                inc_tracecount();
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drComplexAttrAddInt32Value:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__COMPLEX) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drComplexAttrAddInt32Value:", &err2);
                        }
                        return;
                }
        }

        y = (liberty_attribute_value *)my_calloc(
            sizeof(liberty_attribute_value), 1);
        y->type = LIBERTY__VAL_INT;
        y->u.int_val = intgr;

        /* link the attr struct into the lists */
        if (x->last_value) {
                x->last_value->next = y;
                x->last_value = y;
        } else {
                x->last_value = y;
                x->value = y;
        }
        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drComplexAttrAddStringValue(si2drAttrIdT attr,
                                          si2drStringT string,
                                          si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        liberty_attribute_value *y;

        if (trace) {
                fprintf(
                    tracefile1,
                    "\n\tsi2drComplexAttrAddStringValue(%s,\"%s\",&err);\n\n",
                    oid_string(attr), string);
                inc_tracecount();
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drComplexAttrAddStringValue:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__COMPLEX) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drComplexAttrAddStringValue:", &err2);
                        }
                        return;
                }
        }

        y = (liberty_attribute_value *)my_calloc(
            sizeof(liberty_attribute_value), 1);
        y->type = LIBERTY__VAL_STRING;
        y->u.string_val =
            liberty_strtable_enter_string(master_string_table, string);

        /* link the attr struct into the lists */
        if (x->last_value) {
                x->last_value->next = y;
                x->last_value = y;
        } else {
                x->last_value = y;
                x->value = y;
        }

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drComplexAttrAddBooleanValue(si2drAttrIdT attr,
                                           si2drBooleanT boolval,
                                           si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        liberty_attribute_value *y;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drComplexAttrAddBooleanValue(%s,%d,&err);\n\n",
                        oid_string(attr), boolval);
                inc_tracecount();
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drComplexAttrAddBooleanValue:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__COMPLEX) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drComplexAttrAddBooleanValue:", &err2);
                        }
                        return;
                }
        }

        y = (liberty_attribute_value *)my_calloc(
            sizeof(liberty_attribute_value), 1);
        y->type = LIBERTY__VAL_BOOLEAN;
        y->u.int_val = boolval;

        /* link the attr struct into the lists */
        if (x->last_value) {
                x->last_value->next = y;
                x->last_value = y;
        } else {
                x->last_value = y;
                x->value = y;
        }

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drComplexAttrAddFloat64Value(si2drAttrIdT attr,
                                           si2drFloat64T float64,
                                           si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        liberty_attribute_value *y;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drComplexAttrAddFloat64Value(%s,%g,&err);\n\n",
                        oid_string(attr), float64);
                inc_tracecount();
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drComplexAttrAddFloat64Value:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__COMPLEX) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drComplexAttrAddFloat64Value:", &err2);
                        }
                        return;
                }
        }

        y = (liberty_attribute_value *)my_calloc(
            sizeof(liberty_attribute_value), 1);
        y->type = LIBERTY__VAL_DOUBLE;
        y->u.double_val = float64;

        /* link the attr struct into the lists */
        if (x->last_value) {
                x->last_value->next = y;
                x->last_value = y;
        } else {
                x->last_value = y;
                x->value = y;
        }

        *err = SI2DR_NO_ERROR;
}

si2drValuesIdT si2drComplexAttrGetValues(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        iterat *y;

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drComplexAttrGetValues:",
                                             &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__COMPLEX) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drComplexAttrGetValues:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;
        l__iter_val_count++;

        y = (iterat *)my_calloc(sizeof(iterat), 1);
        y->owner = ITER_VAL;
        y->next = x->value;
        if (trace) {
                outinit_iter((si2drValuesIdT)y);

                fprintf(tracefile1,
                        "\n\t%s = si2drComplexAttrGetValues(%s, &err);\n\n",
                        iter_string((si2drIterIdT)y), oid_string(attr));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        return (si2drValuesIdT)y;
}

si2drVoidT si2drIterNextComplexValue(si2drValuesIdT iter, si2drValueTypeT *type,
                                     si2drInt32T *intgr, si2drFloat64T *float64,
                                     si2drStringT *string,
                                     si2drBooleanT *boolval, si2drExprT **expr,
                                     si2drErrorT *err) {
        iterat *y = (iterat *)iter;
        liberty_attribute_value *z;
        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drIterNextComplexValue(%s,&type,&intgr,&"
                        "float64,&string,&boolval,&expr,&err);\n\n",
                        iter_string(iter));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if (y->owner != ITER_VAL) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drIterNextComplexValue:",
                                             &err2);
                        }
                        *intgr = 0;
                        *expr = 0;
                        *float64 = 0.0;
                        *string = 0;
                        *boolval = 0;
                        return;
                }
        }

        if (y->next == (liberty_attribute_value *)NULL) {
                *type = SI2DR_UNDEFINED_VALUETYPE;
                *intgr = 0;
                *expr = 0;
                *float64 = 0.0;
                *string = 0;
                *boolval = 0;
                *err = SI2DR_NO_ERROR;
                return;
        }

        z = y->next;
        switch (z->type) {
                case LIBERTY__VAL_STRING:
                        *type = SI2DR_STRING;
                        *string = z->u.string_val;
                        *intgr = 0;
                        *float64 = 0.0;
                        *boolval = 0;
                        break;

                case LIBERTY__VAL_EXPR:
                        *type = SI2DR_EXPR;
                        *expr = z->u.expr_val;
                        *string = 0;
                        *intgr = 0;
                        *float64 = 0.0;
                        *boolval = 0;
                        break;

                case LIBERTY__VAL_DOUBLE:
                        *type = SI2DR_FLOAT64;
                        *expr = 0;
                        *string = 0;
                        *intgr = 0;
                        *float64 = z->u.double_val;
                        *boolval = 0;
                        break;

                case LIBERTY__VAL_BOOLEAN:
                        *type = SI2DR_BOOLEAN;
                        *string = 0;
                        *expr = 0;
                        *intgr = 0;
                        *float64 = 0.0;
                        *boolval = z->u.int_val;
                        break;

                case LIBERTY__VAL_INT:
                        *type = SI2DR_INT32;
                        *string = 0;
                        *expr = 0;
                        *intgr = z->u.int_val;
                        ;
                        *float64 = 0.0;
                        *boolval = 0;
                        break;
                case LIBERTY__VAL_UNDEFINED:
                        *type = SI2DR_UNDEFINED_VALUETYPE;
                        *intgr = 0;
                        *float64 = 0.0;
                        *string = 0;
                        *expr = 0;
                        *boolval = 0;
                        *err = SI2DR_NO_ERROR;
                        break;
        }

        y->next = z->next;

        *err = SI2DR_NO_ERROR;
}

si2drValueTypeT si2drSimpleAttrGetValueType(si2drAttrIdT attr,
                                            si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrGetValueType (%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrGetValueType:",
                                             &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrGetValueType:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;

        if (x->value == (liberty_attribute_value *)NULL) {
                return SI2DR_UNDEFINED_VALUETYPE;
        }

        switch (x->value->type) {
                case LIBERTY__VAL_STRING:
                        return SI2DR_STRING;
                case LIBERTY__VAL_DOUBLE:
                        return SI2DR_FLOAT64;
                case LIBERTY__VAL_BOOLEAN:
                        return SI2DR_BOOLEAN;
                case LIBERTY__VAL_INT:
                        return SI2DR_INT32;
                case LIBERTY__VAL_EXPR:
                        return SI2DR_EXPR;
                default:
                        break;
        }
        return SI2DR_UNDEFINED_VALUETYPE;
}

si2drInt32T si2drSimpleAttrGetInt32Value(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrGetInt32Value(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrGetInt32Value:",
                                             &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrGetInt32Value:", &err2);
                        }
                        return 0;
                }
                if (x->value == (liberty_attribute_value *)NULL) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetInt32Value:", &err2);
                        }
                        return 0;
                }
                if (x->value->type != LIBERTY__VAL_INT) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetInt32Value:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;
        return x->value->u.int_val;
}

si2drFloat64T si2drSimpleAttrGetFloat64Value(si2drAttrIdT attr,
                                             si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrGetFloat64Value(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrGetFloat64Value:",
                                             &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrGetFloat64Value:", &err2);
                        }
                        return 0;
                }
                if (x->value == (liberty_attribute_value *)NULL) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetFloat64Value:", &err2);
                        }
                        return 0;
                }
                if (x->value->type != LIBERTY__VAL_DOUBLE) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetFloat64Value:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;
        return (si2drFloat64T)x->value->u.double_val;
}

si2drStringT si2drSimpleAttrGetStringValue(si2drAttrIdT attr,
                                           si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrGetStringValue(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrGetStringValue:",
                                             &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrGetStringValue:", &err2);
                        }
                        return 0;
                }
                if (x->value == (liberty_attribute_value *)NULL) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetStringValue:", &err2);
                        }
                        return 0;
                }
                if (x->value->type != LIBERTY__VAL_STRING) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetStringValue:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;
        return x->value->u.string_val;
}

si2drBooleanT si2drSimpleAttrGetBooleanValue(si2drAttrIdT attr,
                                             si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrGetBooleanValue(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrGetBooleanValue:",
                                             &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrGetBooleanValue:", &err2);
                        }
                        return 0;
                }
                if (x->value == (liberty_attribute_value *)NULL) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetBooleanValue:", &err2);
                        }
                        return 0;
                }
                if (x->value->type != LIBERTY__VAL_BOOLEAN) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetBooleanValue:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;
        return x->value->u.bool_val;
}

si2drBooleanT si2drSimpleAttrGetIsVar(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drSimpleAttrGetIsVar(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrGetIsVar:", &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_ATTRTYPE,
                                             "si2drSimpleAttrGetIsVar:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;
        return x->is_var;
}

si2drVoidT si2drSimpleAttrSetInt32Value(si2drAttrIdT attr, si2drInt32T intgr,
                                        si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrSetInt32Value(%s,%d,&err);\n\n",
                        oid_string(attr), (int)intgr);
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrSetInt32Value:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrSetInt32Value:", &err2);
                        }
                        return;
                }
        }

        if (x->value == (liberty_attribute_value *)NULL) {
                x->value = (liberty_attribute_value *)my_calloc(
                    sizeof(liberty_attribute_value), 1);
        }
        x->value->type = LIBERTY__VAL_INT;
        x->value->u.int_val = intgr;

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drSimpleAttrSetBooleanValue(si2drAttrIdT attr,
                                          si2drBooleanT boolval,
                                          si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrSetBooleanValue(%s,%d,&err);\n\n",
                        oid_string(attr), boolval);
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrSetBooleanValue:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrSetBooleanValue:", &err2);
                        }
                        return;
                }
        }

        if (x->value == (liberty_attribute_value *)NULL) {
                x->value = (liberty_attribute_value *)my_calloc(
                    sizeof(liberty_attribute_value), 1);
        }
        x->value->type = LIBERTY__VAL_BOOLEAN;
        x->value->u.bool_val = boolval;

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drSimpleAttrSetIsVar(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drSimpleAttrSetIsVar(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrSetIsVar:", &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_ATTRTYPE,
                                             "si2drSimpleAttrSetIsVar:", &err2);
                        }
                        return;
                }
        }

        x->is_var = 1;

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drSimpleAttrSetFloat64Value(si2drAttrIdT attr,
                                          si2drFloat64T float64,
                                          si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrSetFloat64Value(%s,%g,&err);\n\n",
                        oid_string(attr), float64);
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrSetFloat64Value:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrSetFloat64Value:", &err2);
                        }
                        return;
                }
        }

        if (x->value == (liberty_attribute_value *)NULL) {
                x->value = (liberty_attribute_value *)my_calloc(
                    sizeof(liberty_attribute_value), 1);
        }
        x->value->type = LIBERTY__VAL_DOUBLE;
        x->value->u.double_val = float64;

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drSimpleAttrSetStringValue(si2drAttrIdT attr, si2drStringT string,
                                         si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(
                    tracefile1,
                    "\n\tsi2drSimpleAttrSetStringValue(%s,\"%s\",&err);\n\n",
                    oid_string(attr), string);
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrSetStringValue:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrSetStringValue:", &err2);
                        }
                        return;
                }
        }

        if (x->value == (liberty_attribute_value *)NULL) {
                x->value = (liberty_attribute_value *)my_calloc(
                    sizeof(liberty_attribute_value), 1);
        }
        x->value->type = LIBERTY__VAL_STRING;
        x->value->u.string_val =
            liberty_strtable_enter_string(master_string_table, string);

        *err = SI2DR_NO_ERROR;
}

si2drDefineIdT si2drGroupCreateDefine(si2drGroupIdT group, si2drStringT name,
                                      si2drStringT allowed_group_name,
                                      si2drValueTypeT valtype,
                                      si2drErrorT *err) {
        si2drDefineIdT retoid, toid;
        liberty_group *g = (liberty_group *)group.v2;
        liberty_define *d;

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupCreateDefine:", &err2);
                        }
                        return nulloid;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupCreateDefine:", &err2);
                        }
                        return nulloid;
                }
        }
        if (name && *name) {
                int found;

                liberty_hash_lookup(g->define_hash, name, &toid);
                if (toid.v1 != (void *)0) {
                        char buf[10000], *p, *q;
                        d = (liberty_define *)toid.v2;

                        strcpy(buf, d->group_type);
                        found = 0;
                        q = buf;

                        if (!strcmp(d->group_type, allowed_group_name)) {
                                found = 1;
                        }
                        if (!found) {
                                while ((p = strchr(q, '|'))) {
                                        *p = 0;
                                        if (!strcmp(q, allowed_group_name)) {
                                                found = 1;
                                                break;
                                        }
                                        q = p + 1;
                                }
                                if (!strcmp(q, allowed_group_name)) found = 1;
                        }
                        if (found) {
                                *err = SI2DR_OBJECT_ALREADY_EXISTS;
                                if (liberty___debug_mode) {
                                        si2drErrorT err2;

                                        (*si2ErrMsg)(
                                            SI2DR_SEVERITY_ERR,
                                            SI2DR_OBJECT_ALREADY_EXISTS,
                                            "si2drGroupCreateDefine:", &err2);
                                }
                                return nulloid;
                        } else {
                                char *x = (char *)alloca(
                                    strlen(d->group_type) +
                                    strlen(allowed_group_name) + 2);
                                strcpy(x, d->group_type);
                                strcat(x, "|");
                                strcat(x, allowed_group_name);
                                d->group_type = liberty_strtable_enter_string(
                                    master_string_table, x);
                                *err = SI2DR_NO_ERROR;
                                return toid;
                        }
                }
        } else {
                *err = SI2DR_INVALID_NAME;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                                     "si2drGroupCreateDefine:", &err2);
                }
                return nulloid;
        }

        d = (liberty_define *)my_calloc(sizeof(liberty_define), 1);

        retoid.v1 = (void *)SI2DR_DEFINE;
        retoid.v2 = (void *)d;

        if (trace) {
                outinit_oid(retoid);

                fprintf(tracefile1,
                        "\n\t%s = si2drGroupCreateDefine(%s, \"%s\", \"%s\", "
                        "%s, &err);\n\n",
                        oid_string(retoid), oid_string(group), name,
                        allowed_group_name, vt_string(valtype));
                trace_check(retoid);
                inc_tracecount();
        }

        d->name = liberty_strtable_enter_string(master_string_table, name);
        d->owner = g;

        d->group_type = liberty_strtable_enter_string(master_string_table,
                                                      allowed_group_name);
        switch (valtype) {
                case SI2DR_INT32:
                        d->valtype = LIBERTY__VAL_INT;
                        break;

                case SI2DR_UNDEFINED_VALUETYPE:
                case SI2DR_MAX_VALUETYPE:
                default:
                        d->valtype = LIBERTY__VAL_UNDEFINED;
                        break;

                case SI2DR_STRING:
                        d->valtype = LIBERTY__VAL_STRING;
                        break;

                case SI2DR_FLOAT64:
                        d->valtype = LIBERTY__VAL_DOUBLE;
                        break;

                case SI2DR_BOOLEAN:
                        d->valtype = LIBERTY__VAL_BOOLEAN;
                        break;
        }

        liberty_hash_enter_oid(g->define_hash, d->name, retoid);
        liberty_hash_enter_oid(master_define_hash, d->name, retoid);

        /* link the attr struct into the lists */
        if (g->define_last) {
                g->define_last->next = d;
                g->define_last = d;
        } else {
                g->define_last = d;
                g->define_list = d;
        }

        *err = SI2DR_NO_ERROR;
        return retoid;
}

si2drGroupIdT si2drGroupCreateGroup(si2drGroupIdT group, si2drStringT name,
                                    si2drStringT group_type, si2drErrorT *err) {
        si2drGroupIdT retoid, toid;
        liberty_group *g = (liberty_group *)group.v2;
        liberty_group *ng;
        const libGroupMap *lgm;
        char nameb[1500], *nnb = 0;
        int nbl = 0, dont_addhash = 0;
        int npl = strlen(group_type) + 4;

        if (name) {
                sprintf(nameb, "%s||||%s", group_type, name);
                nbl = strlen(nameb);
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupCreateGroup:", &err2);
                        }
                        return nulloid;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupCreateGroup:", &err2);
                        }
                        return nulloid;
                }

                if (group_type == 0 || *group_type == 0) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_VALUE,
                                             "si2drGroupCreateGroup:", &err2);
                        }
                        return nulloid;
                }
        }
        lgm = lookup_group_name(group_type, strlen(group_type));
        if (name && *name) {
                liberty_hash_lookup(g->define_hash, nameb, &toid);
                if (toid.v1 != (void *)0) {
                        /* disqualify any references here */
                        switch (lgm->type) {
                                case LIBERTY_GROUPENUM_ff_bank:
                                        if (g->names && g->names->next)
                                                dont_addhash = 1;
                                        break;

                                case LIBERTY_GROUPENUM_internal_power:
                                case LIBERTY_GROUPENUM_cell_degradation:
                                case LIBERTY_GROUPENUM_cell_fall:
                                case LIBERTY_GROUPENUM_cell_rise:
                                case LIBERTY_GROUPENUM_fall_constraint:
                                case LIBERTY_GROUPENUM_fall_propagation:
                                case LIBERTY_GROUPENUM_fall_transition:
                                case LIBERTY_GROUPENUM_retaining_fall:
                                case LIBERTY_GROUPENUM_retaining_rise:
                                case LIBERTY_GROUPENUM_rise_constraint:
                                case LIBERTY_GROUPENUM_rise_propagation:
                                case LIBERTY_GROUPENUM_rise_transition:
                                case LIBERTY_GROUPENUM_em_max_toggle_rate:
                                case LIBERTY_GROUPENUM_fall_power:
                                case LIBERTY_GROUPENUM_power:
                                case LIBERTY_GROUPENUM_rise_power:
                                case LIBERTY_GROUPENUM_fall_net_delay:
                                case LIBERTY_GROUPENUM_fall_transition_degradation:
                                case LIBERTY_GROUPENUM_rise_net_delay:
                                case LIBERTY_GROUPENUM_rise_transition_degradation:
                                case LIBERTY_GROUPENUM_interconnect_delay:
                                case LIBERTY_GROUPENUM_scaled_cell:
                                        dont_addhash = 1;
                                        break;

                                default:
                                        break;
                        }

                        if (!dont_addhash) {
                                *err = SI2DR_OBJECT_ALREADY_EXISTS;
                                if (liberty___debug_mode) {
                                        si2drErrorT err2;

                                        (*si2ErrMsg)(
                                            SI2DR_SEVERITY_ERR,
                                            SI2DR_OBJECT_ALREADY_EXISTS,
                                            "si2drGroupCreateGroup:", &err2);
                                }
                                return nulloid;
                        }
                }
        }
        ng = (liberty_group *)my_calloc(sizeof(liberty_group), 1);
        if (lgm) ng->EVAL = lgm->type;

        retoid.v1 = (void *)SI2DR_GROUP;
        retoid.v2 = (void *)ng;

        if (trace) {
                outinit_oid(retoid);

                fprintf(tracefile1,
                        "\n\t%s = si2drGroupCreateGroup(%s, \"%s\", \"%s\", "
                        "&err);\n\n",
                        oid_string(retoid), oid_string(group), name,
                        group_type);
                trace_check(retoid);
                inc_tracecount();
        }

        ng->names =
            (liberty_name_list *)my_calloc(sizeof(liberty_name_list), 1);
        ng->last_name = ng->names;
        ng->owner = g;
        ng->names->prefix_len = npl;

        if (name && *name) {
                nnb = ng->names->name =
                    liberty_strtable_enter_string(master_string_table, nameb);
        } else
                ng->names->name = 0;

        ng->type =
            liberty_strtable_enter_string(master_string_table, group_type);
        lgm = lookup_group_name(group_type, strlen(group_type));
        if (lgm) ng->EVAL = lgm->type;

        /* set up hash tables */

        ng->attr_hash = liberty_hash_create_hash_table(3, 1, 0);

        ng->define_hash = liberty_hash_create_hash_table(3, 1, 0);

        ng->group_hash = liberty_hash_create_hash_table(3, 1, 0);

        if (name && *name && !dont_addhash)
                liberty_hash_enter_oid(g->group_hash, nnb, retoid);

        /* link the attr struct into the lists */
        if (g->group_last) {
                g->group_last->next = ng;
                g->group_last = ng;
        } else {
                g->group_last = ng;
                g->group_list = ng;
        }

        *err = SI2DR_NO_ERROR;
        return retoid;
}

si2drVoidT si2drGroupAddName(si2drGroupIdT group, si2drStringT name,
                             si2drErrorT *err) {
        si2drGroupIdT toid;
        int dont_addhash;
        liberty_group *g = (liberty_group *)group.v2;
        liberty_group *gp = g->owner;
        liberty_name_list *nlp;
        char nameb[1500], *nnb;
        int nbl;
        int npl = strlen(g->type) + 4;

        sprintf(nameb, "%s||||%s", g->type, name);
        nbl = strlen(nameb);
        *err = SI2DR_NO_ERROR;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drGroupAddName(%s,\"%s\",&err);\n\n",
                        oid_string(group), name);
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupAddName:", &err2);
                        }
                        return;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupAddName:", &err2);
                        }
                        return;
                }
        }
        dont_addhash = 0;

        if (name && *name) {
                if (gp)
                        liberty_hash_lookup(gp->group_hash, nameb, &toid);
                else
                        liberty_hash_lookup(master_group_hash, nameb, &toid);
                if (toid.v1 != (void *)0) {
                        /* disqualify any references here */
                        switch ((group_enum)g->EVAL) {
                                case LIBERTY_GROUPENUM_ff_bank:
                                        if (g->names && g->names->next)
                                                dont_addhash = 1;
                                        break;

                                case LIBERTY_GROUPENUM_internal_power:
                                case LIBERTY_GROUPENUM_cell_degradation:
                                case LIBERTY_GROUPENUM_cell_fall:
                                case LIBERTY_GROUPENUM_cell_rise:
                                case LIBERTY_GROUPENUM_fall_constraint:
                                case LIBERTY_GROUPENUM_fall_propagation:
                                case LIBERTY_GROUPENUM_fall_transition:
                                case LIBERTY_GROUPENUM_retaining_fall:
                                case LIBERTY_GROUPENUM_retaining_rise:
                                case LIBERTY_GROUPENUM_rise_constraint:
                                case LIBERTY_GROUPENUM_rise_propagation:
                                case LIBERTY_GROUPENUM_rise_transition:
                                case LIBERTY_GROUPENUM_em_max_toggle_rate:
                                case LIBERTY_GROUPENUM_fall_power:
                                case LIBERTY_GROUPENUM_power:
                                case LIBERTY_GROUPENUM_rise_power:
                                case LIBERTY_GROUPENUM_fall_net_delay:
                                case LIBERTY_GROUPENUM_fall_transition_degradation:
                                case LIBERTY_GROUPENUM_rise_net_delay:
                                case LIBERTY_GROUPENUM_rise_transition_degradation:
                                case LIBERTY_GROUPENUM_interconnect_delay:
                                case LIBERTY_GROUPENUM_scaled_cell:
                                        dont_addhash = 1;
                                        break;

                                default:
                                        break;
                        }
                        if (!dont_addhash) {
                                *err = SI2DR_OBJECT_ALREADY_EXISTS;

                                if (liberty___debug_mode) {
                                        si2drErrorT err2;

                                        (*si2ErrMsg)(
                                            SI2DR_SEVERITY_ERR,
                                            SI2DR_OBJECT_ALREADY_EXISTS,
                                            "si2drGroupAddName:", &err2);
                                }
                        }
                }
        } else {
                *err = SI2DR_INVALID_NAME;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                                     "si2drGroupAddName:", &err2);
                }
                return;
        }

        nnb = liberty_strtable_enter_string(master_string_table, nameb);

        if (*err != SI2DR_OBJECT_ALREADY_EXISTS && !dont_addhash) {
                if (gp)
                        liberty_hash_enter_oid(gp->group_hash, nnb, group);
                else
                        liberty_hash_enter_oid(master_group_hash, nnb, group);
        }

        if (g->names && g->names->name == (char *)NULL) {
                g->names->name = nnb;
                g->names->prefix_len = npl;
                return;
        }
        nlp = (liberty_name_list *)my_calloc(sizeof(liberty_name_list), 1);
        nlp->name = nnb;
        nlp->prefix_len = npl;

        if (g->last_name) {
                g->last_name->next = nlp;
                g->last_name = nlp;
        } else {
                g->names = nlp;
                g->last_name = nlp;
        }
}

si2drVoidT si2drGroupDeleteName(si2drGroupIdT group, si2drStringT name,
                                si2drErrorT *err) {
        si2drGroupIdT toid;
        liberty_group *g = (liberty_group *)group.v2;
        liberty_group *gp = g->owner;
        liberty_name_list *nlp, *nlp_last;
        char nameb[1500];
        int nbl = 0;

        sprintf(nameb, "%s||||%s", g->type, name);
        nbl = strlen(nameb);

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drGroupDeleteName(%s,\"%s\",&err);\n\n",
                        oid_string(group), name);
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupDeleteName:", &err2);
                        }
                        return;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupDeleteName:", &err2);
                        }
                        return;
                }

                if (name && *name) {
                        if (gp)
                                liberty_hash_lookup(gp->group_hash, nameb,
                                                    &toid);
                        else
                                liberty_hash_lookup(master_group_hash, nameb,
                                                    &toid);

                        if (toid.v1 == (void *)0 || toid.v2 != g) {
                                *err = SI2DR_OBJECT_NOT_FOUND;
                                if (liberty___debug_mode) {
                                        si2drErrorT err2;

                                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                                     SI2DR_OBJECT_NOT_FOUND,
                                                     "si2drGroupDeleteName:",
                                                     &err2);
                                }
                                return;
                        }
                } else {
                        *err = SI2DR_INVALID_NAME;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_NAME,
                                             "si2drGroupDeleteName:", &err2);
                        }
                        return;
                }
        }

        /* linear search thru g's namelist for this name and kill it */
        nlp_last = 0;

        for (nlp = g->names; nlp; nlp = nlp->next) {
                if (!strcmp(nlp->name, nameb)) {
                        if (!nlp_last) {
                                g->names = nlp->next;
                        } else {
                                nlp_last->next = nlp->next;
                        }
                        if (nlp == g->last_name) {
                                g->last_name = nlp_last;
                        }
                        if (gp)
                                liberty_hash_delete_elem(gp->group_hash, nameb);
                        else
                                liberty_hash_delete_elem(master_group_hash,
                                                         nameb);

                        nlp->name = 0;
                        nlp->next = 0;

                        my_free(nlp);
                        break;
                }
                nlp_last = nlp;
        }
        *err = SI2DR_NO_ERROR;
}

si2drGroupIdT si2drPIFindGroupByName(si2drStringT name, si2drStringT type,
                                     si2drErrorT *err) {
        si2drGroupIdT retoid;
        char nameb[1500];
        int nbl = 0;

        *err = SI2DR_NO_ERROR;
        sprintf(nameb, "%s||||%s", type, name);
        nbl = strlen(nameb);

        if (!liberty___nocheck_mode) {
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drPIFindGroupByName:", &err2);
                        }
                        return nulloid;
                }
        }

        if (name && *name) {
                liberty_hash_lookup(master_group_hash, nameb, &retoid);
                if (retoid.v1 == (void *)0) {
                        *err = SI2DR_OBJECT_NOT_FOUND;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_NOTE, SI2DR_OBJECT_NOT_FOUND,
                                    "si2drPIFindGroupByName: Object Not Found",
                                    &err2);
                        }
                        retoid = nulloid;
                }
        } else {
                *err = SI2DR_INVALID_NAME;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                                     "si2drPIFindGroupByName:", &err2);
                }
                retoid = nulloid;
        }
        if (trace) {
                if (retoid.v1 == 0 && retoid.v2 == 0) {
                        fprintf(tracefile1,
                                "\n\tsi2drPIFindGroupByName(\"%s\", \"%s\", "
                                "&err);\n\n",
                                name, type);
                } else {
                        outinit_oid(retoid);
                        fprintf(tracefile1,
                                "\n\t%s = si2drPIFindGroupByName(\"%s\", "
                                "\"%s\", &err);\n\n",
                                oid_string(retoid), name, type);
                }

                trace_check(retoid);
                inc_tracecount();
        }

        return retoid;
}

si2drGroupIdT si2drGroupFindGroupByName(si2drGroupIdT group, si2drStringT name,
                                        si2drStringT type, si2drErrorT *err) {
        si2drGroupIdT retoid;
        liberty_group *g = (liberty_group *)group.v2;
        char nameb[1500];

        sprintf(nameb, "%s||||%s", type, name);

        *err = SI2DR_NO_ERROR;

        if (!liberty___nocheck_mode) {
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupFindGroupByName:",
                                             &err2);
                        }
                        return nulloid;
                }
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupFindGroupByName:",
                                             &err2);
                        }
                        return nulloid;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_UNUSABLE_OID,
                                    "si2drGroupFindGroupByName:", &err2);
                        }
                        return nulloid;
                }
        }
        *err = SI2DR_NO_ERROR;

        if (name && *name) {
                liberty_hash_lookup(g->group_hash, nameb, &retoid);
                if (retoid.v1 == (void *)0) {
                        *err = SI2DR_OBJECT_NOT_FOUND;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_NOTE,
                                             SI2DR_OBJECT_NOT_FOUND,
                                             "si2drGroupFindGroupByName: "
                                             "Object Not Found",
                                             &err2);
                        }
                        retoid = nulloid;
                }
        } else {
                *err = SI2DR_INVALID_NAME;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                                     "si2drGroupFindGroupByName:", &err2);
                }
                retoid = nulloid;
        }
        if (trace) {
                if (retoid.v1 == 0 && retoid.v2 == 0) {
                        fprintf(tracefile1,
                                "\n\tsi2drGroupFindGroupByName(%s, \"%s\", "
                                "\"%s\", &err);\n\n",
                                oid_string(group), name, type);
                } else {
                        outinit_oid(retoid);
                        fprintf(tracefile1,
                                "\n\t%s = si2drGroupFindGroupByName(%s, "
                                "\"%s\", \"%s\", &err);\n\n",
                                oid_string(retoid), oid_string(group), name,
                                type);
                }

                trace_check(retoid);
                inc_tracecount();
        }
        return retoid;
}

si2drAttrIdT si2drGroupFindAttrByName(si2drGroupIdT group, si2drStringT name,
                                      si2drErrorT *err) {
        si2drAttrIdT retoid;
        liberty_group *g = (liberty_group *)group.v2;

        if (!liberty___nocheck_mode) {
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupFindAttrByName:",
                                             &err2);
                        }
                        return nulloid;
                }
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupFindAttrByName:",
                                             &err2);
                        }
                        return nulloid;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_UNUSABLE_OID,
                                    "si2drGroupFindAttrByName:", &err2);
                        }
                        return nulloid;
                }
        }

        *err = SI2DR_NO_ERROR;
        if (name && *name) {
                liberty_hash_lookup(g->attr_hash, name, &retoid);
                if (retoid.v1 == (void *)0) {
                        *err = SI2DR_OBJECT_NOT_FOUND;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_OBJECT_NOT_FOUND,
                                    "si2drGroupFindAttrByName:", &err2);
                        }
                        retoid = nulloid;
                }
        } else {
                *err = SI2DR_INVALID_NAME;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_NOTE, SI2DR_INVALID_NAME,
                                     "si2drGroupFindAttrByName: Attr Not Found",
                                     &err2);
                }
                retoid = nulloid;
        }
        if (trace) {
                if (retoid.v1 == 0 && retoid.v2 == 0) {
                        fprintf(tracefile1,
                                "\n\tsi2drGroupFindAttrByName(%s, \"%s\", "
                                "&err);\n\n",
                                oid_string(group), name);
                } else {
                        outinit_oid(retoid);
                        fprintf(tracefile1,
                                "\n\t%s = si2drGroupFindAttrByName(%s, \"%s\", "
                                "&err);\n\n",
                                oid_string(retoid), oid_string(group), name);
                }

                trace_check(retoid);
                inc_tracecount();
        }

        return retoid;
}

si2drDefineIdT si2drGroupFindDefineByName(si2drGroupIdT group,
                                          si2drStringT name, si2drErrorT *err) {
        si2drDefineIdT retoid;
        liberty_group *g = (liberty_group *)group.v2;

        if (!liberty___nocheck_mode) {
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupFindDefineByName:",
                                             &err2);
                        }
                        return nulloid;
                }
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupFindDefineByName:",
                                             &err2);
                        }
                        return nulloid;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_UNUSABLE_OID,
                                    "si2drGroupFindDefineByName:", &err2);
                        }
                        return nulloid;
                }
        }

        *err = SI2DR_NO_ERROR;
        if (name && *name) {
                liberty_hash_lookup(g->define_hash, name, &retoid);
                if (retoid.v1 == (void *)0) {
                        *err = SI2DR_OBJECT_NOT_FOUND;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_NOTE,
                                             SI2DR_OBJECT_NOT_FOUND,
                                             "si2drGroupFindDefineByName: "
                                             "Define Not Found",
                                             &err2);
                        }
                        retoid = nulloid;
                }
        } else {
                *err = SI2DR_INVALID_NAME;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                                     "si2drGroupFindDefineByName:", &err2);
                }
                retoid = nulloid;
        }
        if (trace) {
                if (retoid.v1 == 0 && retoid.v2 == 0) {
                        fprintf(tracefile1,
                                "\n\tsi2drGroupFindDefineByName(%s, \"%s\", "
                                "&err);\n\n",
                                oid_string(group), name);
                } else {
                        outinit_oid(retoid);
                        fprintf(tracefile1,
                                "\n\t%s = si2drGroupFindDefineByName(%s, "
                                "\"%s\", &err);\n\n",
                                oid_string(retoid), oid_string(group), name);
                }

                trace_check(retoid);
                inc_tracecount();
        }
        return retoid;
}

si2drDefineIdT si2drPIFindDefineByName(si2drStringT name, si2drErrorT *err) {
        si2drDefineIdT retoid;

        if (!liberty___nocheck_mode) {
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drPIFindDefineByName:", &err2);
                        }
                        return nulloid;
                }
        }

        *err = SI2DR_NO_ERROR;

        if (name && *name) {
                liberty_hash_lookup(master_define_hash, name, &retoid);
                if (retoid.v1 == (void *)0) {
                        *err = SI2DR_OBJECT_NOT_FOUND;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_NOTE, SI2DR_OBJECT_NOT_FOUND,
                                    "si2drPIFindDefineByName: Define Not Found",
                                    &err2);
                        }
                        retoid = nulloid;
                }
        } else {
                *err = SI2DR_INVALID_NAME;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                                     "si2drPIFindDefineByName:", &err2);
                }
                retoid = nulloid;
        }

        if (trace) {
                if (retoid.v1 == 0 && retoid.v2 == 0) {
                        fprintf(
                            tracefile1,
                            "\n\tsi2drPIFindDefineByName(\"%s\", &err);\n\n",
                            name);
                } else {
                        outinit_oid(retoid);
                        fprintf(tracefile1,
                                "\n\t%s = si2drPIFindDefineByName(\"%s\", "
                                "&err);\n\n",
                                oid_string(retoid), name);
                }

                trace_check(retoid);
                inc_tracecount();
        }
        return retoid;
}

si2drGroupsIdT si2drPIGetGroups(si2drErrorT *err) {
        iterat *y;

        if (!liberty___nocheck_mode) {
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drPIGetGroups:", &err2);
                        }
                        return (si2drGroupsIdT)0;
                }
        }
        *err = SI2DR_NO_ERROR;

        y = (iterat *)my_calloc(sizeof(iterat), 1);
        y->owner = ITER_GROUP;
        y->next = (void *)master_group_list;
        l__iter_group_count++;
        if (trace) {
                outinit_iter((si2drIterIdT)y);

                fprintf(tracefile1, "\n\t%s = si2drPIGetGroups(&err);\n\n",
                        iter_string((si2drIterIdT)y));
                inc_tracecount();
        }
        return (si2drGroupsIdT)y;
}

si2drGroupsIdT si2drGroupGetGroups(si2drGroupIdT group, si2drErrorT *err) {
        iterat *y;
        liberty_group *g = (liberty_group *)group.v2;

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupGetGroups:", &err2);
                        }
                        return 0;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupGetGroups:", &err2);
                        }
                        return (si2drGroupsIdT)0;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupGetGroups:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;

        y = (iterat *)my_calloc(sizeof(iterat), 1);
        y->owner = ITER_GROUP;
        y->next = (void *)g->group_list;
        l__iter_group_count++;
        if (trace) {
                outinit_iter((si2drIterIdT)y);

                fprintf(tracefile1,
                        "\n\t%s = si2drGroupGetGroups(%s, &err);\n\n",
                        iter_string((si2drIterIdT)y), oid_string(group));
                inc_tracecount();
        }
        return (si2drGroupsIdT)y;
}

si2drNamesIdT si2drGroupGetNames(si2drGroupIdT group, si2drErrorT *err) {
        iterat *y;
        liberty_group *g = (liberty_group *)group.v2;

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupGetNames:", &err2);
                        }
                        return 0;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupGetNames:", &err2);
                        }
                        return 0;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupGetNames:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;

        y = (iterat *)my_calloc(sizeof(iterat), 1);
        y->owner = ITER_NAME;
        y->next = (void *)g->names;
        l__iter_name_count++;
        if (trace) {
                outinit_iter((si2drIterIdT)y);

                fprintf(tracefile1,
                        "\n\t%s = si2drGroupGetNames(%s, &err);\n\n",
                        iter_string((si2drIterIdT)y), oid_string(group));
                inc_tracecount();
        }
        return (si2drNamesIdT)y;
}

si2drAttrsIdT si2drGroupGetAttrs(si2drGroupIdT group, si2drErrorT *err) {
        iterat *y;
        liberty_group *g = (liberty_group *)group.v2;

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupGetAttrs:", &err2);
                        }
                        return 0;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupGetAttrs:", &err2);
                        }
                        return 0;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupGetAttrs:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;

        y = (iterat *)my_calloc(sizeof(iterat), 1);
        y->owner = ITER_ATTR;
        y->next = (void *)g->attr_list;
        l__iter_attr_count++;
        if (trace) {
                outinit_iter((si2drIterIdT)y);

                fprintf(tracefile1,
                        "\n\t%s = si2drGroupGetAttrs(%s, &err);\n\n",
                        iter_string((si2drIterIdT)y), oid_string(group));
                inc_tracecount();
        }
        return (si2drAttrsIdT)y;
}

si2drDefinesIdT si2drGroupGetDefines(si2drGroupIdT group, si2drErrorT *err) {
        iterat *y;
        liberty_group *g = (liberty_group *)group.v2;

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupGetDefines:", &err2);
                        }
                        return 0;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupGetDefines:", &err2);
                        }
                        return 0;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupGetDefines:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;

        y = (iterat *)my_calloc(sizeof(iterat), 1);
        y->owner = ITER_DEF;
        l__iter_def_count++;
        y->next = (void *)g->define_list;
        if (trace) {
                outinit_iter((si2drIterIdT)y);

                fprintf(tracefile1,
                        "\n\t%s = si2drGroupGetDefines(%s, &err);\n\n",
                        iter_string((si2drIterIdT)y), oid_string(group));
                inc_tracecount();
        }
        return (si2drDefinesIdT)y;
}

si2drGroupIdT si2drIterNextGroup(si2drGroupsIdT iter, si2drErrorT *err) {
        si2drGroupIdT retoid;
        liberty_group *z;

        iterat *y = (iterat *)iter;

        if (!liberty___nocheck_mode) {
                if (y->owner != ITER_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drIterNextGroup:", &err2);
                        }
                        return nulloid;
                }
        }

        if (y->next == (liberty_group *)NULL) {
                *err = SI2DR_NO_ERROR;
                if (trace) {
                        fprintf(tracefile1,
                                "\n\tsi2drIterNextGroup(%s, &err);\n\n",
                                iter_string(iter));
                        inc_tracecount();
                }
                return nulloid;
        }

        z = (liberty_group *)y->next;

        y->next = (void *)z->next;
        retoid.v1 = (void *)SI2DR_GROUP;
        retoid.v2 = (void *)z;

        if (trace) {
                outinit_oid(retoid);

                fprintf(tracefile1,
                        "\n\t%s = si2drIterNextGroup(%s, &err);\n\n",
                        oid_string(retoid), iter_string(iter));
                trace_check(retoid);
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        return retoid;
}

si2drStringT si2drIterNextName(si2drNamesIdT iter, si2drErrorT *err) {
        liberty_name_list *z;
        iterat *y = (iterat *)iter;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drIterNextName(%s,&err);\n\n",
                        iter_string(iter));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if (y->owner != ITER_NAME) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drIterNextName:", &err2);
                        }
                        return 0;
                }
        }

        if (y->next == (void *)NULL) {
                *err = SI2DR_NO_ERROR;
                return 0;
        }
        z = (liberty_name_list *)y->next;

        y->next = (void *)z->next;

        *err = SI2DR_NO_ERROR;
        if (z->name)
                return z->name + z->prefix_len;
        else
                return 0;
}

si2drAttrIdT si2drIterNextAttr(si2drAttrsIdT iter, si2drErrorT *err) {
        si2drAttrIdT retoid;
        liberty_attribute *z;

        iterat *y = (iterat *)iter;

        if (!liberty___nocheck_mode) {
                if (y->owner != ITER_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drIterNextAttr:", &err2);
                        }
                        return nulloid;
                }
        }

        if (y->next == (void *)NULL) {
                *err = SI2DR_NO_ERROR;
                if (trace) {
                        fprintf(tracefile1,
                                "\n\tsi2drIterNextGroup(%s, &err);\n\n",
                                iter_string(iter));
                        inc_tracecount();
                }
                return nulloid;
        }
        z = (liberty_attribute *)y->next;

        y->next = (void *)z->next;
        retoid.v1 = (void *)SI2DR_ATTR;
        retoid.v2 = (void *)z;
        if (trace) {
                outinit_oid(retoid);

                fprintf(tracefile1, "\n\t%s = si2drIterNextAttr(%s, &err);\n\n",
                        oid_string(retoid), iter_string(iter));
                trace_check(retoid);
                inc_tracecount();
        }

        *err = SI2DR_NO_ERROR;
        return retoid;
}

si2drDefineIdT si2drIterNextDefine(si2drDefinesIdT iter, si2drErrorT *err) {
        si2drDefineIdT retoid;
        liberty_define *z;

        iterat *y = (iterat *)iter;

        if (!liberty___nocheck_mode) {
                if (y->owner != ITER_DEF) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drIterNextDefine:", &err2);
                        }
                        return nulloid;
                }
        }

        if (y->next == (void *)NULL) {
                *err = SI2DR_NO_ERROR;
                if (trace) {
                        fprintf(tracefile1,
                                "\n\tsi2drIterNextDefine(%s, &err);\n\n",
                                iter_string(iter));
                        inc_tracecount();
                }
                return nulloid;
        }
        z = (liberty_define *)y->next;

        y->next = (void *)z->next;
        retoid.v1 = (void *)SI2DR_DEFINE;
        retoid.v2 = (void *)z;
        if (trace) {
                outinit_oid(retoid);

                fprintf(tracefile1,
                        "\n\t%s = si2drIterNextDefine(%s, &err);\n\n",
                        oid_string(retoid), iter_string(iter));
                trace_check(retoid);
                inc_tracecount();
        }

        *err = SI2DR_NO_ERROR;
        return retoid;
}

si2drVoidT si2drIterQuit(si2drIterIdT iter, si2drErrorT *err) {
        iterat *y = (iterat *)iter;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drIterQuit(%s,&err);\n\n",
                        iter_string(iter));
                inc_tracecount();
        }
        switch (y->owner) {
                case ITER_GROUP:
                        l__iter_group_count--;
                        break;
                case ITER_ATTR:
                        l__iter_attr_count--;
                        break;
                case ITER_DEF:
                        l__iter_def_count--;
                        break;
                case ITER_NAME:
                        l__iter_name_count--;
                        break;
                case ITER_VAL:
                        l__iter_val_count--;
                        break;
                default:
                        break;
        }

        y->next = (void *)0;
        y->owner = ITER_NOTHING;
        my_free(y);

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drObjectDelete(si2drObjectIdT object, si2drErrorT *err) {
        liberty_group *g, *gp, *gg, *lgg;
        liberty_attribute *a, *aa, *laa;
        liberty_attribute_value *av, *av2;
        liberty_define *d, *dd, *ldd;
        liberty_name_list *nl;

        si2drAttrIdT attr;
        si2drDefineIdT define;
        si2drGroupIdT group;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectDelete(%s,&err);\n\n",
                        oid_string(object));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        attr.v1 = (void *)SI2DR_ATTR;
        define.v1 = (void *)SI2DR_DEFINE;
        group.v1 = (void *)SI2DR_GROUP;

        switch ((si2drObjectTypeT)(object.v1)) {
                case SI2DR_GROUP:
                        g = (liberty_group *)object.v2;
                        gp = g->owner;

                        /* delete all attrs */
                        while (g->attr_list) {
                                attr.v2 = (void *)g->attr_list;
                                si2drObjectDelete(attr, err);
                        }
                        g->attr_list = 0;

                        /* delete all defines */
                        while (g->define_list) {
                                define.v2 = (void *)g->define_list;

                                si2drObjectDelete(define, err);
                        }
                        g->define_list = 0;

                        /* delete all subgroups */

                        while (g->group_list) {
                                group.v2 = (void *)g->group_list;

                                si2drObjectDelete(group, err);
                        }
                        g->group_list = 0;

                        /* get rid of the hash tabs */

                        liberty_hash_destroy_hash_table(g->attr_hash);
                        g->attr_hash = 0;
                        liberty_hash_destroy_hash_table(g->define_hash);
                        g->define_hash = 0;
                        liberty_hash_destroy_hash_table(g->group_hash);
                        g->group_hash = 0;

                        lgg = (liberty_group *)NULL;
                        if (gp == (liberty_group *)NULL) {
                                /* unlink me from my parent hash table */

                                for (nl = g->names; nl;) {
                                        struct liberty_name_list *nl2 =
                                            nl->next;
                                        liberty_hash_delete_elem(
                                            master_group_hash, nl->name);
                                        nl->next = 0;
                                        free(nl);
                                        nl = nl2;
                                }
                                g->names = 0;
                                g->last_name = 0;

                                /* unlink me from my parent's list */
                                for (gg = master_group_list; gg;
                                     gg = gg->next) {
                                        if (gg == g) {
                                                if (lgg ==
                                                    (liberty_group *)NULL) {
                                                        /* first guy in the list
                                                         */
                                                        master_group_list =
                                                            gg->next;
                                                } else {
                                                        lgg->next = gg->next;
                                                }
                                                break;
                                        }
                                        lgg = gg;
                                }
                        } else {
                                /* unlink me from my parent hash table */
                                for (nl = g->names; nl;) {
                                        struct liberty_name_list *nl2 =
                                            nl->next;

                                        liberty_hash_delete_elem(gp->group_hash,
                                                                 nl->name);
                                        nl->next = 0;
                                        free(nl);
                                        nl = nl2;
                                }
                                g->names = 0;
                                g->last_name = 0;
                                /* unlink me from my parent's list */
                                for (gg = gp->group_list; gg; gg = gg->next) {
                                        if (gg == g) {
                                                if (lgg ==
                                                    (liberty_group *)NULL) {
                                                        /* first guy in the list
                                                         */
                                                        gp->group_list =
                                                            gg->next;
                                                        if (gp->group_last == g)
                                                                gp->group_last =
                                                                    0;
                                                } else {
                                                        lgg->next = gg->next;
                                                        if (gp->group_last == g)
                                                                gp->group_last =
                                                                    lgg;
                                                }
                                                break;
                                        }
                                        lgg = gg;
                                }
                        }
                        free(g);
                        break;

                case SI2DR_ATTR:
                        a = (liberty_attribute *)object.v2;
                        gp = a->owner;

                        liberty_hash_delete_elem(gp->attr_hash, a->name);

                        laa = (liberty_attribute *)0;
                        for (aa = gp->attr_list; aa; aa = aa->next) {
                                if (aa == a) {
                                        if (laa == (liberty_attribute *)NULL) {
                                                /* first guy in the list */
                                                gp->attr_list = aa->next;
                                                if (gp->attr_last == a)
                                                        gp->attr_last = 0;
                                        } else {
                                                laa->next = aa->next;
                                                if (gp->attr_last == a)
                                                        gp->attr_last = laa;
                                        }
                                        break;
                                }
                                laa = aa;
                        }
                        /* free up substructure */
                        for (av = a->value; av;) {
                                av2 = av->next;
                                av->next = 0;
                                free(av);
                                av = av2;
                        }

                        free(a);
                        break;

                case SI2DR_DEFINE:
                        d = (liberty_define *)object.v2;
                        gp = d->owner;
                        liberty_hash_delete_elem(gp->define_hash, d->name);

                        ldd = (liberty_define *)0;
                        for (dd = gp->define_list; dd; dd = dd->next) {
                                if (dd == d) {
                                        if (ldd == (liberty_define *)NULL) {
                                                /* first guy in the list */
                                                gp->define_list = dd->next;
                                                if (gp->define_last == d)
                                                        gp->define_last = 0;
                                        } else {
                                                ldd->next = dd->next;
                                                if (gp->define_last == d)
                                                        gp->define_last = ldd;
                                        }
                                        break;
                                }
                                ldd = dd;
                        }
                        d->next = 0;
                        free(d);
                        break;

                default:
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return;
        }
}

si2drStringT si2drPIGetErrorText(si2drErrorT errorCode, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        switch (errorCode) {
                case SI2DR_NO_ERROR:
                        return "No error Occurred!";
                case SI2DR_INTERNAL_SYSTEM_ERROR:
                        return "Internal System Error";
                case SI2DR_SYNTAX_ERROR:
                        return "Syntax Error Encountered";
                case SI2DR_INVALID_VALUE:
                        return "Invalid Value";
                case SI2DR_INVALID_NAME:
                        return "Invalid Name";
                case SI2DR_INVALID_OBJECTTYPE:
                        return "Invalid Object Type";
                case SI2DR_INVALID_ATTRTYPE:
                        return "Invalid Attribute Type";
                case SI2DR_UNUSABLE_OID:
                        return "Oid Not Usable. Did you run PIInit?";
                case SI2DR_OBJECT_ALREADY_EXISTS:
                        return "Object Name Already Being Used.";
                case SI2DR_OBJECT_NOT_FOUND:
                        return "Object Not Found";
                case SI2DR_TRACE_FILES_CANNOT_BE_OPENED:
                        return "Trace Files cannot be opened.";
                case SI2DR_PIINIT_NOT_CALLED:
                        return "PIInit() not yet called.";
                case SI2DR_SEMANTIC_ERROR:
                        return "Semantic Error found in data.";
                case SI2DR_REFERENCE_ERROR:
                        return "Reference Error found in data.";
                default:
                        break;
        }
        return "?";
}

si2drObjectIdT si2drPIGetNullId(si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        return nulloid;
}

si2drVoidT si2drPIInit(si2drErrorT *err) {
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPIInit(&err);\n\n");
                inc_tracecount();
        }
        master_group_hash = liberty_hash_create_hash_table(53, 1, 0);

        master_define_hash = liberty_hash_create_hash_table(53, 1, 0);

        master_string_table =
            liberty_strtable_create_strtable(129235, 1024 * 1024, 0);

        si2ErrMsg = si2drDefaultMessageHandler;

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drPIQuit(si2drErrorT *err) {
        si2drGroupsIdT groups;
        si2drGroupIdT group;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drPIQuit(&err);\n\n");
                inc_tracecount();
        }
        /* free up any groups laying around */

        groups = si2drPIGetGroups(err);
        while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, err)),
                                  err)) {
                si2drObjectDelete(group, err);
        }
        si2drIterQuit(groups, err);

        /* destroy the master hash */

        liberty_hash_destroy_hash_table(master_group_hash);
        liberty_hash_destroy_hash_table(master_define_hash);
        master_group_hash = 0;
        master_define_hash = 0;
        if (l__iter_group_count != 0 || l__iter_name_count != 0 ||
            l__iter_val_count != 0 || l__iter_attr_count != 0 ||
            l__iter_def_count != 0) {
                si2drErrorT err2;
                char tbuf[1000];

                (*si2ErrMsg)(SI2DR_SEVERITY_WARN, SI2DR_NO_ERROR,
                             "si2drPIQuit: Poor Coding Practice Detected--\n\
            For each Iterator create, an IterQuit function must be called\n",
                             &err2);

                if (l__iter_group_count != 0) {
                        si2drErrorT err2;

                        sprintf(tbuf,
                                "si2drPIQuit: GetGroups called %d more times "
                                "than IterQuit\n",
                                l__iter_group_count);
                        (*si2ErrMsg)(SI2DR_SEVERITY_WARN, SI2DR_NO_ERROR, tbuf,
                                     &err2);
                }
                if (l__iter_name_count != 0) {
                        si2drErrorT err2;

                        sprintf(tbuf,
                                "si2drPIQuit: GetNames called %d more times "
                                "than IterQuit\n",
                                l__iter_name_count);
                        (*si2ErrMsg)(SI2DR_SEVERITY_WARN, SI2DR_NO_ERROR, tbuf,
                                     &err2);
                }
                if (l__iter_val_count != 0) {
                        si2drErrorT err2;

                        sprintf(tbuf,
                                "si2drPIQuit: GetValues called %d more times "
                                "than IterQuit\n",
                                l__iter_val_count);
                        (*si2ErrMsg)(SI2DR_SEVERITY_WARN, SI2DR_NO_ERROR, tbuf,
                                     &err2);
                }
                if (l__iter_attr_count != 0) {
                        si2drErrorT err2;

                        sprintf(tbuf,
                                "si2drPIQuit: GetAttrs called %d more times "
                                "than IterQuit\n",
                                l__iter_attr_count);
                        (*si2ErrMsg)(SI2DR_SEVERITY_WARN, SI2DR_NO_ERROR, tbuf,
                                     &err2);
                }
                if (l__iter_def_count != 0) {
                        si2drErrorT err2;

                        sprintf(tbuf,
                                "si2drPIQuit: GetDefines called %d more times "
                                "than IterQuit\n",
                                l__iter_def_count);
                        (*si2ErrMsg)(SI2DR_SEVERITY_WARN, SI2DR_NO_ERROR, tbuf,
                                     &err2);
                }
        }
        liberty_strtable_destroy_strtable(master_string_table);
        master_string_table = 0;
        /*my_malloc_print_totals();*/
        *err = SI2DR_NO_ERROR;
}

si2drObjectTypeT si2drObjectGetObjectType(si2drObjectIdT object,
                                          si2drErrorT *err) {
        si2drObjectTypeT ret;
        ret = (si2drObjectTypeT)object.v1;
        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drObjectGetObjectType(%s,&err);\n\n",
                        oid_string(object));
                inc_tracecount();
        }

        *err = SI2DR_NO_ERROR;

        return ret;
}

si2drBooleanT si2drObjectIsNull(si2drObjectIdT object, si2drErrorT *err) {
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectIsNull(%s,&err);\n\n",
                        oid_string(object));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        if (object.v2 == (void *)NULL)
                return SI2DR_TRUE;
        else
                return SI2DR_FALSE;
}

si2drBooleanT si2drObjectIsSame(si2drObjectIdT object1, si2drObjectIdT object2,
                                si2drErrorT *err) {
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectIsSame(%s,%s,&err);\n\n",
                        oid_string(object1), oid_string(object2));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        if (object1.v1 == object2.v1 && object1.v2 == object2.v2)
                return SI2DR_TRUE;
        else
                return SI2DR_FALSE;
}

si2drBooleanT si2drObjectIsUsable(si2drObjectIdT object, si2drErrorT *err) {
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectIsUsable(%s,&err);\n\n",
                        oid_string(object));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        if (object.v2 == (void *)NULL) return SI2DR_FALSE;
        if (object.v1 == (void *)SI2DR_GROUP ||
            object.v1 == (void *)SI2DR_ATTR ||
            object.v1 == (void *)SI2DR_DEFINE)
                return SI2DR_TRUE;
        else
                return SI2DR_FALSE;
}

si2drVoidT si2drObjectSetFileName(si2drObjectIdT object, si2drStringT filename,
                                  si2drErrorT *err) {
        liberty_group *g;
        liberty_attribute *a;
        liberty_define *d;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drObjectSetFileName(%s,\"%s\",&err);\n\n",
                        oid_string(object), filename);
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        switch ((si2drObjectTypeT)(object.v1)) {
                case SI2DR_GROUP:
                        g = (liberty_group *)object.v2;
                        g->filename = filename;
                        break;

                case SI2DR_ATTR:
                        a = (liberty_attribute *)object.v2;
                        a->filename = filename;
                        break;

                case SI2DR_DEFINE:
                        d = (liberty_define *)object.v2;
                        d->filename = filename;
                        break;
                default:
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        break;
        }
}

si2drVoidT si2drObjectSetLineNo(si2drObjectIdT object, si2drInt32T lineno,
                                si2drErrorT *err) {
        liberty_group *g;
        liberty_attribute *a;
        liberty_define *d;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectSetLineNo(%s,%d,&err);\n\n",
                        oid_string(object), (int)lineno);
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        switch ((si2drObjectTypeT)(object.v1)) {
                case SI2DR_GROUP:
                        g = (liberty_group *)object.v2;
                        g->lineno = lineno;
                        break;

                case SI2DR_ATTR:
                        a = (liberty_attribute *)object.v2;
                        a->lineno = lineno;
                        break;

                case SI2DR_DEFINE:
                        d = (liberty_define *)object.v2;
                        d->lineno = lineno;
                        break;
                default:
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        break;
        }
}

si2drInt32T si2drObjectGetLineNo(si2drObjectIdT object, si2drErrorT *err) {
        liberty_group *g;
        liberty_attribute *a;
        liberty_define *d;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectSetLineNo(%s,&err);\n\n",
                        oid_string(object));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        switch ((si2drObjectTypeT)(object.v1)) {
                case SI2DR_GROUP:
                        g = (liberty_group *)object.v2;
                        return g->lineno;

                case SI2DR_ATTR:
                        a = (liberty_attribute *)object.v2;
                        return a->lineno;

                case SI2DR_DEFINE:
                        d = (liberty_define *)object.v2;
                        return d->lineno;

                default:
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return -1;
        }
}

si2drStringT si2drObjectGetFileName(si2drObjectIdT object, si2drErrorT *err) {
        liberty_group *g;
        liberty_attribute *a;
        liberty_define *d;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectSetLineNo(%s,&err);\n\n",
                        oid_string(object));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;
        switch ((si2drObjectTypeT)(object.v1)) {
                case SI2DR_GROUP:
                        g = (liberty_group *)object.v2;
                        return g->filename;

                case SI2DR_ATTR:
                        a = (liberty_attribute *)object.v2;
                        return a->filename;

                case SI2DR_DEFINE:
                        d = (liberty_define *)object.v2;
                        return d->filename;

                default:
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return (char *)-1;
        }
}

si2drVoidT si2drReadLibertyFile(char *filename, si2drErrorT *err) {
        extern FILE *liberty_parser2_in;
        int in_trace = 0;
        char comm[500];
        extern char *curr_file;
        extern int syntax_errors;

        curr_file = filename;
        *err = SI2DR_NO_ERROR;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drReadLibertyFile(\"%s\",&err);\n\n", filename);
                inc_tracecount();
                in_trace = 1;
                trace = 0;
        }

        si2drPISetNocheckMode(err);

        if (!strcmp(filename + strlen(filename) - 4, ".bz2")) {
                sprintf(comm, "bzip2 -cd %s", filename);
                liberty_parser2_in = popen(comm, "r");
                if (!liberty_parser2_in) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(
                            SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                            "si2drReadLibertyFile: Could not execute bzip2!",
                            &err2);
                        *err = SI2DR_INVALID_NAME;
                        perror("liberty_parse");
                        return;
                }
        } else if (!strcmp(filename + strlen(filename) - 4, ".zip") ||
                   !strcmp(filename + strlen(filename) - 4, ".ZIP")) {
                sprintf(comm, "unzip -cqq %s", filename);
                liberty_parser2_in = popen(comm, "r");
                if (!liberty_parser2_in) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(
                            SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                            "si2drReadLibertyFile: Could not execute unzip!",
                            &err2);
                        *err = SI2DR_INVALID_NAME;
                        perror("liberty_parse");
                        return;
                }
        } else if (!strcmp(filename + strlen(filename) - 3, ".gz") ||
                   !strcmp(filename + strlen(filename) - 2, ".Z")) {
                sprintf(comm, "gzip -cd %s", filename);
                liberty_parser2_in = popen(comm, "r");
                if (!liberty_parser2_in) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(
                            SI2DR_SEVERITY_ERR, SI2DR_INVALID_NAME,
                            "si2drReadLibertyFile: Could not execute gzip!",
                            &err2);
                        *err = SI2DR_INVALID_NAME;
                        perror("liberty_parse");
                        return;
                }
        } else
                liberty_parser2_in = fopen(filename, "r");

        if (liberty_parser2_in == (FILE *)NULL) {
                *err = SI2DR_INVALID_NAME;
                perror("liberty_parse");
                return;
        }

        liberty_parser_parse();

        if (strcmp(filename + strlen(filename) - 3, ".gz") &&
            strcmp(filename + strlen(filename) - 2, ".Z") &&
            strcmp(filename + strlen(filename) - 4, ".bz2") &&
            strcmp(filename + strlen(filename) - 4, ".ZIP") &&
            strcmp(filename + strlen(filename) - 4, ".zip"))
                fclose(liberty_parser2_in);
        else
                pclose(liberty_parser2_in);

        si2drPIUnSetNocheckMode(err);
        if (in_trace) trace = 1;
        if (syntax_errors) *err = SI2DR_SYNTAX_ERROR;
}

int lib__name_needs_to_be_quoted(char *name) {
        char *s = name;
        if (*s >= '0' && *s <= '9') return 1;
        while (*s) {
                if (*s == ' ') return 1;
                if ((*s < '0') || (*s > '9' && *s < 'A') ||
                    (*s > 'Z' && *s < 'a') || (*s > 'z'))
                        return 1;
                s++;
        }
        return 0;
}

char *expr_string(si2drExprT *e) {
        char *buf1 = NULL;
        char tbuf[10000];
        char *lefts = 0, *rights = 0;
        if (e->left) lefts = expr_string(e->left);
        if (e->right) rights = expr_string(e->right);
        switch (e->type) {
                case SI2DR_EXPR_VAL:
                        switch (e->valuetype) {
                                case SI2DR_STRING:
                                        sprintf(tbuf, "%s", e->u.s);
                                        buf1 = liberty_strtable_enter_string(
                                            master_string_table, tbuf);
                                        break;
                                case SI2DR_FLOAT64:
                                        buf1 = liberty_strtable_enter_string(
                                            master_string_table, tbuf);
                                        break;
                                case SI2DR_INT32:
                                        buf1 = liberty_strtable_enter_string(
                                            master_string_table, tbuf);
                                        break;
                                default:
                                        break;
                        }
                        break;

                case SI2DR_EXPR_OP_ADD:
                        if (rights)
                                sprintf(tbuf, "%s + %s", lefts, rights);
                        else
                                sprintf(tbuf, "+%s", lefts);

                        buf1 = liberty_strtable_enter_string(
                            master_string_table, tbuf);

                        break;

                case SI2DR_EXPR_OP_SUB:
                        if (rights)
                                sprintf(tbuf, "%s - %s", lefts, rights);
                        else
                                sprintf(tbuf, "-%s", lefts);

                        buf1 = liberty_strtable_enter_string(
                            master_string_table, tbuf);

                        break;

                case SI2DR_EXPR_OP_MUL:
                        sprintf(tbuf, "%s * %s", lefts, rights);
                        buf1 = liberty_strtable_enter_string(
                            master_string_table, tbuf);
                        break;

                case SI2DR_EXPR_OP_DIV:
                        sprintf(tbuf, "%s / %s", lefts, rights);
                        buf1 = liberty_strtable_enter_string(
                            master_string_table, tbuf);
                        break;

                case SI2DR_EXPR_OP_PAREN:
                        buf1 = liberty_strtable_enter_string(
                            master_string_table, tbuf);
                        break;

                case SI2DR_EXPR_OP_LOG2:
                        break;

                case SI2DR_EXPR_OP_LOG10:
                        break;

                case SI2DR_EXPR_OP_EXP:
                        break;
        }
        return buf1;
}

void lib__write_group(FILE *of, si2drGroupIdT group, char *indent,
                      char *cellname) {
        char indent2[200];
        si2drErrorT err;
        si2drStringT str;
        si2drNamesIdT names;
        si2drGroupsIdT groups;
        si2drAttrsIdT attrs;
        si2drDefinesIdT defs;
        si2drGroupIdT g1;
        si2drAttrIdT attr;
        si2drDefineIdT def;
        int first;

        sprintf(indent2, "%s\t", indent);

        if ((str = si2drGroupGetComment(group, &err)) && str[0] != 0) {
                fprintf(of, "%s/* %s */\n", indent, str);
        }

        first = 1;

        fprintf(of, "%s%s (", indent, si2drGroupGetGroupType(group, &err));
        names = si2drGroupGetNames(group, &err);
        for (str = si2drIterNextName(names, &err); str && str[0];
             str = si2drIterNextName(names, &err)) {
                if (!first) fprintf(of, ",");
                if (lib__name_needs_to_be_quoted(str))
                        fprintf(of, "\"%s\"", str);
                else
                        fprintf(of, "%s", str);
                first = 0;
        }
        si2drIterQuit(names, &err);

        fprintf(of, ") {\n");

        /* print the defines */

        defs = si2drGroupGetDefines(group, &err);
        for (def = si2drIterNextDefine(defs, &err);
             !si2drObjectIsNull(def, &err);
             def = si2drIterNextDefine(defs, &err)) {
                si2drStringT nam, gnam;
                si2drValueTypeT valtype;
                char *vt;

                if ((str = si2drDefineGetComment(def, &err)) && str[0] != 0) {
                        fprintf(of, "%s/* %s */\n", indent2, str);
                }

                si2drDefineGetInfo(def, &nam, &gnam, &valtype, &err);

                switch (valtype) {
                        case SI2DR_STRING:
                                vt = "string";
                                break;

                        case SI2DR_FLOAT64:
                                vt = "float";
                                break;

                        case SI2DR_INT32:
                                vt = "integer";
                                break;

                        case SI2DR_BOOLEAN:
                                vt = "boolean";
                                break;

                        default:
                                break;
                }
                if (valtype != SI2DR_UNDEFINED_VALUETYPE) {
                        char *p, *q;
                        char strTmp[100] = {0};
                        q = gnam;
                        while ((p = strchr(q, '|'))) {
                                strncpy(strTmp, q, p - q);
                                fprintf(of, "%sdefine(%s,%s,%s);\n", indent2,
                                        nam, strTmp, vt);
                                memset(strTmp, 0, sizeof(strTmp));
                                q = p + 1;
                        }
                        fprintf(of, "%sdefine(%s,%s,%s);\n", indent2, nam, q,
                                vt);
                } else {
                        char *p, *q;
                        char strTmp[100] = {0};
                        q = gnam;
                        while ((p = strchr(q, '|'))) {
                                strncpy(strTmp, q, p - q);
                                fprintf(of, "%sdefine_group(%s,%s);\n", indent2,
                                        nam, strTmp);
                                memset(strTmp, 0, sizeof(strTmp));
                                q = p + 1;
                        }
                        fprintf(of, "%sdefine_group(%s,%s);\n", indent2, nam,
                                q);
                }
        }
        si2drIterQuit(defs, &err);

        /* print the attrs */

        attrs = si2drGroupGetAttrs(group, &err);
        for (attr = si2drIterNextAttr(attrs, &err);
             !si2drObjectIsNull(attr, &err);
             attr = si2drIterNextAttr(attrs, &err)) {
                si2drAttrTypeT at = si2drAttrGetAttrType(attr, &err);
                si2drValuesIdT vals;
                si2drValueTypeT vtype;
                si2drInt32T intgr;
                si2drFloat64T float64;
                si2drStringT string, nam = si2drAttrGetName(attr, &err);
                si2drBooleanT boolval;
                si2drBooleanT is_var;
                si2drExprT *exprp;
                int first;

                if ((str = si2drAttrGetComment(attr, &err)) && str[0] != 0) {
                        fprintf(of, "%s/* %s */\n", indent2, str);
                }

                if (at == SI2DR_SIMPLE &&
                    (!strcmp(nam, "default_operating_conditions") ||
                     !strcmp(nam, "default_wire_load") ||
                     !strcmp(nam, "default_wire_load_selection")))
                        continue;

                fprintf(of, "%s%s", indent2, nam);

                switch (at) {
                        case SI2DR_SIMPLE:
                                vtype = si2drSimpleAttrGetValueType(attr, &err);
                                is_var = si2drSimpleAttrGetIsVar(attr, &err);
                                switch (vtype) {
                                        case SI2DR_STRING:
                                                if (is_var)
                                                        fprintf(
                                                            of, " = \"%s\";\n",
                                                            si2drSimpleAttrGetStringValue(
                                                                attr, &err));
                                                else
                                                        fprintf(
                                                            of, " : \"%s\";\n",
                                                            si2drSimpleAttrGetStringValue(
                                                                attr, &err));
                                                break;

                                        case SI2DR_FLOAT64:
                                                if (is_var)
                                                        fprintf(
                                                            of, " = %1.6f;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (!strcmp(nam,
                                                                 "key_bit") ||
                                                         !strcmp(
                                                             nam,
                                                             "address_width") ||
                                                         !strcmp(
                                                             nam,
                                                             "word_width") ||
                                                         !strcmp(nam,
                                                                 "key_seed") ||
                                                         !strcmp(nam,
                                                                 "tracks") ||
                                                         !strcmp(
                                                             nam,
                                                             "divided_by") ||
                                                         !strcmp(
                                                             nam,
                                                             "multiplied_by") ||
                                                         !strcmp(nam,
                                                                 "bit_from") ||
                                                         !strcmp(nam,
                                                                 "bit_to") ||
                                                         !strcmp(nam,
                                                                 "bit_width"))
                                                        fprintf(
                                                            of, " : %d;\n",
                                                            (int)
                                                                si2drSimpleAttrGetFloat64Value(
                                                                    attr,
                                                                    &err));
                                                else if (
                                                    !strcmp(
                                                        si2drGroupGetGroupType(
                                                            group, &err),
                                                        "leakage_power") &&
                                                    !strcmp(nam, "value"))
                                                        fprintf(
                                                            of, " : %1.6e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (!strcmp(nam,
                                                                 "cell_leakage_"
                                                                 "power"))
                                                        fprintf(
                                                            of, " : %1.6e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (
                                                    !strcmp(
                                                        si2drGroupGetGroupType(
                                                            group, &err),
                                                        "gate_leakage"))
                                                        fprintf(
                                                            of, " : %1.6e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (
                                                    !strcmp(
                                                        si2drGroupGetGroupType(
                                                            group, &err),
                                                        "pg_current"))
                                                        fprintf(
                                                            of, " : %1.6e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (
                                                    !strcmp(
                                                        si2drGroupGetGroupType(
                                                            group, &err),
                                                        "intrinsic_"
                                                        "capacitance"))
                                                        fprintf(
                                                            of, " : %1.6e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (
                                                    !strcmp(
                                                        si2drGroupGetGroupType(
                                                            group, &err),
                                                        "wire_load") &&
                                                    !strcmp(nam, "resistance"))
                                                        fprintf(
                                                            of, " : %e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (
                                                    !strcmp(
                                                        si2drGroupGetGroupType(
                                                            group, &err),
                                                        "cell") &&
                                                    (!strcmp(nam, "MAXX") ||
                                                     !strcmp(nam, "MAXY")))
                                                        fprintf(
                                                            of, " : %g;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else
                                                        fprintf(
                                                            of, " : %1.6f;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                break;

                                        case SI2DR_INT32:
                                                if (is_var)
                                                        fprintf(
                                                            of, " = %d;\n",
                                                            (int)
                                                                si2drSimpleAttrGetInt32Value(
                                                                    attr,
                                                                    &err));
                                                else
                                                        fprintf(
                                                            of, " : %d;\n",
                                                            (int)
                                                                si2drSimpleAttrGetInt32Value(
                                                                    attr,
                                                                    &err));
                                                break;

                                        case SI2DR_EXPR: {
                                                char *es = expr_string(
                                                    si2drSimpleAttrGetExprValue(
                                                        attr, &err));
                                                if (is_var)
                                                        fprintf(of, " = %s;\n",
                                                                es);
                                                else
                                                        fprintf(of, " : %s;\n",
                                                                es);
                                        } break;

                                        case SI2DR_BOOLEAN:
                                                if (is_var) {
                                                        if (si2drSimpleAttrGetBooleanValue(
                                                                attr, &err))
                                                                fprintf(of,
                                                                        " = "
                                                                        "true;"
                                                                        "\n");
                                                        else
                                                                fprintf(of,
                                                                        " = "
                                                                        "false;"
                                                                        "\n");
                                                } else {
                                                        if (si2drSimpleAttrGetBooleanValue(
                                                                attr, &err))
                                                                fprintf(of,
                                                                        " : "
                                                                        "true;"
                                                                        "\n");
                                                        else
                                                                fprintf(of,
                                                                        " : "
                                                                        "false;"
                                                                        "\n");
                                                }
                                                break;
                                        default:
                                                break;
                                }
                                break;

                        case SI2DR_COMPLEX:
                                vals = si2drComplexAttrGetValues(attr, &err);
                                first = 1;
                                fprintf(of, "(");

                                for (si2drIterNextComplexValue(
                                         vals, &vtype, &intgr, &float64,
                                         &string, &boolval, &exprp, &err);
                                     vtype != SI2DR_UNDEFINED_VALUETYPE;
                                     si2drIterNextComplexValue(
                                         vals, &vtype, &intgr, &float64,
                                         &string, &boolval, &exprp, &err)) {
                                        if (!first) fprintf(of, ",");
                                        switch (vtype) {
                                                case SI2DR_STRING:
                                                        if (!strcmp(nam,
                                                                    "define_"
                                                                    "group")) {
                                                                fprintf(of,
                                                                        "%s",
                                                                        string);
                                                        } else {
                                                                if (!first)
                                                                        fprintf(
                                                                            of,
                                                                            " "
                                                                            "\\"
                                                                            "\n"
                                                                            "%s"
                                                                            " "
                                                                            " ",
                                                                            indent2);
                                                                fprintf(
                                                                    of,
                                                                    "\"%s\"",
                                                                    string);
                                                        }
                                                        break;

                                                case SI2DR_FLOAT64:
                                                        if (!strcmp(
                                                                nam,
                                                                "curve_y") ||
                                                            !strcmp(nam,
                                                                    "vector"))
                                                                fprintf(
                                                                    of, "%d",
                                                                    (int)
                                                                        float64);
                                                        else if (
                                                            !strcmp(
                                                                nam,
                                                                "va_values") ||
                                                            !strcmp(nam,
                                                                    "nominal_"
                                                                    "va_"
                                                                    "values"))
                                                                fprintf(
                                                                    of, "%1.6e",
                                                                    float64);
                                                        else
                                                                fprintf(
                                                                    of, "%1.6f",
                                                                    float64);
                                                        break;

                                                case SI2DR_INT32:
                                                        fprintf(of, "%d",
                                                                (int)intgr);
                                                        break;

                                                case SI2DR_BOOLEAN:
                                                        fprintf(of, "%d",
                                                                boolval);
                                                        break;
                                                default:
                                                        break;
                                        }
                                        first = 0;
                                }
                                si2drIterQuit(vals, &err);
                                fprintf(of, ");\n");
                                break;
                }
        }
        si2drIterQuit(attrs, &err);

        /* print the groups */

        groups = si2drGroupGetGroups(group, &err);
        for (g1 = si2drIterNextGroup(groups, &err);
             !si2drObjectIsNull(g1, &err);
             g1 = si2drIterNextGroup(groups, &err)) {
                int printCell;
                /* Only print desired cell */
                if (cellname != NULL && strcmp(cellname, "")) {
                        printCell = 0;
                        if (!strcmp(si2drGroupGetGroupType(g1, &err), "cell")) {
                                names = si2drGroupGetNames(g1, &err);
                                for (str = si2drIterNextName(names, &err);
                                     str && str[0];
                                     str = si2drIterNextName(names, &err)) {
                                        if (!strcasecmp(str, cellname)) {
                                                printCell = 1;
                                                break;
                                        }
                                }
                                si2drIterQuit(names, &err);
                                if (printCell == 0) {
                                        continue;
                                }
                        }
                }

                lib__write_group(of, g1, indent2, NULL);
        }
        si2drIterQuit(groups, &err);

        /* print the attrs */

        attrs = si2drGroupGetAttrs(group, &err);
        for (attr = si2drIterNextAttr(attrs, &err);
             !si2drObjectIsNull(attr, &err);
             attr = si2drIterNextAttr(attrs, &err)) {
                si2drAttrTypeT at = si2drAttrGetAttrType(attr, &err);
                si2drValueTypeT vtype;
                si2drStringT string, nam = si2drAttrGetName(attr, &err);

                if (at != SI2DR_SIMPLE ||
                    (strcmp(nam, "default_operating_conditions") &&
                     strcmp(nam, "default_wire_load") &&
                     strcmp(nam, "default_wire_load_selection")))
                        continue;

                /* fprintf(of,"%s%s", indent2, nam); */
                if ((str = si2drAttrGetComment(attr, &err)) && str[0] != 0) {
                        fprintf(of, "%s/* %s */\n", indent2, str);
                }

                switch (at) {
                        case SI2DR_SIMPLE:
                                vtype = si2drSimpleAttrGetValueType(attr, &err);
                                switch (vtype) {
                                        case SI2DR_STRING:
                                                fprintf(of, "%s%s", indent2,
                                                        nam);
                                                if (!strcmp(nam, "vil") ||
                                                    !strcmp(nam, "vih") ||
                                                    !strcmp(nam, "vimax") ||
                                                    !strcmp(nam, "vimin") ||
                                                    !strcmp(nam, "vol") ||
                                                    !strcmp(nam, "voh") ||
                                                    !strcmp(nam, "vomax") ||
                                                    !strcmp(nam, "vomin"))
                                                        fprintf(
                                                            of, " : %s;\n",
                                                            (string =
                                                                 si2drSimpleAttrGetStringValue(
                                                                     attr,
                                                                     &err)));
                                                else if (!strcmp(nam,
                                                                 "key_bit") ||
                                                         !strcmp(
                                                             nam,
                                                             "address_width") ||
                                                         !strcmp(
                                                             nam,
                                                             "word_width") ||
                                                         !strcmp(nam,
                                                                 "key_seed") ||
                                                         !strcmp(nam,
                                                                 "tracks") ||
                                                         !strcmp(
                                                             nam,
                                                             "divided_by") ||
                                                         !strcmp(
                                                             nam,
                                                             "multiplied_by") ||
                                                         !strcmp(nam,
                                                                 "bit_from") ||
                                                         !strcmp(nam,
                                                                 "bit_to") ||
                                                         !strcmp(nam,
                                                                 "bit_width"))
                                                        fprintf(
                                                            of, " : %d;\n",
                                                            atoi(
                                                                si2drSimpleAttrGetStringValue(
                                                                    attr,
                                                                    &err)));
                                                else
                                                        fprintf(
                                                            of, " : \"%s\";\n",
                                                            (string =
                                                                 si2drSimpleAttrGetStringValue(
                                                                     attr,
                                                                     &err)));
                                                break;

                                        case SI2DR_FLOAT64:
                                                if (!strcmp(nam, "key_bit") ||
                                                    !strcmp(nam,
                                                            "address_width") ||
                                                    !strcmp(nam,
                                                            "word_width") ||
                                                    !strcmp(nam, "key_seed") ||
                                                    !strcmp(nam, "tracks") ||
                                                    !strcmp(nam,
                                                            "divided_by") ||
                                                    !strcmp(nam,
                                                            "multiplied_by") ||
                                                    !strcmp(nam, "bit_from") ||
                                                    !strcmp(nam, "bit_to") ||
                                                    !strcmp(nam, "bit_width"))
                                                        fprintf(
                                                            of, " : %d;\n",
                                                            (int)
                                                                si2drSimpleAttrGetFloat64Value(
                                                                    attr,
                                                                    &err));
                                                else if (
                                                    !strcmp(
                                                        si2drGroupGetGroupType(
                                                            group, &err),
                                                        "leakage_power") &&
                                                    !strcmp(nam, "value"))
                                                        fprintf(
                                                            of, " : %1.6e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else if (!strcmp(nam,
                                                                 "cell_leakage_"
                                                                 "power"))
                                                        fprintf(
                                                            of, " : %1.6e;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                else
                                                        fprintf(
                                                            of, " : %1.6f;\n",
                                                            si2drSimpleAttrGetFloat64Value(
                                                                attr, &err));
                                                break;

                                        case SI2DR_INT32:
                                                fprintf(
                                                    of, " : %d;\n",
                                                    (int)
                                                        si2drSimpleAttrGetInt32Value(
                                                            attr, &err));
                                                break;

                                        case SI2DR_BOOLEAN:
                                                if (si2drSimpleAttrGetBooleanValue(
                                                        attr, &err))
                                                        fprintf(of,
                                                                " : true;\n");
                                                else
                                                        fprintf(of,
                                                                " : false;\n");
                                                break;
                                        default:
                                                break;
                                }
                                break;

                        case SI2DR_COMPLEX:
#ifdef NOMORE
                                vals = si2drComplexAttrGetValues(attr, &err);
                                first = 1;
                                fprintf(of, "(");

                                for (si2drIterNextComplexValue(
                                         vals, &vtype, &intgr, &float64,
                                         &string, &boolval, &expr, &err);
                                     vtype != SI2DR_UNDEFINED_VALUETYPE;
                                     si2drIterNextComplexValue(
                                         vals, &vtype, &intgr, &float64,
                                         &string, &boolval, &expr, &err)) {
                                        if (!first) fprintf(of, ",");
                                        switch (vtype) {
                                                case SI2DR_STRING:
                                                        fprintf(of, "\"%s\"",
                                                                string);
                                                        break;

                                                case SI2DR_FLOAT64:
                                                        if (!strcmp(nam,
                                                                    "nominal_"
                                                                    "va_"
                                                                    "values") ||
                                                            !strcmp(
                                                                nam,
                                                                "va_values"))
                                                                fprintf(
                                                                    of, "%1.6e",
                                                                    float64);
                                                        else
                                                                fprintf(
                                                                    of, "%1.6f",
                                                                    float64);
                                                        break;

                                                case SI2DR_INT32:
                                                        fprintf(of, "%d",
                                                                intgr);
                                                        break;

                                                case SI2DR_BOOLEAN:
                                                        fprintf(of, "%d",
                                                                boolval);
                                                        break;
                                        }
                                        first = 0;
                                }
                                si2drIterQuit(vals, &err);
                                fprintf(of, ");\n");
#endif
                                break;
                }
        }
        si2drIterQuit(attrs, &err);

        fprintf(of, "%s}\n", indent);
}

si2drVoidT si2drWriteLibertyFile(char *filename, si2drGroupIdT group,
                                 char *cellname, si2drErrorT *err) {
        /* open the file */
        FILE *of;
        int in_trace;

        in_trace = 0;

        if (!si2drObjectIsUsable(group, err)) return;
        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drWriteLibertyFile(\"%s\",%s,&err);\n\n",
                        filename, oid_string(group));
                inc_tracecount();
                in_trace = 1;
                trace = 0;
        }

        si2drPISetNocheckMode(err);

        of = fopen(filename, "w");

        /* call the top level routine */
        si2drPISetDebugMode(err);

        lib__write_group(of, group, "", cellname);
        fclose(of);
        si2drPIUnSetDebugMode(err);
        if (in_trace) trace = 1;
}

si2drVoidT si2drCheckLibertyLibrary(si2drGroupIdT group, si2drErrorT *err) {
        int in_trace;
        in_trace = 0;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drCheckLibertyLibrary(%s,&err);\n\n",
                        oid_string(group));
                inc_tracecount();
                in_trace = 1;
                trace = 0;
        }
        *err = syntax_check(group);

        if (in_trace) trace = 1;
}

si2drStringT si2drGroupGetGroupType(si2drGroupIdT group, si2drErrorT *err) {
        liberty_group *g = (liberty_group *)group.v2;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drGroupGetGroupType(%s,&err);\n\n",
                        oid_string(group));
                inc_tracecount();
        }
        if (!g) {
                *err = SI2DR_UNUSABLE_OID;
                return 0;
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(group.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return 0;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        return 0;
                }
                if (g->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        return 0;
                }
        }
        return g->type;
}

si2drVoidT si2drDefineGetInfo(si2drDefineIdT def, si2drStringT *name,
                              si2drStringT *allowed_group_name,
                              si2drValueTypeT *valtype, si2drErrorT *err) {
        liberty_define *d = (liberty_define *)def.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drDefineGetInfo(%s,&name_string, "
                        "&allowed_groups, &valtype, &err);\n\n",
                        oid_string(def));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(def.v1) != SI2DR_DEFINE) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        return;
                }
        }

        *name = d->name;
        *allowed_group_name = d->group_type;
        switch (d->valtype) {
                case LIBERTY__VAL_STRING:
                        *valtype = SI2DR_STRING;
                        break;

                case LIBERTY__VAL_DOUBLE:
                        *valtype = SI2DR_FLOAT64;
                        break;

                case LIBERTY__VAL_BOOLEAN:
                        *valtype = SI2DR_BOOLEAN;
                        break;

                case LIBERTY__VAL_INT:
                        *valtype = SI2DR_INT32;
                        break;

                case LIBERTY__VAL_UNDEFINED:
                        *valtype = SI2DR_UNDEFINED_VALUETYPE;
                        break;
                default:
                        break;
        }
        *err = SI2DR_NO_ERROR;
}

si2drStringT si2drDefineGetName(si2drDefineIdT def, si2drErrorT *err) {
        liberty_define *d = (liberty_define *)def.v2;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drDefineGetName(%s,&err);\n\n",
                        oid_string(def));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(def.v1) != SI2DR_DEFINE) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return "";
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        return "";
                }
        }
        *err = SI2DR_NO_ERROR;
        return d->name;
}

si2drStringT si2drDefineGetAllowedGroupName(si2drDefineIdT def,
                                            si2drErrorT *err) {
        liberty_define *d = (liberty_define *)def.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drDefineGetAllowedGroupName(%s,&err);\n\n",
                        oid_string(def));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(def.v1) != SI2DR_DEFINE) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return 0;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        return 0;
                }
        }
        *err = SI2DR_NO_ERROR;
        return d->group_type;
}

si2drValueTypeT si2drDefineGetValueType(si2drDefineIdT def, si2drErrorT *err) {
        liberty_define *d = (liberty_define *)def.v2;
        si2drValueTypeT valtype;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drDefineGetValueType(%s,&err);\n\n",
                        oid_string(def));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(def.v1) != SI2DR_DEFINE) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return 0;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        return 0;
                }
        }
        switch (d->valtype) {
                case LIBERTY__VAL_STRING:
                        valtype = SI2DR_STRING;
                        break;

                case LIBERTY__VAL_DOUBLE:
                        valtype = SI2DR_FLOAT64;
                        break;

                case LIBERTY__VAL_BOOLEAN:
                        valtype = SI2DR_BOOLEAN;
                        break;

                case LIBERTY__VAL_INT:
                        valtype = SI2DR_INT32;
                        break;

                default:
                case LIBERTY__VAL_UNDEFINED:
                        valtype = SI2DR_UNDEFINED_VALUETYPE;
                        break;
        }
        *err = SI2DR_NO_ERROR;
        return valtype;
}

si2drObjectIdT si2drObjectGetOwner(si2drObjectIdT object, si2drErrorT *err) {
        si2drObjectTypeT ot = (si2drObjectTypeT)object.v1;
        si2drObjectIdT retoid;

        liberty_group *g;
        liberty_attribute *a;
        liberty_define *d;

        if (trace) {
                fprintf(tracefile1, "\n\tsi2drObjectGetOwner(%s,&err);\n\n",
                        oid_string(object));
                inc_tracecount();
        }
        *err = SI2DR_NO_ERROR;

        switch (ot) {
                case SI2DR_GROUP:
                        g = (liberty_group *)object.v2;
                        retoid.v2 = (void *)g->owner;
                        retoid.v1 = (void *)SI2DR_GROUP;
                        break;

                case SI2DR_ATTR:
                        a = (liberty_attribute *)object.v2;
                        retoid.v2 = (void *)a->owner;
                        retoid.v1 = (void *)SI2DR_GROUP;
                        break;

                case SI2DR_DEFINE:
                        d = (liberty_define *)object.v2;
                        retoid.v2 = (void *)d->owner;
                        retoid.v1 = (void *)SI2DR_GROUP;
                        break;

                default:
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        return nulloid;
        }
        return retoid;
}

si2drStringT si2drAttrGetName(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        if (trace) {
                fprintf(tracefile1, "\n\tsi2drAttrGetName(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }

        if (!liberty___nocheck_mode &&
            (si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                     SI2DR_INVALID_OBJECTTYPE,
                                     "si2drAttrGetName:", &err2);
                }
                return 0;
        }
        *err = SI2DR_NO_ERROR;

        return x->name;
}

void create_floating_define_for_cell_area(si2drStringT string) {
        si2drDefineIdT retoid;
        liberty_define *d;

        /* now, given this string, let's create a define; */
        d = (liberty_define *)my_calloc(sizeof(liberty_define), 1);

        retoid.v1 = (void *)SI2DR_DEFINE;
        retoid.v2 = (void *)d;

        d->name = string;

        d->group_type = "cell";
        d->valtype = LIBERTY__VAL_DOUBLE;

        liberty_hash_enter_oid(master_define_hash, string, retoid);
}

si2drVoidT si2drGroupMoveBefore(si2drGroupIdT groupToMove,
                                si2drGroupIdT targetGroup, si2drErrorT *err) {
        int in_trace;
        liberty_group *gm = (liberty_group *)groupToMove.v2;
        liberty_group *gt = (liberty_group *)targetGroup.v2;
        liberty_group *gb;
        liberty_group *gtb;
        liberty_group *go = gm->owner;

        in_trace = 0;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drGroupMoveBefore(\"%s\",%s,&err);\n\n",
                        oid_string(groupToMove), oid_string(targetGroup));
                inc_tracecount();
                in_trace = 1;
                trace = 0;
        }
        /* make sure the groups belong to the same owner */

        if (gt == gm) {
                *err = SI2DR_NO_ERROR;
                return;
        }
        if (gm->next == gt) {
                *err = SI2DR_NO_ERROR;
                return;
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(groupToMove.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupMoveBefore:", &err2);
                        }
                        return;
                }
                if ((si2drObjectTypeT)(targetGroup.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupMoveBefore:", &err2);
                        }
                        return;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupMoveBefore:", &err2);
                        }
                        return;
                }
                if (gm->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupMoveBefore:", &err2);
                        }
                        return;
                }
                if (gt->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupMoveBefore:", &err2);
                        }
                        return;
                }
                if (gm->owner != gt->owner) {
                        *err = SI2DR_REFERENCE_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_REFERENCE_ERROR,
                                             "si2drGroupMoveBefore:", &err2);
                        }
                        return;
                }
        }

        *err = SI2DR_NO_ERROR;

        if (gm != go->group_list) {
                for (gb = go->group_list; gb && gb->next != gm; gb = gb->next)
                        ;
        } else
                gb = 0;

        if (gt != go->group_list) {
                for (gtb = go->group_list; gtb && gtb->next != gt;
                     gtb = gtb->next)
                        ;
        } else
                gtb = 0;

        if (gb == gm || gtb == gt || (!gtb && !gb)) {
                *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                     SI2DR_INTERNAL_SYSTEM_ERROR,
                                     "si2drGroupMoveBefore:", &err2);
                }
                return;
        }

        if (!gb) {
                /* gb is at the front */

                go->group_list = gm->next; /* unlink gm from the list */
                gtb->next =
                    gm; /* attach it the guy in front of gt, unlinking gt */
                gm->next = gt; /* attach gt after gm */
        } else {
                if (go->group_last == gm) go->group_last = gb;
                if (!gtb) {
                        /* the target is at the front of the list */
                        gb->next = gm->next; /* gm now unlinked */
                        go->group_list =
                            gm; /* now the list points to gm, and gt unlinked */
                        gm->next = gt; /* and gt is attached right after gm */
                } else {
                        /* neither are at the head of the list */
                        gb->next = gm->next; /* gm now unlinked */
                        gtb->next = gm; /* the guy before gt now links to gm */
                        gm->next =
                            gt; /* gm now points to gt as its follower. */
                }
        }
}

si2drVoidT si2drGroupMoveAfter(si2drGroupIdT groupToMove,
                               si2drGroupIdT targetGroup, si2drErrorT *err) {
        int in_trace;
        liberty_group *gm = (liberty_group *)groupToMove.v2;
        liberty_group *gt = (liberty_group *)targetGroup.v2;
        liberty_group *gb;
        liberty_group *go = gm->owner;

        in_trace = 0;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drGroupMoveAfter(\"%s\",%s,&err);\n\n",
                        oid_string(groupToMove), oid_string(targetGroup));
                inc_tracecount();
                in_trace = 1;
                trace = 0;
        }
        /* make sure the groups belong to the same owner */

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(groupToMove.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupMoveAfter:", &err2);
                        }
                        return;
                }
                if ((si2drObjectTypeT)(targetGroup.v1) != SI2DR_GROUP) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drGroupMoveAfter:", &err2);
                        }
                        return;
                }
                if (master_group_hash == 0) {
                        *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INTERNAL_SYSTEM_ERROR,
                                             "si2drGroupMoveAfter:", &err2);
                        }
                        return;
                }
                if (gm->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupMoveAfter:", &err2);
                        }
                        return;
                }
                if (gt->attr_hash == (liberty_hash_table *)NULL) {
                        *err = SI2DR_UNUSABLE_OID;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_UNUSABLE_OID,
                                             "si2drGroupMoveAfter:", &err2);
                        }
                        return;
                }
                if (gm->owner != gt->owner) {
                        *err = SI2DR_REFERENCE_ERROR;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_REFERENCE_ERROR,
                                             "si2drGroupMoveAfter:", &err2);
                        }
                        return;
                }
        }

        *err = SI2DR_NO_ERROR;

        if (gm != go->group_list) {
                for (gb = go->group_list; gb && gb->next != gm; gb = gb->next)
                        ;
        } else
                gb = 0;

        if (gb == gm) {
                *err = SI2DR_INTERNAL_SYSTEM_ERROR;
                if (liberty___debug_mode) {
                        si2drErrorT err2;

                        (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                     SI2DR_INTERNAL_SYSTEM_ERROR,
                                     "si2drGroupMoveAfter:", &err2);
                }
                return;
        }
        if (gt == gm) return;
        if (gt->next == gm) return;

        if (go->group_last == gt) go->group_last = gm;

        if (!gb) {
                go->group_list = gm->next;
        } else {
                gb->next = gm->next;
        }
        gm->next = gt->next;
        gt->next = gm;
}

/* expr routines */

si2drExprT *si2drSimpleAttrGetExprValue(si2drAttrIdT attr, si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrGetExprValue(%s,&err);\n\n",
                        oid_string(attr));
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrGetExprValue:",
                                             &err2);
                        }
                        return 0;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrGetExprValue:", &err2);
                        }
                        return 0;
                }
                if (x->value == (liberty_attribute_value *)NULL) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetExprValue:", &err2);
                        }
                        return 0;
                }
                if (x->value->type != LIBERTY__VAL_EXPR) {
                        *err = SI2DR_INVALID_VALUE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_VALUE,
                                    "si2drSimpleAttrGetExprValue:", &err2);
                        }
                        return 0;
                }
        }

        *err = SI2DR_NO_ERROR;
        return (si2drExprT *)x->value->u.expr_val;
}

si2drVoidT si2drSimpleAttrSetExprValue(si2drAttrIdT attr, si2drExprT *expr,
                                       si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drSimpleAttrSetExprValue(%s,%lx,&err);\n\n",
                        oid_string(attr), expr);
                inc_tracecount();
        }
        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drSimpleAttrSetExprValue:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__SIMPLE) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drSimpleAttrSetExprValue:", &err2);
                        }
                        return;
                }
        }

        if (x->value == (liberty_attribute_value *)NULL) {
                x->value = (liberty_attribute_value *)my_calloc(
                    sizeof(liberty_attribute_value), 1);
        }
        x->value->type = LIBERTY__VAL_EXPR;
        x->value->u.expr_val = expr;

        *err = SI2DR_NO_ERROR;
}

si2drVoidT si2drExprDestroy(
    si2drExprT *expr, /* recursively free the structures */
    si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (expr->left) si2drExprDestroy(expr->left, err);
        if (expr->right) si2drExprDestroy(expr->right, err);
        expr->left = 0;
        expr->right = 0;
        my_free(expr);
}

si2drExprT *si2drCreateExpr(
    si2drExprTypeT type, /* malloc an Expr and return it */
    si2drErrorT *err) {
        si2drExprT *x;

        *err = SI2DR_NO_ERROR;
        x = (si2drExprT *)calloc(sizeof(si2drExprT), 1);
        x->type = type;
        return x;
}

si2drExprT *si2drCreateBooleanValExpr(si2drBooleanT b, si2drErrorT *err) {
        si2drExprT *x;

        *err = SI2DR_NO_ERROR;
        x = (si2drExprT *)calloc(sizeof(si2drExprT), 1);
        x->type = SI2DR_EXPR_VAL;
        x->valuetype = SI2DR_BOOLEAN;
        x->u.b = b;
        return x;
}

si2drExprT *si2drCreateDoubleValExpr(si2drFloat64T d, si2drErrorT *err) {
        si2drExprT *x;

        *err = SI2DR_NO_ERROR;
        x = (si2drExprT *)calloc(sizeof(si2drExprT), 1);
        x->type = SI2DR_EXPR_VAL;
        x->valuetype = SI2DR_FLOAT64;
        x->u.d = d;
        return x;
}

si2drExprT *si2drCreateStringValExpr(si2drStringT s, si2drErrorT *err) {
        si2drExprT *x;

        *err = SI2DR_NO_ERROR;

        x = (si2drExprT *)calloc(sizeof(si2drExprT), 1);
        x->type = SI2DR_EXPR_VAL;
        x->valuetype = SI2DR_STRING;
        x->u.s = s;
        return x;
}

si2drExprT *si2drCreateIntValExpr(si2drInt32T i, si2drErrorT *err) {
        si2drExprT *x;

        *err = SI2DR_NO_ERROR;
        x = (si2drExprT *)calloc(sizeof(si2drExprT), 1);
        x->type = SI2DR_EXPR_VAL;
        x->valuetype = SI2DR_INT32;
        x->u.i = i;
        return x;
}

si2drExprT *si2drCreateBinaryOpExpr(si2drExprT *left, si2drExprTypeT optype,
                                    si2drExprT *right, si2drErrorT *err) {
        si2drExprT *x;

        *err = SI2DR_NO_ERROR;
        x = (si2drExprT *)calloc(sizeof(si2drExprT), 1);
        x->type = optype;
        x->left = left;
        x->right = right;
        return x;
}

si2drExprT *si2drCreateUnaryOpExpr(si2drExprTypeT optype, si2drExprT *expr,
                                   si2drErrorT *err) {
        si2drExprT *x;

        *err = SI2DR_NO_ERROR;
        x = (si2drExprT *)calloc(sizeof(si2drExprT), 1);
        x->type = optype;
        x->left = expr;
        return x;
}

si2drStringT si2drExprToString(si2drExprT *expr, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (expr->type != SI2DR_EXPR_VAL && expr->type != SI2DR_EXPR_OP_ADD &&
            expr->type != SI2DR_EXPR_OP_SUB &&
            expr->type != SI2DR_EXPR_OP_MUL &&
            expr->type != SI2DR_EXPR_OP_DIV &&
            expr->type != SI2DR_EXPR_OP_PAREN &&
            expr->type != SI2DR_EXPR_OP_LOG2 &&
            expr->type != SI2DR_EXPR_OP_LOG10 &&
            expr->type != SI2DR_EXPR_OP_EXP) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return "";
        }
        return expr_string(expr);
}

si2drExprTypeT si2drExprGetType(si2drExprT *expr, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return SI2DR_EXPR_VAL;
        }

        return expr->type;
}

si2drValueTypeT si2drValExprGetValueType(si2drExprT *expr, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return SI2DR_MAX_VALUETYPE;
        }
        if (expr->type != SI2DR_EXPR_VAL) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return 0;
        }
        return expr->valuetype;
}

si2drInt32T si2drIntValExprGetInt(si2drExprT *expr, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return 0;
        }
        if (expr->type != SI2DR_EXPR_VAL || expr->valuetype != SI2DR_INT32) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return 0;
        }
        return expr->u.i;
}

si2drFloat64T si2drDoubleValExprGetDouble(si2drExprT *expr, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return 0.0;
        }
        if (expr->type != SI2DR_EXPR_VAL || expr->valuetype != SI2DR_FLOAT64) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return 0;
        }
        return expr->u.d;
}

si2drBooleanT si2drBooleanValExprGetBoolean(si2drExprT *expr,
                                            si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return 0;
        }
        if (expr->type != SI2DR_EXPR_VAL || expr->valuetype != SI2DR_BOOLEAN) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return 0;
        }
        return expr->u.b;
}

si2drStringT si2drStringValExprGetString(si2drExprT *expr, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return 0;
        }
        if (expr->type != SI2DR_EXPR_VAL || expr->valuetype != SI2DR_STRING) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return 0;
        }
        return expr->u.s;
}

si2drExprT *si2drOpExprGetLeftExpr(
    si2drExprT *expr, /* will apply to Unary & binary Ops */
    si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return 0;
        }
        if (expr->type == SI2DR_EXPR_VAL) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return 0;
        }
        return expr->left;
}

si2drExprT *si2drOpExprGetRightExpr(si2drExprT *expr, si2drErrorT *err) {
        *err = SI2DR_NO_ERROR;
        if (!expr) {
                *err = SI2DR_REFERENCE_ERROR;
                return 0;
        }
        if (expr->type != SI2DR_EXPR_OP_ADD &&
            expr->type != SI2DR_EXPR_OP_SUB &&
            expr->type != SI2DR_EXPR_OP_MUL &&
            expr->type != SI2DR_EXPR_OP_DIV &&
            expr->type != SI2DR_EXPR_OP_EXP) {
                *err = SI2DR_INVALID_OBJECTTYPE;
                return 0;
        }
        return expr->right;
}

LONG_DOUBLE liberty_get_element(struct liberty_value_data *vd,
                                ...) /* returns NaN if bounds exceeded */
{
        va_list ap;
        int indices[7], i;
        int pos;

        va_start(ap, vd);

        for (i = 0; i < vd->dimensions; i++) {
                indices[i] = va_arg(ap, int);
                if (indices[i] < 0 || indices[i] > vd->dim_sizes[i]) {
#ifndef NO_NANL
                        return nanl("");
#else
                        return 1.0 / 0.0;
#endif
                }
        }

        va_end(ap);

        pos = 0;
        for (i = 0; i < vd->dimensions; i++) {
                pos = vd->dim_sizes[i] * pos + indices[i];
        }

        return vd->values[pos];
}

void liberty_destroy_value_data(struct liberty_value_data *vd) {
        int i;

        if (vd->values) free(vd->values);
        vd->values = 0;
        if (vd->dim_sizes) free(vd->dim_sizes);
        vd->dim_sizes = 0;
        for (i = 0; i < vd->dimensions; i++) {
                if (vd->index_info) {
                        if (vd->index_info[i]) {
                                free(vd->index_info[i]);
                                vd->index_info[i] = 0;
                        }
                }
        }
        if (vd->index_info) {
                free(vd->index_info);
                vd->index_info = 0;
        }
        vd->dimensions = 0;
        free(vd);
}

static void liberty_get_index_info(si2drAttrIdT index_x,
                                   struct liberty_value_data *vd, int dimno) {
        si2drErrorT err;
        si2drValuesIdT vals;
        si2drValueTypeT vtype;
        si2drInt32T intgr;
        si2drFloat64T float64;
        si2drStringT string;
        si2drBooleanT bool;
        si2drExprT *expr;
        char *p, *t = NULL;
        int numcount, i2;
        LONG_DOUBLE x;

        if (!si2drObjectIsNull(index_x, &err)) {
                /* should be a string with a list of comma/space sep'd numbers
                 */
                if (si2drAttrGetAttrType(index_x, &err) != SI2DR_COMPLEX) {
                        fprintf(stderr,
                                "The 'index_%d' attribute is not a complex "
                                "attribute.",
                                dimno + 1);
                        return; /* this will already be reported */
                }

                vals = si2drComplexAttrGetValues(index_x, &err);
                si2drIterNextComplexValue(vals, &vtype, &intgr, &float64,
                                          &string, &bool, &expr, &err);
                si2drIterQuit(vals, &err);

                if (vtype == SI2DR_UNDEFINED_VALUETYPE) {
                        fprintf(stderr,
                                "The 'index_%d' attribute has no values.",
                                dimno + 1);
                        return;
                }
                if (vtype != SI2DR_STRING) {
                        if (vtype == SI2DR_FLOAT64) {
                                LONG_DOUBLE *pg;
                                numcount = 1;
                                vd->index_info[dimno] = (LONG_DOUBLE *)calloc(
                                    sizeof(LONG_DOUBLE), numcount);
                                vd->dim_sizes[dimno] = numcount;
                                pg = vd->index_info[dimno];
                                pg[0] = float64;
                                return;
                        }
                        fprintf(stderr,
                                "The 'index_%d' attribute should be a string "
                                "or float.\n",
                                dimno + 1);
                        return;
                }

                if (!string || !string[0]) {
                        fprintf(stderr,
                                "The 'index_%d' attribute is an empty string.",
                                dimno + 1);
                        return;
                }

                p = string;
                numcount = 0;

                while ((x = (LONG_DOUBLE)strtod(p, &t)), t != p) {
                        numcount++;

                        if (t && *t == 0) break;

                        /* skip over intervening stuff to the next number */
                        while (t && (*t == ',' || *t == ' ' || *t == '\t' ||
                                     *t == '\n' || *t == '\r' || *t == '\\')) {
                                if (*t == '\\' &&
                                    *(t + 1) ==
                                        '\n') /*skip over line escapes */
                                        t++;
                                t++;
                        }
                        p = t;
                }
                if (numcount) {
                        vd->index_info[dimno] = (LONG_DOUBLE *)calloc(
                            sizeof(LONG_DOUBLE), numcount);
                        vd->dim_sizes[dimno] = numcount;

                        p = string;
                        i2 = 0;

                        while ((x = strtod(p, &t)), t != p) {
                                LONG_DOUBLE *pg = vd->index_info[dimno];

                                pg[i2] = x;
                                if (t && *t == 0) break;

                                i2++;
                                /* skip over intervening stuff to the next
                                 * number */
                                while (*t == ',' || *t == ' ' || *t == '\t' ||
                                       *t == '\n' || *t == '\r' || *t == '\\') {
                                        if (*t == '\\' &&
                                            *(t + 1) == '\n') /*skip over line
                                                                 escapes */
                                                t++;
                                        t++;
                                }
                                p = t;
                        }
                }
        }
}

static si2drGroupIdT get_containing_group(si2drGroupIdT group,
                                          si2drStringT type) {
        si2drErrorT err;
        const struct libGroupMap *lgm;
        si2drGroupIdT gup = group;
        group_enum gt = 0;
        lgm = lookup_group_name(type, strlen(type));
        if (lgm)
                gt = lgm->type;
        else {
                fprintf(stderr, "BAD type (%s) handed to get_containing_group!",
                        type);
        }

        while (
            !si2drObjectIsNull((gup = si2drObjectGetOwner(gup, &err)), &err)) {
                if (si2drGroupGetID(gup, &err) == gt) return gup;
        }
        return si2drPIGetNullId(&err);
}

struct liberty_value_data *liberty_get_values_data(si2drGroupIdT table_group) {
        si2drErrorT err;
        si2drNamesIdT names;
        group_enum thistype = si2drGroupGetID(table_group, &err);
        si2drAttrIdT index_1 =
            si2drGroupFindAttrByName(table_group, "index_1", &err);
        si2drAttrIdT index_2 =
            si2drGroupFindAttrByName(table_group, "index_2", &err);
        si2drAttrIdT index_3 =
            si2drGroupFindAttrByName(table_group, "index_3", &err);
        si2drAttrIdT index_4 =
            si2drGroupFindAttrByName(table_group, "index_4", &err);
        si2drAttrIdT index_5 =
            si2drGroupFindAttrByName(table_group, "index_5", &err);
        si2drAttrIdT index_6 =
            si2drGroupFindAttrByName(table_group, "index_6", &err);
        si2drAttrIdT index_7 =
            si2drGroupFindAttrByName(table_group, "index_7", &err);
        si2drAttrIdT valuesd =
            si2drGroupFindAttrByName(table_group, "values", &err);
        si2drGroupIdT libr, template;
        template.v1 = 0;
        template.v2 = 0;
        libr.v1 = 0;
        libr.v2 = 0;
        si2drValuesIdT vals;
        si2drValueTypeT vtype;
        si2drInt32T intgr;
        si2drFloat64T float64;
        si2drStringT string;
        si2drBooleanT bool;
        si2drExprT *expr;
        char *p, *t;
        LONG_DOUBLE x;
        struct liberty_value_data *vd = (struct liberty_value_data *)calloc(
            1 + 1, sizeof(struct liberty_value_data));
        int numels;
        si2drStringT refname;

        /* find the template, fill in any missing index vals! */
        libr = get_containing_group(table_group, "library");

        names = si2drGroupGetNames(table_group, &err);
        refname = si2drIterNextName(names, &err);
        si2drIterQuit(names, &err);

        if (!refname || !strcasecmp(refname, "scalar")) {
                /* scalar is a brand name for a single value */
                numels = 1;
                vd->dimensions = 1;
                vd->dim_sizes = (int *)calloc(vd->dimensions + 1, sizeof(int));
                vd->dim_sizes[0] = 1;
                vd->index_info = (LONG_DOUBLE **)calloc(sizeof(LONG_DOUBLE *),
                                                        vd->dimensions);
                vd->index_info[0] =
                    (LONG_DOUBLE *)calloc(sizeof(LONG_DOUBLE), 1);
                vd->index_info[0][0] = 0.000;
        } else {
                switch (thistype) {
                        case LIBERTY_GROUPENUM_cell_fall:
                        case LIBERTY_GROUPENUM_cell_rise:
                        case LIBERTY_GROUPENUM_rise_transition:
                        case LIBERTY_GROUPENUM_fall_transition:
                        case LIBERTY_GROUPENUM_retaining_fall:
                        case LIBERTY_GROUPENUM_retaining_rise:
                        case LIBERTY_GROUPENUM_retain_rise_slew:
                        case LIBERTY_GROUPENUM_retain_fall_slew:
                        case LIBERTY_GROUPENUM_receiver_capacitance1_fall:
                        case LIBERTY_GROUPENUM_receiver_capacitance1_rise:
                        case LIBERTY_GROUPENUM_receiver_capacitance2_fall:
                        case LIBERTY_GROUPENUM_receiver_capacitance2_rise:
                        case LIBERTY_GROUPENUM_va_receiver_capacitance2_rise:
                        case LIBERTY_GROUPENUM_va_receiver_capacitance2_fall:
                        case LIBERTY_GROUPENUM_va_receiver_capacitance1_rise:
                        case LIBERTY_GROUPENUM_va_receiver_capacitance1_fall:
                        case LIBERTY_GROUPENUM_va_fall_constraint:
                        case LIBERTY_GROUPENUM_va_rise_constraint:
                        case LIBERTY_GROUPENUM_rise_constraint:
                        case LIBERTY_GROUPENUM_fall_constraint:
                        case LIBERTY_GROUPENUM_rise_propagation:
                        case LIBERTY_GROUPENUM_fall_propagation:
                        case LIBERTY_GROUPENUM_dc_current:
                        case LIBERTY_GROUPENUM_normalized_driver_waveform:
                        case LIBERTY_GROUPENUM_fall_transition_degradation:
                        case LIBERTY_GROUPENUM_rise_transition_degradation:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "lu_table_template", &err);
                                if (si2drObjectIsNull(template, &err))
                                        template = si2drGroupFindGroupByName(
                                            libr, refname, "poly_template",
                                            &err);
                                break;
                        case LIBERTY_GROUPENUM_fall_power:
                        case LIBERTY_GROUPENUM_rise_power:
                        case LIBERTY_GROUPENUM_internal_power:
                        case LIBERTY_GROUPENUM_power:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "power_lut_template", &err);
                                if (si2drObjectIsNull(template, &err))
                                        template = si2drGroupFindGroupByName(
                                            libr, refname,
                                            "power_poly_template", &err);
                                break;
                        case LIBERTY_GROUPENUM_noise_immunity_above_high:
                        case LIBERTY_GROUPENUM_noise_immunity_below_low:
                        case LIBERTY_GROUPENUM_noise_immunity_high:
                        case LIBERTY_GROUPENUM_noise_immunity_low:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "noise_lut_template", &err);
                                if (si2drObjectIsNull(template, &err))
                                        template = si2drGroupFindGroupByName(
                                            libr, refname, "poly_template",
                                            &err);
                                break;
                        case LIBERTY_GROUPENUM_propagated_noise_height_below_low:
                        case LIBERTY_GROUPENUM_propagated_noise_height_above_high:
                        case LIBERTY_GROUPENUM_propagated_noise_height_high:
                        case LIBERTY_GROUPENUM_propagated_noise_height_low:
                        case LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_above_high:
                        case LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_below_low:
                        case LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_high:
                        case LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_low:
                        case LIBERTY_GROUPENUM_propagated_noise_width_above_high:
                        case LIBERTY_GROUPENUM_propagated_noise_width_below_low:
                        case LIBERTY_GROUPENUM_propagated_noise_width_high:
                        case LIBERTY_GROUPENUM_propagated_noise_width_low:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "propagation_lut_template",
                                    &err);
                                if (si2drObjectIsNull(template, &err))
                                        template = si2drGroupFindGroupByName(
                                            libr, refname, "poly_template",
                                            &err);
                                break;
                        case LIBERTY_GROUPENUM_steady_state_current_high:
                        case LIBERTY_GROUPENUM_steady_state_current_low:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "iv_lut_template", &err);
                                if (si2drObjectIsNull(template, &err))
                                        template = si2drGroupFindGroupByName(
                                            libr, refname, "poly_template",
                                            &err);
                                break;
                        case LIBERTY_GROUPENUM_vector:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "output_current_template",
                                    &err);
                                if (si2drObjectIsNull(template, &err))
                                        template = si2drGroupFindGroupByName(
                                            libr, refname,
                                            "pg_current_template", &err);
                                if (si2drObjectIsNull(template, &err))
                                        template = si2drGroupFindGroupByName(
                                            libr, refname, "lu_table_template",
                                            &err);
                                break;

                        case LIBERTY_GROUPENUM_em_max_toggle_rate:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "em_lut_template", &err);
                                break;

                        case LIBERTY_GROUPENUM_max_cap:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "maxcap_lut_template", &err);
                                break;

                        case LIBERTY_GROUPENUM_max_trans:
                                template = si2drGroupFindGroupByName(
                                    libr, refname, "maxtrans_lut_template",
                                    &err);
                                break;
                        default:
                                break;
                }

                if (si2drObjectIsNull(template, &err)) {
                        fprintf(stderr,
                                "Could not find the template '%s' for the "
                                "table '%s'. Sorry!\n",
                                refname,
                                si2drGroupGetGroupType(table_group, &err));
                }

                /* fill in any missing index values from the template */

                if (si2drObjectIsNull(index_1, &err))
                        index_1 =
                            si2drGroupFindAttrByName(template, "index_1", &err);
                if (si2drObjectIsNull(index_2, &err))
                        index_2 =
                            si2drGroupFindAttrByName(template, "index_2", &err);
                if (si2drObjectIsNull(index_3, &err))
                        index_3 =
                            si2drGroupFindAttrByName(template, "index_3", &err);
                if (si2drObjectIsNull(index_4, &err))
                        index_4 =
                            si2drGroupFindAttrByName(template, "index_4", &err);
                if (si2drObjectIsNull(index_5, &err))
                        index_5 =
                            si2drGroupFindAttrByName(template, "index_5", &err);
                if (si2drObjectIsNull(index_6, &err))
                        index_6 =
                            si2drGroupFindAttrByName(template, "index_6", &err);
                if (si2drObjectIsNull(index_7, &err))
                        index_7 =
                            si2drGroupFindAttrByName(template, "index_7", &err);

                /* find the number of dims */
                if (!si2drObjectIsNull(index_1, &err)) {
                        vd->dimensions = 1;
                        if (!si2drObjectIsNull(index_2, &err)) {
                                vd->dimensions = 2;
                                if (!si2drObjectIsNull(index_3, &err)) {
                                        vd->dimensions = 3;
                                        if (!si2drObjectIsNull(index_4, &err)) {
                                                vd->dimensions = 4;
                                                if (!si2drObjectIsNull(index_5,
                                                                       &err)) {
                                                        vd->dimensions = 5;
                                                        if (!si2drObjectIsNull(
                                                                index_6,
                                                                &err)) {
                                                                vd->dimensions =
                                                                    6;
                                                                if (!si2drObjectIsNull(
                                                                        index_7,
                                                                        &err)) {
                                                                        vd->dimensions =
                                                                            7;
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
                vd->dim_sizes = (int *)calloc(sizeof(int), vd->dimensions);
                vd->index_info = (LONG_DOUBLE **)calloc(sizeof(LONG_DOUBLE *),
                                                        vd->dimensions);

                /* now, find and set the sizes of each dim */
                if (!si2drObjectIsNull(index_1, &err)) {
                        liberty_get_index_info(index_1, vd, 0);

                        if (!si2drObjectIsNull(index_2, &err)) {
                                liberty_get_index_info(index_2, vd, 1);
                                if (!si2drObjectIsNull(index_3, &err)) {
                                        liberty_get_index_info(index_3, vd, 2);
                                        if (!si2drObjectIsNull(index_4, &err)) {
                                                liberty_get_index_info(index_4,
                                                                       vd, 3);
                                                if (!si2drObjectIsNull(index_5,
                                                                       &err)) {
                                                        liberty_get_index_info(
                                                            index_5, vd, 4);
                                                        if (!si2drObjectIsNull(
                                                                index_6,
                                                                &err)) {
                                                                liberty_get_index_info(
                                                                    index_6, vd,
                                                                    5);
                                                                if (!si2drObjectIsNull(
                                                                        index_7,
                                                                        &err)) {
                                                                        liberty_get_index_info(
                                                                            index_7,
                                                                            vd,
                                                                            6);
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }

                /* now, malloc and fill the values */
                numels = 1;
                if (!si2drObjectIsNull(index_1, &err)) {
                        numels *= vd->dim_sizes[0];

                        if (!si2drObjectIsNull(index_2, &err)) {
                                numels *= vd->dim_sizes[1];
                                if (!si2drObjectIsNull(index_3, &err)) {
                                        numels *= vd->dim_sizes[2];
                                        if (!si2drObjectIsNull(index_4, &err)) {
                                                numels *= vd->dim_sizes[3];
                                                if (!si2drObjectIsNull(index_5,
                                                                       &err)) {
                                                        numels *=
                                                            vd->dim_sizes[4];
                                                        if (!si2drObjectIsNull(
                                                                index_6,
                                                                &err)) {
                                                                numels *=
                                                                    vd->dim_sizes
                                                                        [5];
                                                                if (!si2drObjectIsNull(
                                                                        index_7,
                                                                        &err)) {
                                                                        numels *=
                                                                            vd->dim_sizes
                                                                                [6];
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }

        /* Now, fill in the values array from the values attr */

        vd->values = (LONG_DOUBLE *)calloc(sizeof(LONG_DOUBLE), numels);

        if (!si2drObjectIsNull(valuesd, &err)) {
                int totalcount = 0;
                int numcount;
                int first = 1;

                /* should be a string with a list of comma/space sep'd numbers
                 */
                if (si2drAttrGetAttrType(valuesd, &err) != SI2DR_COMPLEX) {
                        fprintf(stderr,
                                "The 'values' attribute is not a complex "
                                "attribute.\n");
                        return vd; /* this will already be reported */
                }

                vals = si2drComplexAttrGetValues(valuesd, &err);
                do {
                        si2drIterNextComplexValue(vals, &vtype, &intgr,
                                                  &float64, &string, &bool,
                                                  &expr, &err);
                        if (first && vtype == SI2DR_UNDEFINED_VALUETYPE) {
                                fprintf(
                                    stderr,
                                    "The 'values' attribute has no values.\n");
                                break;
                        }
                        if (vtype == SI2DR_UNDEFINED_VALUETYPE) break;

                        first = 0;
                        if (vtype != SI2DR_STRING) {
                                fprintf(stderr,
                                        "The 'values' attribute should be a "
                                        "string.\n");
                                break;
                        }

                        if (!string || !string[0]) {
                                fprintf(stderr,
                                        "The 'values' attribute is an empty "
                                        "string.\n");
                                break;
                        }

                        p = string;
                        numcount = 0;

                        while ((x = strtod(p, &t)), t != p) {
                                vd->values[totalcount + numcount] = x;

                                numcount++;

                                if (t && *t == 0) break;

                                /* skip over intervening stuff to the next
                                 * number */
                                while (*t == ',' || *t == ' ' || *t == '\t' ||
                                       *t == '\n' || *t == '\r' || *t == '\\') {
                                        if (*t == '\\' &&
                                            *(t + 1) == '\n') /*skip over line
                                                                 escapes */
                                                t++;
                                        t++;
                                }
                                p = t;
                        }
                        totalcount += numcount;

                } while (vtype != SI2DR_UNDEFINED_VALUETYPE);

                si2drIterQuit(vals, &err);
                if (totalcount != numels) {
                        fprintf(stderr,
                                "The table 'values' attribute has %d vs. %d "
                                "expected values\n",
                                totalcount, numels);
                }
        } else {
                fprintf(stderr, "The group contains no 'values' attribute.\n");
        }
        return vd;
}

si2drVoidT si2drComplexAttrAddExprValue(si2drAttrIdT attr, si2drExprT *expr,
                                        si2drErrorT *err) {
        liberty_attribute *x = (liberty_attribute *)attr.v2;
        liberty_attribute_value *y;

        if (trace) {
                fprintf(tracefile1,
                        "\n\tsi2drComplexAttrAddExprValue(%s,%lx,&err);\n\n",
                        oid_string(attr), expr);
                inc_tracecount();
        }

        if (!liberty___nocheck_mode) {
                if ((si2drObjectTypeT)(attr.v1) != SI2DR_ATTR) {
                        *err = SI2DR_INVALID_OBJECTTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(SI2DR_SEVERITY_ERR,
                                             SI2DR_INVALID_OBJECTTYPE,
                                             "si2drComplexAttrAddExprValue:",
                                             &err2);
                        }
                        return;
                }
                if (x->type != LIBERTY__COMPLEX) {
                        *err = SI2DR_INVALID_ATTRTYPE;
                        if (liberty___debug_mode) {
                                si2drErrorT err2;

                                (*si2ErrMsg)(
                                    SI2DR_SEVERITY_ERR, SI2DR_INVALID_ATTRTYPE,
                                    "si2drComplexAttrAddExprValue:", &err2);
                        }
                        return;
                }
        }

        y = (liberty_attribute_value *)my_calloc(
            sizeof(liberty_attribute_value), 1);
        y->type = LIBERTY__VAL_EXPR;
        y->u.expr_val = expr;

        /* link the attr struct into the lists */
        if (x->last_value) {
                x->last_value->next = y;
                x->last_value = y;
        } else {
                x->last_value = y;
                x->value = y;
        }
        *err = SI2DR_NO_ERROR;
}

// Guilherme Flach 2016-03-27
// Added to ignore some warning messages in this file.
#pragma GCC diagnostic pop