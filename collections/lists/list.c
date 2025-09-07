#include "list.h"

SizeT foundAtPosition = 0;

const List NULL_LIST = {
	.capacity = 0,
	.n_elements = 0,
	.elementSize = 0,
	.flags = NO_FLAGS,
	.elements = NULL
};

bool isListFragmented(ListPtr list) {
	return (list->flags & LIST_FRAGMENTED) == LIST_FRAGMENTED;
}

bool isListInitialised(ListPtr list) {
	return (list->flags & LIST_INITIALISED) == LIST_INITIALISED;
}

uint8 listFragmented(ListPtr list, bool val) {
	uint8 flags = list->flags;

	if (val) {
		flags = flags | LIST_FRAGMENTED;
	} else {
		flags = flags & ~LIST_FRAGMENTED;
	}

	list->flags = flags;

	return flags;
}


uint8 listInitialised(ListPtr list, bool val) {
	uint8 flags = list->flags;

	if (val) {
		flags = flags | LIST_INITIALISED;
	} else {
		flags = flags & ~LIST_INITIALISED;
	}

	list->flags = flags;

	return flags;
}

void initializeList(ListPtr list, SizeT initSize, SizeT elementSize) {
	if (isListInitialised(list)) {
		freeList(list);
	}

	list->capacity = initSize;
	list->n_elements = 0;
	list->elementSize = elementSize;

	listInitialised(list, true);
	listFragmented(list, false);

	ptr tempPtr = (ptr)calloc(initSize, elementSize);

	if (tempPtr != NULL) {
		list->elements = tempPtr;
	} else {
		fprintf(stderr, "Couldn't properly initialize the list, `calloc()` can't find enough space!\n");

		listInitialised(list, false);
		list->capacity = 0;
		list->elementSize = 0;

		free(tempPtr);
		exit(EXIT_FAILURE);
	}
}

void freeList(ListPtr list) {
	if (!isListInitialised(list)) {
		fprintf(stderr, "Can't free a list that was never initialized!\n");
		exit(EXIT_FAILURE);
	}

	setMemory(list->elements, 0, list->capacity * list->elementSize);

	free(list->elements);

	list->capacity = 0;
	list->elementSize = 0;
	list->n_elements = 0;
	listFragmented(list, false);
	listInitialised(list, false);
}

void resizeList(ListPtr list, SizeT newCapacity) {
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

void addElement(ListPtr list, ptr newElement) {
	if ((list->n_elements + 1) > list->capacity) {
		resizeList(list, list->n_elements + 50);
	}

	SizeT nBytes = list->n_elements * list->elementSize;
	copyMemory((ptr)(list->elements + nBytes), newElement, list->elementSize);

	list->n_elements++;
}

void removeElement(ListPtr list, SizeT index) {
	if (index >= list->n_elements) {
		fprintf(stderr, "Can't remove an element that is not in a list\n");
		exit(EXIT_FAILURE);
	}

	ptr indexPtr = getElement(list, index);
	ptr jPtr = getElement(list, index + 1);

	SizeT nBytes = (list->n_elements - (index + 1)) * list->elementSize;

	setMemory(indexPtr, 0, list->elementSize);

	if (nBytes > 0) {
		memcpy(indexPtr, jPtr, nBytes);
	}

	list->n_elements--;
}

/*
	Returns pointer to element in list as `ptr`
*/
ptr getElement(ListPtr list, SizeT index) {
	if (index >= list->capacity) {
		return NULL;
	}

	return list->elements + index * list->elementSize;
}

void replaceElement(ListPtr list, SizeT index, ptr newElement) {
	if (index >= list->capacity) {
		fprintf(stderr, "Can't replace an element outside of a list\n");
		exit(EXIT_FAILURE);
	}

	ptr indexPtr = getElement(list, index);

	setMemory(indexPtr, 0, list->elementSize);
	copyMemory(indexPtr, newElement, list->elementSize);

	listFragmented(list, false);
}

bool contains(ListPtr list, ptr refElement) {
	for (SizeT i = 0; i < list->n_elements; i++) {
		if (equalMemory(refElement, getElement(list, i), list->elementSize)) {
			foundAtPosition = i;
			return true;
		}
	}

	return false;
}

void swapListElements(ListPtr list, SizeT i, SizeT j) {
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

SizeT shrinkToFit(ListPtr list) {
	resizeList(list, 0);

	return list->capacity;
}

void copyList(ListPtr listDest, ListPtr listSrc) {
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

SizeT partition(ListPtr list, SizeT lo, SizeT hi, ComparisonFunc compFunc) {
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

void QuickSort(ListPtr list, SizeT lo, SizeT hi, ComparisonFunc compFunc) {
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

void sortList(ListPtr list, ComparisonFunc compFunc) {
	if (list->n_elements < 2 || list->capacity < 2) {
		return;
	}

	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	QuickSort(list, 0, list->n_elements - 1, compFunc);
}

void reverseList(ListPtr list) {
	if (list->n_elements < 2 || list->capacity < 2) {
		return;
	}

	for (SizeT i = 0; i < list->n_elements/2; i++) {
		SizeT j = list->n_elements - i - 1;

		swapListElements(list, i, j);
	}
}

List sliceList(ListPtr list, SizeT start, SizeT end) {
	List output = NULL_LIST;

	if ((start >= end) || (start >= list->n_elements)) {
		return output;
	}

	if (end >= list->n_elements) {
		return sliceList(list, start, list->n_elements-1);
	}

	SizeT capacity = end - start + 1;

	initializeList(&output, capacity, list->elementSize);

	for (SizeT i = 0; i < capacity; i++) {
		replaceElement(&output, i, getElement(list, start + i));
	}

	return output;
}