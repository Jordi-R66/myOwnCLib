#include "list.h"

SizeT foundAtPosition = 0;

#pragma pack(1)
struct List {
	Collection collection;
};

#pragma pack()

const List NULL_LIST = {
	.collection = {
		.capacity = 0,
		.length = 0,
		.elementSize = 0,
		.flags = NO_FLAGS,
		.elements = NULL
	}
};

#define LIST_SIZE sizeof(List)

ListPtr generateNullList() {
	ListPtr output = calloc(1, LIST_SIZE);

	copyMemory(&NULL_LIST, output, LIST_SIZE);

	return output;
}

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

ListPtr initializeList(SizeT initSize, SizeT elementSize) {
	ListPtr output = NULL;

	output = (ListPtr)calloc(1, LIST_SIZE);

	if (output != NULL)
		initializeCollection(&output->collection, initSize, elementSize);

	return output;
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

ListPtr sliceList(ListPtr list, SizeT start, SizeT end) {
	ListPtr output = &NULL_LIST;

	if ((start >= end) || (start >= list->collection.length)) {
		output = &NULL_LIST;
	} else if (end >= list->collection.length) {
		output = sliceList(list, start, list->collection.length-1);
	} else {
		SizeT capacity = end - start + 1;

		output = initializeList(capacity, list->collection.elementSize);

		for (SizeT i = 0; i < capacity; i++) {
			replaceListElement(&output, i, getElement(list, start + i));
		}
	}

	return output;
}