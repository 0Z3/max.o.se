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

/* for debugging */
#ifdef OSE_DEBUG
__attribute__((used))
static void pbndl(ose_bundle bundle, const char * const str)
{
    char buf[8192];
    memset(buf, 0, 8192);
    ose_pprintBundle(bundle, buf, 8192);
    fprintf(stderr, "\n\r%s>>>>>\n\r%s\n\r%s<<<<<\n\r",
            str, buf, str);
}
__attribute__((used))
static void pbytes(ose_bundle bundle, int32_t start, int32_t end)
{
    char *b = ose_getBundlePtr(bundle);
    for(int32_t i = start; i < end; i++)
    {
        fprintf(stderr, "%d: %c %d\n\r", i,
                (unsigned char)b[i],
                (unsigned char)b[i]);
    }
}
void ose_maxobj_debugFromMax(ose_maxobj *x){}
void ose_maxobj_debugFromVM(ose_bundle osevm){}
#endif

/* utility functions for dealing with the max obj
   and vm data structures 
*/

ose_maxobj *ose_maxobj_getMaxObjPtr(ose_bundle osevm)
{
    ose_maxobj *x =
        (ose_maxobj *)*((intptr_t *)(ose_getBundlePtr(osevm)
                                     + OSEVM_CACHE_OFFSET_8));
    return x;
}

int32_t ose_maxobj_getinlet(ose_maxobj *x)
{
    const int32_t inlet = (int32_t)proxy_getinlet((t_object *)x);
    const int32_t n = inlet
        ? (int32_t)(inlet - 1)
        : x->nproxies;
    return n;
}

void ose_maxobj_run(ose_maxobj *x, ose_bundle osevm)
{
    osevm_run(osevm);
}

/* outlets */
void ose_maxobj_outletBundle(void *outlet,
                             t_symbol *msg,
                             ose_bundle bundle)
{
    t_atom out[2];
    atom_setlong(out, ose_readSize(bundle));
    atom_setlong(out + 1, (long)ose_getBundlePtr(bundle));
    outlet_anything(outlet, msg, 2, out);
}

void ose_maxobj_outletBundleElem(void *outlet,
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

/* hooks and callbacks to/from the vm */
void ose_maxobj_popInputToControl(ose_bundle osevm)
{
    /* HOOK */
    osevm_popInputToControl(osevm);
}

void ose_maxobj_respondToString(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_copyBundle(vm_e, vm_s);
    ose_pushString(vm_s, OSE_MAXOBJ_HOOK_RESPOND);
    OSEVM_LOOKUP(osevm);
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(vm_s);
        ose_pushString(vm_c, "/</_e");
        ose_swap(vm_c);
        ose_pushString(vm_c, "/!/exec");
        ose_swap(vm_c);
    }
    else
    {
        ose_2drop(vm_s);
    }
}

void ose_maxobj_lookup(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_x = ose_enter(osevm, "/_x");

    const char * const address = ose_peekString(vm_s);
    int32_t mo = ose_getFirstOffsetForMatch(vm_e, address);
    if(mo >= OSE_BUNDLE_HEADER_LEN)
    {
        ose_drop(vm_s);
        ose_copyElemAtOffset(mo, vm_e, vm_s);
        return;
    }
    /* if it wasn't present in env, lookup in _x */
    mo = ose_getFirstOffsetForMatch(vm_x, address);
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

void ose_maxobj_runHook(ose_bundle osevm,
                        const char * const hookaddr,
                        int32_t hookaddrlen)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_copyBundle(vm_e, vm_s);
    ose_pushString(vm_s, hookaddr);
    OSEVM_LOOKUP(osevm);
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(vm_s);
        ose_pushMessage(vm_i, "/!/exec", strlen("/!/exec"), 0);
        osevm_run(osevm);
        ose_replaceBundle(vm_s, vm_e);
        /* ose_clear(vm_s); */
    }
    else
    {
        ose_2drop(vm_s);
    }
}

/* max methods */

