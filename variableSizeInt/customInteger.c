// TODO: Lire le fichier TODO.md
#include "customInteger.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#pragma region Misc Operations

CustomInteger allocInteger(SizeT capacity) {
	if (capacity == 0) {
		fprintf(stderr, "Undefined behaviour for 0 bit integer\n");
		exit(EXIT_FAILURE);
	}
	else if (capacity > MAX_CUSTOM_INT_CAPACITY) {
		fprintf(stderr, "The requested capacity can't be allocated\n");
		exit(EXIT_FAILURE);
	}

	CustomInteger output;

	output.capacity = capacity;
	output.size = 0;
	output.isNegative = false;

	ptr temp = calloc(capacity, I8_SIZE);

	if (temp == NULL) {
		fprintf(stderr, "Not enough space to allocate to a %zu bits integer\n", capacity * 8);
		exit(EXIT_FAILURE);
	}

	output.value = (uint8*)temp;

	return output;
}

CustomInteger copyIntegerToNew(CustomInteger original) {
	CustomInteger output = allocInteger(original.capacity);

	output.size = original.size;
	output.isNegative = original.isNegative;

	copyMemory(original.value, output.value, output.size);

	return output;
}

void printInteger(CustomInteger integer, Base base, bool alwaysPutSign) {
	String repr = integerToString(integer, base, alwaysPutSign);

	printf("%s\n", repr.chars);
	freeString(&repr);
}

void copyInteger(CustomIntegerPtr src, CustomIntegerPtr dest) {
	if (dest == src || !custIntInitialized(src)) {
		return;
	}

	if (dest->capacity < src->size) {
		setMemory(dest->value, 0, dest->capacity);
		reallocInteger(dest, src->size);
	}

	copyMemory(src->value, dest->value, src->size);
	dest->size = src->size;
	dest->isNegative = src->isNegative;
}

void reallocToFitInteger(CustomIntegerPtr integer) {
	if (integer->size <= 1) return;

	// On part de la fin de l'entier
	SizeT i = integer->size;
	while (i > 1 && integer->value[i - 1] == 0) {
		i--;
	}

	// Si la taille réelle est inférieure à la capacité, on réduit
	if (i < integer->capacity) {
		reallocInteger(integer, i);
	}
	integer->size = i;
}

void reallocInteger(CustomIntegerPtr integer, SizeT newCapacity) {
	if (newCapacity == integer->capacity) {
		return;
	}

	ptr oldPtr = (ptr)integer->value;
	ptr newPtr = calloc(newCapacity, I8_SIZE);

	if (newPtr == NULL) {
		fprintf(stderr, "Not enough space to allocate to a %zu bits integer\n", newCapacity * 8);
		exit(EXIT_FAILURE);
	}

	copyMemory(oldPtr, newPtr, integer->size);
	setMemory(oldPtr, 0, integer->capacity);

	integer->capacity = newCapacity;
	integer->value = (uint8*)newPtr;

	free(oldPtr);
}

void setToZero(CustomIntegerPtr integer) {
	setMemory(integer->value, 0, integer->capacity);
	integer->isNegative = false;
}

void freeInteger(CustomIntegerPtr integer) {
	if (integer->value == NULL) return;

	// Effacement sécurisé du contenu
	setMemory(integer->value, 0, integer->capacity);
	free(integer->value);

	// Neutralisation de la structure pour éviter les "double free"
	integer->value = NULL;
	integer->capacity = 0;
	integer->size = 0;

	integer = (CustomIntegerPtr)NULL;
}

static string baseChars = "0123456789ABCDEF";

