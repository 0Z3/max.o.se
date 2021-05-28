/*
  Copyright (c) 2019-21 John MacCallum Permission is hereby granted,
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

#include "o.se.h"

#define OVM_HOOK_INIT_POST "/hook/init/post"
#define OVM_HOOK_INLET_POST "/hook/inlet/post"
#define OVM_HOOK_RESPOND "/hook/respond"
/* #define OVM_HOOK_BANG "/hook/bang" */

/* For debugging */
#ifdef OSE_DEBUG
static void pbndl(ose_bundle bundle, const char * const str)
__attribute__((used))
{
    char buf[8192];
    memset(buf, 0, 8192);
    ose_pprintBundle(bundle, buf, 8192);
    printf("\n%s>>>>>\n%s\n%s<<<<<\n", str, buf, str);
}
static void pbytes(ose_bundle bundle, int32_t start, int32_t end)
__attribute__((used))
{
    char *b = ose_getBundlePtr(bundle);
    for(int32_t i = start; i < end; i++)
    {
        fprintf(stderr, "%d: %c %d\n", i,
                (unsigned char)b[i],
                (unsigned char)b[i]);
    }
}
static void ovm_debugFromMax(ovm *x){}
static void ovm_debugFromVM(ose_bundle osevm){}
#endif
static void ovm_runHook(ovm *x,
                        const char * const hookaddr,
                        int32_t hookaddrlen);

static ovm *ovm_getMaxObjPtr(ose_bundle osevm)
{
    ovm *x = (ovm *)*((intptr_t *)(ose_getBundlePtr(osevm)
                                   + OSEVM_CACHE_OFFSET_8));
    return x;
}

static void ovm_run(ovm *x)
{
    osevm_run(x->osevm);
}

static void ovm_outlet_bundle(void *outlet,
                              t_symbol *msg,
                              ose_bundle bundle)
{
    t_atom out[2];
    atom_setlong(out, ose_readInt32(bundle, -4));
    atom_setlong(out + 1, (long)ose_getBundlePtr(bundle));
    outlet_anything(outlet, msg, 2, out);
}

static void ovm_outlet_bundleElem(void *outlet,
                                  t_symbol *msg,
                                  ose_bundle bundle,
                                  int32_t offset)
{
    t_atom out[2];
    atom_setlong(out, ose_readInt32(bundle, offset));
    atom_setlong(out + 1,
                 (long)(ose_getBundlePtr(bundle) + offset + 4));
    outlet_anything(outlet, msg, 2, out);
}

static void ovm_delegate(ovm *x, void *outlet)
{
    ovm_outlet_bundle(outlet,
                      ps_FullPacket, x->vm_g);
    ose_clear(x->vm_g);
}

void ovm_popInputToControl(ose_bundle osevm)
{
    /* HOOK */
    osevm_popInputToControl(osevm);
}

