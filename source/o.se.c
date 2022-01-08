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

/* NOTE: THIS CODE IS NOT THREAD-SAFE */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ext.h"
#include "ext_obex.h"
#include "ext_obex_util.h"
#include "ext_critical.h"
#include "ext_path.h"

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_symtab.h"
#include "ose_builtins.h"
#include "ose_match.h"
#include "ose_print.h"
#include "sys/ose_load.h"

#include "ose_maxobj.h"
#include "ose_libmax.h"

#ifndef O_SE_VMSIZE
#define O_SE_VMSIZE 524288
#endif

t_class *o_se_class;
t_symbol *ps_FullPacket;

void o_se_popInputToControl(ose_bundle osevm)
{
    ose_maxobj_popInputToControl(osevm);
}

void o_se_respondToString(ose_bundle osevm)
{
    ose_maxobj_respondToString(osevm);
}

void o_se_lookup(ose_bundle osevm)
{
    ose_maxobj_lookup(osevm);
}

static void *o_se_new(t_symbol *sym, long argc, t_atom *argv)
{
    ose_maxobj *x = (ose_maxobj *)object_alloc(o_se_class);
    if(!x)
    {
        return NULL;
    }
    ose_maxobj_init(x, sym, argc, argv, O_SE_VMSIZE);

    /* stdlib */
    ose_libmax_addObjInfoToEnv(x, x->osevm, sym, argc, argv);
    ose_libmax_addFunctionsToEnv(x->osevm);

    ose_maxobj_loadSubclass(x, sym);

    /* process args */
    ose_maxobj_processArgs(x->osevm, sym, argc, argv);
    return x;
}

void ext_main(void *r)
{
    common_symbols_init();
    t_class *c;

    c = class_new("o.se",
                  (method)o_se_new,
                  (method)ose_maxobj_free,
                  sizeof(ose_maxobj), 0L, A_GIMME, 0);

    /* 
       would like to add "anything" as object_addmethod, but it seems
       like it has to be done here in the class setup routine
    */
    class_addmethod(c, (method)ose_maxobj_anything,
                    "anything", A_GIMME, 0);
    class_addmethod(c, (method)ose_maxobj_assist,
                    "assist", A_CANT, 0);
    class_addmethod(c, (method)ose_maxobj_loadbang,
                    "loadbang", A_CANT, 0);
    class_addmethod(c, (method)stdinletinfo,
                    "inletinfo", A_CANT, 0);

#ifdef OSE_DEBUG
    class_addmethod(c, (method)ose_maxobj_debugFromMax, "debug", 0);
#endif
    
    class_register(CLASS_BOX, c);
    o_se_class = c;

    ps_FullPacket = gensym("FullPacket");
}
