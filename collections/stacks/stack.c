#include "stack.h"

void initStack(Stack* stack, SizeT initCapacity, SizeT elementSize) {
	initializeList(asList(stack), initCapacity, elementSize);
}

void freeStack(Stack* stack) {
	freeList(asList(stack));
}

void push(Stack* stack, ptr newElement) {
	addElement(asList(stack), newElement);
}

ptr peek(Stack* stack) {
	return getElement(asList(stack), stack->n_elements - 1);
}

ptr pop(Stack* stack) {
	ptr temp = calloc(1, stack->elementSize);

	copyMemory(peek(stack), temp, stack->elementSize);
	removeElement(asList(stack), stack->n_elements - 1, false);

	return temp;
}