#include "list.h"

#include "../memory/memfuncs.h"

void initializeList(List* list, SizeT initSize, SizeT elementSize) {
	if (list->initialized) {
		freeList(list);
	}

	list->capacity = initSize;
	list->n_elements = 0;
	list->elementSize = elementSize;

	list->fragmented = false;
	list->initialized = true;

	void* ptr = (void*)calloc(initSize, elementSize);

	if (ptr != NULL) {
		list->elements = ptr;
	} else {
		fprintf(stderr, "Couldn't properly initialize the list, `calloc()` can't find enough space!\n");

		list->initialized = false;
		list->capacity = 0;
		list->elementSize = 0;

		free(ptr);
		exit(EXIT_FAILURE);
	}
}

void freeList(List* list) {
	if (!list->initialized) {
		fprintf(stderr, "Can't free a list that was never initialized!\n");
		exit(EXIT_FAILURE);
	}

	memset(list->elements, 0, list->capacity * list->elementSize);

	free(list->elements);

	list->capacity = 0;
	list->elementSize = 0;
	list->n_elements = 0;
	list->initialized = false;
}

void resizeList(List* list, SizeT newCapacity) {
	if (newCapacity < list->n_elements) {
		newCapacity = list->n_elements;
	}

	if (newCapacity <= 0) {
		freeList(list);
		fprintf(stderr, "Can't resize a list to a null or negative length (%zu elements)\n", newCapacity);
		exit(EXIT_FAILURE);
	}

	void* ptr = (void*)realloc(list->elements, newCapacity * list->elementSize);

	if (ptr != NULL) {
		//printf("new capacity : %zu elements\n", newCapacity);
		list->elements = ptr;
		list->capacity = newCapacity;
	} else {
		free(ptr);
		fprintf(stderr, "Reallocation failure in `resizeList(%p, index)`\n", (void*)list);
		exit(EXIT_FAILURE);
	}
}

void addElement(List* list, void* newElement) {
	if ((list->n_elements + 1) > list->capacity) {
		resizeList(list, list->n_elements + 50);
	}

	SizeT nBytes = list->n_elements * list->elementSize;
	memcpy((int8*)(list->elements + nBytes), newElement, list->elementSize);

	list->n_elements++;
}

void removeElement(List* list, SizeT index, bool shiftElements) {
	if (index >= list->n_elements) {
		fprintf(stderr, "Can't remove an element that is not in a list\n");
		exit(EXIT_FAILURE);
	}

	SizeT nBytes = index * list->elementSize;

	memset((int8*)list->elements + nBytes, 0, list->elementSize);

	if (shiftElements) {
		SizeT i = index;
		SizeT j = index + 1;

		SizeT nBytesI = i * list->elementSize;
		SizeT nBytesJ = j * list->elementSize;

		SizeT bytesToCopy = (list->n_elements - j) * list->elementSize;

		memmove((int8*)list->elements + nBytesI, (int8*)list->elements + nBytesJ, bytesToCopy);

		list->n_elements--;
	} else {
		list->fragmented |= true;
	}
}

/*
	Returns pointer to element in list as `void*`
*/
void* getElement(List* list, SizeT index) {
	if (index >= list->capacity) {
		return NULL;
	}

	return list->elements + index * list->elementSize;
}

void replaceElement(List* list, SizeT index, void* newElement) {
	if (index >= list->capacity) {
		fprintf(stderr, "Can't replace an element outside of a list\n");
		exit(EXIT_FAILURE);
	}

	SizeT nBytes = index * list->elementSize;

	removeElement(list, index, false);
	memcpy((int8*)list->elements + nBytes, newElement, list->elementSize);

	list->fragmented = false;
}

bool contains(List* list, void* refElement) {
	for (SizeT i = 0; i < list->n_elements; i++) {
		if (equalMemory(refElement, getElement(list, i), list->elementSize)) {
			return true;
		}
	}

	return false;
}

void swapElements(List* list, SizeT i, SizeT j) {
	uint8 temp;

	uint8* a = (uint8*)getElement(list, i);
	uint8* b = (uint8*)getElement(list, j);

	for (SizeT k = 0; k < list->elementSize; k++) {
		temp = a[k];
		a[k] = b[k];
		b[k] = temp;
	}
}

SizeT shrinkToFit(List* list) {
	if (list->fragmented) {
		fprintf(stderr, "Can't shrink a list if it is fragmented\n");
		exit(EXIT_FAILURE);
	}

	resizeList(list, 0);

	return list->capacity;
}

void copyList(List* listDest, List* listSrc) {
	memcpy(listDest, listSrc, sizeof(*listSrc));

	void* ptr = calloc(listDest->capacity, listDest->elementSize);

	if (ptr != NULL) {
		listDest->elements = ptr;
	} else {

		free(ptr);
		exit(EXIT_FAILURE);
	}

	memcpy(listDest->elements, listSrc->elements, listSrc->capacity * listSrc->elementSize);
}

SizeT partition(List* list, SizeT lo, SizeT hi) {
	SizeT n = list->elementSize;

	void* pivot = getElement(list, hi);

	SizeT i = lo;

	for (SizeT j = lo; j < hi; j++) {
		void* jPtr = getElement(list, j);
		if (lessThanMemory(jPtr, pivot, n) || equalMemory(jPtr, pivot, n)) {
			swapElements(list, i, j);
			i++;
		}
	}

	swapElements(list, i, hi);
	return i;
}

void QuickSort(List* list, SizeT lo, SizeT hi) {
	if ((lo >= hi) || (hi >= list->n_elements)) {
		return;
	}

	SizeT p = partition(list, lo, hi);

	QuickSort(list, lo, p - 1);
	QuickSort(list, p + 1, hi);
}

void sortList(List* list) {
	if (list->n_elements < 2 || list->capacity < 2) {
		return;
	}

	QuickSort(list, 0, list->n_elements - 1);
}