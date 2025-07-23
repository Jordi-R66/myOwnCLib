#pragma once

#ifndef STACKS
#define STACKS

#include "../lists/list.h"

typedef List Stack;

#define STACK_SIZE sizeof(Stack)

void initStack(Stack* stack, SizeT initCapacity, SizeT elementSize);
void freeStack(Stack* stack);

void push(Stack* stack, ptr newElement);
ptr peek(Stack* stack);
ptr pop(Stack* stack);

#endif