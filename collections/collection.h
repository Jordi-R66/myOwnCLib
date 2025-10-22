#pragma once

#ifndef COLLECTIONS

#include "../../common.h"
#include "../../memory/memfuncs.h"
#include "../targetSpecific/types.h"

#pragma region COLLECTION_FLAGS
typedef uint8 CollectionFlag;

#define NO_FLAGS				(CollectionFlag)0b00000000
#define COLLECTION_INITIALISED	(CollectionFlag)0b00000001
#define COLLECTION_FRAGMENTED	(CollectionFlag)0b00000010
#pragma endregion

#pragma pack(1)
typedef struct Collection {
	SizeT capacity; // Collection capacity in number of elements
	SizeT n_elements; // Current number of elements
	SizeT elementSize; // Size of elements in bytes
	CollectionFlag flags; // Refer to the COLLECTION_FLAGS region

	ptr elements; // The actual array
} Collection, *CollectionPtr;
#pragma pack()

extern const Collection NULL_COLLECTION;

#define COLLECTION_SIZE sizeof(Collection)

#pragma region COLLECTION_FLAGS_GETTER
bool isCollectionFragmented(CollectionPtr collection); //(((CollectionPtr)(CollectionPtr)->flags & COLLECTION_FRAGMENTED) == COLLECTION_FRAGMENTED)
bool isCollectionInitialised(CollectionPtr collection); //(((CollectionPtr)(CollectionPtr)->flags & COLLECTION_INITIALISED) == COLLECTION_INITIALISED)
#pragma endregion

#pragma region COLLECTION_FLAGS_SETTER
CollectionFlag collectionFragmented(CollectionPtr collection, bool val); //(CollectionPtr)(CollectionPtr)->flags = boolean ? ((CollectionPtr)(CollectionPtr)->flags | COLLECTION_FRAGMENTED) : ((CollectionPtr)(CollectionPtr)->flags & ~COLLECTION_FRAGMENTED)
CollectionFlag collectionInitialised(CollectionPtr collection, bool val); //(CollectionPtr)(CollectionPtr)->flags = boolean ? ((CollectionPtr)(CollectionPtr)->flags | COLLECTION_INITIALISED) : ((CollectionPtr)(CollectionPtr)->flags & ~COLLECTION_INITIALISED)
#pragma endregion

void initializeCollection(CollectionPtr collection, SizeT initCapacity, SizeT elementSize);
void freeCollection(CollectionPtr collection);
void resizeCollection(CollectionPtr collection, SizeT newCapacity);
SizeT shrinkTshrinkCollectionToFitoFit(CollectionPtr collection);

void addCollectionElement(CollectionPtr collection, ptr newElement);
void removeCollectionElement(CollectionPtr collection, SizeT index);
ptr getCollectionElement(CollectionPtr collection, SizeT index);
void replaceCollectionElement(CollectionPtr collection, SizeT index, ptr newElement);
bool collectionContains(CollectionPtr collection, ptr refElement);

void swapCollectionElements(CollectionPtr collection, SizeT i, SizeT j);

void copyCollection(CollectionPtr collectionDest, CollectionPtr collectionSrc);

/**
 * @brief Sorts the given collection using the given comparison function
 * 
 * @param collection The pointer of the collection to sort
 * @param compFunc the function to use to compare (set to NULL to use default)
 */
void sortCollection(CollectionPtr collection, ComparisonFunc compFunc);
void reverseCollection(CollectionPtr collection);

#define COLLECTIONS
#endif
