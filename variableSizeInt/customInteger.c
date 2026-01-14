// TODO: Lire le fichier TODO.md
#include "customInteger.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// Pour la multiplication Karatsuba
// Seuil en Mots. En dessous de 32 mots (1024 bits), l'algo naïf est souvent plus rapide
#define KARATSUBA_THRESHOLD 32

#pragma region Misc Operations

CustomInteger allocInteger(SizeT capacity) {
	if (capacity == 0) {
		fprintf(stderr, "Undefined behaviour for 0 bit integer\n");
		exit(EXIT_FAILURE);
	} else if (capacity > MAX_CUSTOM_INT_CAPACITY) {
		fprintf(stderr, "The requested capacity can't be allocated\n");
		exit(EXIT_FAILURE);
	}

	CustomInteger output;

	output.capacity = capacity;
	output.size = 0;
	output.isNegative = false;

	// Allocation en octets = capacité * taille d'un mot
	ptr temp = calloc(capacity, WORD_SIZE);

	if (temp == NULL) {
		fprintf(stderr, "Not enough space to allocate to a %zu bits integer\n", capacity * 32);
		exit(EXIT_FAILURE);
	}

	output.value = (Word*)temp;

	return output;
}

CustomInteger copyIntegerToNew(CustomInteger original) {
	CustomInteger output = allocInteger(original.capacity);

	output.size = original.size;
	output.isNegative = original.isNegative;

	// Copie mémoire : attention copyMemory prend généralement une taille en octets
	copyMemory(original.value, output.value, output.size * WORD_SIZE);

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
		setMemory(dest->value, 0, dest->capacity * WORD_SIZE);
		reallocInteger(dest, src->size);
	}

	copyMemory(src->value, dest->value, src->size * WORD_SIZE);
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
	SizeT sizeToCopy = newCapacity < integer->size ? newCapacity : integer->size;

	if (newCapacity == integer->capacity) {
		return;
	}

	ptr oldPtr = (ptr)integer->value;
	ptr newPtr = calloc(newCapacity, WORD_SIZE);

	if (newPtr == NULL) {
		fprintf(stderr, "Not enough space to allocate to a %zu bits integer\n", newCapacity * 32);
		exit(EXIT_FAILURE);
	}

	copyMemory(oldPtr, newPtr, sizeToCopy * WORD_SIZE);
	// Effacement de l'ancien buffer pour sécurité avant free (optionnel mais bonne pratique)
	setMemory(oldPtr, 0, integer->capacity * WORD_SIZE);

	integer->capacity = newCapacity;
	integer->value = (Word*)newPtr;

	free(oldPtr);
}

void setToZero(CustomIntegerPtr integer) {
	setMemory(integer->value, 0, integer->capacity * WORD_SIZE);
	integer->isNegative = false;
}

void freeInteger(CustomIntegerPtr integer) {
	if (integer->value == NULL) return;

	// Effacement sécurisé du contenu
	setMemory(integer->value, 0, integer->capacity * WORD_SIZE);
	free(integer->value);

	// Neutralisation de la structure pour éviter les "double free"
	integer->value = NULL;
	integer->capacity = 0;
	integer->size = 0;

	integer = (CustomIntegerPtr)NULL;
}

