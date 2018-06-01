#pragma once
#ifndef SMALLTALK_H
#define SMALLTALK_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t ST_Size;
typedef void *ST_OpaqueStruct;
typedef ST_OpaqueStruct ST_Context;
typedef ST_OpaqueStruct ST_Object;
typedef unsigned char ST_U8;
typedef uint16_t ST_U16;
typedef uint32_t ST_U32;

typedef ST_Object (*ST_Method)(ST_Context, ST_Object, ST_Object[]);
ST_Object ST_sendMessage(ST_Context context, ST_Object receiver,
                         ST_Object selector, ST_U8 argc, ST_Object argv[]);

void ST_setMethod(ST_Context context, ST_Object targetClass, ST_Object selector,
                  ST_Method method, ST_U8 argc);

ST_Object ST_getIVar(ST_Context context, ST_Object object, ST_U16 position);

void ST_setIVar(ST_Context context, ST_Object object, ST_U16 position,
                ST_Object value);

ST_U16 ST_getIVarCount(ST_Context context, ST_Object object);

ST_Object ST_getClass(ST_Context context, ST_Object object);

ST_Object ST_getSuper(ST_Context context, ST_Object object);

typedef struct ST_Context_Configuration {
    struct Memory {
        void *(*allocFn)(size_t);
        void (*freeFn)(void *);
        void *(*copyFn)(void *, const void *, size_t);
        void *(*setFn)(void *, int c, size_t n);
        ST_Size stackCapacity;
    } memory;
} ST_Context_Configuration;

ST_Context ST_createContext(const ST_Context_Configuration *config);

void ST_destroyContext(ST_Context context);

ST_Object ST_requestSymbol(ST_Context context, const char *symbolName);

const char *ST_Symbol_toString(ST_Context context, ST_Object symbol);

void ST_setGlobal(ST_Context context, ST_Object symbol, ST_Object value);

ST_Object ST_getGlobal(ST_Context context, ST_Object symbol);

ST_Object ST_getNilValue(ST_Context context);

ST_Object ST_getTrueValue(ST_Context context);

ST_Object ST_getFalseValue(ST_Context context);

typedef struct ST_Code {
    ST_Object *symbTab;
    ST_Size symbTabSize;
    ST_U8 *instructions;
    ST_Size length;
} ST_Code;

void ST_VM_execute(ST_Context context, const ST_Code *code, ST_Size offset);

/* Shortcuts for some common stuff. Not super efficient compared to caching
   the symbols and globals up front if you're calling a method repeatedly. */

#define ST_UNARYSEND(CONTEXT, OBJ, MESSAGE)                                    \
    ST_sendMessage(CONTEXT, OBJ, ST_requestSymbol(CONTEXT, MESSAGE), 0, NULL)

#define ST_NEW(CONTEXT, CLASSNAME_CSTR)                                        \
    ST_UNARYSEND(CONTEXT, ST_getGlobal(CONTEXT, ST_requestSymbol(              \
                                                    CONTEXT, CLASSNAME_CSTR)), \
                 "new")

#define ST_SUBCLASS(CONTEXT, BASE_CLASSNAME_STR, DERIVED_CLASSNAME_STR)        \
    ST_setGlobal(CONTEXT, ST_requestSymbol(CONTEXT, DERIVED_CLASSNAME_STR),    \
                 ST_sendMessage(                                               \
                     CONTEXT,                                                  \
                     ST_getGlobal(context, ST_requestSymbol(                   \
                                               CONTEXT, BASE_CLASSNAME_STR)),  \
                     ST_requestSymbol(CONTEXT, "subclass"), 0, NULL))

#define ST_SETMETHOD(CONTEXT, CNAME, MNAME, C_FUNC, ARGC)                      \
    ST_setMethod(CONTEXT,                                                      \
                 ST_getGlobal(CONTEXT, ST_requestSymbol(CONTEXT, CNAME)),      \
                 ST_requestSymbol(CONTEXT, MNAME), C_FUNC, ARGC)

#endif /* SMALLTALK_H */

#ifdef __cplusplus
} /* extern "C" */
#endif
