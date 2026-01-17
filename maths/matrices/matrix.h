#pragma once

#include "localCommon.h"

#pragma pack(1)
struct Matrix {
	SizeT rows, cols, size;

	Values data;
	bool memFreed;
};
#pragma pack()

#define MATRIX_SIZE sizeof(Matrix)

typedef struct Matrix Matrix;

void allocMatrix(Matrix* matrix);
void deallocMatrix(Matrix* matrix);

void getMatrixRow(Matrix* matrix, SizeT row, Values rowBuffer);
void getMatrixColumn(Matrix* matrix, SizeT column, Values colBuffer);

void setMatrixRow(Matrix* matrix, SizeT row, Values rowBuffer);
void setMatrixColumn(Matrix* matrix, SizeT column, Values colBuffer);

void setMatrixCase(Matrix* matrix, value_t value, SizeT row, SizeT col);
value_t getMatrixCase(Matrix* matrix, SizeT row, SizeT col);

void setMatrix(Matrix* matrix, Values values);

void scalarMul(Matrix* matrix, value_t scalar);
void matrixMultiplication(Matrix* matA, Matrix* matB, Matrix* matDest);
void matrixAddition(Matrix* matA, Matrix* matB);

Matrix scalarMulNewMatrix(Matrix* matrix, value_t scalar);
Matrix matrixAdditionNewMatrix(Matrix* matA, Matrix* matB);

void genIdentityMatrix(Matrix* matrix, SizeT n);

void printMatrix(Matrix* matrix, ValType valFormat);

// Originally in gauss.h

void swapRows(Matrix* mat, SizeT rowAId, SizeT rowBId);
void swapCols(Matrix* mat, SizeT colAId, SizeT colBId);

void subtractRows(Matrix* mat, SizeT rowAId, SizeT rowBId, value_t coeffRowB);
void multiplyRow(Matrix* mat, SizeT rowId, value_t coeffRow);

#define MATRIX_INCLUDED