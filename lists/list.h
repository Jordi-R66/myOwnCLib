#pragma once

#include "commons.h"

struct List {
	size_t capacity; // List capacity in number of elements
	size_t n_elements; // Actual number of elements
	size_t elementSize; // Size of elements in bytes
	bool fragmented; // Is the list fragmented
	bool initialized; // Is the list initialized

	void* elements; // The actual array
};

typedef struct List List;

void initializeList(List* list, size_t initSize, size_t elementSize);
void freeList(List* list);

void resizeList(List* list, size_t newCapacity);

void addElement(List* list, void* newElement);
void removeElement(List* list, size_t index, bool shiftElements);
void* getElement(List* list, size_t index);
void replaceElement(List* list, size_t index, void* newElement);

size_t shrinkToFit(List* list);
void copyList(List* listDest, List* listSrc);