#include "stack.h"

void initStack(StackPTR stack, SizeT initCapacity, SizeT elementSize) {
	initializeCollection(&stack->collection, initCapacity, elementSize);
}

void freeStack(StackPTR stack) {
	freeCollection(&stack->collection);
}

void push(StackPTR stack, ptr newElement) {
	addCollectionElement(&stack->collection, newElement);
}

ptr peek(StackPTR stack) {
	return getCollectionElement(&stack->collection, stack->collection.length - 1);
}

ptr pop(StackPTR stack) {
	ptr temp = calloc(1, stack->collection.elementSize);

	copyMemory(peek(stack), temp, stack->collection.elementSize);
	removeCollectionElement(&stack->collection, stack->collection.length - 1);

	return temp;
}