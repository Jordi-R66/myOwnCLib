#include "tracking.h"

void InitTracker(Tracker* tracker) {
	tracker->transformationMade = 0;
	tracker->transformations = (MatrixTransformation*)calloc(1, sizeof(MatrixTransformation));

	if (tracker->transformations == NULL) {
		fprintf(stderr, "Error or not enough space available in memory to continue\n");
		exit(EXIT_FAILURE);
	}

	tracker->initialised = true;
	tracker->memFreed = false;
}

void RecordTransformation(Tracker* tracker, MatrixTransformation transformation) {
	tracker->transformationMade += 1;
	MatrixTransformation* newPtr = (MatrixTransformation*)realloc(tracker->transformations, sizeof(MatrixTransformation));

	if (newPtr != NULL) {
		tracker->transformations = newPtr;
	}
	else {
		fprintf(stderr, "Error or not enough space available in memory to continue\n");
		exit(EXIT_FAILURE);
	}

	tracker->transformations[tracker->transformationMade - 1] = transformation;
}

void deallocTracker(Tracker* tracker) {
	for (size_t i = 0; i < tracker->transformationMade; i++) {
		memset(&(tracker->transformations[i]), 0, sizeof(MatrixTransformation));
	}

	free(tracker->transformations);
	tracker->transformationMade = 0;
	tracker->memFreed = true;

	memset(tracker, 0, sizeof(Tracker));
}

void recordSwap(Tracker* tracker, size_t idA, size_t idB, SwapType type) {
	if (idA == idB) {
		return;
	}

	MatrixTransformation transformation;

	transformation.transType = SWAP;

	transformation.A = idA;
	transformation.B = idB;
	transformation.coeff = 0;

	transformation.swapType = type;

	RecordTransformation(tracker, transformation);
}

void recordSub(Tracker* tracker, size_t idA, size_t idB, value_t coeff) {
	if ((idA == idB) || (coeff == -1.0)) {
		return;
	}

	MatrixTransformation transformation;

	transformation.transType = SUB;

	transformation.A = idA;
	transformation.B = idB;
	transformation.coeff = coeff;

	transformation.swapType = None;

	RecordTransformation(tracker, transformation);
}

void recordMul(Tracker* tracker, size_t idA, value_t coeff) {
	if (coeff == 1.0) {
		return;
	}

	MatrixTransformation transformation;

	transformation.transType = MUL;

	transformation.A = idA;
	transformation.B = idA;
	transformation.coeff = coeff;

	transformation.swapType = None;

	RecordTransformation(tracker, transformation);
}

void printLog(MatrixTransformation transformation) {
	char typeTrans[5];
	char typeSwap[] = "NUL";

	for (int i = 0; i < 5; i++) {
		typeTrans[i] = 0;

		if (i < 4) {
			typeSwap[i] = 0;
		}
	}

	switch (transformation.transType) {
		case SWAP:
			strcpy(typeTrans, "SWAP");
			break;
		case SUB:
			strcpy(typeTrans, "SUB");
			break;
		case MUL:
			strcpy(typeTrans, "MUL");
			break;
		default:
			exit(EXIT_FAILURE);
			break;
	}

	if (transformation.transType == SWAP) {
		switch (transformation.swapType) {
			case Row:
				strcpy(typeSwap, "ROW");
				break;

			case Column:
				strcpy(typeSwap, "COL");
				break;

			default:
				exit(EXIT_FAILURE);
				break;
		}
	}

	printf("%s, %lu %lu, %lf, %s\n", typeTrans, transformation.A, transformation.B, transformation.coeff, typeSwap);

}

void printTrackingLogs(Tracker* tracker) {
	for (size_t i = 0; i < tracker->transformationMade; i++) {
		printLog(tracker->transformations[i]);
	}
}