/*typedef String (*FormatterFunc)(CustomInteger, Base);

String byteFormatter(CustomInteger integer, Base base) {
	SizeT byteLength = 0; // Longueur d'un octet en nombre de caractères dans la base donnée
	SizeT divider = (SizeT)base;

	switch (base) {
	case BIN:
		byteLength = 8;
		break;

	case HEX:
		byteLength = 2;
		break;

	default:
		fprintf(stderr, "Unsupported base\n");
		exit(EXIT_FAILURE);
		break;
	}

	SizeT strLength = integer.capacity * byteLength + 2;

	String obj = allocString(strLength);

	char c;

	for (SizeT byteI = 0; byteI < integer.capacity; byteI++) {
		uint8 byte = integer.value[byteI];
		SizeT index;

		for (SizeT i = 0; i < byteLength; i++) {
			index = byte % divider;
			byte /= divider;

			c = baseChars[index];
			appendChar(&obj, c);
		}
	}

	char sign = integer.isNegative ? '-' : '+';

	appendChar(&obj, sign);

	stringLength(&obj);
	reverseString(&obj);

	return obj;
}

String moduloFormatter(CustomInteger integer, Base base) {
	String output = {0};

	SizeT byteLength = 0; // Longueur d'un octet en nombre de caractères dans la base donnée
	SizeT divider = (SizeT)base;

	CustomInteger dividerInt = allocIntegerFromValue((uint64)base, false, true), workInt = copyIntegerToNew(integer), tempInt;

	freeInteger(&dividerInt);
	freeInteger(&workInt);
	freeInteger(&tempInt);

	return output;
}

String integerToString(CustomInteger integer, Base base) {
	SizeT divider = (SizeT)base;

	if (divider == 0) {
		fprintf(stderr, "Base 0 doesn't exist, you dumbass!\n");
		exit(EXIT_FAILURE);
	}

	FormatterFunc formatter = (8 % divider) == 0 ? byteFormatter : moduloFormatter;

	return formatter(integer, base);
}*/

/**
 * Refactor de la fonction réalisé par Gemini 3.0 Pro pour l'ajout du support des Bases exotiques
 */
String integerToString(CustomInteger integer, Base base, bool alwaysPutSign) {
	string baseChars = "0123456789ABCDEF";
	String obj;

	// 1. Vérification : Est-ce une puissance de 2 ?
	// L'astuce (base & (base - 1)) == 0 renvoie true si c'est une puissance de 2.
	bool isPowerOfTwo = (base > 1) && ((base & (base - 1)) == 0);

	// 2. Calcul du nombre de bits par caractère (log2 de la base)
	SizeT bitsPerChar = 0;
	if (isPowerOfTwo) {
		SizeT temp = base;
		while (temp > 1) {
			temp >>= 1;
			bitsPerChar++;
		}
	}

	// 3. Choix de l'algorithme
	// On utilise le chemin RAPIDE si :
	// - C'est une puissance de 2
	// - ET que les bits s'alignent parfaitement dans un octet (8 % bits == 0)
	// Ex: Base 16 (4 bits) -> OK. Base 8 (3 bits) -> NON (utilisera l'algo lent).
	if (isPowerOfTwo && (8 % bitsPerChar == 0)) {

		// --- ALGORITHME RAPIDE (Bitmasking / Modulo local) ---

		SizeT divider = (SizeT)base;
		SizeT charsPerByte = 8 / bitsPerChar; // Ex: 2 chars pour Hex, 8 pour Bin

		// Taille exacte : (capacité * chars_par_octet) + signe + null terminator
		SizeT strLength = integer.capacity * charsPerByte + 2;
		obj = allocString(strLength);

		char c;
		for (SizeT byteI = 0; byteI < integer.capacity; byteI++) {
			uint8 byte = integer.value[byteI];
			SizeT index;

			for (SizeT i = 0; i < charsPerByte; i++) {
				// Ici le modulo fonctionne car la base est alignée sur l'octet
				index = byte % divider;
				byte /= divider;

				c = baseChars[index];
				appendChar(&obj, c);
			}
		}
	} else {

		// --- ALGORITHME LENT (Divisions Euclidiennes Générales) ---
		// Utilisé pour Base 10, Base 8, ou toute base exotique.

		if (isZero(integer)) {
			obj = allocString(2);
			appendChar(&obj, '0');
			// Le signe sera ajouté après
		} else {
			// Estimation pire cas (Base 2) : size * 8
			SizeT estimatedLen = integer.size * 8 + 2;
			obj = allocString(estimatedLen);

			CustomInteger temp = copyIntegerToNew(integer);
			temp.isNegative = false;

			// Création d'un BigInt pour représenter la base
			CustomInteger baseBigInt = allocIntegerFromValue(base, false, true);

			while (!isZero(temp)) {
				EuclideanDivision div = euclideanDivInteger(temp, baseBigInt);

				// Le reste est forcément plus petit que la base, il tient dans un uint8
				// sauf si tu as une base > 255, mais baseChars limite à 16 ici.
				uint8 remainderVal = div.remainder.value[0];
				appendChar(&obj, baseChars[remainderVal]);

				freeInteger(&temp);
				freeInteger(&div.remainder);

				temp = div.quotient;
			}

			freeInteger(&baseBigInt);
			freeInteger(&temp);
		}
	}

	// --- FINITION (Signe + Inversion) ---
	char sign = integer.isNegative ? '-' : '+';

	if (integer.isNegative || alwaysPutSign)
		appendChar(&obj, sign);

	stringLength(&obj);
	reverseString(&obj);

	return obj;
}

