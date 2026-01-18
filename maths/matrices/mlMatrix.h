#pragma once

#ifndef ML_MATRIX_INCLUDED

#include "matrix.h"

Value relu(Value x);
Value sigmoid(Value x);
Value d_relu(Value x);
Value d_sigmoid(Value x);

#define ML_MATRIX_INCLUDED 1
#endif
