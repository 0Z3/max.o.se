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
#include "ose_maxgui.h"

/* max methods */

void ose_maxgui_processArgs(ose_bundle osevm,
                            t_symbol *sym,
                            long argc,
                            t_atom *argv)
{
    ose_maxobj_processArgs(osevm, sym, argc, argv);
}

/* class and subclass */

void ose_maxgui_loadSubclass(ose_maxgui *x, t_symbol *sym)
{
    ose_bundle vm_i = OSEVM_INPUT(OSE_MAXGUI_GET_OSEVM(x));
    if(sym->s_name[8])
    {
        char filename[MAX_PATH_CHARS];
        snprintf(filename, MAX_PATH_CHARS,
                 "%s.ose", sym->s_name + 9);
        ose_pushMessage(vm_i, OSE_ADDRESS_ANONVAL,
                        OSE_ADDRESS_ANONVAL_LEN,
                        2,
                        OSETT_STRING, filename,
                        OSETT_STRING, "/!/load");
        ose_maxobj_run(x);
    }
}

/* max methods */

void ose_maxgui_methodFinalize(ose_maxgui *x,
                               const char * const name,
                               const int32_t namelen)
{
    ose_bundle osevm = OSE_MAXGUI_GET_OSEVM(x);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxobj_callMethod(osevm, name, namelen);
    ose_maxobj_run((ose_maxobj *)x);
    ose_pushString(vm_s, "/hook/gui/finalize");
    OSEVM_LOOKUP(osevm);
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_BLOB)
    {
        ose_bundle vm_i = OSEVM_INPUT(osevm);
        ose_blobToElem(vm_s);
        ose_replaceBundle(vm_s, vm_i);
        ose_maxobj_run(x);
    }
    else
    {
        ose_drop(vm_s);
    }
}

/* default handlers for max messages */

void ose_maxgui_init(ose_maxgui *x,
                     t_symbol *sym,
                     long argc,
                     t_atom *argv,
                     int32_t vmsize)
{
    ose_maxobj_init(x, sym, argc, argv, vmsize);
}

void ose_maxgui_free(ose_maxgui *x)
{
    jbox_free((t_jbox *)x);
    ose_maxobj_free((ose_maxgui *)x);
}

void ose_maxgui_enter(ose_maxgui *x)
{
    ose_bundle vm_s = OSEVM_STACK(OSE_MAXGUI_GET_OSEVM(x));
	long size   = 0;
    char *text  = NULL;

    t_object *textfield = jbox_get_textfield((t_object *)x);
    object_method(textfield, gensym("gettextptr"), &text, &size);

    /* size returned by object_method is bogus */
    size = strlen(text);
    if(size == 0)
    {
        return;
    }
    object_post((t_object *)x, "%s", text);
    ose_pushString(vm_s, text);
    ose_pushString(vm_s, "enter");
    ose_maxgui_methodFinalize(x, "enter", strlen("enter"));
}