CustomInteger allocIntegerFromValue(uint64 value, bool negative, bool fitToValue) {
	SizeT capacity = sizeof(uint64);
	SizeT size = 1;

	while (value >> (size * 8)) {
		size++;
	}

	if (fitToValue) {
		capacity = size;
	}

	CustomInteger output = allocInteger(capacity);

	output.size = size;

	output.isNegative = negative;

	for (SizeT i = 0; i < size; i++) {
		output.value[i] = (uint8)((value >> (i * 8)) % 256);
	}

	return output;
}

uint8 getByteFromInteger(CustomInteger integer, SizeT byteIndex) {
	if (byteIndex >= integer.size || byteIndex >= integer.capacity) {
		return 0;
	}

	return integer.value[byteIndex];
}

#pragma endregion

#pragma region Bitwise Operations

CustomInteger BitwiseAND(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	SizeT smallest = a.size < b.size ? a.size : b.size;

	result = allocInteger(smallest);

	for (SizeT i = 0; i < smallest; i++) {
		result.value[i] = a.value[i] & b.value[i];

		result.size++;
	}

	return result;
}

CustomInteger BitwiseOR(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	SizeT smallest = a.size < b.size ? a.size : b.size;

	result = allocInteger(smallest);

	for (SizeT i = 0; i < smallest; i++) {
		result.value[i] = a.value[i] | b.value[i];

		result.size++;
	}

	return result;
}

CustomInteger BitwiseXOR(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	SizeT smallest = a.size < b.size ? a.size : b.size;

	result = allocInteger(smallest);

	for (SizeT i = 0; i < smallest; i++) {
		result.value[i] = a.value[i] ^ b.value[i];

		result.size++;
	}

	return result;
}

CustomInteger BitwiseNOT(CustomInteger a) {
	CustomInteger result;

	result = allocInteger(a.capacity);

	for (SizeT i = 0; i < a.capacity; i++) {
		result.value[i] = ~a.value[i];

		result.size++;
	}

	return result;
}

uint8 getBit(CustomInteger integer, SizeT bitIndex) {
	SizeT byteIdx = bitIndex >> 3; // bitIndex / 8
	uint8 output = 0;

	if (byteIdx < integer.size)
		output = (integer.value[byteIdx] >> (bitIndex & 7)) & 1;

	return output;
}

void setBit(CustomIntegerPtr integer, uint8 newVal, SizeT bitIndex) {
	SizeT byteIdx = bitIndex >> 3;
	if (byteIdx >= integer->capacity) return;

	if (byteIdx >= integer->size) integer->size = byteIdx + 1;

	if (newVal & 1)
		integer->value[byteIdx] |= (1 << (bitIndex & 7));
	else
		integer->value[byteIdx] &= ~(1 << (bitIndex & 7));
}

