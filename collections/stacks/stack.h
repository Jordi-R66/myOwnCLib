#pragma once

#ifndef STACKS
#define STACKS

#include "../collection.h"

typedef struct Collection {
	Collection collection;
} Stack, *StackPTR;

#define STACK_SIZE sizeof(Stack)

void initStack(StackPTR stack, SizeT initCapacity, SizeT elementSize);
void freeStack(StackPTR stack);

void push(StackPTR stack, ptr newElement);
ptr peek(StackPTR stack);
ptr pop(StackPTR stack);

#endif