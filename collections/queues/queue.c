#include "queue.h"

void initQueue(QueuePTR queue, SizeT initCapacity, SizeT elementSize) {
	initializeCollection(&queue->collection, initCapacity, elementSize);
}

void freeQueue(QueuePTR queue) {
	freeCollection(&queue->collection);
}

void enqueue(QueuePTR queue, ptr newElement) {
	addCollectionElement(&queue->collection, newElement);
}

ptr peek(QueuePTR queue) {
	return getCollectionElement(&queue->collection, 0);
}

ptr dequeue(QueuePTR queue) {
	ptr temp = calloc(1, queue->collection.elementSize);

	copyMemory(peek(queue), temp, queue->collection.elementSize);
	removeCollectionElement(&queue->collection, 0);

	return temp;
}