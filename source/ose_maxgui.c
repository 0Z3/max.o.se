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
