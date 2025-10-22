#include "queue.h"

void initQueue(Queue* queue, SizeT initCapacity, SizeT elementSize) {
	initializeList(asList(queue), initCapacity, elementSize);
}

void freeQueue(Queue* queue) {
	freeList(asList(queue));
}

void enqueue(Queue* queue, ptr newElement) {
	addElement(asList(queue), newElement);
}

ptr peek(Queue* queue) {
	return getElement(asList(queue), 0);
}

ptr dequeue(Queue* queue) {
	ptr temp = calloc(1, queue->elementSize);

	copyMemory(peek(queue), temp, queue->elementSize);
	removeElement(asList(queue), 0);

	return temp;
}