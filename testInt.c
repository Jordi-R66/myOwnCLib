#include <stdio.h>
#include <stdlib.h>

#include "variableSizeInt/customInteger.h"

#define MIN_TEST_VAL 0
#define MAX_TEST_VAL 1024
#define TEST_VAL_STEP 1

#define N_TESTS_VALS (MAX_TEST_VAL - MIN_TEST_VAL + 1) / TEST_VAL_STEP

int32* generateTestVals(void) {
	void* temp = calloc(N_TESTS_VALS, sizeof(int32));

	int32* arr = (int32*)temp;

	SizeT i = 0;

	for (int32 v = MIN_TEST_VAL; v < MAX_TEST_VAL; v += TEST_VAL_STEP) {
		arr[i++] = v;
	}

	return arr;
}

bool intEqualsCustom(int32 val, CustomInteger valCust) {
	bool valNeg = val < 0;
	bool absEq = true;

	if (val == 0 && isZero(valCust)) {
		return true;
	}

	if (valNeg) {
		val *= -1;
	}

	for (SizeT k = 0; k < I32_SIZE; k++) {

		uint8 byteSub = ((uint8*)&val)[k];
		uint8 byteCust = valCust.value[k];

		if (byteSub != byteCust) {
			absEq = false;
			break;
		}
	}

	return absEq && (valNeg == valCust.isNegative);
}

int main(void) {
	CustomInteger a, b;
	CustomIntegerPtr c, r;
	EuclideanDivision euclid;

	a = allocIntegerFromValue(2549, false, true);
	b = allocIntegerFromValue(4, false, true);

	printf("INTEGERS INITIALIZED!\n");

	euclid = euclideanDivInteger(a, b);

	c = &euclid.quotient;
	r = &euclid.remainder;

	printf("INTEGERS DIVIDED!\n");

	String A, B, C, R;

	A = integerToString(a, HEX);
	B = integerToString(b, HEX);
	C = integerToString(*c, HEX);
	R = integerToString(*r, HEX);

	freeInteger(&a);
	freeInteger(&b);
	freeInteger(c);
	freeInteger(r);

	printf("%s = %s * %s + %s\n", A.chars, B.chars, C.chars, R.chars);

	freeString(&A);
	freeString(&B);
	freeString(&C);
	freeString(&R);

	return 0;
}