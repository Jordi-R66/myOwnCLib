#pragma once

#include "../matrices/matrix.h"

#ifndef VECTORS_INCLUDED

typedef Matrix Vector;
typedef Vector* VectorPtr;

extern const Vector NULL_VECTOR;

Vector createVector(SizeT coords);
bool allocVector(VectorPtr vector, SizeT coords);
void deallocVector(VectorPtr vector);
bool isNullVector(VectorPtr vPtr);

bool getCoord(VectorPtr vector, SizeT coordNumber, Value* destVar);
bool setCoord(VectorPtr vector, SizeT coordNumber, Value value);

void setVector(VectorPtr vector, Values colBuffer);

bool crossProduct(VectorPtr vA, VectorPtr vB, VectorPtr vDest);
bool dotProduct(VectorPtr vectorA, VectorPtr vectorB, Value* result);

#pragma region ML Primitives
bool vectorNorm(VectorPtr v, Value* result);
bool vectorAxpy(Value alpha, VectorPtr x, VectorPtr y);
bool vectorSoftmax(VectorPtr v);
bool vectorArgmax(VectorPtr v, SizeT* resultIndex);
#pragma endregion

#define VECTORS_INCLUDED 1
#endif
