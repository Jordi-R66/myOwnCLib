#include "queue.h"

void initQueue(Queue* queue, SizeT initCapacity, SizeT elementSize) {
	initializeCollection(&queue->collection, initCapacity, elementSize);
}

void freeQueue(Queue* queue) {
	freeCollection(&queue->collection);
}

void enqueue(Queue* queue, ptr newElement) {
	addCollectionElement(&queue->collection, newElement);
}

ptr peek(Queue* queue) {
	return getCollectionElement(&queue->collection, 0);
}

ptr dequeue(Queue* queue) {
	ptr temp = calloc(1, queue->collection.elementSize);

	copyMemory(peek(queue), temp, queue->collection.elementSize);
	removeCollectionElement(&queue->collection, 0);

	return temp;
}