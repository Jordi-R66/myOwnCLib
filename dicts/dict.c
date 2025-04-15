#include "dict.h"

void initDict(Dict* dict, size_t keySize, size_t valSize, size_t initCapacity) {
	initializeList(&dict->pairs, initCapacity, sizeof(KeyValuePair_t));

	dict->keySize = keySize;
	dict->valSize = valSize;
}


void setPair(Dict* dict, void* currentKey, KeyValuePair_t newKvp) {
	for (size_t i=0; i < dict->pairs.n_elements; i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getElement(&dict->pairs, i);

		if (memcmp(currentKey, kvp_ptr->key, dict->keySize)) {
			kvp_ptr->key = newKvp.key;
			kvp_ptr->value = newKvp.value;
			break;
		}
	}
}

void setValue(Dict* dict, void* key, void* newValue) {
	KeyValuePair_t kvp = {key, newValue};

	setPair(dict, key, kvp);
}

void* getValue(Dict* dict, void* key) {
	void* val = NULL;

	for (size_t i=0; i < dict->pairs.n_elements; i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getElement(&dict->pairs, i);

		if (memcmp(key, kvp_ptr->key, dict->keySize)) {
			val = kvp_ptr->value;
			break;
		}
	}

	return val;
}

void addPair(Dict* dict, KeyValuePair_t kvp) {
	addElement(&dict->pairs, &kvp);
}

void addEntry(Dict* dict, void* key, void* value) {
	KeyValuePair_t kvp;

	kvp.key = key;
	kvp.value = value;

	// D'abord on initialise deux arrays, pour respectivement stocker la valeur de la clé, et la valeur elle-même
	void* temp_key = calloc(1, dict->keySize);
	void* temp_val = calloc(1, dict->valSize);

	// Ensuite on copie le contenu
	memcpy(temp_key, key, dict->keySize);
	memcpy(temp_val, value, dict->valSize);

	kvp.key = temp_key;
	kvp.value = temp_val;

	addPair(dict, kvp);
}

void removePair(Dict* dict, void* key) {
	for (size_t i=0; i < dict->pairs.n_elements; i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getElement(&dict->pairs, i);

		if (memcmp(key, kvp_ptr->key, dict->keySize)) {
			free(kvp_ptr->key);
			free(kvp_ptr->value);
			removeElement(&dict->pairs, i, true);
			break;
		}
	}
}

void freeDict(Dict* dict, bool freeKeys, bool freeValues) {
	for (size_t i=0; (i < dict->pairs.n_elements) && ((freeKeys) || (freeValues)); i++) {
		KeyValuePair_t* kvp_ptr = (KeyValuePair_t*)getElement(&dict->pairs, i);
		removePair(dict, kvp_ptr->key);
	}

	freeList(&dict->pairs);
}