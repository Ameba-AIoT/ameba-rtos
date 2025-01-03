/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user or with the express written consent of
 * Sun Microsystems, Inc.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * From: @(#)rpc_clntout.c 1.11 89/02/22 (C) 1987 SMI
 */
char clntout_rcsid[] =
	"$Id: rpc_clntout.c,v 1.6 1999/11/23 17:10:21 drepper Exp $";

/*
 * rpc_clntout.c, Client-stub outputter for the RPC protocol compiler
 * Copyright (C) 1987, Sun Microsystems, Inc.
 */
#include <stdio.h>
#include <string.h>
#include <rpc/types.h>
#include "rpc_parse.h"
#include "rpc_util.h"
#include "proto.h"

#define DEFAULT_TIMEOUT 25	/* in seconds */
//edit by ycyang
//static const char RESULT[] = "clnt_res";
static const char RESULT[] = "result";

static void write_program(definition *def);
static void printbody(proc_list *proc);
static const char *ampr(const char *type);
static void printbody(proc_list *proc);


void
write_stubs(void)
{
	list *l;
	definition *def;

	//edit by ycyang
//  fprintf (fout,
//	   "\n/* Default timeout can be changed using clnt_control() */\n");
//  fprintf (fout, "static struct timeval TIMEOUT = { %d, 0 };\n",
//	   DEFAULT_TIMEOUT);

	for (l = defined; l != NULL; l = l->next) {
		def = (definition *) l->val;
		if (def->def_kind == DEF_PROGRAM) {
			write_program(def);
		}
	}
}

static void
write_program(definition *def)
{
	version_list *vp;
	proc_list *proc;

	for (vp = def->def.pr.versions; vp != NULL; vp = vp->next) {
		for (proc = vp->procs; proc != NULL; proc = proc->next) {
			fprintf(fout, "\n");
			if (mtflag == 0) {
				ptype(proc->res_prefix, proc->res_type, 1);
				fprintf(fout, "*\n");
				pvname(proc->proc_name, vp->vers_num);
				//edit by ycyang
				printarglist(proc, RESULT, "clnt", "CLNT_STRUCT *");
				//printarglist (proc, RESULT, "clnt, bool_t *isSuccess", "CLNT_STRUCT *");
			} else {
				fprintf(fout, "enum clnt_stat \n");
				pvname(proc->proc_name, vp->vers_num);
				//edit by ycyang
				printarglist(proc, RESULT, "clnt", "CLNT_STRUCT *");
				//printarglist (proc, RESULT, "clnt, bool_t *isSuccess", "CLNT_STRUCT *");
			}
			fprintf(fout, "{\n");
			printbody(proc);
			fprintf(fout, "}\n");
		}
	}
}

/* Writes out declarations of procedure's argument list.
   In either ANSI C style, in one of old rpcgen style (pass by reference),
   or new rpcgen style (multiple arguments, pass by value);
 */

/* sample addargname = "clnt"; sample addargtype = "CLIENT * " */

void
printarglist(proc_list *proc,  const char *result,
			 const char *addargname, const char *addargtype)
{

	decl_list *l;

	if (!newstyle) {
		/* old style: always pass argument by reference */
		if (Cflag) {
			/* C++ style heading */
			//ycyang: come in here!!
			fprintf(fout, "(");
			ptype(proc->args.decls->decl.prefix,
				  proc->args.decls->decl.type, 1);

			if (mtflag) {
				/* Generate result field */
				fprintf(fout, "*argp, ");
				ptype(proc->res_prefix, proc->res_type, 1);
				fprintf(fout, "*%s, %s%s)\n", result, addargtype, addargname);
			} else { //ycyang: come in here!
				fprintf(fout, "*argp, %s%s)\n", addargtype, addargname);
			}
			//fprintf (fout, "argp, %s%s)\n", addargtype, addargname);

		} else {
			if (!mtflag) {
				fprintf(fout, "(argp, %s)\n", addargname);
			} else {
				fprintf(fout, "(argp, %s, %s)\n", result, addargname);
			}
			fprintf(fout, "\t");
			ptype(proc->args.decls->decl.prefix,
				  proc->args.decls->decl.type, 1);
			fprintf(fout, "*argp;\n");
			if (mtflag) {
				fprintf(fout, "\t");
				ptype(proc->res_prefix, proc->res_type, 1);
				fprintf(fout, "*%s;\n", result);
			}
		}
	} else if (streq(proc->args.decls->decl.type, "void")) {
		/* newstyle, 0 argument */
		if (mtflag) {
			fprintf(fout, "(");
			if (Cflag) {
				ptype(proc->res_prefix, proc->res_type, 1);
				fprintf(fout, "*%s, %s%s)\n", result, addargtype, addargname);
			} else {
				fprintf(fout, "(%s)\n", addargname);
			}
		} else if (Cflag) {
			fprintf(fout, "(%s%s)\n", addargtype, addargname);
		} else {
			fprintf(fout, "(%s)\n", addargname);
		}
	} else {
		/* new style, 1 or multiple arguments */
		if (!Cflag) {
			fprintf(fout, "(");
			for (l = proc->args.decls; l != NULL; l = l->next) {
				fprintf(fout, "%s, ", l->decl.name);
			}
			if (mtflag) {
				fprintf(fout, "%s, ", result);
			}
			fprintf(fout, "%s)\n", addargname);
			for (l = proc->args.decls; l != NULL; l = l->next) {
				pdeclaration(proc->args.argname, &l->decl, 1, ";\n");
			}
			if (mtflag) {
				fprintf(fout, "\t");
				ptype(proc->res_prefix, proc->res_type, 1);
				fprintf(fout, "*%s;\n", result);
			}
		} else {
			/* C++ style header */
			fprintf(fout, "(");
			for (l = proc->args.decls; l != NULL; l = l->next) {
				pdeclaration(proc->args.argname, &l->decl, 0, ", ");
			}
			if (mtflag) {
				ptype(proc->res_prefix, proc->res_type, 1);
				fprintf(fout, "*%s, ", result);
			}
			fprintf(fout, " %s%s)\n", addargtype, addargname);
		}
	}

	if (!Cflag) {
		fprintf(fout, "\t%s%s;\n", addargtype, addargname);
	}
}


static
const char *
ampr(const char *type)
{
	if (isvectordef(type, REL_ALIAS)) {
		return "";
	} else {
		return "&";
	}
}

static void
printbody(proc_list *proc)
{
	decl_list *l;
	bool_t args2 = (proc->arg_num > 1);
	/*  int i; */
#if 0
	fprintf(fout, "#if defined(ENABLE_RPC_PROXY_INIT_CHECK)\n");
	fprintf(fout, "  #if 1 == BUILD_FOR_MACARTHUR3\n");
	fprintf(fout, "  #pragma message( \"BUILD_FOR_MACARTHUR3 is #define'd to 1\" )\n");
	fprintf(fout, "  #else\n");
	fprintf(fout, "  #pragma message( \"BUILD_FOR_MACARTHUR3 is #define'd to 0\" )\n");
	fprintf(fout, "  #endif\n");
	fprintf(fout, "  #if 1 == BUILD_FOR_KCPU_COMMON\n");
	fprintf(fout, "  #pragma message(\"BUILD_FOR_KCPU_COMMON is #define'd to 1\" )\n");
	fprintf(fout, "  #else\n");
	fprintf(fout, "  #pragma message(\"BUILD_FOR_KCPU_COMMON is #define'd to 0\" )\n");
	fprintf(fout, "  #endif\n");

	fprintf(fout, "  #if 1 == BUILD_FOR_MACARTHUR3 && 0 == BUILD_FOR_KCPU_COMMON\n");
	fprintf(fout, "    //THIS IS COMPILED FOR SCPU\n");
	fprintf(fout, "    extern int g_RPCProxyInitDone;\n");
	fprintf(fout, "    if (0 == g_RPCProxyInitDone)\n");
	fprintf(fout, "    {\n");
	fprintf(fout, "        //while (1)\n");
	fprintf(fout, "        //{\n");
	fprintf(fout, "        //    printf (\"g_RPCProxyInitDone not init to 1\\n\");\n");
	fprintf(fout, "        //    sleep (1);\n");
	fprintf(fout, "        //}\n");
	fprintf(fout, "        initRPCProxy();\n");
	fprintf(fout, "    }\n");
	fprintf(fout, "  #endif\n");
	fprintf(fout, "#endif //ENABLE_RPC_PROXY_INIT_CHECK\n");
#endif

	/* For new style with multiple arguments, need a structure in which
	   to stuff the arguments. */
	if (newstyle && args2) {
		fprintf(fout, "\t%s", proc->args.argname);
		fprintf(fout, " arg;\n");
	}
	//edit by ycyang
	fprintf(fout, "\tRPC_STRUCT rpc;\n");
	if (!mtflag) {
		//ycyang: come in here~
		//fprintf (fout, "\tstatic ");
		fprintf(fout, "\t");
		if (streq(proc->res_type, "void")) {
			fprintf(fout, "char ");
		} else {
			ptype(proc->res_prefix, proc->res_type, 0);
		}
		//edit by ycyang
		fprintf(fout, "* %s = NULL ;\n", RESULT);
		//fprintf (fout, "%s;\n", RESULT);
		fprintf(fout, "\tlong args_size = sizeof(");
		ptype(proc->args.decls->decl.prefix,
			  proc->args.decls->decl.type, 1);
		fprintf(fout, ");\n");
		fprintf(fout, "\n\n");
//      fprintf (fout, "\tmemset((char *)%s%s, 0, sizeof(%s));\n",
//	       ampr (proc->res_type), RESULT, RESULT);
	}
	if (newstyle && !args2 && (streq(proc->args.decls->decl.type, "void"))) {
		/* newstyle, 0 arguments */
		if (mtflag) {
			fprintf(fout, "\t return ");
		} else {
			fprintf(fout, "\t if ");
		}
		fprintf(fout,
				"(clnt_call (clnt, %s, (xdrproc_t) xdr_void, ", proc->proc_name);

		fprintf(fout,
				"(caddr_t) NULL,\n\t\t(xdrproc_t) xdr_%s, (caddr_t) %s%s,",
				stringfix(proc->res_type), (mtflag) ? "" : ampr(proc->res_type),
				RESULT);
		if (mtflag) {
			fprintf(fout, "\n\t\tTIMEOUT));\n\n");
		} else {
			fprintf(fout, "\n\t\tTIMEOUT) != 0) {\n");
		}
	} else if (newstyle && args2) {
		/* newstyle, multiple arguments:  stuff arguments into structure */
		for (l = proc->args.decls; l != NULL; l = l->next) {
			fprintf(fout, "\targ.%s = %s;\n",
					l->decl.name, l->decl.name);
		}
		if (mtflag) {
			fprintf(fout, "\treturn ");
		} else {
			fprintf(fout, "\tif ");
		}

		fprintf(fout,
				"(clnt_call (clnt, %s, (xdrproc_t) xdr_%s", proc->proc_name,
				proc->args.argname);
		fprintf(fout,
				", (caddr_t) &arg,\n\t\t(xdrproc_t) xdr_%s, (caddr_t) %s%s,",
				stringfix(proc->res_type), (mtflag) ? "" : ampr(proc->res_type),
				RESULT);
		if (mtflag) {
			fprintf(fout, "\n\t\tTIMEOUT));\n");
		} else {
			fprintf(fout, "\n\t\tTIMEOUT) != 0) {\n");
		}
	} else {
		/* single argument, new or old style */
		//ycyang: come in here~!
		if (!mtflag) {
			//ycyang: come in here~!

			//edit by ycyang

			fprintf(fout, "\t// if NONBLOCK_MODE, dont need to alloc memory\n");
			fprintf(fout, "\tif (clnt->send_mode & BLOCK_MODE) {\n");
			fprintf(fout, "\t\t%s = (", RESULT);
			ptype(proc->res_prefix, proc->res_type, 0);
			fprintf(fout, "*) rpc_malloc(sizeof(");
			ptype(proc->res_prefix, proc->res_type, 0);
			fprintf(fout, "));\n");
			fprintf(fout, "\t}\n\n");

			fprintf(fout, "\n");
			fprintf(fout, "\t// prepare the RPC call structure\n");
			fprintf(fout, "\t// including programID, versionID, TaskID...\n");
			fprintf(fout, "\trpc = RPC_PrepareCall(clnt, (int)result);\n\n");
//	fprintf (fout, "\tif (openStubRPC()) {\n");
//	fprintf (fout, "\t\texit(1);\n");
//	fprintf (fout, "\t}\n\n\n");
			fprintf(fout, "\n");
			fprintf(fout,
					"\tif (RPC_ClientCall (&rpc, %s, clnt->send_mode,\n\t\t(xdrproc_t) xdr_%s, (caddr_t) %s%s, args_size)\n\t\t!= 0) {\n",
					proc->proc_name,
					stringfix(proc->args.decls->decl.type),
					(newstyle ? "&" : ""),
					(newstyle ? proc->args.decls->decl.name : "argp"));

//	fprintf (fout,
//		 "\tif (clnt_call (clnt, %s,\n\t\t(xdrproc_t) xdr_%s, (caddr_t) %s%s,\n\t\t(xdrproc_t) xdr_%s, (caddr_t) %s%s,\n\t\tTIMEOUT) != 0) {\n",
//		 proc->proc_name,
//		 stringfix (proc->args.decls->decl.type),
//		 (newstyle ? "&" : ""),

//		 (newstyle ? proc->args.decls->decl.name : "argp"),
//		 stringfix (proc->res_type), ampr (proc->res_type),
//		 RESULT);
		} else {
			fprintf(fout,
					"\treturn (clnt_call(clnt, %s,\n\t\t(xdrproc_t) xdr_%s, (caddr_t) %s%s,\n\t\t(xdrproc_t) xdr_%s, (caddr_t) %s%s,\n\t\tTIMEOUT));\n",
					proc->proc_name,
					stringfix(proc->args.decls->decl.type),
					(newstyle ? "&" : ""),
					(newstyle ? proc->args.decls->decl.name : "argp"),
					stringfix(proc->res_type), "",
					RESULT);
		}
	}
	if (!mtflag) {
		//ycyang: come in here~!
		//edit by ycyang
//      fprintf (fout, "\t\treturn (NULL);\n");
//      fprintf (fout, "\t\tcloseStubRPC();\n");
//      fprintf (fout, "\t\t*isSuccess = FALSE;\n");
		fprintf(fout, "\t\tif(%s)\n", RESULT);
		fprintf(fout, "\t\t\trpc_free(%s);\n", RESULT);
		fprintf(fout, "\t\treturn (%s *)-1;\n", stringfix(proc->res_type));
//      fprintf (fout, "\t\treturn result;\n");
		fprintf(fout, "\t}\n\n\n");

		fprintf(fout, "\t//for blocking use\n");
		fprintf(fout, "\tif (clnt->send_mode & BLOCK_MODE) {\n");
		fprintf(fout, "\t\tXDR xdrs;\n\n");
		fprintf(fout, "\t\tWaitReply();\n");
//      fprintf (fout, "\t\tprintf(\"!!! got reply...\\n\");\n\n");
		fprintf(fout, "\t\txdrmem_create(&xdrs, (char *)%s, sizeof(%s), XDR_DECODE);\n", RESULT, stringfix(proc->res_type));


		if (streq(proc->res_type, "void")) {
			fprintf(fout, "\t\txdr_void();\n\n");
		} else {
			//fprintf (fout, "\t\txdr_%s(&xdrs, %s);\n\n",stringfix(proc->res_type),RESULT);
			fprintf(fout, "\t\t if(!xdr_%s(&xdrs, %s))\n", stringfix(proc->res_type), RESULT);
			fprintf(fout, "\t\t\t return (%s *)-1;\n", stringfix(proc->res_type));
		}

//      fprintf (fout, "\t\tcloseStubRPC();\n");
//      fprintf (fout, "\t\t*isSuccess = TRUE;\n");
		fprintf(fout, "\t\treturn %s;\n", RESULT);
		fprintf(fout, "\t}\n\n");
//      fprintf (fout, "\tcloseStubRPC();\n");
		fprintf(fout, "\treturn 0;\n\n");
//      fprintf (fout, "\t*isSuccess = TRUE;\n");
//      fprintf (fout, "\treturn result;\n\n");

		//  if (streq (proc->res_type, "void"))
//	{
//	  fprintf (fout, "\treturn ((void *)%s%s);\n",
//		   ampr (proc->res_type), RESULT);

//	}
//     else
//	{
//	  fprintf (fout, "\treturn (%s%s);\n", ampr (proc->res_type), RESULT);
//	}
	}
}
