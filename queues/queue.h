#pragma once

/*
	What is a queue?

	A queue is a data structure used to store data using the "first-come,
	first-served" logic, like a queue in a shop at the cashier
*/

#ifndef QUEUES
#define QUEUES

#include "../lists/list.h"

typedef List Queue;

#define QUEUE_SIZE sizeof(Queue)

void initQueue(Queue* queue, SizeT initCapacity, SizeT elementSize);
void freeQueue(Queue* queue);

void enqueue(Queue* queue, ptr newElement);
ptr peek(Queue* queue);
ptr dequeue(Queue* queue);

#endif