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

	a = allocIntegerFromValue(149597870700, false, true);
	b = allocIntegerFromValue(299792458, false, true);

	String A, B, Q, R;

	for (uint8 i = 0; i < 2; i++) {
		a.isNegative = (bool)i;
		for (uint8 j = 0; j < 2; j++) {
			b.isNegative = (bool)j;

			A = integerToString(a, DEC, false);
			B = integerToString(b, DEC, false);

			euclid = euclideanDivInteger(a, b);

			c = &euclid.quotient;
			r = &euclid.remainder;

			Q = integerToString(*c, DEC, false);
			R = integerToString(*r, DEC, false);

			freeInteger(c);
			freeInteger(r);

			printf("Calcul de A / B:\n\tA=%s\n\tB=%s\n\tQ=%s\n\tR=%s\n", A.chars, B.chars, Q.chars, R.chars);

			freeString(&A);
			freeString(&B);
			freeString(&Q);
			freeString(&R);
		}
	}


	freeInteger(&a);
	freeInteger(&b);

	return 0;
}