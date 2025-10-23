#include "binaryHeap.h"

const BinaryHeap NULL_BINHEAP = {
	.collection = {
		.capacity = 0,
		.n_elements = 0,
		.elementSize = 0,
		.flags = NO_FLAGS,
		.elements = NULL},
	.rule = 0,
	.compFunc = NULL
};

bool violatesHeapIntegrity(BinHeapPtr binHeap, SizeT i, SizeT j) {
	ptr a, b;
	bool output = false;

	if ((i < binHeap->collection.n_elements) && (j < binHeap->collection.n_elements) && (i == PARENT_BUBBLE(j))) {
		a = getCollectionElement(&binHeap->collection, i);
		b = getCollectionElement(&binHeap->collection, j);

		Comparison comp = binHeap->compFunc(a, b, binHeap->collection.elementSize);

		switch (binHeap->rule) {
			case MIN_HEAP:
				// VIOLATES INTEGRITY IF PARENT > CHILDREN
				output = comp == GREATER;
				break;

			case MAX_HEAP:
				// VIOLATES INTEGRITY IF PARENT < CHILDREN
				output = comp == LESS;
				break;

			default:
				output = true;
				break;
		}
	}

	return output;
}

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

void freeBinaryHeap(BinHeapPtr binHeap) {
	if (isBinHeapInitialised(binHeap)) {
		freeCollection(&binHeap->collection);
	}

	setMemory(binHeap, 0, BINHEAP_SIZE);
}

void insert(Bin);