CustomInteger Bitshift(CustomInteger integer, SizeT shift, ShiftDirection direction, bool adaptCapacity) {
	CustomInteger result;

	setMemory(&result, 0, CUSTOM_INT_SIZE);

	if (shift == 0) {
		return copyIntegerToNew(integer);
	}
	else if (direction == LEFT || direction == RIGHT) {
		SizeT deltaSize = shift / 8;
		SizeT resultCapacity = integer.capacity;

		if (adaptCapacity && direction == LEFT) {
			resultCapacity += deltaSize + 1;
		}

		result = allocInteger(resultCapacity);

		result.isNegative = integer.isNegative;
		result.size = result.capacity;

		setMemory(result.value, 0, result.capacity);

		for (SizeT i = integer.size; i > 0; i--) {
			SizeT j = i - 1;

			for (SizeT k = 8; k > 0; k--) {
				SizeT currentBit = j * 8 + (k - 1);

				if (direction == RIGHT && currentBit < shift) {
					break;
				}

				uint8 bitVal = GET_BIT(integer.value[j], k);

				SizeT destBit = direction == LEFT ? currentBit + shift : currentBit - shift;

				setBit(&result, bitVal, destBit);
			}
		}
	}
	else {
		exit(EXIT_FAILURE);
	}

	for (SizeT i = result.size; i > 0; i--) {
		SizeT j = i - 1;

		if (result.value[j] == 0) {
			result.size = i;
		}
		else {
			break;
		}
	}

	reallocToFitInteger(&result);

	return result;
}

void BitshiftPtr(CustomIntegerPtr integer, SizeT shift, ShiftDirection direction, bool adaptCapacity) {
	CustomInteger shifted = Bitshift(*integer, shift, direction, adaptCapacity);
	copyInteger(&shifted, integer);
}

#pragma endregion

#pragma region Arithmetic Operations

CustomInteger addInteger(CustomInteger a, CustomInteger b) {
	if (b.isNegative) {
		b.isNegative = false;

		return subtractInteger(a, b);
	}
	else if ((a.isNegative != b.isNegative) && a.isNegative) {
		a.isNegative = false;

		return subtractInteger(b, a);
	}

	SizeT longest = a.size >= b.size ? a.size : b.size;

	uint8 SUM = 0, CARRY = 0;
	uint8 A_BYTE, B_BYTE;

	uint16 TEMP = 0;

	CustomInteger result = allocInteger(longest + 1);

	for (SizeT i = 0; i <= longest; i++) {
		result.value[i] = 0;

		A_BYTE = getByteFromInteger(a, i);
		B_BYTE = getByteFromInteger(b, i);

		TEMP = A_BYTE + B_BYTE + CARRY;

		SUM = (uint8)(TEMP & 0xFF);
		CARRY = (uint8)(TEMP > 0xFF);

		result.value[i] = SUM;

		result.size++;

		if (i == SIZET_MAX_VAL) {
			break;
		}
	}

	reallocToFitInteger(&result);

	return result;
}

CustomInteger subtractInteger(CustomInteger a, CustomInteger b) {
	CustomInteger result, a_temp, b_temp;

	bool redirected = false;
	bool returnedZero = false;

	if (b.isNegative && !redirected) {
		b.isNegative = false;
		result = addInteger(a, b);
		redirected = true;
	}
	else if (equalsInteger(a, b)) {
		result = allocIntegerFromValue(0, false, true);
		result.isNegative = a.isNegative;
		redirected = true;
		returnedZero = true;
	}
	else if (isZero(a) && !redirected) {
		result = copyIntegerToNew(b);
		result.isNegative = !result.isNegative;
		redirected = true;
	}
	else if (isZero(b) && !redirected) {
		result = copyIntegerToNew(a);
		redirected = true;
	}
	else if (!redirected && (a.isNegative != b.isNegative)) {
		if (lessThanInteger(a, b)) {
			a.isNegative = false;
			result = addInteger(a, b);
			result.isNegative = true;
			redirected = true;
		}
	}
	else if (!redirected && (a.isNegative == b.isNegative)) {
		if (!a.isNegative && lessThanInteger(a, b)) {
			redirected = true;

			result = subtractInteger(b, a);
			result.isNegative = !result.isNegative;
		}
	}

	if (!redirected && (a.isNegative == b.isNegative) && (a.isNegative == false) && greaterThanInteger(a, b)) {
		SizeT longest = a.size >= b.size ? a.size : b.size;

		a_temp = copyIntegerToNew(a);
		b_temp = copyIntegerToNew(b);

		reallocInteger(&a_temp, longest);
		reallocInteger(&b_temp, longest);

		result = allocInteger(longest + 1);

		uint8 BORROW = 0;
		uint8 A, B;

		for (SizeT i = 0; i <= longest; i++) {
			A = getByteFromInteger(a_temp, i);
			B = getByteFromInteger(b_temp, i);

			result.value[i] = A - B - BORROW;

			BORROW = ((A <= B && BORROW) || (A < B));

			result.size++;

			if (i == SIZET_MAX_VAL) {
				break;
			}
		}

		reallocToFitInteger(&result);

		freeInteger(&a_temp);
		freeInteger(&b_temp);
	}
	else if (!redirected) {
		exit(EXIT_FAILURE);
	}

	return result;
}

