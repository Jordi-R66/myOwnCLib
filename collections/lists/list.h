#pragma once

#ifndef LISTS
#define LISTS

#include "../../common.h"
#include "../../memory/memfuncs.h"

#pragma pack(1)
typedef struct List {
	SizeT capacity; // List capacity in number of elements
	SizeT n_elements; // Current number of elements
	SizeT elementSize; // Size of elements in bytes
	bool fragmented; // Is the list fragmented?
	bool initialized; // Is the list initialized?

	ptr elements; // The actual array
} List;
#pragma pack()

#define LIST_SIZE sizeof(List)
#define asList(PTR) ((List*)PTR)

void initializeList(List* list, SizeT initSize, SizeT elementSize);
void freeList(List* list);

void resizeList(List* list, SizeT newCapacity);

void addElement(List* list, ptr newElement);
void removeElement(List* list, SizeT index, bool shiftElements);
ptr getElement(List* list, SizeT index);
void replaceElement(List* list, SizeT index, ptr newElement);
bool contains(List* list, ptr refElement);

void swapListElements(List* list, SizeT i, SizeT j);

SizeT shrinkToFit(List* list);
void copyList(List* listDest, List* listSrc);

/**
 * @brief Sorts the given list using the given comparison function
 * 
 * @param list The pointer of the list to sort
 * @param compFunc the function to use to compare (set to NULL to use default)
 */
void sortList(List* list, ComparisonFunc compFunc);
void reverseList(List* list);
#endif