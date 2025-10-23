#pragma once

#ifndef BIN_HEAP

#include "../collection.h"

#define LEFT_BUBBLE(i)		(2 * i + 1)
#define RIGHT_BUBBLE(i)		(2 * i + 2)
#define PARENT_BUBBLE(i)	((i - 1) / 2)

#pragma pack(1)

typedef uint8 BinHeapRule;

#define MIN_HEAP (BinHeapRule)1
#define MAX_HEAP (BinHeapRule)2

typedef struct BinHeap {
	BinHeapRule rule;
	ComparisonFunc compFunc;
	Collection collection;
} BinaryHeap, *BinHeapPtr;

#pragma pack()

#define BINHEAP_SIZE sizeof(BinaryHeap)

/**
 * @brief 
 * 
 * @param binHeap 
 * @param i the parent's index
 * @param j the child's index
 * @return true 
 * @return false 
 */
bool violatesHeapIntegrity(BinHeapPtr binHeap, SizeT i, SizeT j);

bool isBinHeapFragmented(BinHeapPtr binHeap);
bool isBinHeapInitialised(BinHeapPtr binHeap);
CollectionFlag binHeapFragmented(BinHeapPtr binHeap, bool val);
CollectionFlag binHeapInitialised(BinHeapPtr binHeap, bool val);

void initBinaryHeap(BinHeapPtr binHeap, SizeT initCapacity, SizeT elementSize, BinHeapRule rule, ComparisonFunc compFunc);
void freeBinaryHeap(BinHeapPtr binHeap);

void insert(BinHeapPtr binHeap, ptr newElement);
ptr extract(BinHeapPtr binHeap);

#define BIN_HEAP
#endif
