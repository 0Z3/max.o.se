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
    t_symbol *filename;
    ose_bundle osevm_gui;
} ose_maxgui;

#define OSE_MAXGUI_GET_OSEVM(x) (((ose_maxobj *)x)->osevm)

void ose_maxgui_method(ose_maxobj *x,
                       t_symbol *msg,
                       long ac,
                       t_atom *av);
int ose_maxgui_addMaxMethod(ose_maxobj *x, t_symbol *name);
void ose_maxgui_processArgs(ose_bundle osevm,
                            t_symbol *sym,
                            long argc,
                            t_atom *argv);
/* class and subclass */
void ose_maxgui_loadSubclass(ose_maxgui *x, t_symbol *sym);
/* default handlers for max messages */
int ose_maxgui_init(ose_maxgui *x,
                    t_symbol *sym,
                    long argc,
                    t_atom *argv);
void ose_maxgui_free(ose_maxgui *x);
void ose_maxgui_FullPacket(ose_maxobj *x, long len, long ptr);
void ose_maxgui_anything(ose_maxobj *x,
                         t_symbol *s,
                         long ac,
                         t_atom *av);
void ose_maxgui_list(ose_maxobj *x,
                     t_symbol *s,
                     long ac,
                     t_atom *av);
void ose_maxgui_float(ose_maxobj *x, double f);
void ose_maxgui_int(ose_maxobj *x, long l);
void ose_maxgui_bang(ose_maxobj *x);
void ose_maxgui_paint(ose_maxgui *x, t_object *patcherview);
void ose_maxgui_gettext(ose_maxgui *x);
void ose_maxgui_enter(ose_maxgui *x);
void ose_maxgui_mousedown(ose_maxgui *x,
                          t_object *patcherview,
                          t_pt pt, long modifiers);

#ifdef __cplusplus
}
#endif
#endif