void ose_maxobj_callMethod(ose_bundle osevm,
                           const char * const name,
                           int32_t namelen)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_copyBundle(vm_e, vm_s);
    ose_pushString(vm_s, name);
    OSEVM_LOOKUP(osevm);
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(vm_s);
        if(ose_getBundleElemCount(vm_c) > 0)
        {
            ose_pushString(vm_c, "/!/exec");
            ose_swap(vm_c);
        }
        else
        {
            ose_pushMessage(vm_i,
                            "/!/exec", strlen("/!/exec"), 0);
        }
    }
    else
    {
        ose_2drop(vm_s);
    }
}

int ose_maxobj_typedMethodArgChk(ose_maxobj *x,
                                 ose_bundle osevm,
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

void ose_maxobj_untypedMethod(ose_maxobj *x,
                              ose_bundle osevm,
                              t_symbol *msg,
                              long ac,
                              t_atom *av)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    int i;
    for(i = 0; i < ac; i++)
    {
        switch(atom_gettype(av + i))
        {
        case A_LONG:
            ose_pushInt32(vm_s, atom_getlong(av + i));
            break;
        case A_FLOAT:
            ose_pushFloat(vm_s, atom_getfloat(av + i));
            break;
        case A_SYM:
            ose_pushString(vm_s, atom_getsym(av + i)->s_name);
            break;
        }
        if(i > 0)
        {
            ose_push(vm_s);
        }
    }
}

void ose_maxobj_typedMethod(ose_maxobj *x,
                            ose_bundle osevm,
                            long method_index,
                            t_symbol *msg,
                            long ac,
                            t_atom *av)
{
    const char * const tts = x->typed_method_types[method_index];
    const int32_t n = x->typed_method_ntypes[method_index];
    if(ose_maxobj_typedMethodArgChk(x, osevm, n, tts, msg, ac, av))
    {
        return;
    }
    ose_maxobj_untypedMethod(x, osevm, msg, ac, av);
}

void ose_maxobj_methodFinalize(ose_maxobj *x,
                               ose_bundle osevm,
                               const char * const name,
                               const int32_t namelen)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    const int32_t inlet = ose_maxobj_getinlet(x);
    ose_pushInt32(vm_s, inlet);
    ose_maxobj_callMethod(osevm, name, namelen);
    ose_maxobj_run(x, osevm);
    ose_pushString(vm_s, "/hook/inlet/finalize");
    OSEVM_LOOKUP(osevm);
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && ose_peekMessageArgType(vm_s) == OSETT_BLOB)
    {
        ose_blobToElem(vm_s);
        ose_replaceBundle(vm_s, vm_i);
        ose_maxobj_run(x, osevm);
    }
    else
    {
        ose_drop(vm_s);
    }
}

void ose_maxobj_method_impl(ose_maxobj *x,
                            ose_bundle osevm,
                            t_symbol *msg,
                            long ac,
                            t_atom *av)
{
    long i = 0;
    for(i = 0; i < x->typed_methods_count; i++)
    {
        if(msg == x->typed_method_names[i])
        {
            ose_maxobj_typedMethod(x, osevm, i, msg, ac, av);
            ose_maxobj_methodFinalize(x, osevm, msg->s_name,
                                      strlen(msg->s_name));
            return;
        }
    }
    ose_maxobj_untypedMethod(x, osevm, msg, ac, av);
    ose_maxobj_methodFinalize(x, osevm, msg->s_name,
                              strlen(msg->s_name));
}

void ose_maxobj_method(ose_maxobj *x,
                       t_symbol *msg,
                       long ac,
                       t_atom *av)
{
    ose_maxobj_method_impl(x, x->osevm, msg, ac, av);
}

