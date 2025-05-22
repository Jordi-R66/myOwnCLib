#pragma once

#ifdef INCLUDE_MATRIX_TRACKING

#ifndef INCLUDE_MATRIX
#include INCLUDE_MATRIX
#endif

#include "matrix.h"

#pragma pack(1)
enum TransformationType {
	SWAP = 0,
	SUB = 1,
	MUL = 2
};

typedef enum TransformationType TransformationType;

enum SwapType {
	Row = 0,
	Column = 1,
	None = 2
};

typedef enum SwapType SwapType;

struct Swap {
	SizeT A, B;
	SwapType type;
};

typedef struct Swap Swap;

struct MatrixTransformation {
	TransformationType transType;

	SizeT A, B;
	value_t coeff;

	SwapType swapType;
};

typedef struct MatrixTransformation MatrixTransformation;

struct Tracker {
	bool initialised, memFreed;
	SizeT transformationMade;
	MatrixTransformation* transformations;
};

typedef struct Tracker Tracker;
#pragma pack()

#define TRACKER_SIZE sizeof(Tracker);

void InitTracker(Tracker* tracker);
void deallocTracker(Tracker* tracker);

void RecordTransformation(Tracker* tracker, MatrixTransformation transformation);

void recordSwap(Tracker* tracker, SizeT idA, SizeT idB, SwapType type);
void recordSub(Tracker* tracker, SizeT idA, SizeT idB, value_t coeff);
void recordMul(Tracker* tracker, SizeT idA, value_t coeff);

void printTrackingLogs(Tracker* tracker);
#endif