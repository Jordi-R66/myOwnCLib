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
	CustomInteger a, b, c;

	a = allocIntegerFromValue(343, false, true);
	b = allocIntegerFromValue(20, false, true);

	c = divideInteger(a, b);

	String A, B, C;

	A = integerToString(a, HEX);
	B = integerToString(b, HEX);
	C = integerToString(c, HEX);

	freeInteger(&a);
	freeInteger(&b);
	freeInteger(&c);

	printf("%s / %s = %s\n", A.chars, B.chars, C.chars);

	freeString(&A);
	freeString(&B);
	freeString(&C);

	return 0;
}