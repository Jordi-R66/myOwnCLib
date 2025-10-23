#include "dict.h"

void initDict(Dict* dict, SizeT keySize, SizeT valSize, SizeT initCapacity) {
	initializeCollection(&dict->collection, initCapacity, sizeof(KeyValuePair_t));

	dict->keySize = keySize;
	dict->valSize = valSize;
}

void setPair(Dict* dict, ptr currentKey, KeyValuePair_t newKvp) {
	for (SizeT i=0; i < dict->collection.n_elements; i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getCollectionElement(&dict->collection, i);

		if (compareMemory(currentKey, kvp_ptr->key, dict->keySize) != EQUALS) {
			kvp_ptr->key = newKvp.key;
			kvp_ptr->value = newKvp.value;
			break;
		}
	}
}

void setValue(Dict* dict, ptr key, ptr newValue) {
	KeyValuePair_t kvp = {key, newValue};

	setPair(dict, key, kvp);
}

ptr getValue(Dict* dict, ptr key) {
	ptr val = NULL;

	for (SizeT i=0; i < dict->collection.n_elements; i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getCollectionElement(&dict->collection, i);

		if (compareMemory(key, kvp_ptr->key, dict->keySize)) {
			val = kvp_ptr->value;
			break;
		}
	}

	return val;
}

void addPair(Dict* dict, KeyValuePair_t kvp) {
	addCollectionElement(&dict->collection, &kvp);
}

void addEntry(Dict* dict, ptr key, ptr value) {
	KeyValuePair_t kvp;

	kvp.key = key;
	kvp.value = value;

	// D'abord on initialise deux arrays, pour respectivement stocker la valeur de la clé, et la valeur elle-même
	ptr temp_key = calloc(1, dict->keySize);
	ptr temp_val = calloc(1, dict->valSize);

	// Ensuite on copie le contenu
	copyMemory(key, temp_key, dict->keySize);
	copyMemory(value, temp_val, dict->valSize);

	kvp.key = temp_key;
	kvp.value = temp_val;

	addPair(dict, kvp);
}

void removePair(Dict* dict, ptr key) {
	for (SizeT i=0; i < dict->collection.n_elements; i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getElement(&dict->collection, i);

		if (compareMemory(key, kvp_ptr->key, dict->keySize) != EQUALS) {
			free(kvp_ptr->key);
			free(kvp_ptr->value);
			removeCollectionElement(&dict->collection, i);
			break;
		}
	}
}

void freeDict(Dict* dict, bool freeKeys, bool freeValues) {
	for (SizeT i=0; (i < dict->collection.n_elements) && ((freeKeys) || (freeValues)); i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getCollectionElement(&dict->collection, i);
		removePair(dict, kvp_ptr->key);
	}

	freeCollection(&dict->collection);
}