int ose_maxobj_addMaxMethod(ose_maxobj *x, t_symbol *name)
{
    extern t_symbol *ps_FullPacket;
    if(name == gensym("bang"))
    {
        object_addmethod((t_object *)x, (method)ose_maxobj_bang,
                         "bang", 0);
    }
    else if(name == gensym("int"))
    {
        object_addmethod((t_object *)x, (method)ose_maxobj_int,
                         "int", A_LONG, 0);
    }
    else if(name == gensym("float"))
    {
        object_addmethod((t_object *)x, (method)ose_maxobj_float,
                         "float", A_FLOAT, 0);
    }
    else if(name == gensym("list"))
    {
        object_addmethod((t_object *)x, (method)ose_maxobj_list,
                         "list", A_GIMME, 0);
    }
    /* the anything method seems to need to be bound in the class */
    /* else if(name == gensym("anything")) */
    /* { */
    /*     object_addmethod((t_object *)x, (method)ose_maxobj_anything, */
    /*                      "anything", A_GIMME, 0); */
    /* } */
    else if(name == ps_FullPacket)
    {
        object_addmethod((t_object *)x,
                         (method)ose_maxobj_FullPacket,
                         "FullPacket", A_LONG, A_LONG, 0);
    }
    else
    {
        return 1;
    }
    return 0;
}

void ose_maxobj_processArgs(ose_bundle osevm,
                            t_symbol *sym,
                            long argc,
                            t_atom *argv)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    int32_t i;
    for(i = 0; i < argc; i++)
    {
        switch(atom_gettype(argv + i))
        {
        case A_LONG:
            ose_pushInt32(vm_s, atom_getlong(argv + i));
            break;
        case A_FLOAT:
            ose_pushFloat(vm_s, atom_getfloat(argv + i));
            break;
        case A_SYM:
            ose_pushString(vm_s, atom_getsym(argv + i)->s_name);
            break;
        }
    }
    ose_bundleAll(vm_s);
    ose_appendBundle(vm_s, vm_i);
    osevm_run(osevm);
    ose_maxobj_runHook(osevm, OSE_MAXOBJ_HOOK_INIT_POST,
                       strlen(OSE_MAXOBJ_HOOK_INIT_POST));
}

/* class and subclass */

void ose_maxobj_loadSubclass_impl(ose_maxobj *x,
                                  const char * const filename)
{
    ose_bundle vm_i = OSEVM_INPUT(x->osevm);
    ose_pushMessage(vm_i, OSE_ADDRESS_ANONVAL,
                    OSE_ADDRESS_ANONVAL_LEN,
                    2,
                    OSETT_STRING, filename,
                    OSETT_STRING, "/!/load");
    ose_maxobj_run(x, x->osevm);
}

void ose_maxobj_loadSubclass(ose_maxobj *x, t_symbol *sym)
{
    if(sym->s_name[4])
    {
        char filename[MAX_PATH_CHARS];
        snprintf(filename, MAX_PATH_CHARS,
                 "%s.ose", sym->s_name + 5);
        ose_maxobj_loadSubclass_impl(x, filename);
    }
}

/* default handlers for max messages */

void ose_maxobj_FullPacket_impl(ose_maxobj *x, ose_bundle osevm,
                           long len, long ptr)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_pushBlob(vm_s, (int32_t)len, (char *)ptr);
    ose_blobToElem(vm_s);
    ose_maxobj_methodFinalize(x, osevm,
                              "FullPacket",
                              strlen("FullPacket"));
}

void ose_maxobj_FullPacket(ose_maxobj *x, long len, long ptr)
{
    ose_maxobj_FullPacket_impl(x, x->osevm, len, ptr);
}

void ose_maxobj_anything_impl(ose_maxobj *x,
                              ose_bundle osevm,
                              t_symbol *s,
                              long ac,
                              t_atom *av)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_pushString(vm_s, s->s_name);
    int i;
    for(i = 0; i < ac; i++)
    {
        switch(atom_gettype(av + i))
        {
        case A_LONG:
            ose_pushInt32(vm_s, atom_getlong(av + i));
            break;
        case A_FLOAT:
            ose_pushFloat(vm_s, atom_getfloat(av + i));
            break;
        case A_SYM:
            ose_pushString(vm_s, atom_getsym(av + i)->s_name);
            break;
        }
        ose_push(vm_s);
    }
    ose_maxobj_methodFinalize(x, osevm,
                              "anything", strlen("anything"));
}
    