CustomInteger multiplyInteger(CustomInteger a, CustomInteger b) {
	if (isZero(a) || isZero(b)) {
		return allocIntegerFromValue(0, false, true);
	}

	// Une seule allocation pour le résultat final
	// La taille max d'un produit est la somme des tailles des opérandes
	CustomInteger result = allocInteger(a.size + b.size);
	result.size = a.size + b.size;
	result.isNegative = a.isNegative ^ b.isNegative;
	setToZero(&result); // Initialise le buffer à 0

	for (SizeT i = 0; i < a.size; i++) {
		uint32 carry = 0;
		if (a.value[i] == 0) continue; // Optimisation si l'octet est nul

		for (SizeT j = 0; j < b.size; j++) {
			// On utilise un type plus large (uint32) pour le calcul intermédiaire
			// Index actuel dans le résultat : i + j
			uint32 current = (uint32)result.value[i + j] +
				((uint32)a.value[i] * (uint32)b.value[j]) +
				carry;

			result.value[i + j] = (uint8)(current & 0xFF);
			carry = current >> 8;
		}

		// Propagation de la retenue finale de la ligne
		result.value[i + b.size] += (uint8)carry;
	}

	reallocToFitInteger(&result); // Nettoie les zéros de tête
	return result;
}

EuclideanDivision euclideanDivInteger(CustomInteger a, CustomInteger b) {
	EuclideanDivision result;

	// Gestion du signe (votre logique actuelle est correcte)
	bool quotientIsNegative = a.isNegative ^ b.isNegative;

	// 1. Cas d'erreur : division par zéro
	if (isZero(b)) {
		fprintf(stderr, "Math error: division by 0\n");
		exit(EXIT_FAILURE);
	}

	// 2. Initialisation du quotient et du reste
	CustomInteger quotient = allocInteger(a.size);
	CustomInteger remainder = allocIntegerFromValue(0, false, true);

	// Sauvegarde des signes originaux et passage en valeur absolue pour le calcul
	bool origANeg = a.isNegative;
	bool origBNeg = b.isNegative;
	a.isNegative = false;
	b.isNegative = false;

	// 3. Boucle de division bit-à-bit (Division Longue)
	SizeT totalBits = a.size * 8;

	SizeT i = totalBits;
	while (i > 0) {
		i--; // On décrémente au début. i passera de (totalBits-1) à 0 inclus.

		// Décalage du reste : remainder = remainder << 1
		BitshiftPtr(&remainder, 1, LEFT, true);

		// On récupère le i-ème bit de 'a' et on l'injecte dans le reste
		uint8 bit = getBit(a, i);
		setBit(&remainder, bit, 0);

		// Si remainder >= b
		if (compareAbs(remainder, b) != LESS) {
			CustomInteger temp = subtractInteger(remainder, b);
			copyInteger(&temp, &remainder);
			freeInteger(&temp);

			// On met le bit correspondant à 1 dans le quotient
			setBit(&quotient, 1, i);
		}
	}

	/*if (quotientIsNegative) {
		CustomInteger One = allocIntegerFromValue(1, false, true), temp;

		quotient.isNegative = true;
		remainder.isNegative = true;

		temp = subtractInteger(quotient, One);
		freeInteger(&quotient);
		copyInteger(&temp, &quotient);
		freeInteger(&temp);

		temp = addInteger(remainder, b);
		freeInteger(&remainder);
		copyInteger(&temp, &remainder);
		freeInteger(&temp);
		freeInteger(&One);
	}*/

	// Nouvelle version - fidèle à l'approche de Python
	quotient.isNegative = quotientIsNegative;
	remainder.isNegative = origANeg;

	if (!isZero(remainder) && (remainder.isNegative ^ origBNeg)) {
		CustomInteger	One = allocIntegerFromValue(1, false, true),
						tempQ = subtractInteger(quotient, One), tempR;

		freeInteger(&quotient);
		quotient = tempQ;
		freeInteger(&One);

		b.isNegative = origBNeg;
		tempR = addInteger(remainder, b);
		freeInteger(&remainder);
		remainder = tempR;

		b.isNegative = false;
	}

	// Restauration des signes
	a.isNegative = origANeg;
	b.isNegative = origBNeg;

	result.quotient = quotient;
	result.remainder = remainder;

	reallocToFitInteger(&result.quotient);
	reallocToFitInteger(&result.remainder);

	return result;
}

