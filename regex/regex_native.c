// regex_native.c — POSIX Regular Expression Engine (TRE)
//
// Provides three native functions:
//   compile(pattern, flags?) -> Resource   — compile regex with optional "i"/"m" flags
//   find(handle, text)       -> Array|Null — first match, returns [[start,end], ...] groups
//   free(handle)             -> Null        — explicit cleanup (also handled by GC finalizer)
//
// Supported flags string:
//   'i' / 'I' — case-insensitive (REG_ICASE)
//   'm' / 'M' — multiline: ^ and $ match at line boundaries (REG_NEWLINE)

#include "vm/vm.h"
#include "vm/registry.h"
#include "runtime/utf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define ERRBUF_SIZE 256

// ═══════════════════════════════════════════════════════════════
// GC Finalizer
// ═══════════════════════════════════════════════════════════════

static void regexFinalizer(void *ptr) {
    regex_t *regex = (regex_t *)ptr;
    if (regex) {
        regfree(regex);
        free(regex);
    }
}

// ═══════════════════════════════════════════════════════════════
// Compilation
// ═══════════════════════════════════════════════════════════════

static Value regexCompileNative(djazairVM *vm, int argCount, Value *args) {
    if (argCount < 1) {
        runtimeError(vm, "regex.compile(): Expected at least %d argument(s) but got %d.", 1, argCount);
        return NULL_VAL;
    }
    if (!IS_STRING(args[0])) {
        runtimeError(vm, "regex.compile(): Argument 1 must be a string.");
        return NULL_VAL;
    }

    const char *pattern = AS_CSTRING(args[0]);
    bool case_insensitive = false;
    bool multiline = false;

    if (argCount > 1) {
        if (!IS_STRING(args[1])) {
            runtimeError(vm, "regex.compile(): Argument 2 must be a string.");
            return NULL_VAL;
        }
        const char *flags = AS_CSTRING(args[1]);
        for (const char *f = flags; *f; f++) {
            if (*f == 'i' || *f == 'I') case_insensitive = true;
            if (*f == 'm' || *f == 'M') multiline = true;
        }
    }

    regex_t *regex = (regex_t *)malloc(sizeof(regex_t));
    if (regex == NULL) {
        runtimeError(vm, "regex.compile(): Out of memory allocating regex structure.");
        return NULL_VAL;
    }

    int cflags = REG_EXTENDED;
    if (case_insensitive) cflags |= REG_ICASE;
    if (multiline)        cflags |= REG_NEWLINE;

    int rc = regcomp(regex, pattern, cflags);
    if (rc != 0) {
        char errbuf[ERRBUF_SIZE];
        regerror(rc, regex, errbuf, sizeof(errbuf));
        regfree(regex);
        free(regex);
        runtimeError(vm, "regex.compile(): Failed to compile pattern '%s': %s", pattern, errbuf);
        return NULL_VAL;
    }

    return OBJ_VAL(newResourceWithFinalizer(vm, regex, copyString(vm, "Regex", 5), regexFinalizer));
}

// ═══════════════════════════════════════════════════════════════
// Execution
// ═══════════════════════════════════════════════════════════════

static Value regexFindNative(djazairVM *vm, int argCount, Value *args) {
    if (argCount != 2) {
        runtimeError(vm, "regex.find(): Expected %d argument(s) but got %d.", 2, argCount);
        return NULL_VAL;
    }
    if (!IS_RESOURCE(args[0])) {
        runtimeError(vm, "regex.find(): Argument 1 must be a resource.");
        return NULL_VAL;
    }
    if (!IS_STRING(args[1])) {
        runtimeError(vm, "regex.find(): Argument 2 must be a string.");
        return NULL_VAL;
    }

    regex_t *regex = (regex_t *)AS_RESOURCE(args[0])->ptr;
    const char *text = AS_CSTRING(args[1]);

    if (regex == NULL) {
        runtimeError(vm, "regex.find(): Invalid or freed regex handle.");
        return NULL_VAL;
    }

    size_t nmatch = regex->re_nsub + 1;
    regmatch_t *pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);
    if (pmatch == NULL) {
        runtimeError(vm, "regex.find(): Out of memory allocating match results.");
        return NULL_VAL;
    }

    int rc = regexec(regex, text, nmatch, pmatch, 0);
    if (rc == REG_NOMATCH) {
        free(pmatch);
        return NULL_VAL;
    } else if (rc != 0) {
        char errbuf[ERRBUF_SIZE];
        regerror(rc, regex, errbuf, sizeof(errbuf));
        free(pmatch);
        runtimeError(vm, "regex.find(): %s", errbuf);
        return NULL_VAL;
    }

    Value result = OBJ_VAL(newArray(vm));
    push(vm, result);

    for (size_t i = 0; i < nmatch; i++) {
        Value group = OBJ_VAL(newArray(vm));
        push(vm, group);

        int start = pmatch[i].rm_so < 0 ? -1 : utf8_byte_index_to_offset(text, (int)pmatch[i].rm_so);
        int end   = pmatch[i].rm_eo < 0 ? -1 : utf8_byte_index_to_offset(text, (int)pmatch[i].rm_eo);

        writeValueArray(vm, &AS_ARRAY(group)->items, INT_VAL((int)start));
        writeValueArray(vm, &AS_ARRAY(group)->items, INT_VAL((int)end));

        writeValueArray(vm, &AS_ARRAY(result)->items, group);
        pop(vm);
    }

    pop(vm);
    free(pmatch);
    return result;
}

// ═══════════════════════════════════════════════════════════════
// Cleanup
// ═══════════════════════════════════════════════════════════════

static Value regexFreeNative(djazairVM *vm, int argCount, Value *args) {
    if (argCount != 1) {
        runtimeError(vm, "regex.free(): Expected %d argument(s) but got %d.", 1, argCount);
        return NULL_VAL;
    }
    if (!IS_RESOURCE(args[0])) {
        runtimeError(vm, "regex.free(): Argument 1 must be a resource.");
        return NULL_VAL;
    }

    ObjResource *resource = AS_RESOURCE(args[0]);
    regex_t *regex = (regex_t *)resource->ptr;
    if (regex != NULL) {
        regfree(regex);
        free(regex);
        resource->ptr = NULL;
        resource->isOpen = false;
    }
    return NULL_VAL;
}

// ═══════════════════════════════════════════════════════════════
// Registration
// ═══════════════════════════════════════════════════════════════

static NativeMethod regex_nativeMethods[] = {
    {"compile", regexCompileNative, -1},
    {"find",    regexFindNative,     2},
    {"free",    regexFreeNative,     1},
    {NULL, NULL, 0}
};

void init_regex_module(djazairVM *vm) {
    djazair_register_native_module(vm, "std_regex_native", regex_nativeMethods);
}