void ose_maxobj_anything(ose_maxobj *x,
                         t_symbol *s,
                         long ac,
                         t_atom *av)
{
    ose_maxobj_anything_impl(x, x->osevm, s, ac, av);
}

void ose_maxobj_list_impl(ose_maxobj *x,
                          ose_bundle osevm,
                          t_symbol *s,
                          long ac,
                          t_atom *av)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    int i;
    for(i = 0; i < ac; i++)
    {
        switch(atom_gettype(av + i))
        {
        case A_LONG:
            ose_pushInt32(vm_s, atom_getlong(av + i));
            break;
        case A_FLOAT:
            ose_pushFloat(vm_s, atom_getfloat(av + i));
            break;
        case A_SYM:
            ose_pushString(vm_s, atom_getsym(av + i)->s_name);
            break;
        }
        if(i > 0)
        {
        	ose_push(vm_s);
        }
    }
    ose_maxobj_methodFinalize(x, osevm, "list", strlen("list"));
}

void ose_maxobj_list(ose_maxobj *x,
                     t_symbol *s,
                     long ac,
                     t_atom *av)
{
    ose_maxobj_list_impl(x, x->osevm, s, ac, av);
}

void ose_maxobj_float_impl(ose_maxobj *x, ose_bundle osevm, double f)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_pushFloat(vm_s, (float)f);
    ose_maxobj_methodFinalize(x, osevm, "float", strlen("float"));
}

void ose_maxobj_float(ose_maxobj *x, double f)
{
    ose_maxobj_float_impl(x, x->osevm, f);
}

void ose_maxobj_int_impl(ose_maxobj *x, ose_bundle osevm, long l)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_pushInt32(vm_s, (int32_t)l);
    ose_maxobj_methodFinalize(x, osevm, "int", strlen("int"));
}

void ose_maxobj_int(ose_maxobj *x, long l)
{
    ose_maxobj_int_impl(x, x->osevm, l);
}

void ose_maxobj_bang_impl(ose_maxobj *x, ose_bundle osevm)
{
    ose_maxobj_methodFinalize(x, osevm, "bang", strlen("bang"));
}

void ose_maxobj_bang(ose_maxobj *x)
{
    ose_maxobj_bang_impl(x, x->osevm);
}

void ose_maxobj_loadbang(ose_maxobj *x)
{
}

void ose_maxobj_assist(ose_maxobj *x,
                       void *b,
                       long m,
                       long a,
                       char *s)
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

void ose_maxobj_free(ose_maxobj *x)
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

int ose_maxobj_init(ose_maxobj *x,
                    t_symbol *sym,
                    long argc,
                    t_atom *argv)
{
    critical_new(&(x->lock));
    x->nproxies = 0;
    x->proxies = (void **)calloc(32, sizeof(void *));
    if(!x->proxies)
    {
        object_error((t_object *)x,
                     "couldn't allocate space for proxies");
        ose_maxobj_free(x);
        return 1;
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
        ose_maxobj_free(x);
        return 1;
    }
    x->typed_method_types =
        (char **)calloc(x->typed_methods_size,
                        sizeof(char *));
    if(!x->typed_method_types)
    {
        object_error((t_object *)x,
                     "out of memory!");
        ose_maxobj_free(x);
        return 1;
    }
    x->typed_method_ntypes =
        (int32_t *)calloc(x->typed_methods_size, 4);
    if(!x->typed_method_ntypes)
    {
        object_error((t_object *)x,
                     "out of memory!");
        ose_maxobj_free(x);
        return 1;
    }

    *((intptr_t *)(ose_getBundlePtr(x->osevm)
                   + OSEVM_CACHE_OFFSET_8))
        = (intptr_t)x;
    return 0;
}
