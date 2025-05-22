#pragma once

#ifdef INCLUDE_ADV_MATRICES

#ifndef INCLUDE_GAUSS
#define INCLUDE_GAUSS
#endif

#include "gauss.h"

void mirrorMatrix(Matrix* mat, SwapType toSwap, Tracker* tracker);

bool inversibilityCheck(Matrix* mat);
void inverseMatrix(Matrix* mat, Matrix* invMat);
#endif