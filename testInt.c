#include <stdio.h>
#include <stdlib.h>

#include "variableSizeInt/customInteger.h"

#define MIN_TEST_VAL -1024
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

/*int main() {
	int32* vals = generateTestVals();
	CustomInteger* valsCust = calloc(N_TESTS_VALS, CUSTOM_INT_SIZE);

	for (SizeT i = 0; i < N_TESTS_VALS; i++) {
		CustomInteger custInt = allocInteger(I32_SIZE);
		int32 val = vals[i];

		if (val < 0) {
			val *= -1;
			custInt.isNegative = true;
		} else {
			custInt.isNegative = false;
		}

		custInt.size = I32_SIZE;

		copyMemory(&val, custInt.value, I32_SIZE);

		valsCust[i] = custInt;
	}

	for (SizeT i = 0; i < N_TESTS_VALS; i++) {
		CustomInteger a = valsCust[i];

		int32 A, B, C;

		A = vals[i];

		for (SizeT j = 0; j < N_TESTS_VALS; j++) {
			B = vals[j];
			C = A - B;

			CustomInteger b = valsCust[j];
			CustomInteger c = subtractInteger(a, b);

			if (!intEqualsCustom(C, c)) {
				String strA = integerToString(a, HEX), strB = integerToString(b, HEX), strC = integerToString(c, HEX);
				fprintf(stderr, "Error for %d - %d = %d\nGot %s - %s = %s\n\n", A, B, C, strA.chars, strB.chars, strC.chars);

				freeString(&strA);
				freeString(&strB);
				freeString(&strC);

				for (SizeT k = 0; k < I32_SIZE; k++) {

					uint8 byteSub = ((uint8*)&C)[k];
					uint8 byteCust = c.value[k];

					printf("%X\t%X | %c\n", byteSub, byteCust, byteSub == byteCust ? 'Y' : 'N');
				}

				goto ENDING;
			} else {
				//printf("SUCCESS WITH A = %d AND B = %d\n", A, B);
			}
		}
	}

ENDING:
	printf("THE END\n");

	for (SizeT i = 0; i < N_TESTS_VALS; i++) {
		freeInteger(&valsCust[i]);
	}

	free(valsCust);
	free(vals);

	return 0;
}*/

int main(void) {
	CustomInteger a, b, c;

	a = allocIntegerFromValue(299792458, false, true);

	//b = Bitshift(a, 1, LEFT, true);
	c = Bitshift(a, 16, RIGHT, true);

	String A, B, C;

	A = integerToString(a, BIN);
	//B = integerToString(b, BIN);
	C = integerToString(c, BIN);

	freeInteger(&a);
	//freeInteger(&b);
	freeInteger(&c);

	printf("%s\n%s\n", A.chars, C.chars);

	freeString(&A);
	//freeString(&B);
	freeString(&C);

	return 0;
}