static void splitInteger(CustomInteger src, SizeT splitIndex, CustomIntegerPtr low, CustomIntegerPtr high) {
	// Partie Basse
	SizeT lowSize = (splitIndex < src.size) ? splitIndex : src.size;

	if (lowSize == 0) {
		// On crée proprement un Zéro (taille 1, valeur 0)
		*low = allocIntegerFromValue(0, false, true);
	} else {
		*low = allocInteger(lowSize);
		low->size = lowSize;
		copyMemory(src.value, low->value, lowSize * WORD_SIZE);
	}

	low->isNegative = false;

	// Partie Haute
	if (src.size > splitIndex) {
		SizeT highSize = src.size - splitIndex;

		*high = allocInteger(highSize);
		high->size = highSize;

		// Arithmétique des pointeurs sur Word* (src.value) fonctionne correctement
		copyMemory(&(src.value[splitIndex]), high->value, highSize * WORD_SIZE);
	} else {
		*high = allocIntegerFromValue(0, false, true);
	}

	high->isNegative = false;

	reallocToFitInteger(low);
	reallocToFitInteger(high);
}

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
	// - ET que les bits s'alignent parfaitement dans un Mot (32 % bits == 0)
	// Ex: Base 16 (4 bits) -> OK (8 chars/word). Base 8 (3 bits) -> NON (32%3 != 0).
	if (isPowerOfTwo && (32 % bitsPerChar == 0)) {

		// --- ALGORITHME RAPIDE (Bitmasking / Modulo local) ---

		SizeT divider = (SizeT)base;
		SizeT charsPerWord = 32 / bitsPerChar;

		// Taille exacte : (capacité * chars_par_mot) + signe + null terminator
		SizeT strLength = integer.capacity * charsPerWord + 2;
		obj = allocString(strLength);

		char c;
		for (SizeT wordI = 0; wordI < integer.capacity; wordI++) {
			Word word = integer.value[wordI];
			SizeT index;

			for (SizeT i = 0; i < charsPerWord; i++) {
				index = word % divider;
				word /= divider;

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
			// Estimation pire cas (Base 2) : size * 32
			SizeT estimatedLen = integer.size * 32 + 2;
			obj = allocString(estimatedLen);

			CustomInteger temp = copyIntegerToNew(integer);
			temp.isNegative = false;

			// Création d'un BigInt pour représenter la base
			CustomInteger baseBigInt = allocIntegerFromValue(base, false, true);

			while (!isZero(temp)) {
				EuclideanDivision div = euclideanDivInteger(temp, baseBigInt);

				// Le reste est forcément plus petit que la base.
				// Pour les bases <= 36 (alphanumérique standard), le reste tient dans un mot
				// et indexe baseChars directement.
				Word remainderVal = div.remainder.value[0];

				// Sécurité pour ne pas dépasser baseChars si base > 16 (non géré par baseChars ici)
				if (remainderVal < 16) {
					appendChar(&obj, baseChars[remainderVal]);
				} else {
					// Fallback générique si on étendait baseChars
					appendChar(&obj, '?');
				}

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
	SizeT capacity = sizeof(uint64) / WORD_SIZE; // Typiquement 2 pour 64 bits / 32 bits
	if (capacity < 1) capacity = 1;

	SizeT size = 1;

	// Calcul de la taille nécessaire en Mots
	while (value >> (size * 32)) {
		size++;
	}

	if (fitToValue) {
		capacity = size;
	}

	CustomInteger output = allocInteger(capacity);
	output.size = size;
	output.isNegative = negative;

	for (SizeT i = 0; i < size; i++) {
		// Extraction de 32 bits
		output.value[i] = (Word)((value >> (i * 32)) & WORD_MAX_VAL);
	}

	if (fitToValue) {
		reallocToFitInteger(&output);
	}

	return output;
}

Word getWordFromInteger(CustomInteger integer, SizeT wordIndex) {
	if (wordIndex >= integer.size || wordIndex >= integer.capacity) {
		return 0;
	}

	return integer.value[wordIndex];
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

bool getBit(CustomInteger integer, SizeT bitIndex) {
	SizeT wordIdx = bitIndex >> 5; // bitIndex / 32
	bool output = false;

	if (wordIdx < integer.size)
		output = (integer.value[wordIdx] >> (bitIndex & 31)) & 1;

	return output;
}

void setBit(CustomIntegerPtr integer, bool bitValue, SizeT bitIndex) {
	SizeT wordIdx = bitIndex >> 5; // bitIndex / 32
	if (wordIdx >= integer->capacity) return;

	if (wordIdx >= integer->size) integer->size = wordIdx + 1;

	if (bitValue)
		integer->value[wordIdx] |= (1 << (bitIndex & 31));
	else
		integer->value[wordIdx] &= ~(1 << (bitIndex & 31));
}

CustomInteger Bitshift(CustomInteger integer, SizeT shift, ShiftDirection direction, bool adaptCapacity) {
	CustomInteger result;

	setMemory(&result, 0, CUSTOM_INT_SIZE);

	if (shift == 0) {
		return copyIntegerToNew(integer);
	} else if (direction == LEFT || direction == RIGHT) {
		SizeT deltaSize = shift / 32;
		SizeT resultCapacity = integer.capacity;

		if (adaptCapacity && direction == LEFT) {
			resultCapacity += deltaSize + 1;
		}

		result = allocInteger(resultCapacity);

		result.isNegative = integer.isNegative;
		result.size = result.capacity;

		setMemory(result.value, 0, result.capacity * WORD_SIZE);

		for (SizeT i = integer.size; i > 0; i--) {
			SizeT j = i - 1;

			// On parcourt les 32 bits du mot
			for (SizeT k = 32; k > 0; k--) {
				SizeT currentBit = j * 32 + (k - 1);

				if (direction == RIGHT && currentBit < shift) {
					break;
				}

				// Lecture bit par bit (pourrait être optimisé par des shifts de mots)
				bool bitVal = (integer.value[j] >> (k - 1)) & 1;

				SizeT destBit = direction == LEFT ? currentBit + shift : currentBit - shift;

				setBit(&result, bitVal, destBit);
			}
		}
	} else {
		exit(EXIT_FAILURE);
	}

	reallocToFitInteger(&result);

	return result;
}

void BitshiftPtr(CustomIntegerPtr integer, SizeT shift, ShiftDirection direction, bool adaptCapacity) {
	CustomInteger shifted = Bitshift(*integer, shift, direction, adaptCapacity);
	copyInteger(&shifted, integer);
	freeInteger(&shifted);
}

#pragma endregion

#pragma region Arithmetic Operations

CustomInteger addInteger(CustomInteger a, CustomInteger b) {
	if (b.isNegative) {
		b.isNegative = false;

		return subtractInteger(a, b);
	} else if ((a.isNegative != b.isNegative) && a.isNegative) {
		a.isNegative = false;

		return subtractInteger(b, a);
	}

	SizeT longest = a.size >= b.size ? a.size : b.size;

	Word SUM = 0, CARRY = 0;
	Word A_VAL, B_VAL;

	DoubleWord TEMP = 0;

	CustomInteger result = allocInteger(longest + 1);

	for (SizeT i = 0; i <= longest; i++) {
		result.value[i] = 0;

		A_VAL = getWordFromInteger(a, i);
		B_VAL = getWordFromInteger(b, i);

		TEMP = (DoubleWord)A_VAL + (DoubleWord)B_VAL + (DoubleWord)CARRY;

		SUM = (Word)(TEMP & WORD_MAX_VAL);
		CARRY = (Word)(TEMP >> 32);

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

	// La logique de redirection des signes reste la même
	if (b.isNegative && !redirected) {
		b.isNegative = false;
		result = addInteger(a, b);
		redirected = true;
	} else if (equalsInteger(a, b)) {
		result = allocIntegerFromValue(0, false, true);
		result.isNegative = a.isNegative;
		redirected = true;
	} else if (isZero(a) && !redirected) {
		result = copyIntegerToNew(b);
		result.isNegative = !result.isNegative;
		redirected = true;
	} else if (isZero(b) && !redirected) {
		result = copyIntegerToNew(a);
		redirected = true;
	} else if (!redirected && (a.isNegative != b.isNegative)) {
		if (lessThanInteger(a, b)) {
			a.isNegative = false;
			result = addInteger(a, b);
			result.isNegative = true;
			redirected = true;
		}
	} else if (!redirected && (a.isNegative == b.isNegative)) {
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

		Word BORROW = 0;
		Word A, B;

		for (SizeT i = 0; i <= longest; i++) {
			A = getWordFromInteger(a_temp, i);
			B = getWordFromInteger(b_temp, i);

			// Soustraction avec emprunt sur 32 bits
			// Si A < B + BORROW, on emprunte
			DoubleWord subVal = (DoubleWord)B + (DoubleWord)BORROW;

			if ((DoubleWord)A < subVal) {
				// On emprunte 1 au rang suivant (donc on ajoute 2^32 à A)
				// A - subVal en arithmétique modulo 2^32 revient à faire la soustraction standard
				// le résultat sera correct, mais on note le borrow
				result.value[i] = (Word)(((DoubleWord)1 << 32) + (DoubleWord)A - subVal);
				BORROW = 1;
			} else {
				result.value[i] = A - (Word)subVal;
				BORROW = 0;
			}

			result.size++;

			if (i == SIZET_MAX_VAL) {
				break;
			}
		}

		reallocToFitInteger(&result);

		freeInteger(&a_temp);
		freeInteger(&b_temp);
	} else if (!redirected) {
		exit(EXIT_FAILURE);
	}

	return result;
}

CustomInteger multiplyNaive(CustomInteger a, CustomInteger b) {
	if (isZero(a) || isZero(b)) {
		return allocIntegerFromValue(0, false, true);
	}

	CustomInteger result = allocInteger(a.size + b.size);
	result.size = a.size + b.size;
	result.isNegative = a.isNegative ^ b.isNegative;
	setToZero(&result);

	for (SizeT i = 0; i < a.size; i++) {
		DoubleWord carry = 0;
		if (a.value[i] == 0) continue;

		for (SizeT j = 0; j < b.size; j++) {
			// Calcul intermédiaire sur 64 bits (DoubleWord)
			DoubleWord current = (DoubleWord)result.value[i + j] +
				((DoubleWord)a.value[i] * (DoubleWord)b.value[j]) +
				carry;

			result.value[i + j] = (Word)(current & WORD_MAX_VAL);
			carry = current >> 32;
		}

		result.value[i + b.size] += (Word)carry;
	}

	return result;
}

CustomInteger multiplyKaratsuba(CustomInteger a, CustomInteger b) {
	if (a.size < KARATSUBA_THRESHOLD || b.size < KARATSUBA_THRESHOLD) {
		return multiplyNaive(a, b);
	}

	SizeT m = (a.size > b.size ? a.size : b.size) / 2;

	CustomInteger low1, high1, low2, high2;
	splitInteger(a, m, &low1, &high1);
	splitInteger(b, m, &low2, &high2);

	CustomInteger z0 = multiplyKaratsuba(low1, low2);
	CustomInteger z2 = multiplyKaratsuba(high1, high2);

	CustomInteger sum1 = addInteger(low1, high1);
	CustomInteger sum2 = addInteger(low2, high2);
	CustomInteger z1_inter = multiplyKaratsuba(sum1, sum2);

	freeInteger(&low1); freeInteger(&high1);
	freeInteger(&low2); freeInteger(&high2);
	freeInteger(&sum1); freeInteger(&sum2);

	CustomInteger tempSub = subtractInteger(z1_inter, z2);
	CustomInteger z1 = subtractInteger(tempSub, z0);

	freeInteger(&z1_inter);
	freeInteger(&tempSub);

	// B^m correspond à un décalage de m Mots (donc m*32 bits)
	CustomInteger resultZ2 = allocInteger(z2.capacity + m * 2);
	copyInteger(&z2, &resultZ2);
	BitshiftPtr(&resultZ2, m * 32 * 2, LEFT, true);

	CustomInteger resultZ1 = allocInteger(z1.capacity + m);
	copyInteger(&z1, &resultZ1);
	BitshiftPtr(&resultZ1, m * 32, LEFT, true);

	CustomInteger tempRes = addInteger(resultZ2, resultZ1);
	CustomInteger result = addInteger(tempRes, z0);

	freeInteger(&z0);
	freeInteger(&z2);
	freeInteger(&z1);
	freeInteger(&resultZ2);
	freeInteger(&resultZ1);
	freeInteger(&tempRes);

	return result;
}

CustomInteger multiplyInteger(CustomInteger a, CustomInteger b) {
	if (isZero(a) || isZero(b)) {
		return allocIntegerFromValue(0, false, true);
	}

	bool resultNegative = a.isNegative ^ b.isNegative;

	a.isNegative = false;
	b.isNegative = false;

	CustomInteger result = multiplyKaratsuba(a, b);

	result.isNegative = resultNegative;

	reallocToFitInteger(&result);

	return result;
}

// Multiplie un CustomInteger par un mot (uint32)
static CustomInteger multiplyByWord(CustomInteger a, Word b) {
	if (b == 0) return allocIntegerFromValue(0, false, true);
	if (b == 1) return copyIntegerToNew(a);

	CustomInteger res = allocInteger(a.size + 1);
	res.size = a.size;
	res.isNegative = a.isNegative;

	DoubleWord carry = 0;
	for (SizeT i = 0; i < a.size; i++) {
		DoubleWord val = (DoubleWord)a.value[i] * (DoubleWord)b + carry;
		res.value[i] = (Word)(val & WORD_MAX_VAL);
		carry = val >> 32;
	}

	if (carry > 0) {
		res.size++;
		res.value[a.size] = (Word)carry;
	}

	return res;
}

EuclideanDivision euclideanDivInteger(CustomInteger a, CustomInteger b) {
	if (isZero(b)) {
		fprintf(stderr, "Math error: division by 0\n");
		exit(EXIT_FAILURE);
	}

	EuclideanDivision result;
	CustomInteger Q, R;

	bool quotientIsNegative = a.isNegative ^ b.isNegative;
	bool remIsNegative = a.isNegative;
	bool origBNeg = b.isNegative;

	Comparison comp = compareAbs(a, b);

	if (comp == LESS) {
		Q = allocIntegerFromValue(0, false, true);
		R = copyIntegerToNew(a);
		R.isNegative = false;
	} else if (comp == EQUALS) {
		Q = allocIntegerFromValue(1, false, true);
		R = allocIntegerFromValue(0, false, true);
	} else {
		// Cas A > B : Algorithme de Knuth (Base 2^32)
		CustomInteger U = copyIntegerToNew(a);
		CustomInteger V = copyIntegerToNew(b);
		U.isNegative = false;
		V.isNegative = false;

		// --- Normalisation ---
		reallocToFitInteger(&V);
		Word msbV = V.value[V.size - 1];
		SizeT shift = 0;
		// On cherche à aligner le MSB sur le bit de poids fort du mot (bit 31)
		while ((msbV & 0x80000000) == 0) {
			msbV <<= 1;
			shift++;
		}

		if (shift > 0) {
			BitshiftPtr(&U, shift, LEFT, true);
			BitshiftPtr(&V, shift, LEFT, true);
		}

		SizeT n = U.size;
		SizeT m = V.size;

		SizeT qSize = (n >= m) ? (n - m + 1) : 1;
		Q = allocInteger(qSize);
		Q.size = qSize;
		setMemory(Q.value, 0, Q.capacity * WORD_SIZE);

		if (n >= m) {
			for (SizeT j = n - m; j < n - m + 1; j--) {
				if (j > n) break;

				// A. Estimation (q_est = (u_n * B + u_{n-1}) / v_{n-1})
				DoubleWord num = 0;
				if (j + m < U.size) num |= ((DoubleWord)U.value[j + m]) << 32;
				if (j + m - 1 < U.size) num |= ((DoubleWord)U.value[j + m - 1]);

				DoubleWord den = V.value[m - 1];
				DoubleWord q_est = num / den;

				if (q_est > WORD_MAX_VAL) q_est = WORD_MAX_VAL;

				// B. Multiplication
				Word q_word = (Word)q_est;
				CustomInteger prod = multiplyByWord(V, q_word);

				CustomInteger shiftedProd = allocInteger(prod.size + j);
				shiftedProd.size = prod.size + j;
				setMemory(shiftedProd.value, 0, j * WORD_SIZE);
				copyMemory(prod.value, shiftedProd.value + j, prod.size * WORD_SIZE);

				// C. Correction
				while (compareAbs(shiftedProd, U) == GREATER) {
					q_word--;

					CustomInteger vShifted = allocInteger(V.size + j);
					vShifted.size = V.size + j;
					setMemory(vShifted.value, 0, j * WORD_SIZE);
					copyMemory(V.value, vShifted.value + j, V.size * WORD_SIZE);

					CustomInteger newProd = subtractInteger(shiftedProd, vShifted);
					freeInteger(&shiftedProd);
					freeInteger(&vShifted);
					shiftedProd = newProd;
				}

				// D. Soustraction
				CustomInteger newU = subtractInteger(U, shiftedProd);
				freeInteger(&U);
				U = newU;

				Q.value[j] = q_word;

				freeInteger(&prod);
				freeInteger(&shiftedProd);

				if (j == 0) break;
			}
		}

		if (shift > 0) {
			BitshiftPtr(&U, shift, RIGHT, false);
		}

		freeInteger(&V);
		R = U;
	}

	Q.isNegative = quotientIsNegative;
	R.isNegative = remIsNegative;

	if (!isZero(R) && (R.isNegative ^ origBNeg)) {
		CustomInteger One = allocIntegerFromValue(1, false, true);
		CustomInteger tempQ = subtractInteger(Q, One);
		freeInteger(&Q);
		Q = tempQ;
		freeInteger(&One);

		CustomInteger origB = copyIntegerToNew(b);
		origB.isNegative = origBNeg;

		CustomInteger tempR = addInteger(R, origB);
		freeInteger(&R);
		R = tempR;

		freeInteger(&origB);
	}

	result.quotient = Q;
	result.remainder = R;

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

CustomInteger powInteger(CustomInteger base, CustomInteger exp) {
	CustomInteger output;

	// --- PRÉ-ANALYSE : Vérification Puissance de 2 ---
	// On scanne 'base' pour voir si c'est une puissance de 2 (un seul bit à 1)
	SizeT setBitCount = 0;
	SizeT bitPosition = 0;

	for (SizeT i = 0; i < base.size; i++) {
		Word b = base.value[i];
		if (b != 0) {
			for (int k = 0; k < 32; k++) { // Scan des 32 bits du mot
				if ((b >> k) & 1) {
					setBitCount++;
					if (setBitCount == 1) bitPosition = i * 32 + k;
				}
			}
			if (setBitCount > 1) break; // Pas une puissance de 2
		}
	}

	// --- LOGIQUE PRINCIPALE ---

	if (isZero(exp)) {
		// Cas trivial 1 : x^0 = 1
		output = allocIntegerFromValue(1, false, true);
	} else if (isZero(base)) {
		// Cas trivial 2 : 0^x = 0
		output = allocIntegerFromValue(0, false, true);
	} else if (setBitCount == 1 && (exp.size * sizeof(Word)) <= sizeof(SizeT)) {
		// Condition : Base est 2^k ET Exposant tient dans un SizeT pour le shift
		// Note : exp.size est en Mots (4 octets). On vérifie que la taille totale en octets tient dans SizeT.
		// --- OPTIMISATION PUISSANCE DE 2 ---

		SizeT expVal = 0;
		for (SizeT i = 0; i < exp.size; i++) {
			// Reconstitution de la valeur (Word vers SizeT)
			// On cast en SizeT avant le shift pour éviter l'overflow si SizeT > 32 bits
			expVal |= ((SizeT)exp.value[i]) << (i * 32);
		}

		// Calcul du décalage : (2^k)^exp = 2^(k*exp)
		SizeT totalShift = bitPosition * expVal;

		output = allocIntegerFromValue(1, false, true);
		BitshiftPtr(&output, totalShift, LEFT, true);
	} else {
		// --- ALGORITHME GÉNÉRAL (Square and Multiply) ---

		output = allocIntegerFromValue(1, false, true);
		CustomInteger baseAccumulator = copyIntegerToNew(base);

		// Optimisation : scan uniquement jusqu'au bit le plus significatif de l'exposant
		// Calcul précis de maxBits pour éviter de boucler inutilement (ex: 32 itérations pour 5^3)
		SizeT maxBits = 0;
		if (exp.size > 0) {
			// 1. Trouver le mot le plus significatif non nul
			SizeT msWordIdx = exp.size;
			while (msWordIdx > 0 && exp.value[msWordIdx - 1] == 0) {
				msWordIdx--;
			}

			if (msWordIdx > 0) {
				// 2. Trouver le bit le plus significatif dans ce mot
				Word topWord = exp.value[msWordIdx - 1];
				int msBit = 31;
				while (msBit >= 0 && !((topWord >> msBit) & 1)) {
					msBit--;
				}

				// Index absolu du dernier bit à 1
				maxBits = (msWordIdx - 1) * 32 + msBit + 1;
			}
		}

		for (SizeT i = 0; i < maxBits; i++) {
			// 1. Si le bit est à 1, on multiplie le résultat courant par l'accumulateur
			if (getBit(exp, i) == 1) {
				CustomInteger newResult = multiplyInteger(output, baseAccumulator);
				freeInteger(&output);
				output = newResult;
			}

			// 2. On prépare la base pour le prochain tour (Carré)
			// On ne le fait pas si c'était la dernière itération utile
			if (i < maxBits - 1) {
				CustomInteger newBase = multiplyInteger(baseAccumulator, baseAccumulator);
				freeInteger(&baseAccumulator);
				baseAccumulator = newBase;
			}
		}

		freeInteger(&baseAccumulator);
	}

	// --- POST-TRAITEMENT ---

	// Gestion du signe commune à tous les cas non triviaux
	if (!isZero(output)) {
		output.isNegative = base.isNegative && (getBit(exp, 0) == 1);
	}

	reallocToFitInteger(&output);

	return output;
}
#pragma endregion

#pragma region Comparison operations
Comparison compareAbs(CustomInteger a, CustomInteger b) {
	SizeT highestCapacity = a.capacity >= b.capacity ? a.capacity : b.capacity;

	Word A, B;
	bool different = false;

	for (SizeT i = highestCapacity; i > 0; i--) {
		SizeT index = i - 1;

		A = getWordFromInteger(a, index);
		B = getWordFromInteger(b, index);

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
	} else if (lessThanInteger(a, b)) {
		output = LESS;
	} else if (greaterThanInteger(a, b)) {
		output = GREATER;
	}

	return output;
}

bool isZero(CustomInteger integer) {
	bool output = true;

	for (SizeT i = 0; i < integer.size; i++) {

		output = getWordFromInteger(integer, i) == 0;

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

	return (!equalSigns && a.isNegative) || (equalSigns && !a.isNegative && less) || (a.isNegative && !less && !equal);
}

bool greaterThanInteger(CustomInteger a, CustomInteger b) {
	bool greater = compareAbs(a, b) == GREATER;
	bool equal = compareAbs(a, b) == EQUALS;

	bool equalSigns = a.isNegative == b.isNegative;

	return (!a.isNegative && (!equalSigns || greater)) || (a.isNegative && equalSigns && !greater && !equal);
}

#pragma endregion

#pragma region Modular Arithmetics

Euclide ExtendedEuclide(CustomInteger a, CustomInteger b) {
	Euclide output;
	output.gcd = copyIntegerToNew(a);
	output.u = allocIntegerFromValue(1, false, true);
	output.v = allocIntegerFromValue(0, false, true);

	Euclide temp;
	temp.gcd = copyIntegerToNew(b);
	temp.u = allocIntegerFromValue(0, false, true);
	temp.v = allocIntegerFromValue(1, false, true);

	while (!isZero(temp.gcd)) {
		EuclideanDivision div = euclideanDivInteger(output.gcd, temp.gcd);
		CustomInteger q = div.quotient;
		CustomInteger r_new = div.remainder;

		CustomInteger q_mult_u2 = multiplyInteger(q, temp.u);
		CustomInteger u_new = subtractInteger(output.u, q_mult_u2);
		freeInteger(&q_mult_u2);

		CustomInteger q_mult_v2 = multiplyInteger(q, temp.v);
		CustomInteger v_new = subtractInteger(output.v, q_mult_v2);
		freeInteger(&q_mult_v2);

		freeInteger(&q);

		freeInteger(&output.gcd);
		freeInteger(&output.u);
		freeInteger(&output.v);

		output = temp;

		temp.gcd = r_new;
		temp.u = u_new;
		temp.v = v_new;
	}

	freeInteger(&temp.gcd);
	freeInteger(&temp.u);
	freeInteger(&temp.v);

	return output;
}

CustomInteger gcdInteger(CustomInteger a, CustomInteger b) {
	CustomInteger r_prev = copyIntegerToNew(a);
	CustomInteger r_curr = copyIntegerToNew(b);

	r_prev.isNegative = false;
	r_curr.isNegative = false;

	while (!isZero(r_curr)) {
		EuclideanDivision div = euclideanDivInteger(r_prev, r_curr);

		freeInteger(&div.quotient);
		freeInteger(&r_prev);

		r_prev = r_curr;
		r_curr = div.remainder;
	}

	freeInteger(&r_curr);

	return r_prev;
}

CustomInteger modularInverse(CustomInteger a, CustomInteger m) {
	Euclide res = ExtendedEuclide(a, m);

	CustomInteger One = allocIntegerFromValue(1, false, true);
	if (!equalsInteger(res.gcd, One)) {
		freeInteger(&One);
		return allocIntegerFromValue(0, false, true);
	}
	freeInteger(&One);
	freeInteger(&res.gcd);
	freeInteger(&res.v);

	CustomInteger result = res.u;

	if (result.isNegative) {
		CustomInteger temp = addInteger(result, m);
		freeInteger(&result);
		result = temp;
	}

	return result;
}

CustomInteger modPowInteger(CustomInteger base, CustomInteger exp, CustomInteger mod) {
	CustomInteger output;
	CustomInteger One = allocIntegerFromValue(1, false, true);

	if (isZero(mod)) {
		freeInteger(&One);
		fprintf(stderr, "Math error: modulo by 0\n");
		exit(EXIT_FAILURE);
	} else if (equalsInteger(mod, One)) {
		output = allocIntegerFromValue(0, false, true);
	} else if (isZero(exp)) {
		output = allocIntegerFromValue(1, false, true);
	} else {
		output = allocIntegerFromValue(1, false, true);
		CustomInteger baseAcc = modInteger(base, mod);

		SizeT maxBits = exp.size * 32;

		for (SizeT i = 0; i < maxBits; i++) {
			if (getBit(exp, i) == 1) {
				CustomInteger prod = multiplyInteger(output, baseAcc);
				CustomInteger newOutput = modInteger(prod, mod);

				freeInteger(&prod);
				freeInteger(&output);
				output = newOutput;
			}

			if (i < maxBits - 1) {
				CustomInteger sq = multiplyInteger(baseAcc, baseAcc);
				CustomInteger newBaseAcc = modInteger(sq, mod);

				freeInteger(&sq);
				freeInteger(&baseAcc);
				baseAcc = newBaseAcc;
			}
		}

		freeInteger(&baseAcc);
	}

	freeInteger(&One);
	reallocToFitInteger(&output);

	return output;
}

#pragma endregion