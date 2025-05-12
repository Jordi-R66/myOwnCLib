#pragma once

#include "../common.h"
#include "../targetDetection/types.h"
#include "../memory/memfuncs.h"

#pragma pack(1)
typedef struct List {
	SizeT capacity; // List capacity in number of elements
	SizeT n_elements; // Current number of elements
	SizeT elementSize; // Size of elements in bytes
	bool fragmented; // Is the list fragmented?
	bool initialized; // Is the list initialized?

	void* elements; // The actual array
} List;
#pragma pack()

#define LIST_SIZE sizeof(List)

void initializeList(List* list, SizeT initSize, SizeT elementSize);
void freeList(List* list);

void resizeList(List* list, SizeT newCapacity);

void addElement(List* list, void* newElement);
void removeElement(List* list, SizeT index, bool shiftElements);
void* getElement(List* list, SizeT index);
void replaceElement(List* list, SizeT index, void* newElement);
bool contains(List* list, void* refElement);

void swapElements(List* list, SizeT i, SizeT j);

SizeT shrinkToFit(List* list);
void copyList(List* listDest, List* listSrc);

/**
 * @brief Sorts the given list using the given comparison function
 * 
 * @param list The pointer of the list to sort
 * @param compFunc the function to use to compare (set to NULL to use default)
 */
void sortList(List* list, Comparison (*compFunc)(void*, void*, SizeT));
void reverseList(List* list);