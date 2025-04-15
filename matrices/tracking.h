#pragma once

#include "common.h"
#include "matrix.h"

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
	size_t A, B;
	SwapType type;
};

typedef struct Swap Swap;

struct MatrixTransformation {
	TransformationType transType;

	size_t A, B;
	value_t coeff;

	SwapType swapType;
};

typedef struct MatrixTransformation MatrixTransformation;

struct Tracker {
	bool initialised, memFreed;
	size_t transformationMade;
	MatrixTransformation* transformations;
};

typedef struct Tracker Tracker;

void InitTracker(Tracker* tracker);
void deallocTracker(Tracker* tracker);

void RecordTransformation(Tracker* tracker, MatrixTransformation transformation);

void recordSwap(Tracker* tracker, size_t idA, size_t idB, SwapType type);
void recordSub(Tracker* tracker, size_t idA, size_t idB, value_t coeff);
void recordMul(Tracker* tracker, size_t idA, value_t coeff);

void printTrackingLogs(Tracker* tracker);