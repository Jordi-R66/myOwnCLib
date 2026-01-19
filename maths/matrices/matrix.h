#pragma once

#ifndef MATRIX_INCLUDED
#include "localCommon.h"

#pragma pack(1)
typedef struct Matrix {
	SizeT rows, cols, size;

	Values data;
	bool memFreed;
} Matrix, *MatrixPtr;
#pragma pack()

#define MATRIX_SIZE sizeof(Matrix)

#define IS_NULL_MATRIX(m) (m.rows == (SizeT)0 && m.cols == (SizeT)0 && m.size == (SizeT)0 && m.data == NULL && m.memFreed == false)

extern const Matrix NULL_MATRIX;

typedef Value (*ValueFunc)(Value);

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
bool matrixMultiplicationNT(MatrixPtr matA, MatrixPtr matB, MatrixPtr matDest);
bool matrixAddition(MatrixPtr matA, MatrixPtr matB);

Matrix scalarMulNewMatrix(MatrixPtr matrix, Value scalar);
Matrix matrixAdditionNewMatrix(MatrixPtr matA, MatrixPtr matB);
#pragma endregion

#pragma region Other
void genIdentityMatrix(MatrixPtr matrix, SizeT n);
void printMatrix(MatrixPtr matrix, ValType valFormat);
bool matrixTranspose(MatrixPtr src, MatrixPtr dest);

/**
 * @brief Applies a function to every values of a given Matrix
 * 
 * @param mat The pointer to the Matrix struct
 * @param func the function to apply, must return a Value, and give a Value as a parameter
 */
void matrixMap(MatrixPtr mat, ValueFunc func);
#pragma endregion

// Originally in gauss.h

#pragma region Gauss Functions
bool swapRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId);
bool swapCols(MatrixPtr mat, SizeT colAId, SizeT colBId);
void subtractRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId, Value coeffRowB);
void multiplyRow(MatrixPtr mat, SizeT rowId, Value coeffRow);
#pragma endregion

#define MATRIX_INCLUDED 1
#endif
