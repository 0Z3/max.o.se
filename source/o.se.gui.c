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
#include "ose_maxgui.h"
#include "ose_libmax.h"
#include "ose_libmaxgui.h"

#define O_SE_GUI_VMX_SIZE 65536

t_class *o_se_gui_class;
t_symbol *ps_FullPacket;

void o_se_gui_popInputToControl(ose_bundle osevm)
{
    ose_maxobj_popInputToControl(osevm);
}

void o_se_gui_respondToString(ose_bundle osevm)
{
    ose_maxobj_respondToString(osevm);
}

void o_se_gui_lookup(ose_bundle osevm)
{
    ose_maxobj_lookup(osevm);
}

static void *o_se_gui_new(t_symbol *sym, long argc, t_atom *argv)
{
    ose_maxgui *x = (ose_maxgui *)object_alloc(o_se_gui_class);
    if(!x)
    {
        return NULL;
    }
    t_dictionary *d = object_dictionaryarg(argc, argv);
    if(!d)
    {
        return NULL;
    }
    /* { */
    /*     t_symbol **keys; */
    /*     long n = 0; */
    /*     dictionary_getkeys(d, &n, &keys); */
    /*     for(int i = 0; i < n; i++) */
    /*     { */
    /*         printf("%d: %s\n", i, keys[i]->s_name); */
    /*     } */
    /* } */
    /* set up jbox */
    long boxflags = 0
        | JBOX_DRAWFIRSTIN
        | JBOX_NODRAWBOX
        | JBOX_DRAWINLAST
        | JBOX_TRANSPARENT
        //| JBOX_NOGROW
        //| JBOX_GROWY
        //| JBOX_GROWBOTH
        //| JBOX_HILITE
        //| JBOX_BACKGROUND
        //| JBOX_DRAWBACKGROUND
        //| JBOX_NOFLOATINSPECTOR
        //| JBOX_MOUSEDRAGDELTA
        | JBOX_TEXTFIELD
        ;
    jbox_new((t_jbox *)x, boxflags, argc, argv);
    ((t_jbox *)x)->b_firstin = (void *)x;

    attr_dictionary_process(x, d);
    
    long ac = 0;
    t_atom *av = NULL;
    dictionary_getatoms(d, gensym("args"), &ac, &av);

#ifdef OSEVM_HAVE_SIZES
    const int32_t totalsize =
        OSE_CONTEXT_MAX_OVERHEAD
        + ((OSEVM_TOTAL_SIZE
            + O_SE_GUI_VMX_SIZE
            + OSE_CONTEXT_MESSAGE_OVERHEAD
            + OSE_CONTEXT_MESSAGE_OVERHEAD)
           * 2);
#else
#error "VM sizes not defined!"
#endif
    {
        x->ob.bytes = (char *)malloc(totalsize);
        if(!x->ob.bytes)
        {
            object_error((t_object *)x,
                         "couldn't allocate %d bytes for ose vm",
                         totalsize);
            ose_maxobj_free(x);
            return NULL;
        }
        x->ob.bundle = ose_newBundleFromCBytes(totalsize, x->ob.bytes);
        {
            ose_pushContextMessage(x->ob.bundle,
                                   (OSEVM_TOTAL_SIZE
                                    + O_SE_GUI_VMX_SIZE
                                    + OSE_CONTEXT_MESSAGE_OVERHEAD),
                                   "/xo");
            ose_bundle xo = ose_enter(x->ob.bundle, "/xo");
            x->ob.osevm = osevm_init(xo);
            ose_pushContextMessage(x->ob.osevm,
                                   O_SE_GUI_VMX_SIZE, "/_x");
        }
        {
            ose_pushContextMessage(x->ob.bundle,
                                   (OSEVM_TOTAL_SIZE
                                    + O_SE_GUI_VMX_SIZE
                                    + OSE_CONTEXT_MESSAGE_OVERHEAD),
                                   "/xg");
            ose_bundle xg = ose_enter(x->ob.bundle, "/xg");
            x->osevm_gui = osevm_init(xg);
            ose_pushContextMessage(x->osevm_gui,
                                   O_SE_GUI_VMX_SIZE, "/_x");
        }
    }
    
    if(ose_maxgui_init(x, sym, ac, av))
    {
        return NULL;
    }

    /* stdlib */
    {
        /* outer GUI object */
        ose_bundle osevm_gui =
            OSE_MAXGUI_GET_OSEVM_GUI((ose_maxgui *)x);
        ose_libmax_addObjInfoToEnv((ose_maxobj *)x,
                                   osevm_gui,
                                   sym, ac, av);
        ose_libmaxgui_addObjInfoToEnv((ose_maxobj *)x,
                                      osevm_gui,
                                      sym, ac, av);
        ose_libmax_addStdlibToEnv(osevm_gui);
        ose_libmaxgui_addMaxGUIFunctionsToEnv(osevm_gui);
        ose_libmaxgui_addMethodAddFunctionsToEnv(osevm_gui);
    }
    {
        /* inner object */
        ose_bundle osevm =
            OSE_MAXGUI_GET_OSEVM(x);
        ose_libmax_addObjInfoToEnv((ose_maxobj *)x,
                                   osevm,
                                   sym, ac, av);
        ose_libmaxgui_addObjInfoToEnv((ose_maxobj *)x,
                                      osevm,
                                      sym, ac, av);
        ose_libmax_addStdlibToEnv(osevm);
        ose_libmaxgui_addMaxGUIFunctionsToEnv(osevm);
        ose_libmaxgui_addMethodAddFunctionsToEnv(osevm);
    }

    ose_maxgui_loadSubclass(x, sym);

    

    /* process args */
    ose_maxobj_processArgs(OSE_MAXGUI_GET_OSEVM(x), sym, ac, av);
    ose_maxgui_processArgs(OSE_MAXGUI_GET_OSEVM_GUI(x), sym, ac, av);

    
    t_object *textfield = jbox_get_textfield((t_object *)x);
    if(textfield)
    {
        object_attr_setchar(textfield,
                            gensym("editwhenunlocked"), 1);
        textfield_set_editonclick(textfield, 0);
        textfield_set_textmargins(textfield, 5, 5, 15, 5);
        /* textfield_set_textcolor(textfield, &(x->text_color)); */
    }
    jbox_ready((t_jbox *)x);
    return x;
}

void ext_main(void *r)
{
    common_symbols_init();
    t_class *c;

    c = class_new("o.se.gui",
                  (method)o_se_gui_new,
                  (method)ose_maxgui_free,
                  sizeof(ose_maxgui), 0L, A_GIMME, 0);

    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
    jbox_initclass(c,
                   JBOX_TEXTFIELD | JBOX_FIXWIDTH | JBOX_FONTATTR);
    class_addmethod(c, (method)ose_maxgui_paint, "paint", A_CANT, 0);
    class_addmethod(c, (method)ose_maxgui_enter, "enter", A_CANT, 0);
    class_addmethod(c, (method)ose_maxgui_mousedown, "mousedown",
                    A_CANT, 0);
    class_addmethod(c, (method)ose_maxgui_mouseup, "mouseup",
                    A_CANT, 0);

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

    CLASS_ATTR_SYM(c, "o.se_initfile", 0, ose_maxgui, filename);
    CLASS_ATTR_SAVE(c, "o.se_initfile", 0);
    CLASS_ATTR_INVISIBLE(c, "o.se_initfile", 0);
    
    class_register(CLASS_BOX, c);
    o_se_gui_class = c;

    ps_FullPacket = gensym("FullPacket");
}
