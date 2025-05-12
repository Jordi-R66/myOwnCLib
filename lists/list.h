#pragma once

#include "../common.h"
#include "../targetDetection/types.h"

struct List {
	SizeT capacity; // List capacity in number of elements
	SizeT n_elements; // Actual number of elements
	SizeT elementSize; // Size of elements in bytes
	bool fragmented; // Is the list fragmented
	bool initialized; // Is the list initialized

	void* elements; // The actual array
};

typedef struct List List;

void initializeList(List* list, SizeT initSize, SizeT elementSize);
void freeList(List* list);

void resizeList(List* list, SizeT newCapacity);

void addElement(List* list, void* newElement);
void removeElement(List* list, SizeT index, bool shiftElements);
void* getElement(List* list, SizeT index);
void replaceElement(List* list, SizeT index, void* newElement);
bool contains(List* list, void* Element);

void swapElements(List* list, SizeT i, SizeT j);

SizeT shrinkToFit(List* list);
void copyList(List* listDest, List* listSrc);

void sortList(List* list, Comparison (*compFunc)(void*, void*, SizeT));
void reverseList(List* list);