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
#include "ose_libmax.h"

static void ose_libmaxgui_gettext(ose_bundle osevm)
{
    ose_maxgui *x = (ose_maxgui *)ose_maxobj_getMaxObjPtr(osevm);
    ose_maxgui_gettext(x, osevm);
}

static void ose_libmaxgui_settext(ose_bundle osevm)
{
    ose_maxgui *x = (ose_maxgui *)ose_maxobj_getMaxObjPtr(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 1)
       && ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_STRING)
    {
        const char * const ptr = ose_peekString(vm_s);
        object_method(jbox_get_textfield((t_object *)x),
                      gensym("settext"), ptr);
    }
}

static void ose_libmaxgui_refresh(ose_bundle osevm)
{
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    jbox_redraw((t_jbox *)x);
}

static void *getPtr(ose_bundle vm_s)
{
    if(ose_bundleHasAtLeastNElems(vm_s, 1)
       && ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_BLOB)
    {
        int32_t o = ose_getLastBundleElemOffset(vm_s);
        int32_t to, ntt, lto, po, lpo;
        void *ptr = NULL;
        ose_getNthPayloadItem(vm_s, 1, o, &to, &ntt, &lto, &po, &lpo);
        ose_alignPtr(vm_s, lpo + 4);
        ptr = ose_readAlignedPtr(vm_s, lpo + 4);
        return ptr;
    }
    else
    {
    	return NULL;
    }
}

static void ose_libmaxgui_patcherviewGetJGraphics(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    void *ptr = getPtr(vm_s);
    if(ptr)
    {
        ose_drop(vm_s);
        t_jgraphics *g = patcherview_get_jgraphics(ptr);
        if(g)
        {
            ose_pushAlignedPtr(vm_s, g);
        }
    }
    /* if(ose_bundleHasAtLeastNElems(vm_s, 1) */
    /*    && ose_peekType(vm_s) == OSETT_MESSAGE */
    /*    && ose_peekMessageArgType(vm_s) == OSETT_BLOB) */
    /* { */
    /*     int32_t o = ose_getLastBundleElemOffset(vm_s); */
    /*     int32_t to, ntt, lto, po, lpo; */
    /*     void *pv = NULL; */
    /*     ose_getNthPayloadItem(vm_s, 1, o, &to, &ntt, &lto, &po, &lpo); */
    /*     ose_alignPtr(vm_s, lpo + 4); */
    /*     pv = ose_readAlignedPtr(vm_s, lpo + 4); */
    /*     ose_drop(vm_s); */
    /*     if(pv) */
    /*     { */
    /*         t_jgraphics *g = patcherview_get_jgraphics(pv); */
    /*         if(g) */
    /*         { */
    /*             ose_pushAlignedPtr(vm_s, g); */
    /*         } */
    /*     } */
    /* } */
    /* else */
    /* { */

    /* } */
}

