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

#ifndef OSE_MAXGUI_H
#define OSE_MAXGUI_H

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
#include "jpatcher_api.h"
#include "jgraphics.h"

#include "ose_maxobj.h"

typedef struct _ose_maxgui
{
    ose_maxobj ob;
} ose_maxgui;

#define OSE_MAXGUI_GET_OSEVM(x) (((ose_maxobj *)x)->osevm)

void ose_maxgui_processArgs(ose_bundle osevm,
                            t_symbol *sym,
                            long argc,
                            t_atom *argv);
/* class and subclass */
void ose_maxgui_loadSubclass(ose_maxgui *x, t_symbol *sym);
/* default handlers for max messages */
void ose_maxgui_init(ose_maxgui *x,
                     t_symbol *sym,
                     long argc,
                     t_atom *argv,
                     int32_t vmsize);
void ose_maxgui_free(ose_maxgui *x);

#ifdef __cplusplus
}
#endif
#endif
