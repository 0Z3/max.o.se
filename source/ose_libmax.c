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

void ose_libmax_load(ose_bundle osevm)
{
    ose_maxobj *x =
        (ose_maxobj *)*((intptr_t *)(ose_getBundlePtr(osevm)
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

void ose_libmax_outlet(ose_bundle osevm)
{
    ose_maxobj *x = ose_maxobj_getMaxObjPtr(osevm);
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
    extern t_symbol *ps_FullPacket;
    ose_maxobj_outletBundleElem(outlet, ps_FullPacket, vm_s, o);
    ose_drop(vm_s);
}

void ose_libmax_makeoutlet(ose_bundle osevm)
{
    ose_maxobj *x = ose_maxobj_getMaxObjPtr(osevm);
    void *outlet = outlet_new(x, "FullPacket");
    ose_pushAlignedPtr(x->vm_s, outlet);
}

void ose_libmax_makeinlet(ose_bundle osevm)
{
    ose_maxobj *x = ose_maxobj_getMaxObjPtr(osevm);
    x->proxies[x->nproxies]
        = proxy_new((t_object *)x,
                    x->nproxies + 1,
                    &(x->inlet));
    ++(x->nproxies);
}

/* 
   We simulate the ability to add typed methods dynamically, even
   though we can't. Later, we can add a code gen or jit stage that
   will do them for real.
*/
void ose_libmax_addTypedMethod(ose_bundle osevm)
{
    ose_maxobj *x = ose_maxobj_getMaxObjPtr(osevm);
    ose_bundle vm_s = x->vm_s;
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && (ose_isStringType(ose_peekMessageArgType(vm_s))
           == OSETT_TRUE))
    {
        t_symbol *name = gensym(ose_peekString(vm_s));
        ose_drop(vm_s);
        if(!ose_maxobj_addMaxMethod(x, name))
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
        object_addmethod(x, (method)ose_maxobj_method, name->s_name,
                         A_GIMME, 0);
    }
    else
    {
        object_error((t_object *)x,
                     "/method/typed/add: requires the name "
                     "of the method on top of the stack");
    }
}

void ose_libmax_addUntypedMethod(ose_bundle osevm)
{
    ose_maxobj *x = ose_maxobj_getMaxObjPtr(osevm);
    ose_bundle vm_s = x->vm_s;
    if(ose_peekType(vm_s) == OSETT_MESSAGE
       && (ose_isStringType(ose_peekMessageArgType(vm_s))
           == OSETT_TRUE))
    {
        const char * const name = ose_peekString(vm_s);
        if(!ose_maxobj_addMaxMethod(x, gensym(name)))
        {
            return;
        }
        object_addmethod(x, (method)ose_maxobj_method, name, A_GIMME, 0);
        ose_drop(vm_s);
    }
    else
    {
        object_error((t_object *)x,
                     "/method/untyped/add: requires the name "
                     "of the method on top of the stack");
    }
}

/* initialization functions */
void ose_libmax_addFunctionsToEnv(ose_bundle osevm)
{
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_pushMessage(vm_e, "/outlet", strlen("/outlet"), 1,
                    OSETT_ALIGNEDPTR, ose_libmax_outlet);
    ose_pushMessage(vm_e, "/load", strlen("/load"), 1,
                    OSETT_ALIGNEDPTR, ose_libmax_load);
    ose_pushMessage(vm_e, "/method/typed/add",
                    strlen("/method/typed/add"), 1,
                    OSETT_ALIGNEDPTR, ose_libmax_addTypedMethod);
    ose_pushMessage(vm_e, "/method/untyped/add",
                    strlen("/method/untyped/add"), 1,
                    OSETT_ALIGNEDPTR, ose_libmax_addUntypedMethod);
    ose_pushMessage(vm_e, "/makeoutlet", strlen("/makeoutlet"), 1,
                    OSETT_ALIGNEDPTR, ose_libmax_makeoutlet);
    ose_pushMessage(vm_e, "/makeinlet", strlen("/makeinlet"), 1,
                    OSETT_ALIGNEDPTR, ose_libmax_makeinlet);
#ifdef OSE_DEBUG
    ose_pushMessage(vm_e, "/debug", strlen("/debug"), 1,
                    OSETT_ALIGNEDPTR, ose_maxobj_debugFromVM);
#endif
}

void ose_libmax_addObjInfoToEnv(ose_maxobj *x,
                                ose_bundle osevm,
                                t_symbol *sym,
                                long argc,
                                t_atom *argv)
{
    ose_pushMessage(x->vm_e, "/maxobj", strlen("/maxobj"), 1,
                    OSETT_ALIGNEDPTR, (void *)x);
    ose_pushMessage(x->vm_e, "/name", strlen("/name"), 1,
                    OSETT_STRING, sym->s_name);
    /* could also add:
       - args the box was instantiated with
    */
}
