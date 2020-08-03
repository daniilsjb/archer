#pragma once

#include "common.h"
#include "value.h"

typedef struct {
    ObjString* key;
    Value value;
} Entry;

typedef struct {
    int count;
    int capacityMask;
    Entry* entries;
} Table;

void table_init(Table* table);
void table_free(Table* table);

bool table_get(Table* table, ObjString* key, Value* value);

bool table_put(Table* table, ObjString* key, Value value);
void table_put_from(Table* source, Table* destination);

bool table_remove(Table* table, ObjString* key);

ObjString* table_find_string(Table* table, const char* chars, size_t length, uint32_t hash);

void table_remove_white(Table* table);
void mark_table(Table* table);