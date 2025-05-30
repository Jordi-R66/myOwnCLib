#include "list.h"

void initializeList(List* list, SizeT initSize, SizeT elementSize) {
	if (list->initialized) {
		freeList(list);
	}

	list->capacity = initSize;
	list->n_elements = 0;
	list->elementSize = elementSize;

	list->fragmented = false;
	list->initialized = true;

	ptr tempPtr = (ptr)calloc(initSize, elementSize);

	if (tempPtr != NULL) {
		list->elements = tempPtr;
	} else {
		fprintf(stderr, "Couldn't properly initialize the list, `calloc()` can't find enough space!\n");

		list->initialized = false;
		list->capacity = 0;
		list->elementSize = 0;

		free(tempPtr);
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

	ptr tempPtr = (ptr)realloc(list->elements, newCapacity * list->elementSize);

	if (tempPtr != NULL) {
		//printf("new capacity : %zu elements\n", newCapacity);
		list->elements = tempPtr;
		list->capacity = newCapacity;
	} else {
		free(tempPtr);
		fprintf(stderr, "Reallocation failure in `resizeList(%p, index)`\n", (ptr)list);
		exit(EXIT_FAILURE);
	}
}

void addElement(List* list, ptr newElement) {
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
	Returns pointer to element in list as `ptr`
*/
ptr getElement(List* list, SizeT index) {
	if (index >= list->capacity) {
		return NULL;
	}

	return list->elements + index * list->elementSize;
}

void replaceElement(List* list, SizeT index, ptr newElement) {
	if (index >= list->capacity) {
		fprintf(stderr, "Can't replace an element outside of a list\n");
		exit(EXIT_FAILURE);
	}

	SizeT nBytes = index * list->elementSize;

	removeElement(list, index, false);
	memcpy((int8*)list->elements + nBytes, newElement, list->elementSize);

	list->fragmented = false;
}

bool contains(List* list, ptr refElement) {
	for (SizeT i = 0; i < list->n_elements; i++) {
		if (equalMemory(refElement, getElement(list, i), list->elementSize)) {
			return true;
		}
	}

	return false;
}

void swapListElements(List* list, SizeT i, SizeT j) {
	uint8 temp;

	if (i == j) {
		return;
	}

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

	ptr ptr = calloc(listDest->capacity, listDest->elementSize);

	if (ptr != NULL) {
		listDest->elements = ptr;
	} else {

		free(ptr);
		exit(EXIT_FAILURE);
	}

	memcpy(listDest->elements, listSrc->elements, listSrc->capacity * listSrc->elementSize);
}

SizeT partition(List* list, SizeT lo, SizeT hi, ComparisonFunc compFunc) {
	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	SizeT n = list->elementSize;

	ptr pivot = getElement(list, hi);

	SizeT i = lo;

	for (SizeT j = lo; j < hi; j++) {
		ptr jPtr = getElement(list, j);
		if (compFunc(jPtr, pivot, n) == LESS || compFunc(jPtr, pivot, n) == EQUALS) {
			swapListElements(list, i, j);
			i++;
		}
	}

	swapListElements(list, i, hi);

	return i;
}

void QuickSort(List* list, SizeT lo, SizeT hi, ComparisonFunc compFunc) {
	if ((lo >= hi) || (hi >= list->n_elements)) {
		return;
	}

	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	SizeT p = partition(list, lo, hi, compFunc);

	QuickSort(list, lo, p - 1, compFunc);
	QuickSort(list, p + 1, hi, compFunc);
}

void sortList(List* list, ComparisonFunc compFunc) {
	if (list->n_elements < 2 || list->capacity < 2) {
		return;
	}

	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	QuickSort(list, 0, list->n_elements - 1, compFunc);
}

void reverseList(List* list) {
	if (list->n_elements < 2 || list->capacity < 2) {
		return;
	}

	for (SizeT i = 0; i < list->n_elements/2; i++) {
		SizeT j = list->n_elements - i - 1;

		swapListElements(list, i, j);
	}
}
