#pragma once

#ifdef INCLUDE_DICTS

#ifndef INCLUDE_LISTS
#define INCLUDE_LISTS
#endif

#include "../lists/list.h"

#pragma pack(1)
typedef struct kvp {
	ptr key;
	ptr value;
} KeyValuePair_t;

typedef struct dict {
	List pairs; // List of KeyValuePair_t

	SizeT keySize; // Size of the key in bytes
	SizeT valSize; // Size of the value in bytes
} Dict;
#pragma pack()

#define KVP_SIZE sizeof(KeyValuePair_t)
#define DICT_SIZE sizeof(Dict)

void initDict(Dict* dict, SizeT keySize, SizeT valSize, SizeT n_records);

void setPair(Dict* dict, ptr currentKey, KeyValuePair_t newKvp);
void setValue(Dict* dict, ptr key, ptr newValue);
ptr getValue(Dict* dict, ptr key);

void addEntry(Dict* dict, ptr key, ptr value);

void removeValue(Dict* dict, ptr key);

void freeDict(Dict* dict, bool freeKeys, bool freeValues);
#endif