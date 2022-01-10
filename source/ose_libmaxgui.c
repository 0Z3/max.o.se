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

void ose_libmaxgui_gettext(ose_bundle osevm)
{
    ose_maxgui *x = (ose_maxgui *)ose_maxobj_getMaxObjPtr(osevm);
    ose_maxgui_gettext(x);
}

void ose_libmaxgui_settext(ose_bundle osevm)
{
    ose_maxgui *x = (ose_maxgui *)ose_maxobj_getMaxObjPtr(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 1) == OSETT_TRUE
       && ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_STRING)
    {
        const char * const ptr = ose_peekString(vm_s);
        object_method(jbox_get_textfield((t_object *)x),
                      gensym("settext"), ptr);
    }
}

void ose_libmaxgui_refresh(ose_bundle osevm)
{
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    jbox_redraw((t_jbox *)x);
}

/* initialization functions */
void ose_libmaxgui_addFunctionsToEnv(ose_bundle osevm)
{
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_pushMessage(vm_e, "/gettext", strlen("/gettext"),
                    1, OSETT_ALIGNEDPTR, ose_libmaxgui_gettext);
    ose_pushMessage(vm_e, "/settext", strlen("/settext"),
                    1, OSETT_ALIGNEDPTR, ose_libmaxgui_settext);
    ose_pushMessage(vm_e, "/refresh", strlen("/refresh"),
                    1, OSETT_ALIGNEDPTR, ose_libmaxgui_refresh);
}

void ose_libmaxgui_addObjInfoToEnv(ose_maxobj *x,
                                   ose_bundle osevm,
                                   t_symbol *sym,
                                   long argc,
                                   t_atom *argv)
{
    /* box rect, etc */
}
