#pragma once

#include "gauss.h"

void mirrorMatrix(MatrixPtr mat, SwapType toSwap, Tracker* tracker);

bool inversibilityCheck(MatrixPtr mat);
void inverseMatrix(MatrixPtr mat, MatrixPtr invMat);

#define ADVANCED_MATRIX_INCLUDED