void ovm_respondToString(ose_bundle osevm)
{
    ovm *x = ovm_getMaxObjPtr(osevm);
    ose_copyBundle(x->vm_e, x->vm_s);
    ose_pushString(x->vm_s, OVM_HOOK_RESPOND);
    OSEVM_LOOKUP(x->osevm);
    if(ose_peekType(x->vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(x->vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(x->vm_s);
        ose_pushString(x->vm_c, "/</_e");
        ose_swap(x->vm_c);
        ose_pushString(x->vm_c, "/!/exec");
        ose_swap(x->vm_c);
    }
    else
    {
        ose_2drop(x->vm_s);
    }
}

void ovm_lookup(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_x = ose_enter(osevm, "/_x");

    const char * const address = ose_peekString(vm_s);
    int32_t mo = ose_getFirstOffsetForPMatch(vm_e, address);
    if(mo >= OSE_BUNDLE_HEADER_LEN)
    {
        ose_drop(vm_s);
        ose_copyElemAtOffset(mo, vm_e, vm_s);
        return;
    }
    /* if it wasn't present in env, lookup in _x */
    mo = ose_getFirstOffsetForPMatch(vm_x, address);
    if(mo >= OSE_BUNDLE_HEADER_LEN)
    {
        ose_drop(vm_s);
        ose_copyElemAtOffset(mo, vm_x, vm_s);
        return;
    }
    /* if it wasn't present in _x, lookup in the symtab */
    {
        ose_fn f = ose_symtab_lookup_fn(address);
        if(f)
        {
            ose_drop(vm_s);               
            ose_pushAlignedPtr(vm_s, f);
        }else
        {
            ;
        }
    }
}

static void ovm_runHook(ovm *x,
                        const char * const hookaddr,
                        int32_t hookaddrlen)
{
    ose_copyBundle(x->vm_e, x->vm_s);
    ose_pushString(x->vm_s, hookaddr);
    OSEVM_LOOKUP(x->osevm);
    if(ose_peekType(x->vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(x->vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(x->vm_s);
        ose_pushMessage(x->vm_i, "/!/exec", strlen("/!/exec"), 0);
        ovm_run(x);
        ose_replaceBundle(x->vm_s, x->vm_e);
        ose_clear(x->vm_s);
    }
    else
    {
        ose_2drop(x->vm_s);
    }
}

static void ovm_callMethod(ovm *x,
                           const char * const name,
                           int32_t namelen)
{
    ose_copyBundle(x->vm_e, x->vm_s);
    ose_pushString(x->vm_s, name);
    OSEVM_LOOKUP(x->osevm);
    if(ose_peekType(x->vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(x->vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(x->vm_s);
        if(ose_getBundleElemCount(x->vm_c) > 0)
        {
            ose_pushString(x->vm_c, "/!/exec");
            ose_swap(x->vm_c);
        }
        else
        {
            ose_pushMessage(x->vm_i,
                            "/!/exec", strlen("/!/exec"), 0);
        }
        /* ovm_run(x); */
        /* ose_replaceBundle(x->vm_s, x->vm_e); */
        /* ose_clear(x->vm_s); */
    }
    else
    {
        ose_2drop(x->vm_s);
    }
}

static void ovm_load(ose_bundle osevm)
{
    ovm *x = (ovm *)*((intptr_t *)(ose_getBundlePtr(osevm)
                                   + OSEVM_CACHE_OFFSET_8));
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    if(ose_isStringType(ose_peekMessageArgType(vm_s))
       == OSETT_TRUE)
    {
        const char * const str = ose_peekString(vm_s);
        char filename[MAX_PATH_CHARS];
        strcpy(filename, str);
        t_fourcc outtype;
        short pathid;
        if(locatefile_extended(filename,
                               &pathid,
                               &outtype,
                               NULL, 0))
        {
            object_error((t_object *)x,
                         "couldn't find %s; "
                         "make sure it is in your search path",
                         filename);
            return;
        }
        char filestring[MAX_PATH_CHARS];
        path_topathname(pathid, filename, filestring);
        char absolutepath[MAX_PATH_CHARS];
        path_nameconform(filestring, absolutepath, 
                         PATH_STYLE_NATIVE,
                         PATH_TYPE_PATH);

        const size_t len = strlen(str);
        if((len > 3 && !strcmp(str + (len - 3), ".so"))
           || (len > 4 && !strcmp(str + (len - 4), ".dll"))
           || (len > 6 && !strcmp(str + (len - 6), ".dylib")))
        {
            ose_loadLib(osevm, absolutepath);
        }
        else
        {
            FILE *fp = fopen(absolutepath, "rb");
            if(!fp)
            {
                object_error((t_object *)x,
                             "couldn't open file %s",
                             absolutepath);
                return;
            }
            const int maxlen = 256;
            char buf[maxlen];
            ose_pushBundle(vm_i);
            while(fgets(buf, maxlen, fp))
            {
                int len = strlen(buf);
                if(buf[len - 1] == '\n')
                {
                    buf[len - 1] = 0;
                }
                ose_pushMessage(vm_i, buf, strlen(buf), 0);
                ose_push(vm_i);
            }
            fclose(fp);
            ose_drop(vm_s);
            ose_popAllDrop(vm_i);
        }
    }
    else
    {
        object_error((t_object *)x,
                     "/!/load requires a path on the stack");
    }
}

static void ovm_outlet(ose_bundle osevm)
{
    ovm *x = ovm_getMaxObjPtr(osevm);
    if(!x)
    {
        error("o.se: got a vm with no Max object!");
        return;
    }
    ose_bundle vm_s = OSEVM_STACK(osevm);
    if(ose_getBundleElemCount(vm_s) < 2)
    {
        object_error((t_object *)x,
                     "/!/outlet requires two arguments: "
                     "the outlet and an element to be output.");
        return;
    }
    if(ose_peekType(vm_s) != OSETT_MESSAGE
       || ose_peekMessageArgType(vm_s) != OSETT_BLOB)
    {
        object_error((t_object *)x,
                     "The first argument to /!/outlet must be "
                     "a message containing a blob (the outlet). "
                     "Found a bundle.");
        return;
    }
    
    int32_t o = ose_getLastBundleElemOffset(vm_s);
    int32_t to, ntt, lto, po, lpo;
    ose_getNthPayloadItem(vm_s, 1, o, &to, &ntt, &lto, &po, &lpo);
    ose_alignPtr(vm_s, lpo + 4);
    void *outlet = ose_readAlignedPtr(vm_s, lpo + 4);
    ose_drop(vm_s);
    o = ose_getLastBundleElemOffset(vm_s);
    ovm_outlet_bundleElem(outlet, ps_FullPacket, vm_s, o);
    ose_drop(vm_s);
}

static int32_t ovm_getinlet(ovm *x)
{
    const int32_t inlet = (int32_t)proxy_getinlet((t_object *)x);
    const int32_t n = inlet
        ? (int32_t)(inlet - 1)
        : x->nproxies;
    /* const int32_t n = inlet */
    /*     ? (int32_t)((x->nproxies - inlet) + 1) */
    /*     : 0; */
    /* object_post((t_object *)x, "inlet = %d, n = %d", inlet, n); */
    return n;
}

static void ovm_assist(ovm *x, void *b, long m, long a, char *s)
{
    if(m == ASSIST_OUTLET)
    {
        /* ose_pushString(x->vm_x, "/outlets"); */
        /* if(ose_pickMatch_impl(x->vm_x)){ */
        /*         ose_pushInt32(x->vm_x, a); */
        /*         ose_nth(x->vm_x); */
        /*         const char * const p = ose_peekString(x->vm_x); */
        /*         sprintf(s, "Outlet %d (FullPacket)", */
        /*                 a); */
        /*         ose_drop(x->vm_x); */
        /* } */
    }
    else
    {
        /* switch(a){ */
        /* case 0: */
        /*         sprintf(s, "Input (OSC bundle)"); */
        /*         break; */
        /* default: */
        /*         ose_pushString(x->vm_x, "/inlets"); */
        /*         if(ose_pickMatch_impl(x->vm_x)){ */
        /*                 ose_pushInt32(x->vm_x, x->nproxies - a); */
        /*                 ose_nth(x->vm_x); */
        /*                 const char * const p = ose_peekString(x->vm_x); */
        /*                 sprintf(s, "Arguments for address %s", */
        /*                         ose_peekString(x->vm_x)); */
        /*                 ose_drop(x->vm_x); */
        /*         } */
        /*         break; */
        /* } */
    }
}

static int ovm_typedMethodArgChk(ovm *x,
                                 int32_t n, const char * const tts,
                                 t_symbol *msg, long ac, t_atom *av)
{
    if(n != ac)
    {
        object_error((t_object *)x,
                     "wrong number of arguments for message "
                     "%s. expected %d and got %d",
                     msg->s_name, n, ac);
        return 1;
    }
    int32_t i = 0;
    for(i = 0; i < n; i++)
    {
        switch(tts[i + 1])
        {
        case OSETT_INT32:
            switch(atom_gettype(av + i))
            {
            case A_LONG:
                break;
            default:
                object_error((t_object *)x,
                             "expected arg %d to be an int", i);
                return 1;
            }
            break;
        case OSETT_FLOAT:
            switch(atom_gettype(av + i))
            {
            case A_FLOAT:
                break;
            default:
                object_error((t_object *)x,
                             "expected arg %d to be a float", i);
                return 1;
            }
            break;
        case OSETT_STRING:
            switch(atom_gettype(av + i))
            {
            case A_SYM:
                break;
            default:
                object_error((t_object *)x,
                             "expected arg %d to be a symbol", i);
                return 1;
            }
            break;
        }
    }
    return 0;
}

static void ovm_untypedMethod(ovm *x,
                              t_symbol *msg,
                              long ac,
                              t_atom *av)
{
    int i;
    for(i = 0; i < ac; i++)
    {
        switch(atom_gettype(av + i))
        {
        case A_LONG:
            ose_pushInt32(x->vm_s, atom_getlong(av + i));
            break;
        case A_FLOAT:
            ose_pushFloat(x->vm_s, atom_getfloat(av + i));
            break;
        case A_SYM:
            ose_pushString(x->vm_s, atom_getsym(av + i)->s_name);
            break;
        }
        if(i > 0)
        {
            ose_push(x->vm_s);
        }
    }
}

static void ovm_typedMethod(ovm *x,
                            long method_index,
                            t_symbol *msg,
                            long ac,
                            t_atom *av)
{
    const char * const tts = x->typed_method_types[method_index];
    const int32_t n = x->typed_method_ntypes[method_index];
    if(ovm_typedMethodArgChk(x, n, tts, msg, ac, av))
    {
        return;
    }
    ovm_untypedMethod(x, msg, ac, av);
}

static void ovm_methodFinalize(ovm *x,
                               const char * const name,
                               const int32_t namelen)
{
    const int32_t inlet = ovm_getinlet(x);
    ose_pushInt32(x->vm_s, inlet);
    ovm_callMethod(x, name, namelen);
    ovm_run(x);
    ose_pushString(x->vm_s, "/hook/inlet/finalize");
    OSEVM_LOOKUP(x->osevm);
    if(ose_peekType(x->vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(x->vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(x->vm_s);
        ose_replaceBundle(x->vm_s, x->vm_i);
        ovm_run(x);
    }
    else
    {
        ose_drop(x->vm_s);
    }
}

static void ovm_method(ovm *x,
                       t_symbol *msg,
                       long ac,
                       t_atom *av)
{
    long i = 0;
    for(i = 0; i < x->typed_methods_count; i++)
    {
        if(msg == x->typed_method_names[i])
        {
            ovm_typedMethod(x, i, msg, ac, av);
            ovm_methodFinalize(x, msg->s_name, strlen(msg->s_name));
            return;
        }
    }
    ovm_untypedMethod(x, msg, ac, av);
    ovm_methodFinalize(x, msg->s_name, strlen(msg->s_name));
}

static void ovm_FullPacket(ovm *x, long len, long ptr)
{
    ose_pushBlob(x->vm_s, (int32_t)len, (char *)ptr);
    ose_blobToElem(x->vm_s);
    ovm_methodFinalize(x, "FullPacket", strlen("FullPacket"));
}

static void ovm_anything(ovm *x, t_symbol *s, long ac, t_atom *av)
{
    ose_pushString(x->vm_s, s->s_name);
    int i;
    for(i = 0; i < ac; i++)
    {
        switch(atom_gettype(av + i))
        {
        case A_LONG:
            ose_pushInt32(x->vm_s, atom_getlong(av + i));
            break;
        case A_FLOAT:
            ose_pushFloat(x->vm_s, atom_getfloat(av + i));
            break;
        case A_SYM:
            ose_pushString(x->vm_s, atom_getsym(av + i)->s_name);
            break;
        }
        ose_push(x->vm_s);
    }
    ovm_methodFinalize(x, "anything", strlen("anything"));
}

static void ovm_list(ovm *x, t_symbol *s, long ac, t_atom *av)
{
    int i;
    for(i = 0; i < ac; i++)
    {
        switch(atom_gettype(av + i))
        {
        case A_LONG:
            ose_pushInt32(x->vm_s, atom_getlong(av + i));
            break;
        case A_FLOAT:
            ose_pushFloat(x->vm_s, atom_getfloat(av + i));
            break;
        case A_SYM:
            ose_pushString(x->vm_s, atom_getsym(av + i)->s_name);
            break;
        }
        if(i > 0)
        {
        	ose_push(x->vm_s);
        }
    }
    ovm_methodFinalize(x, "list", strlen("list"));
}

static void ovm_float(ovm *x, double f)
{
    ose_pushFloat(x->vm_s, (float)f);
    ovm_methodFinalize(x, "float", strlen("float"));
}

static void ovm_int(ovm *x, long l)
{
    ose_pushInt32(x->vm_s, (int32_t)l);
    ovm_methodFinalize(x, "int", strlen("int"));
}

static void ovm_bang(ovm *x)
{
    ovm_methodFinalize(x, "bang", strlen("bang"));
}

static void ovm_free(ovm *x)
{
    if(x->proxies)
    {
        for(int i = 1; i < x->nproxies; i++)
        {
            if(x->proxies[i])
            {
                object_free(x->proxies[i]);
            }
        }
        free(x->proxies);
    }
    if(x->bytes)
    {
        free(x->bytes);
    }
    critical_free(x->lock);
}

static void ovm_loadbang(ovm *x){}

static void ovm_makeoutlet(ose_bundle osevm)
{
    ovm *x = ovm_getMaxObjPtr(osevm);
    void *outlet = outlet_new(x, "FullPacket");
    ose_pushAlignedPtr(x->vm_s, outlet);
}

static void ovm_makeinlet(ose_bundle osevm)
{
    ovm *x = ovm_getMaxObjPtr(osevm);
    x->proxies[x->nproxies]
        = proxy_new((t_object *)x,
                    x->nproxies + 1,
                    &(x->inlet));
    ++(x->nproxies);
}

static int ovm_addMaxMethod(ovm *x, t_symbol *name)
{
    if(name == gensym("bang"))
    {
        object_addmethod((t_object *)x, (method)ovm_bang,
                         "bang", 0);
    }
    else if(name == gensym("int"))
    {
        object_addmethod((t_object *)x, (method)ovm_int,
                         "int", A_LONG, 0);
    }
    else if(name == gensym("float"))
    {
        object_addmethod((t_object *)x, (method)ovm_float,
                         "float", A_FLOAT, 0);
    }
    else if(name == gensym("list"))
    {
        object_addmethod((t_object *)x, (method)ovm_list,
                         "list", A_GIMME, 0);
    }
    /* the anything method seems to need to be bound in the class */
    /* else if(name == gensym("anything")) */
    /* { */
    /*     object_addmethod((t_object *)x, (method)ovm_anything, */
    /*                      "anything", A_GIMME, 0); */
    /* } */
    else if(name == ps_FullPacket)
    {
        object_addmethod((t_object *)x, (method)ovm_FullPacket,
                         "FullPacket", A_LONG, A_LONG, 0);
    }
    else
    {
        return 1;
    }
    return 0;
}

/* 
   We simulate the ability to add typed methods dynamically, even
   though we can't. Later, we can add a code gen or jit stage that
   will do them for real.
*/
static void ovm_addTypedMethod(ose_bundle osevm)
{
    ovm *x = ovm_getMaxObjPtr(osevm);
    ose_bundle vm_s = x->vm_s;
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && (ose_isStringType(ose_peekMessageArgType(vm_s))
           == OSETT_TRUE))
    {
        t_symbol *name = gensym(ose_peekString(vm_s));
        ose_drop(vm_s);
        if(!ovm_addMaxMethod(x, name))
        {
            return;
        }
        
        if(x->typed_methods_count + 1 == x->typed_methods_size)
        {
            x->typed_methods_size += 8;
            x->typed_method_names =
                (t_symbol **)realloc((void *)x->typed_method_names,
                                     x->typed_methods_size
                                     * sizeof(t_symbol *));
            x->typed_method_types =
                (char **)realloc((void *)x->typed_method_types,
                                 x->typed_methods_size
                                 * sizeof(char *));
            x->typed_method_ntypes =
                (int32_t *)realloc((void *)x->typed_method_ntypes,
                                   x->typed_methods_size * 4);
        }
        
        int32_t n = ose_getBundleElemCount(vm_s);
        char *tts = (char *)calloc(ose_pnbytes(n + 1), 1);
        tts[0] = OSETT_ID;
        int32_t i = 0;
        for(i = 0; i < n; i++)
        {
            if(ose_peekType != OSETT_MESSAGE)
            {
                object_error((t_object *)x,
                             "/method/typed/add: expected to find "
                             "a message");
                free(tts);
                return;
            }
            tts[i + 1] = ose_peekMessageArgType(vm_s);
        }
        x->typed_method_types[x->typed_methods_count] = tts;
        x->typed_method_ntypes[x->typed_methods_count] = n;
        x->typed_method_names[x->typed_methods_count] = name;
        ++(x->typed_methods_count);
        object_addmethod(x, (method)ovm_method, name->s_name,
                         A_GIMME, 0);
    }
    else
    {
        object_error((t_object *)x,
                     "/method/typed/add: requires the name "
                     "of the method on top of the stack");
    }
}

static void ovm_addUntypedMethod(ose_bundle osevm)
{
    ovm *x = ovm_getMaxObjPtr(osevm);
    ose_bundle vm_s = x->vm_s;
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && (ose_isStringType(ose_peekMessageArgType(vm_s))
           == OSETT_TRUE))
    {
        const char * const name = ose_peekString(vm_s);
        if(!ovm_addMaxMethod(x, gensym(name)))
        {
            return;
        }
        object_addmethod(x, (method)ovm_method, name, A_GIMME, 0);
        ose_drop(vm_s);
    }
    else
    {
        object_error((t_object *)x,
                     "/method/untyped/add: requires the name "
                     "of the method on top of the stack");
    }
}

static void *ovm_new(t_symbol *sym, long argc, t_atom *argv)
{
    ovm *x = (ovm *)object_alloc(ovm_class);
    if(!x)
    {
        return NULL;
    }
    critical_new(&(x->lock));
    x->nproxies = 0;
    x->proxies = (void **)calloc(32, sizeof(void *));
    if(!x->proxies)
    {
        object_error((t_object *)x,
                     "couldn't allocate space for proxies");
        ovm_free(x);
        return NULL;
    }
    x->typed_methods_count = 0;
    x->typed_methods_size = 16;
    x->typed_method_names =
        (t_symbol **)calloc(x->typed_methods_size,
                            sizeof(t_symbol *));
    if(!x->typed_method_names)
    {
        object_error((t_object *)x,
                     "out of memory!");
        ovm_free(x);
        return NULL;
    }
    x->typed_method_types =
        (char **)calloc(x->typed_methods_size,
                        sizeof(char *));
    if(!x->typed_method_types)
    {
        object_error((t_object *)x,
                     "out of memory!");
        ovm_free(x);
        return NULL;
    }
    x->typed_method_ntypes =
        (int32_t *)calloc(x->typed_methods_size, 4);
    if(!x->typed_method_ntypes)
    {
        object_error((t_object *)x,
                     "out of memory!");
        ovm_free(x);
        return NULL;
    }
    
    x->bytes = (char *)malloc(524288);
    if(!x->bytes)
    {
        object_error((t_object *)x,
                     "couldn't allocate %d bytes for ose vm",
                     65536);
        ovm_free(x);
        return NULL;
    }
    x->bundle = ose_newBundleFromCBytes(524288, x->bytes);
    x->osevm = osevm_init(x->bundle);
    x->vm_i = OSEVM_INPUT(x->osevm);
    x->vm_s = OSEVM_STACK(x->osevm);
    x->vm_e = OSEVM_ENV(x->osevm);
    x->vm_c = OSEVM_CONTROL(x->osevm);
    x->vm_d = OSEVM_DUMP(x->osevm);
    x->vm_o = OSEVM_OUTPUT(x->osevm);
    ose_pushContextMessage(x->osevm, 65536, "/_x");
    x->vm_x = ose_enter(x->osevm, "/_x");

    /* object name and instance */
    *((intptr_t *)(ose_getBundlePtr(x->osevm)
                   + OSEVM_CACHE_OFFSET_8))
        = (intptr_t)x;
    ose_pushMessage(x->vm_e, "/maxobj", strlen("/maxobj"), 1,
                    OSETT_ALIGNEDPTR, (void *)x);
    ose_pushMessage(x->vm_e, "/name", strlen("/name"), 1,
                    OSETT_STRING, sym->s_name);

    /* stdlib */
    ose_pushMessage(x->vm_e, "/outlet", strlen("/outlet"), 1,
                    OSETT_ALIGNEDPTR, ovm_outlet);
    ose_pushMessage(x->vm_e, "/load", strlen("/load"), 1,
                    OSETT_ALIGNEDPTR, ovm_load);
    ose_pushMessage(x->vm_e, "/method/typed/add",
                    strlen("/method/typed/add"), 1,
                    OSETT_ALIGNEDPTR, ovm_addTypedMethod);
    ose_pushMessage(x->vm_e, "/method/untyped/add",
                    strlen("/method/untyped/add"), 1,
                    OSETT_ALIGNEDPTR, ovm_addUntypedMethod);
#ifdef OSE_DEBUG
    ose_pushMessage(x->vm_e, "/debug", strlen("/debug"), 1,
                    OSETT_ALIGNEDPTR, ovm_debugFromVM);
#endif

    /* inlet and outlet creation functions */
    ose_pushMessage(x->vm_e, "/makeoutlet", strlen("/makeoutlet"), 1,
                    OSETT_ALIGNEDPTR, ovm_makeoutlet);
    ose_pushMessage(x->vm_e, "/makeinlet", strlen("/makeinlet"), 1,
                    OSETT_ALIGNEDPTR, ovm_makeinlet);

    if(sym->s_name[4])
    {
        char filename[MAX_PATH_CHARS];
        snprintf(filename, MAX_PATH_CHARS,
                 "%s.ose", sym->s_name + 5);
        ose_pushMessage(x->vm_i, OSE_ADDRESS_ANONVAL,
                        OSE_ADDRESS_ANONVAL_LEN,
                        2,
                        OSETT_STRING, filename,
                        OSETT_STRING, "/!/load");
        ovm_run(x);
    }

    /* process args */
    int32_t i;
    for(i = 0; i < argc; i++)
    {
        switch(atom_gettype(argv + i))
        {
        case A_LONG:
            ose_pushInt32(x->vm_s, atom_getlong(argv + i));
            break;
        case A_FLOAT:
            ose_pushFloat(x->vm_s, atom_getfloat(argv + i));
            break;
        case A_SYM:
            ose_pushString(x->vm_s, atom_getsym(argv + i)->s_name);
            break;
        }
    }
    ose_bundleAll(x->vm_s);
    ose_appendBundle(x->vm_s, x->vm_i);
    ose_clear(x->vm_s);
    ovm_run(x);
    ovm_runHook(x, OVM_HOOK_INIT_POST, strlen(OVM_HOOK_INIT_POST));

    return x;
}

void ext_main(void *r)
{
    t_class *c;

    c = class_new("o.se", (method)ovm_new, (method)ovm_free,
                  sizeof(ovm), 0L, A_GIMME, 0);

    /* 
       would like to do this as object_addmethod, but it seems like
       it has to be done here in the class setup routine
    */
    class_addmethod(c, (method)ovm_anything, "anything", A_GIMME, 0);
    class_addmethod(c, (method)ovm_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)ovm_loadbang, "loadbang", A_CANT, 0);
    class_addmethod(c, (method)stdinletinfo, "inletinfo", A_CANT, 0);

#ifdef OSE_DEBUG
    class_addmethod(c, (method)ovm_debugFromMax, "debug", 0);
#endif
    
    class_register(CLASS_BOX, c);
    ovm_class = c;

    ps_FullPacket = gensym("FullPacket");
}
