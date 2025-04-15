#pragma once

#include "gauss.h"

void mirrorMatrix(Matrix* mat, SwapType toSwap, Tracker* tracker);

bool inversibilityCheck(Matrix* mat);
void inverseMatrix(Matrix* mat, Matrix* invMat);