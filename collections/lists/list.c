#include "list.h"

SizeT foundAtPosition = 0;

const List NULL_LIST = {
	.collection = {
		.capacity = 0,
		.n_elements = 0,
		.elementSize = 0,
		.flags = NO_FLAGS,
		.elements = NULL
	}
};

bool isListFragmented(ListPtr list) {
	return isCollectionFragmented(&list->collection);
}

bool isListInitialised(ListPtr list) {
	return isCollectionInitialised(&list->collection);
}

CollectionFlag listFragmented(ListPtr list, bool val) {
	return collectionFragmented(&list->collection, val);
}

CollectionFlag listInitialised(ListPtr list, bool val) {
	return collectionInitialised(&list->collection, val);
}

void initializeList(ListPtr list, SizeT initSize, SizeT elementSize) {
	initializeCollection(&list->collection, initSize, elementSize);
}

void freeList(ListPtr list) {
	freeCollection(&list->collection);
}

void resizeList(ListPtr list, SizeT newCapacity) {
	resizeCollection(&list->collection, newCapacity);
}

void addListElement(ListPtr list, ptr newElement) {
	addCollectionElement(&list->collection, newElement);
}

void removeListElement(ListPtr list, SizeT index) {
	removeCollectionElement(&list->collection, index);
}

/*
	Returns pointer to element in list as `ptr`
*/
ptr getListElement(ListPtr list, SizeT index) {
	return getCollectionElement(&list->collection, index);
}

void replaceListElement(ListPtr list, SizeT index, ptr newElement) {
	replaceCollectionElement(&list->collection, index, newElement);
}

bool listContains(ListPtr list, ptr refElement) {
	return collectionContains(&list->collection, refElement);
}

void swapListElements(ListPtr list, SizeT i, SizeT j) {
	swapCollectionElements(&list->collection, i, j);
}

SizeT shrinkListToFit(ListPtr list) {
	return shrinkCollectionToFit(&list->collection);
}

void copyList(ListPtr listDest, ListPtr listSrc) {
	copyCollection(&listDest->collection, &listSrc->collection);
}

void sortList(ListPtr list, ComparisonFunc compFunc) {
	sortCollection(&list->collection, compFunc);
}

void reverseList(ListPtr list) {
	reverseCollection(&list->collection);
}

List sliceList(ListPtr list, SizeT start, SizeT end) {
	List output = NULL_LIST;

	if ((start >= end) || (start >= list->collection.n_elements)) {
		return output;
	}

	if (end >= list->collection.n_elements) {
		return sliceList(list, start, list->collection.n_elements-1);
	}

	SizeT capacity = end - start + 1;

	initializeList(&output, capacity, list->collection.elementSize);

	for (SizeT i = 0; i < capacity; i++) {
		replaceListElement(&output, i, getElement(list, start + i));
	}

	return output;
}