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
Value dotProduct(VectorPtr vectorA, VectorPtr vectorB);

#define VECTORS_INCLUDED 1
#endif
