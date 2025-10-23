#pragma once

/*
	What is a queue?

	A queue is a data structure used to store data using the "first-come,
	first-served" logic, like a queue in a shop at the cashier
*/

#ifndef QUEUES
#define QUEUES

#include "../collection.h"

typedef struct Queue {
	Collection collection;
} Queue, *QueuePTR;

#define QUEUE_SIZE sizeof(Queue)

void initQueue(QueuePTR queue, SizeT initCapacity, SizeT elementSize);
void freeQueue(QueuePTR queue);

void enqueue(QueuePTR queue, ptr newElement);
ptr peek(QueuePTR queue);
ptr dequeue(QueuePTR queue);

#endif