static void ose_libmaxgui_jboxGetWidthForView(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 1))
    {
        t_object *pv = getPtr(vm_s);
        if(pv)
        {
            t_rect r;
            t_max_err e = jbox_get_rect_for_view(x, pv, &r);
            ose_pushInt32(vm_s, r.width);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jboxGetHeightForView(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 1))
    {
        t_object *pv = getPtr(vm_s);
        if(pv)
        {
            t_rect r;
            t_max_err e = jbox_get_rect_for_view(x, pv, &r);
            ose_pushInt32(vm_s, r.height);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsSetSourceJRGBA(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 5))
    {
        float a = ose_popFloat(vm_s);
        float b = ose_popFloat(vm_s);
        float g = ose_popFloat(vm_s);
        float r = ose_popFloat(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            t_jrgba c = (t_jrgba){r, g, b, a};
            jgraphics_set_source_jrgba(gc, &c);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsSetLineWidth(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 2))
    {
        float w = ose_popFloat(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            jgraphics_set_line_width(gc, w);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsFill(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 1))
    {
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            jgraphics_fill(gc);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsStroke(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 1))
    {
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            jgraphics_stroke(gc);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsMoveTo(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 3))
    {
        int32_t y = ose_popInt32(vm_s);
        int32_t x = ose_popInt32(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            jgraphics_move_to(gc, x, y);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsLineTo(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 3))
    {
        int32_t y = ose_popInt32(vm_s);
        int32_t x = ose_popInt32(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            jgraphics_line_to(gc, x, y);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsRectangle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 5))
    {
        int32_t h = ose_popInt32(vm_s);
        int32_t w = ose_popInt32(vm_s);
        int32_t y = ose_popInt32(vm_s);
        int32_t x = ose_popInt32(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            jgraphics_rectangle(gc, x, y, w, h);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsRoundedRectangle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 7))
    {
        int32_t p2 = ose_popInt32(vm_s);
        int32_t p1 = ose_popInt32(vm_s);
        int32_t h = ose_popInt32(vm_s);
        int32_t w = ose_popInt32(vm_s);
        int32_t y = ose_popInt32(vm_s);
        int32_t x = ose_popInt32(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        if(gc)
        {
            jgraphics_rectangle_rounded(gc, x, y, w, h, p1, p2);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_setTextMargins(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 5))
    {
        int32_t l = ose_popInt32(vm_s);
        int32_t t = ose_popInt32(vm_s);
        int32_t r = ose_popInt32(vm_s);
        int32_t b = ose_popInt32(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        t_object *tf = jbox_get_textfield((t_object *)x);
        if(gc && tf)
        {
            textfield_set_textmargins(tf, l, t, r, b);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_jgraphicsSetTextColorRGBA(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_maxgui *x = ose_maxobj_getMaxObjPtr(osevm);
    if(ose_bundleHasAtLeastNElems(vm_s, 5))
    {
        float a = ose_popFloat(vm_s);
        float b = ose_popFloat(vm_s);
        float g = ose_popFloat(vm_s);
        float r = ose_popFloat(vm_s);
        t_jgraphics *gc = getPtr(vm_s);
        t_object *tf = jbox_get_textfield((t_object *)x);
        if(gc && tf)
        {
            t_jrgba c = (t_jrgba){r, g, b, a};
            textfield_set_textcolor(tf, &c);
        }
    }
    else
    {

    }
}

static void ose_libmaxgui_addTypedMethod(ose_bundle osevm)
{
    ose_maxobj *x = ose_maxobj_getMaxObjPtr(osevm);
    ose_libmax_addTypedMethod_impl(x, osevm,
                                   ose_maxgui_addMaxMethod,
                                   ose_maxgui_method);
}

static void ose_libmaxgui_addUntypedMethod(ose_bundle osevm)
{
    ose_maxobj *x = ose_maxobj_getMaxObjPtr(osevm);
    ose_libmax_addUntypedMethod_impl(x, osevm,
                                     ose_maxgui_addMaxMethod,
                                     ose_maxgui_method);
}

/* initialization functions */
void ose_libmaxgui_addMaxGUIFunctionsToEnv(ose_bundle osevm)
{
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_pushMessage(vm_e, "/gettext", strlen("/gettext"),
                    1, OSETT_ALIGNEDPTR, ose_libmaxgui_gettext);
    ose_pushMessage(vm_e, "/settext", strlen("/settext"),
                    1, OSETT_ALIGNEDPTR, ose_libmaxgui_settext);
    ose_pushMessage(vm_e, "/refresh", strlen("/refresh"),
                    1, OSETT_ALIGNEDPTR, ose_libmaxgui_refresh);
    ose_pushMessage(vm_e, "/patcherview/jgraphics/get",
                    strlen("/patcherview/jgraphics/get"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_patcherviewGetJGraphics);
    ose_pushMessage(vm_e, "/jgraphics/source/jrgba/set",
                    strlen("/jgraphics/source/jrgba/set"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsSetSourceJRGBA);
    ose_pushMessage(vm_e, "/jgraphics/line/width/set",
                    strlen("/jgraphics/line/width/set"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsSetLineWidth);
    ose_pushMessage(vm_e, "/jgraphics/fill",
                    strlen("/jgraphics/fill"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsFill);
    ose_pushMessage(vm_e, "/jgraphics/stroke",
                    strlen("/jgraphics/stroke"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsStroke);
    ose_pushMessage(vm_e, "/jbox/width/get",
                    strlen("/jbox/width/get"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jboxGetWidthForView);
    ose_pushMessage(vm_e, "/jbox/height/get",
                    strlen("/jbox/height/get"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jboxGetHeightForView);
    ose_pushMessage(vm_e, "/jgraphics/moveto",
                    strlen("/jgraphics/moveto"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsMoveTo);
    ose_pushMessage(vm_e, "/jgraphics/lineto",
                    strlen("/jgraphics/lineto"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsLineTo);
    ose_pushMessage(vm_e, "/jgraphics/rectangle",
                    strlen("/jgraphics/rectangle"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsRectangle);
    ose_pushMessage(vm_e, "/jgraphics/rounded/rectangle",
                    strlen("/jgraphics/rounded/rectangle"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsRoundedRectangle);
    ose_pushMessage(vm_e, "/textfield/margins/set",
                    strlen("/textfield/margins/set"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_setTextMargins);
    ose_pushMessage(vm_e, "/textfield/textcolor/rgba/set",
                    strlen("/textfield/textcolor/rgba/set"),
                    1, OSETT_ALIGNEDPTR,
                    ose_libmaxgui_jgraphicsSetTextColorRGBA);
}

void ose_libmaxgui_addMethodAddFunctionsToEnv(ose_bundle osevm)
{
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_pushMessage(vm_e, "/method/typed/add",
                    strlen("/method/typed/add"), 1,
                    OSETT_ALIGNEDPTR, ose_libmaxgui_addTypedMethod);
    ose_pushMessage(vm_e, "/method/untyped/add",
                    strlen("/method/untyped/add"), 1,
                    OSETT_ALIGNEDPTR, ose_libmaxgui_addUntypedMethod);
}

void ose_libmaxgui_addObjInfoToEnv(ose_maxobj *x,
                                   ose_bundle osevm,
                                   t_symbol *sym,
                                   long argc,
                                   t_atom *argv)
{
    /* box rect, etc */
}
