#pragma once

#include "../lists/list.h"

typedef struct kvp {
	void* key;
	void* value;
} KeyValuePair_t;

typedef struct dict {
	List pairs; // List of KeyValuePair_t

	SizeT keySize; // Size of the key in bytes
	SizeT valSize; // Size of the value in bytes
} Dict;

void initDict(Dict* dict, SizeT keySize, SizeT valSize, SizeT n_records);

void setPair(Dict* dict, void* currentKey, KeyValuePair_t newKvp);
void setValue(Dict* dict, void* key, void* newValue);
void* getValue(Dict* dict, void* key);

void addEntry(Dict* dict, void* key, void* value);

void removeValue(Dict* dict, void* key);

void freeDict(Dict* dict, bool freeKeys, bool freeValues);