#pragma once

#ifndef LISTS
#define LISTS

#include "../collection.h"

#pragma pack(1)
typedef struct List {
	Collection collection;
} List, *ListPtr;
#pragma pack()

#define LIST_SIZE sizeof(List)

#pragma region public variables

extern SizeT foundAtPosition;

#pragma endregion

#pragma region LIST_FLAGS_GETTER
bool isListFragmented(ListPtr list); //(((ListPtr)(ListPtr)->flags & COLLECTION_FRAGMENTED) == COLLECTION_FRAGMENTED)
bool isListInitialised(ListPtr list); //(((ListPtr)(ListPtr)->flags & COLLECTION_INITIALISED) == COLLECTION_INITIALISED)
#pragma endregion

#pragma region LIST_FLAGS_SETTER
CollectionFlag listFragmented(ListPtr list, bool val); //(ListPtr)(ListPtr)->flags = boolean ? ((ListPtr)(ListPtr)->flags | COLLECTION_FRAGMENTED) : ((ListPtr)(ListPtr)->flags & ~COLLECTION_FRAGMENTED)
CollectionFlag listInitialised(ListPtr list, bool val); //(ListPtr)(ListPtr)->flags = boolean ? ((ListPtr)(ListPtr)->flags | COLLECTION_INITIALISED) : ((ListPtr)(ListPtr)->flags & ~COLLECTION_INITIALISED)
#pragma endregion

#define LIST_SIZE sizeof(List)
#define asList(PTR) ((ListPtr)PTR)

void initializeList(ListPtr list, SizeT initSize, SizeT elementSize);
void freeList(ListPtr list);

void resizeList(ListPtr list, SizeT newCapacity);

void addListElement(ListPtr list, ptr newElement);
void removeListElement(ListPtr list, SizeT index);
ptr getListElement(ListPtr list, SizeT index);
void replaceListElement(ListPtr list, SizeT index, ptr newElement);
bool listContains(ListPtr list, ptr refElement);

void swapListElements(ListPtr list, SizeT i, SizeT j);

SizeT shrinkListToFit(ListPtr list);
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
