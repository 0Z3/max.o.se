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

#ifndef OSE_LIBMAXGUI_H
#define OSE_LIBMAXGUI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ose.h"

#include "ext.h"
#include "ext_obex.h"
#include "ext_obex_util.h"
#include "jpatcher_api.h"
#include "jgraphics.h"

void ose_libmaxgui_gettext(ose_bundle osevm);
void ose_libmaxgui_settext(ose_bundle osevm);
void ose_libmaxgui_refresh(ose_bundle osevm);

void ose_libmaxgui_addMaxGUIFunctionsToEnv(ose_bundle osevm);
void ose_libmaxgui_addMethodAddFunctionsToEnv(ose_bundle osevm);
void ose_libmaxgui_addObjInfoToEnv(ose_maxobj *x,
                                   ose_bundle osevm,
                                   t_symbol *sym,
                                   long argc,
                                   t_atom *argv);

#ifdef __cplusplus
}
#endif
#endif
