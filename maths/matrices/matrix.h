#pragma once

#ifndef MATRIX_INLCUDED
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

#pragma region Matrix Init
bool allocMatrix(MatrixPtr matrix);
Matrix createMatrix(SizeT rows, SizeT cols);
Matrix createMatrixWithValues(SizeT rows, SizeT cols, Values vals);
void deallocMatrix(MatrixPtr matrix, bool destroyValues);
#pragma endregion

#pragma region Accessors
bool getMatrixRow(MatrixPtr matrix, SizeT row, Values rowBuffer);
bool getMatrixColumn(MatrixPtr matrix, SizeT column, Values colBuffer);
bool setMatrixRow(MatrixPtr matrix, SizeT row, Values rowBuffer);
bool setMatrixColumn(MatrixPtr matrix, SizeT column, Values colBuffer);
bool setMatrixCase(MatrixPtr matrix, Value value, SizeT row, SizeT col);
bool getMatrixCase(MatrixPtr matrix, SizeT row, SizeT col, Value* destVar);
void setMatrix(MatrixPtr matrix, Values values);
#pragma endregion

#pragma region Arithmetics
void scalarMul(MatrixPtr matrix, Value scalar);
bool matrixMultiplication(MatrixPtr matA, MatrixPtr matB, MatrixPtr matDest);
bool matrixAddition(MatrixPtr matA, MatrixPtr matB);

Matrix scalarMulNewMatrix(MatrixPtr matrix, Value scalar);
Matrix matrixAdditionNewMatrix(MatrixPtr matA, MatrixPtr matB);
#pragma endregion

void genIdentityMatrix(MatrixPtr matrix, SizeT n);
void printMatrix(MatrixPtr matrix, ValType valFormat);

// Originally in gauss.h

bool swapRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId);
bool swapCols(MatrixPtr mat, SizeT colAId, SizeT colBId);
void subtractRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId, Value coeffRowB);
void multiplyRow(MatrixPtr mat, SizeT rowId, Value coeffRow);

#define MATRIX_INCLUDED 1
#endif