CustomInteger modInteger(CustomInteger a, CustomInteger b) {
	EuclideanDivision euclid = euclideanDivInteger(a, b);
	freeInteger(&euclid.quotient);
	return euclid.remainder;
}

CustomInteger divideInteger(CustomInteger a, CustomInteger b) {
	EuclideanDivision euclid = euclideanDivInteger(a, b);
	freeInteger(&euclid.remainder);
	return euclid.quotient;
}

CustomInteger powInteger(CustomInteger a, CustomInteger exp);
#pragma endregion

#pragma region Comparison operations
Comparison compareAbs(CustomInteger a, CustomInteger b) {
	SizeT highestCapacity = a.capacity >= b.capacity ? a.capacity : b.capacity;

	uint8 A, B;
	bool different = false;

	for (SizeT i = highestCapacity; i > 0; i--) {
		SizeT index = i - 1;

		A = getByteFromInteger(a, index);
		B = getByteFromInteger(b, index);

		different = A != B;

		if (different && A > B) {
			return GREATER;
		} else if (different) {
			return LESS;
		}
	}

	return different ? UNDEF : EQUALS;
}

Comparison compareIntegers(CustomInteger a, CustomInteger b) {
	Comparison output = UNDEF;

	if (equalsInteger(a, b)) {
		output = EQUALS;
	}
	else if (lessThanInteger(a, b)) {
		output = LESS;
	}
	else if (greaterThanInteger(a, b)) {
		output = GREATER;
	}

	return output;
}

bool isZero(CustomInteger integer) {
	bool output = true;

	for (SizeT i = 0; i < integer.size; i++) {

		output = getByteFromInteger(integer, i) == 0;

		if (i == SIZET_MAX_VAL || !output) {
			break;
		}
	}

	return output;
}

bool equalsInteger(CustomInteger a, CustomInteger b) {
	Comparison compAbs = compareAbs(a, b);

	return (compAbs == EQUALS && (a.isNegative == b.isNegative)) || (isZero(a) && isZero(b));
}

bool differentFromInteger(CustomInteger a, CustomInteger b) {
	Comparison compAbs = compareAbs(a, b);

	return ((compAbs == LESS) || (compAbs == GREATER));
}

bool lessThanInteger(CustomInteger a, CustomInteger b) {
	bool less = compareAbs(a, b) == LESS;
	bool equal = compareAbs(a, b) == EQUALS;

	bool equalSigns = a.isNegative == b.isNegative;

	/*if (equalSigns) {
		if (a.isNegative && !less && !equal) {
			return true;
		} else if (!a.isNegative && less) {
			return true;
		}
	} else {
		if (a.isNegative) {
			return true;
		}
	}*/

	return (!equalSigns && a.isNegative) || (equalSigns && !a.isNegative && less) || (a.isNegative && !less && !equal);
}

bool greaterThanInteger(CustomInteger a, CustomInteger b) {
	bool greater = compareAbs(a, b) == GREATER;
	bool equal = compareAbs(a, b) == EQUALS;

	bool equalSigns = a.isNegative == b.isNegative;

	/*if (!equalSigns) {
		if (b.isNegative) {
			return true;
		}
	} else {
		if (a.isNegative && !greater && !equal) {
			return true;
		} else if (!a.isNegative && greater) {
			return true;
		}
	}*/

	return (!a.isNegative && (!equalSigns || greater)) || (a.isNegative && equalSigns && !greater && !equal);
}

#pragma endregion