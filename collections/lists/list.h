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
	uint8 flags; // Refer to the LIST_FLAGS region

	ptr elements; // The actual array
} List, *ListPtr;
#pragma pack()

#pragma region LIST_FLAGS

#define NO_FLAGS			(uint8)0b00000000
#define LIST_INITIALISED	(uint8)0b00000001
#define LIST_FRAGMENTED		(uint8)0b00000010

#pragma endregion

#pragma region public variables

extern SizeT foundAtPosition;

#pragma endregion

#pragma region LIST_FLAGS_GETTER
bool isListFragmented(ListPtr list); //(((ListPtr)(ListPtr)->flags & LIST_FRAGMENTED) == LIST_FRAGMENTED)
bool isListInitialised(ListPtr list); //(((ListPtr)(ListPtr)->flags & LIST_INITIALISED) == LIST_INITIALISED)
#pragma endregion

#pragma region LIST_FLAGS_SETTER
uint8 listFragmented(ListPtr list, bool val); //(ListPtr)(ListPtr)->flags = boolean ? ((ListPtr)(ListPtr)->flags | LIST_FRAGMENTED) : ((ListPtr)(ListPtr)->flags & ~LIST_FRAGMENTED)
uint8 listInitialised(ListPtr list, bool val); //(ListPtr)(ListPtr)->flags = boolean ? ((ListPtr)(ListPtr)->flags | LIST_INITIALISED) : ((ListPtr)(ListPtr)->flags & ~LIST_INITIALISED)
#pragma endregion

#define LIST_SIZE sizeof(List)
#define asList(PTR) ((ListPtr)PTR)

void initializeList(ListPtr list, SizeT initSize, SizeT elementSize);
void freeList(ListPtr list);

void resizeList(ListPtr list, SizeT newCapacity);

void addElement(ListPtr list, ptr newElement);
void removeElement(ListPtr list, SizeT index);
ptr getElement(ListPtr list, SizeT index);
void replaceElement(ListPtr list, SizeT index, ptr newElement);
bool contains(ListPtr list, ptr refElement);

void swapListElements(ListPtr list, SizeT i, SizeT j);

SizeT shrinkToFit(ListPtr list);
void copyList(ListPtr listDest, ListPtr listSrc);

/**
 * @brief Sorts the given list using the given comparison function
 * 
 * @param list The pointer of the list to sort
 * @param compFunc the function to use to compare (set to NULL to use default)
 */
void sortList(ListPtr list, ComparisonFunc compFunc);
void reverseList(ListPtr list);

/**
 * @brief Returns a slice of the given list of (end - start + 1) elements
 * 
 * @param list 
 * @param start 
 * @param end 
 * @return List 
 */
List sliceList(ListPtr list, SizeT start, SizeT end);
#endif