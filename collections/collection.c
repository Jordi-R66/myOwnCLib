#include "collection.h"

const Collection NULL_COLLECTION = {
	.capacity = 0,
	.length = 0,
	.elementSize = 0,
	.flags = NO_FLAGS,
	.elements = NULL
};

#pragma region COLLECTION_FLAGS_GETTER

bool isCollectionFragmented(CollectionPtr collection) {
	return (collection->flags & COLLECTION_FRAGMENTED) == COLLECTION_FRAGMENTED;
}

bool isCollectionInitialised(CollectionPtr collection) {
	return (collection->flags & COLLECTION_INITIALISED) == COLLECTION_INITIALISED;
}

#pragma endregion

#pragma region COLLECTION_FLAGS_SETTER

CollectionFlag collectionFragmented(CollectionPtr collection, bool val) {
	CollectionFlag flags = collection->flags;

	if (val) {
		flags = flags | COLLECTION_FRAGMENTED;
	} else {
		flags = flags & ~COLLECTION_FRAGMENTED;
	}

	collection->flags = flags;

	return flags;
}

CollectionFlag collectionInitialised(CollectionPtr collection, bool val) {
	CollectionFlag flags = collection->flags;

	if (val) {
		flags = flags | COLLECTION_INITIALISED;
	} else {
		flags = flags & ~COLLECTION_INITIALISED;
	}

	collection->flags = flags;

	return flags;
}

#pragma endregion

#pragma region MANAGEMENT

void initializeCollection(CollectionPtr collection, SizeT initCapacity, SizeT elementSize) {
	if (isCollectionInitialised(collection)) {
		freeCollection(collection);
	}

	collection->capacity = initCapacity;
	collection->length = 0;
	collection->elementSize = elementSize;

	collectionInitialised(collection, true);
	collectionFragmented(collection, false);

	ptr tempPtr = (ptr)calloc(initCapacity, elementSize);

	if (tempPtr != NULL) {
		collection->elements = tempPtr;
	} else {
		fprintf(stderr, "Couldn't properly initialize the collection, `calloc()` can't find enough space!\n");

		collectionInitialised(collection, false);
		collection->capacity = 0;
		collection->elementSize = 0;

		free(tempPtr);
		exit(EXIT_FAILURE);
	}
}

void freeCollection(CollectionPtr collection) {
	if (!isCollectionInitialised(collection)) {
		fprintf(stderr, "Can't free a collection that was never initialized!\n");
		exit(EXIT_FAILURE);
	}

	setMemory(collection->elements, 0, collection->capacity * collection->elementSize);

	free(collection->elements);

	collection->capacity = 0;
	collection->elementSize = 0;
	collection->length = 0;
	collectionFragmented(collection, false);
	collectionInitialised(collection, false);
}

void resizeCollection(CollectionPtr collection, SizeT newCapacity) {
	if (newCapacity < collection->length) {
		newCapacity = collection->length;
	}

	if (newCapacity <= 0) {
		freeCollection(collection);
		fprintf(stderr, "Can't resize a collection to a null or negative length (%zu elements)\n", newCapacity);
		exit(EXIT_FAILURE);
	}

	ptr tempPtr = (ptr)realloc(collection->elements, newCapacity * collection->elementSize);

	if (tempPtr != NULL) {
		//printf("new capacity : %zu elements\n", newCapacity);
		collection->elements = tempPtr;
		collection->capacity = newCapacity;
	} else {
		free(tempPtr);
		fprintf(stderr, "Reallocation failure in `resizeCollection(%p, index)`\n", (ptr)collection);
		exit(EXIT_FAILURE);
	}
}

SizeT shrinkCollectionToFit(CollectionPtr collection) {
	resizeCollection(collection, 0);

	return collection->capacity;
}

