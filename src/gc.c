#include "gc.h"
#include "vm.h"
#include "common.h"
#include "memory.h"
#include "memlib.h"
#include "object.h"
#include "value.h"
#include "table.h"

#if DEBUG_LOG_GC
#include <stdio.h>
#endif

#define GC_THRESHOLD_GROW_FACTOR 2

void gc_init(GC* gc)
{
    gc->vm = NULL;
    gc->allocatedObjects = NULL;

    gc->bytesAllocated = 0;
    gc->threshold = 1024 * 1024;

    gc->grayCount = 0;
    gc->grayCapacity = 0;
    gc->grayStack = NULL;
}

static void free_object(GC* gc, Obj* object)
{
    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            deallocate(gc, string, string->length + 1);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            chunk_free(gc, &function->chunk);
            FREE(gc, ObjFunction, function);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            FREE_ARRAY(gc, ObjUpvalue*, closure->upvalues, closure->upvalueCount);
            FREE(gc, ObjClosure, closure);
            break;
        }
        case OBJ_UPVALUE: {
            FREE(gc, ObjUpvalue, object);
            break;
        }
        case OBJ_NATIVE: {
            FREE(gc, ObjNative, object);
            break;
        }
        case OBJ_CLASS: {
            ObjClass* loxClass = (ObjClass*)object;
            table_free(gc, &loxClass->methods);
            FREE(gc, ObjClass, object);
            break;
        }
        case OBJ_INSTANCE: {
            ObjInstance* instance = (ObjInstance*)object;
            table_free(gc, &instance->fields);
            FREE(gc, ObjInstance, object);
            break;
        }
        case OBJ_BOUND_METHOD: {
            FREE(gc, ObjBoundMethod, object);
            break;
        }
    }
}

void gc_free(GC* gc)
{
    Obj* current = gc->allocatedObjects;

    while (current) {
        Obj* next = current->next;
        free_object(gc, current);
        current = next;
    }

    raw_deallocate(gc->grayStack);
}

void gc_allocate_bytes(GC* gc, size_t size)
{
    gc->bytesAllocated += size;
}

void gc_deallocate_bytes(GC* gc, size_t size)
{
    gc->bytesAllocated -= size;
}

void gc_mark_object(GC* gc, Obj* object)
{
    if (!object || object->marked) {
        return;
    }

#if DEBUG_LOG_GC
    printf("%p mark", (void*)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif

    object->marked = true;

    if (gc->grayCapacity < gc->grayCount + 1) {
        gc->grayCapacity = GROW_CAPACITY(gc->grayCapacity);

        void* reallocated = raw_reallocate(gc->grayStack, sizeof(Obj*) * gc->grayCapacity);
        if (reallocated) {
            gc->grayStack = reallocated;
        }
    }

    gc->grayStack[gc->grayCount++] = object;
}

static void mark_value(GC* gc, Value value)
{
    if (IS_OBJ(value)) {
        gc_mark_object(gc, AS_OBJ(value));
    }
}

static void mark_array(GC* gc, ValueArray* array)
{
    for (size_t i = 0; i < array->count; i++) {
        mark_value(gc, array->data[i]);
    }
}

void mark_table(GC* gc, Table* table)
{
    for (int i = 0; i <= table->capacityMask; i++) {
        Entry* entry = &table->entries[i];
        gc_mark_object(gc, (Obj*)entry->key);
        mark_value(gc, entry->value);
    }
}

static void mark_roots(GC* gc)
{
    VM* vm = gc->vm;
    for (Value* slot = vm->stack; slot < vm->stackTop; slot++) {
        mark_value(gc, *slot);
    }

    for (size_t i = 0; i < vm->frameCount; i++) {
        gc_mark_object(gc, (Obj*)vm->frames[i].closure);
    }

    for (ObjUpvalue* upvalue = vm->openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        gc_mark_object(gc, (Obj*)upvalue);
    }

    mark_table(gc, &vm->globals);
    mark_compiler_roots(gc->vm);
    gc_mark_object(gc, (Obj*)vm->initString);
}

static void traverse_object(GC* gc, Obj* object)
{
#if DEBUG_LOG_GC
    printf("%p blacken ", (void*)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_UPVALUE: {
            mark_value(gc, ((ObjUpvalue*)object)->closed);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            gc_mark_object(gc, (Obj*)function->name);
            mark_array(gc, &function->chunk.constants);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            gc_mark_object(gc, (Obj*)closure->function);
            for (size_t i = 0; i < closure->upvalueCount; i++) {
                gc_mark_object(gc, (Obj*)closure->upvalues[i]);
            }
            break;
        }
        case OBJ_CLASS: {
            ObjClass* loxClass = (ObjClass*)object;
            gc_mark_object(gc, (Obj*)loxClass->name);
            mark_table(gc, &loxClass->methods);

            ObjInstance* instance = (ObjInstance*)&loxClass->obj;
            gc_mark_object(gc, (Obj*)instance->loxClass);
            mark_table(gc, &instance->fields);
            break;
        }
        case OBJ_INSTANCE: {
            ObjInstance* instance = (ObjInstance*)object;
            gc_mark_object(gc, (Obj*)instance->loxClass);
            mark_table(gc, &instance->fields);
            break;
        }
        case OBJ_BOUND_METHOD: {
            ObjBoundMethod* boundMethod = (ObjBoundMethod*)object;
            mark_value(gc, boundMethod->receiver);
            gc_mark_object(gc, (Obj*)boundMethod->method);
            break;
        }
        case OBJ_NATIVE: {
            break;
        }
        case OBJ_STRING: {
            break;
        }
    }
}

static void trace_references(GC* gc)
{
    VM* vm = gc->vm;
    while (gc->grayCount > 0) {
        Obj* object = gc->grayStack[--gc->grayCount];
        traverse_object(gc, object);
    }
}

void table_remove_white(Table* table)
{
    for (int i = 0; i <= table->capacityMask; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key != NULL && !entry->key->obj.marked) {
            table_remove(table, entry->key);
}
    }
}

static void sweep(GC* gc)
{
    Obj* previous = NULL;
    Obj* object = gc->allocatedObjects;

    while (object != NULL) {
        if (object->marked) {
            object->marked = false;
            previous = object;
            object = object->next;
        } else {
            Obj* unreached = object;

            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                gc->allocatedObjects = object;
            }

            free_object(gc, unreached);
        }
    }
}

static void perform_collection(GC* gc)
{
#if DEBUG_LOG_GC
    printf("-- GC Begin\n");
    size_t before = gc->bytesAllocated;
#endif

    mark_roots(gc);
    trace_references(gc);
    table_remove_white(&gc->vm->strings);
    sweep(gc);

    gc->threshold = gc->bytesAllocated * GC_THRESHOLD_GROW_FACTOR;

#if DEBUG_LOG_GC
    printf("-- GC End\n");
    printf("-- Collected %zu bytes (from %zu to %zu), next at %zu\n", before - gc->bytesAllocated, before, gc->bytesAllocated, gc->threshold);
#endif
}

void gc_attempt_collection(GC* gc)
{
    if (gc->bytesAllocated > gc->threshold) {
        perform_collection(gc);
    }
}

void gc_append_object(GC* gc, Obj* object)
{
    object->next = gc->allocatedObjects;
    gc->allocatedObjects = object;
}
