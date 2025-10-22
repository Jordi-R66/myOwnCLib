#include "binaryHeap.h"

const BinaryHeap NULL_BINHEAP = {
	.collection = {
		.capacity = 0,
		.n_elements = 0,
		.elementSize = 0,
		.flags = NO_FLAGS,
		.elements = NULL},
	.rule = 0
};

bool isBinHeapFragmented(BinHeapPtr binHeap) {
	return isCollectionFragmented(&binHeap->collection);
}

bool isBinHeapInitialised(BinHeapPtr binHeap) {
	return isCollectionInitialised(&binHeap->collection);
}

CollectionFlag binHeapFragmented(BinHeapPtr binHeap, bool val) {
	return collectionFragmented(&binHeap->collection, val);
}

CollectionFlag binHeapInitialised(BinHeapPtr binHeap, bool val) {
	return collectionInitialised(&binHeap->collection, val);
}

void initBinaryHeap(BinHeapPtr binHeap, SizeT initCapacity, SizeT elementSize, BinHeapRule rule, ComparisonFunc compFunc) {
	if (!((MIN_HEAP <= rule) && (rule <= MAX_HEAP))) {
		return;
	}

	if (compFunc == NULL) {
		compFunc = compareMemory;
	}

	initializeCollection(&binHeap->collection, initCapacity, elementSize);
	binHeap->rule = rule;
}

void freeBinaryHeap(BinHeapPtr binHeap);
