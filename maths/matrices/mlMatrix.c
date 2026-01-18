#include "mlMatrix.h"

#include "math.h"

Value relu(Value x) {
	Value output;

	output = x > 0 ? x : 0;

	return output;
}

Value sigmoid(Value x) {
	Value output;

	output = 1.0 / (1.0 + exp(-x));

	return output;
}

Value d_relu(Value x) {
	Value output;

	output = x > 0 ? 1.0 : 0.0;

	return output;
}

Value d_sigmoid(Value x) {
	Value output, sig;

	sig = sigmoid(x);
	output = sig * (1.0 - sig);

	return output;
}
