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

typedef struct Matrix Matrix, *MatrixPtr;

void allocMatrix(MatrixPtr matrix);
void deallocMatrix(MatrixPtr matrix);

void getMatrixRow(MatrixPtr matrix, SizeT row, Values rowBuffer);
void getMatrixColumn(MatrixPtr matrix, SizeT column, Values colBuffer);

void setMatrixRow(MatrixPtr matrix, SizeT row, Values rowBuffer);
void setMatrixColumn(MatrixPtr matrix, SizeT column, Values colBuffer);

void setMatrixCase(MatrixPtr matrix, Value value, SizeT row, SizeT col);
Value getMatrixCase(MatrixPtr matrix, SizeT row, SizeT col);

void setMatrix(MatrixPtr matrix, Values values);

void scalarMul(MatrixPtr matrix, Value scalar);
void matrixMultiplication(MatrixPtr matA, MatrixPtr matB, MatrixPtr matDest);
void matrixAddition(MatrixPtr matA, MatrixPtr matB);

Matrix scalarMulNewMatrix(MatrixPtr matrix, Value scalar);
Matrix matrixAdditionNewMatrix(MatrixPtr matA, MatrixPtr matB);

void genIdentityMatrix(MatrixPtr matrix, SizeT n);

void printMatrix(MatrixPtr matrix, ValType valFormat);

// Originally in gauss.h

void swapRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId);
void swapCols(MatrixPtr mat, SizeT colAId, SizeT colBId);

void subtractRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId, Value coeffRowB);
void multiplyRow(MatrixPtr mat, SizeT rowId, Value coeffRow);

#define MATRIX_INCLUDED