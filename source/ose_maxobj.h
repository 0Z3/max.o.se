/*
  Copyright (c) 2019-22 John MacCallum Permission is hereby granted,
  free of charge, to any person obtaining a copy of this software
  and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute,
  sublicense, and/or sell copies of the Software, and to permit
  persons to whom the Software is furnished to do so, subject to the
  following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#ifndef OSE_MAXOBJ_H
#define OSE_MAXOBJ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ose.h"

#include "ext.h"
#include "ext_obex.h"
#include "ext_obex_util.h"
/* for t_pxjbox */
#include "z_dsp.h"
#include "ext_critical.h"

#define OSE_MAXOBJ_HOOK_INIT_POST "/hook/init/post"
#define OSE_MAXOBJ_HOOK_INLET_POST "/hook/inlet/post"
#define OSE_MAXOBJ_HOOK_RESPOND "/hook/respond"

typedef struct _ose_maxobj
{
	t_pxjbox ob;
	t_critical lock;
	
	long inlet;
	void **proxies;
	int32_t nproxies;

	char *bytes;
	ose_bundle bundle, osevm;

    t_symbol **typed_method_names;
    char **typed_method_types;
    int32_t *typed_method_ntypes;
    long typed_methods_count;
    long typed_methods_size;

} ose_maxobj;

/* for debugging */
#ifdef OSE_DEBUG
void ose_maxobj_debugFromMax(ose_maxobj *x);
void ose_maxobj_debugFromVM(ose_bundle osevm);
#endif

/* utilities for dealing with the maxobj and vm data structures */
ose_maxobj *ose_maxobj_getMaxObjPtr(ose_bundle osevm);
int32_t ose_maxobj_getinlet(ose_maxobj *x);
void ose_maxobj_run(ose_maxobj *x, ose_bundle osevm);
/* outlets */
void ose_maxobj_outletBundle(void *outlet,
                             t_symbol *msg,
                             ose_bundle bundle);
void ose_maxobj_outletBundleElem(void *outlet,
                                 t_symbol *msg,
                                 ose_bundle bundle,
                                 int32_t offset);
/* hooks and callbacks */
void ose_maxobj_popInputToControl(ose_bundle osevm);
void ose_maxobj_respondToString(ose_bundle osevm);
void ose_maxobj_lookup(ose_bundle osevm);
void ose_maxobj_runHook(ose_bundle osevm,
                        const char * const hookaddr,
                        int32_t hookaddrlen);
/* max methods */
void ose_maxobj_callMethod(ose_bundle osevm,
                           const char * const name,
                           int32_t namelen);
int ose_maxobj_typedMethodArgChk(ose_maxobj *x,
                                 ose_bundle osevm,
                                 int32_t n, const char * const tts,
                                 t_symbol *msg, long ac, t_atom *av);
void ose_maxobj_untypedMethod(ose_maxobj *x,
                              ose_bundle osevm,
                              t_symbol *msg,
                              long ac,
                              t_atom *av);
void ose_maxobj_typedMethod(ose_maxobj *x,
                            ose_bundle osevm,
                            long method_index,
                            t_symbol *msg,
                            long ac,
                            t_atom *av);
void ose_maxobj_methodFinalize(ose_maxobj *x,
                               ose_bundle osevm,
                               const char * const name,
                               const int32_t namelen);
void ose_maxobj_method_impl(ose_maxobj *x,
                            ose_bundle osevm,
                            t_symbol *msg,
                            long ac,
                            t_atom *av);
void ose_maxobj_method(ose_maxobj *x,
                       t_symbol *msg,
                       long ac,
                       t_atom *av);
int ose_maxobj_addMaxMethod(ose_maxobj *x, t_symbol *name);
void ose_maxobj_processArgs(ose_bundle osevm,
                            t_symbol *sym,
                            long argc,
                            t_atom *argv);
/* class and subclass */
void ose_maxobj_loadSubclass_impl(ose_maxobj *x,
                                  ose_bundle osevm,
                                  const char * const filename);
void ose_maxobj_loadSubclass(ose_maxobj *x,
                             ose_bundle osevm,
                             t_symbol *sym);
/* default handlers for max messages */
void ose_maxobj_FullPacket_impl(ose_maxobj *x, ose_bundle osevm,
                                long len, long ptr);
void ose_maxobj_FullPacket(ose_maxobj *x, long len, long ptr);
void ose_maxobj_anything_impl(ose_maxobj *x,
                              ose_bundle osevm,
                              t_symbol *s,
                              long ac,
                              t_atom *av);
void ose_maxobj_anything(ose_maxobj *x,
                         t_symbol *s,
                         long ac,
                         t_atom *av);
void ose_maxobj_list_impl(ose_maxobj *x,
                          ose_bundle osevm,
                          t_symbol *s,
                          long ac,
                          t_atom *av);
void ose_maxobj_list(ose_maxobj *x,
                     t_symbol *s,
                     long ac,
                     t_atom *av);
void ose_maxobj_float_impl(ose_maxobj *x, ose_bundle osevm, double f);
void ose_maxobj_float(ose_maxobj *x, double f);
void ose_maxobj_int_impl(ose_maxobj *x, ose_bundle osevm, long l);
void ose_maxobj_int(ose_maxobj *x, long l);
void ose_maxobj_bang_impl(ose_maxobj *x, ose_bundle osevm);
void ose_maxobj_bang(ose_maxobj *x);
void ose_maxobj_loadbang(ose_maxobj *x);
void ose_maxobj_assist(ose_maxobj *x,
                       void *b,
                       long m,
                       long a,
                       char *s);
void ose_maxobj_free(ose_maxobj *x);
int ose_maxobj_init(ose_maxobj *x,
                    t_symbol *sym,
                    long argc,
                    t_atom *argv);

#ifdef __cplusplus
}
#endif
#endif