void copyCollection(CollectionPtr collectionDest, CollectionPtr collectionSrc) {
	if ((collectionSrc == NULL) || (collectionDest == NULL) || (collectionDest == collectionSrc)) {
		if (collectionSrc != NULL) {
			freeCollection(collectionSrc);
		}

		fprintf(stderr, "Couldn't copy the collection\n");
		exit(EXIT_FAILURE);
	}

	memcpy(collectionDest, collectionSrc, sizeof(Collection));

	ptr ptr = calloc(collectionDest->capacity, collectionDest->elementSize);

	if (ptr != NULL) {
		collectionDest->elements = ptr;
	} else {

		free(ptr);
		exit(EXIT_FAILURE);
	}

	memcpy(collectionDest->elements, collectionSrc->elements, collectionSrc->capacity * collectionSrc->elementSize);
}

#pragma endregion

#pragma region CRUD

void addCollectionElement(CollectionPtr collection, ptr newElement) {
	if ((collection->length + 1) > collection->capacity) {
		resizeCollection(collection, collection->length + 50);
	}

	SizeT nBytes = collection->length * collection->elementSize;
	copyMemory((ptr)(collection->elements + nBytes), newElement, collection->elementSize);

	collection->length++;
}

void removeCollectionElement(CollectionPtr collection, SizeT index) {
	if (index >= collection->length) {
		fprintf(stderr, "Can't remove an element that is not in a collection\n");
		exit(EXIT_FAILURE);
	}

	ptr indexPtr = getElement(collection, index);
	ptr jPtr = getElement(collection, index + 1);

	SizeT nBytes = (collection->length - (index + 1)) * collection->elementSize;

	setMemory(indexPtr, 0, collection->elementSize);

	if (nBytes > 0) {
		memcpy(indexPtr, jPtr, nBytes);
	}

	collection->length--;
}

/*
	Returns pointer to element in collection as `ptr`
*/
ptr getCollectionElement(CollectionPtr collection, SizeT index) {
	if (index >= collection->capacity) {
		return NULL;
	}

	return collection->elements + index * collection->elementSize;
}

void replaceCollectionElement(CollectionPtr collection, SizeT index, ptr newElement) {
	if (index >= collection->capacity) {
		fprintf(stderr, "Can't replace an element outside of a collection\n");
		exit(EXIT_FAILURE);
	}

	ptr indexPtr = getElement(collection, index);

	setMemory(indexPtr, 0, collection->elementSize);
	copyMemory(indexPtr, newElement, collection->elementSize);

	collectionFragmented(collection, false);
}

bool collectionContains(CollectionPtr collection, ptr refElement) {
	for (SizeT i = 0; i < collection->length; i++) {
		if (equalMemory(refElement, getElement(collection, i), collection->elementSize)) {
			return true;
		}
	}

	return false;
}

#pragma endregion

#pragma region Sorting

SizeT partition(CollectionPtr collection, SizeT lo, SizeT hi, ComparisonFunc compFunc) {
	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	SizeT n = collection->elementSize;

	ptr pivot = getCollectionElement(collection, hi);

	SizeT i = lo;

	for (SizeT j = lo; j < hi; j++) {
		ptr jPtr = getCollectionElement(collection, j);
		if ((compFunc(jPtr, pivot, n) == LESS) || (compFunc(jPtr, pivot, n) == EQUALS)) {
			swapCollectionElements(collection, i, j);
			i++;
		}
	}

	swapCollectionElements(collection, i, hi);

	return i;
}

void QuickSort(CollectionPtr collection, SizeT lo, SizeT hi, ComparisonFunc compFunc) {
	if ((lo >= hi) || (hi >= collection->length)) {
		return;
	}

	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	SizeT p = partition(collection, lo, hi, compFunc);

	QuickSort(collection, lo, p - 1, compFunc);
	QuickSort(collection, p + 1, hi, compFunc);
}

void sortCollection(CollectionPtr collection, ComparisonFunc compFunc) {
	if (collection->length < 2 || collection->capacity < 2) {
		return;
	}

	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	QuickSort(collection, 0, collection->length - 1, compFunc);
}

void reverseCollection(CollectionPtr collection) {
	if (collection->length < 2 || collection->capacity < 2) {
		return;
	}

	for (SizeT i = 0; i < collection->length/2; i++) {
		SizeT j = collection->length - i - 1;

		swapCollectionElements(collection, i, j);
	}
}

#pragma endregion