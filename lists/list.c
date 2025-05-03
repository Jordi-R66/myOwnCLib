#include "list.h"

void initializeList(List* list, size_t initSize, size_t elementSize) {
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

void resizeList(List* list, size_t newCapacity) {
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
	if ((list->n_elements + 1) >= list->capacity) {
		resizeList(list, list->n_elements + 50);
	}

	size_t nBytes = list->n_elements * list->elementSize;
	memcpy((int8_t*)(list->elements + nBytes), newElement, list->elementSize);

	list->n_elements++;
}

void removeElement(List* list, size_t index, bool shiftElements) {
	if (index >= list->n_elements) {
		fprintf(stderr, "Can't remove an element that is not in a list\n");
		exit(EXIT_FAILURE);
	}

	size_t nBytes = index * list->elementSize;

	memset((int8_t*)list->elements + nBytes, 0, list->elementSize);

	if (shiftElements) {
		size_t i = index;
		size_t j = index + 1;

		size_t nBytesI = i * list->elementSize;
		size_t nBytesJ = j * list->elementSize;

		size_t bytesToCopy = (list->n_elements - j) * list->elementSize;

		memmove((int8_t*)list->elements + nBytesI, (int8_t*)list->elements + nBytesJ, bytesToCopy);

		list->n_elements--;
	} else {
		list->fragmented |= true;
	}
}

/*
	Returns pointer to element in list as `void*`
*/
void* getElement(List* list, size_t index) {
	if (index >= list->capacity) {
		return NULL;
	}

	return list->elements + index * list->elementSize;
}

void replaceElement(List* list, size_t index, void* newElement) {
	if (index >= list->capacity) {
		fprintf(stderr, "Can't replace an element outside of a list\n");
		exit(EXIT_FAILURE);
	}

	size_t nBytes = index * list->elementSize;

	removeElement(list, index, false);
	memcpy((int8_t*)list->elements + nBytes, newElement, list->elementSize);

	list->fragmented = false;
}

bool contains(List* list, void* Element) {
	for (size_t i = 0; i < list->n_elements; i++) {
		if (memcmp(Element, getElement(list, i), list->elementSize)) {
			return true;
		}
	}

	return false;
}

size_t shrinkToFit(List* list) {
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