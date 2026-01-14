// TODO: Lire le fichier TODO.md
#include "customInteger.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// Pour la multiplication Karatsuba
// Seuil en octets. En dessous de 32 octets (256 bits), l'algo naïf est souvent plus rapide
// à cause de l'overhead des allocations mémoire de la récursion.
#define KARATSUBA_THRESHOLD 32

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
	SizeT sizeToCopy = newCapacity < integer->size ? newCapacity : integer->size;

	if (newCapacity == integer->capacity) {
		return;
	}

	ptr oldPtr = (ptr)integer->value;
	ptr newPtr = calloc(newCapacity, I8_SIZE);

	if (newPtr == NULL) {
		fprintf(stderr, "Not enough space to allocate to a %zu bits integer\n", newCapacity * 8);
		exit(EXIT_FAILURE);
	}

	copyMemory(oldPtr, newPtr, sizeToCopy);
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

static void splitInteger(CustomInteger src, SizeT splitIndex, CustomIntegerPtr low, CustomIntegerPtr high) {
	// Partie Basse
	SizeT lowSize = (splitIndex < src.size) ? splitIndex : src.size;

	if (lowSize == 0) {
		// On crée proprement un Zéro (taille 1, valeur 0)
		*low = allocIntegerFromValue(0, false, true);
	}
	else {
		*low = allocInteger(lowSize);
		low->size = lowSize;
		copyMemory(src.value, low->value, lowSize);
	}

	low->isNegative = false;

	// Partie Haute
	if (src.size > splitIndex) {
		SizeT highSize = src.size - splitIndex;

		*high = allocInteger(highSize);
		high->size = highSize;

		copyMemory(&(src.value[splitIndex]), high->value, highSize);
	}
	else {
		*high = allocIntegerFromValue(0, false, true);
	}

	high->isNegative = false;

	reallocToFitInteger(low);
	reallocToFitInteger(high);
}

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

	if (fitToValue) {
		reallocToFitInteger(&output);
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

CustomInteger multiplyNaive(CustomInteger a, CustomInteger b) {
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

	return result;
}

CustomInteger multiplyKaratsuba(CustomInteger a, CustomInteger b) {
	if (a.size < KARATSUBA_THRESHOLD || b.size < KARATSUBA_THRESHOLD) {
		//printInteger(a, HEX, true);
		//printInteger(b, HEX, true);
		return multiplyNaive(a, b);
	}

	// 1. Calcul du point de coupe (m)
	// On coupe au milieu du plus grand nombre
	SizeT m = (a.size > b.size ? a.size : b.size) / 2;

	// 2. Découpage (Split)
	CustomInteger low1, high1, low2, high2;
	splitInteger(a, m, &low1, &high1);
	splitInteger(b, m, &low2, &high2);

	// 3. Appels Récursifs (Les 3 multiplications)
	// z0 = low1 * low2
	CustomInteger z0 = multiplyKaratsuba(low1, low2);

	// z2 = high1 * high2
	CustomInteger z2 = multiplyKaratsuba(high1, high2);

	// Pour z1, il faut calculer (low1 + high1) * (low2 + high2)
	CustomInteger sum1 = addInteger(low1, high1);
	CustomInteger sum2 = addInteger(low2, high2);
	CustomInteger z1_inter = multiplyKaratsuba(sum1, sum2);

	// Nettoyage des opérandes intermédiaires
	freeInteger(&low1); freeInteger(&high1);
	freeInteger(&low2); freeInteger(&high2);
	freeInteger(&sum1); freeInteger(&sum2);

	// 4. Calcul final de z1 = z1_inter - z2 - z0
	CustomInteger tempSub = subtractInteger(z1_inter, z2);
	CustomInteger z1 = subtractInteger(tempSub, z0);

	freeInteger(&z1_inter);
	freeInteger(&tempSub);

	// 5. Assemblage du résultat : z2*(B^2m) + z1*(B^m) + z0
	// B^m correspond à un décalage de m octets (donc m*8 bits)

	// On décale z2 de 2*m octets
	CustomInteger resultZ2 = allocInteger(z2.capacity + m * 2); // Pré-alloc large
	copyInteger(&z2, &resultZ2);
	BitshiftPtr(&resultZ2, m * 8 * 2, LEFT, true);

	// On décale z1 de m octets
	CustomInteger resultZ1 = allocInteger(z1.capacity + m);
	copyInteger(&z1, &resultZ1);
	BitshiftPtr(&resultZ1, m * 8, LEFT, true);

	// Somme finale
	CustomInteger tempRes = addInteger(resultZ2, resultZ1);
	CustomInteger result = addInteger(tempRes, z0);

	// Nettoyage final
	freeInteger(&z0);
	freeInteger(&z2);
	freeInteger(&z1);
	freeInteger(&resultZ2);
	freeInteger(&resultZ1);
	freeInteger(&tempRes);

	return result;
}

CustomInteger multiplyInteger(CustomInteger a, CustomInteger b) {
	// 1. Cas triviaux
	if (isZero(a) || isZero(b)) {
		return allocIntegerFromValue(0, false, true);
	}

	// 2. Calcul du signe attendu (XOR)
	bool resultNegative = a.isNegative ^ b.isNegative;

	// 3. Passage en positif pour le calcul
	// Note : Comme 'a' et 'b' sont passés par valeur (copie de la struct),
	// modifier .isNegative ici n'affecte pas les variables de la fonction appelante (testInt.c).
	a.isNegative = false;
	b.isNegative = false;

	// 4. Exécution de l'algorithme
	// multiplyKaratsuba est la fonction itérative avec la pile que je t'ai donnée.
	// Elle gère elle-même le seuil (THRESHOLD) pour appeler multiplyNaive si besoin.
	CustomInteger result = multiplyKaratsuba(a, b);

	// 5. APPLICATION DU SIGNE (Le correctif est ici)
	result.isNegative = resultNegative;

	reallocToFitInteger(&result);

	return result;
}

// Multiplie un CustomInteger par un petit entier (uint8)
// Utile pour l'algorithme de division en base 256
static CustomInteger multiplyByByte(CustomInteger a, uint8 b) {
	if (b == 0) return allocIntegerFromValue(0, false, true);
	if (b == 1) return copyIntegerToNew(a);

	CustomInteger res = allocInteger(a.size + 1);
	res.size = a.size;
	res.isNegative = a.isNegative; // En général false ici

	uint16 carry = 0;
	for (SizeT i = 0; i < a.size; i++) {
		uint16 val = (uint16)a.value[i] * (uint16)b + carry;
		res.value[i] = (uint8)(val & 0xFF);
		carry = val >> 8;
	}

	if (carry > 0) {
		res.size++;
		res.value[a.size] = (uint8)carry;
	}

	// Pas de reallocToFit ici pour gagner du temps, la taille est connue
	return res;
}

EuclideanDivision euclideanDivInteger(CustomInteger a, CustomInteger b) {
	// 1. Gestion des cas d'erreur
	if (isZero(b)) {
		fprintf(stderr, "Math error: division by 0\n");
		exit(EXIT_FAILURE);
	}

	EuclideanDivision result;
	CustomInteger Q, R; // Variables de travail pour le résultat brut (absolu)

	// Pré-calcul des signes attendus
	bool quotientIsNegative = a.isNegative ^ b.isNegative;
	bool remIsNegative = a.isNegative;
	bool origBNeg = b.isNegative; // Nécessaire pour l'ajustement Python

	// 2. Logique de Division (Valeurs Absolues)
	Comparison comp = compareAbs(a, b);

	if (comp == LESS) {
		// Cas A < B : Q = 0, R = A
		Q = allocIntegerFromValue(0, false, true);
		R = copyIntegerToNew(a);
		R.isNegative = false; // On travaille en absolu pour l'instant
	}
	else if (comp == EQUALS) {
		// Cas A == B : Q = 1, R = 0
		Q = allocIntegerFromValue(1, false, true);
		R = allocIntegerFromValue(0, false, true);
	}
	else {
		// Cas A > B : Algorithme de Knuth (Base 256)

		// --- Préparation ---
		CustomInteger U = copyIntegerToNew(a); // Dividende
		CustomInteger V = copyIntegerToNew(b); // Diviseur
		U.isNegative = false;
		V.isNegative = false;

		// --- Normalisation ---
		reallocToFitInteger(&V);
		uint8 msbV = V.value[V.size - 1];
		SizeT shift = 0;
		while ((msbV & 0x80) == 0) {
			msbV <<= 1;
			shift++;
		}

		if (shift > 0) {
			BitshiftPtr(&U, shift, LEFT, true);
			BitshiftPtr(&V, shift, LEFT, true);
		}

		SizeT n = U.size;
		SizeT m = V.size;

		// Allocation du quotient
		// Si par hasard n < m après shift (très improbable ici grâce au check GREATER), 
		// calloc renvoie un bloc valide et la boucle ne s'exécute pas -> Q=0.
		SizeT qSize = (n >= m) ? (n - m + 1) : 1;
		Q = allocInteger(qSize);
		Q.size = qSize;
		setMemory(Q.value, 0, Q.capacity);

		// --- Boucle Principale ---
		// Sécurité : on ne rentre dans la boucle que si n >= m
		if (n >= m) {
			for (SizeT j = n - m; j < n - m + 1; j--) {
				if (j > n) break; // Protection underflow

				// A. Estimation
				uint64 num = 0;
				if (j + m < U.size) num |= ((uint64)U.value[j + m]) << 8;
				if (j + m - 1 < U.size) num |= ((uint64)U.value[j + m - 1]);

				uint64 den = V.value[m - 1];
				uint64 q_est = num / den;
				if (q_est > 255) q_est = 255;

				// B. Multiplication
				uint8 q_byte = (uint8)q_est;
				CustomInteger prod = multiplyByByte(V, q_byte);

				CustomInteger shiftedProd = allocInteger(prod.size + j);
				shiftedProd.size = prod.size + j;
				setMemory(shiftedProd.value, 0, j);
				copyMemory(prod.value, shiftedProd.value + j, prod.size);

				// C. Correction
				while (compareAbs(shiftedProd, U) == GREATER) {
					q_byte--;

					CustomInteger vShifted = allocInteger(V.size + j);
					vShifted.size = V.size + j;
					setMemory(vShifted.value, 0, j);
					copyMemory(V.value, vShifted.value + j, V.size);

					CustomInteger newProd = subtractInteger(shiftedProd, vShifted);
					freeInteger(&shiftedProd);
					freeInteger(&vShifted);
					shiftedProd = newProd;
				}

				// D. Soustraction
				CustomInteger newU = subtractInteger(U, shiftedProd);
				freeInteger(&U);
				U = newU;

				Q.value[j] = q_byte;

				freeInteger(&prod);
				freeInteger(&shiftedProd);

				if (j == 0) break;
			}
		}

		// --- Dénormalisation ---
		if (shift > 0) {
			BitshiftPtr(&U, shift, RIGHT, false);
		}

		freeInteger(&V);

		// Le reste final est ce qui reste dans U
		R = U;
	}

	// 3. Application des Signes (Post-Traitement Centralisé)
	Q.isNegative = quotientIsNegative;
	R.isNegative = remIsNegative;

	// 4. Ajustement Python (Floored Division)
	// Si Reste != 0 ET signe(Reste) != signe(DiviseurOriginal)
	if (!isZero(R) && (R.isNegative ^ origBNeg)) {
		// Q = Q - 1
		CustomInteger One = allocIntegerFromValue(1, false, true);
		CustomInteger tempQ = subtractInteger(Q, One);
		freeInteger(&Q);
		Q = tempQ;
		freeInteger(&One);

		// R = R + B (avec signe original)
		CustomInteger origB = copyIntegerToNew(b);
		origB.isNegative = origBNeg;

		CustomInteger tempR = addInteger(R, origB);
		freeInteger(&R);
		R = tempR;

		freeInteger(&origB);
	}

	// 5. Finalisation
	result.quotient = Q;
	result.remainder = R;

	reallocToFitInteger(&result.quotient);
	reallocToFitInteger(&result.remainder);

	return result;
}

/*
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

	//if (quotientIsNegative) {
	//	CustomInteger One = allocIntegerFromValue(1, false, true), temp;

	//	quotient.isNegative = true;
	//	remainder.isNegative = true;

	//	temp = subtractInteger(quotient, One);
	//	freeInteger(&quotient);
	//	copyInteger(&temp, &quotient);
	//	freeInteger(&temp);

	//	temp = addInteger(remainder, b);
	//	freeInteger(&remainder);
	//	copyInteger(&temp, &remainder);
	//	freeInteger(&temp);
	//	freeInteger(&One);
	//}

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
*/

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
	// Cas triviaux
	if (isZero(exp)) return allocIntegerFromValue(1, false, true);
	if (isZero(base)) return allocIntegerFromValue(0, false, true);

	// --- 1. OPTIMISATION PUISSANCE DE 2 (Améliorée) ---
	SizeT setBitCount = 0;
	SizeT bitPosition = 0;

	// On scanne pour voir si c'est une puissance de 2
	for (SizeT i = 0; i < base.size; i++) {
		uint8 b = base.value[i];
		if (b != 0) {
			// Compte les bits dans cet octet
			for (int k = 0; k < 8; k++) {
				if ((b >> k) & 1) {
					setBitCount++;
					if (setBitCount == 1) bitPosition = i * 8 + k;
				}
			}
			// SI on a trouvé plus d'un bit au total, on arrête TOUT DE SUITE.
			// Inutile de scanner le reste du grand nombre.
			if (setBitCount > 1) break;
		}
	}

	if (setBitCount == 1) {
		// Logique puissance de 2 (identique à avant, mais attention à l'overflow SizeT sur expVal)
		// Note: Si exp est immense, expVal ne tiendra pas dans un SizeT. 
		// Cette optimisation est valide pour des exposants "raisonnables" (< 64 bits).
		if (exp.size > sizeof(SizeT)) {
			// Fallback vers l'algo général si l'exposant est trop grand pour calculer le shift
		} else {
			SizeT expVal = 0;
			for (SizeT i = 0; i < exp.size; i++) {
				expVal |= ((SizeT)exp.value[i]) << (i * 8);
			}

			// Attention: totalShift peut overflow, mais on assume que c'est géré ou que l'alloc échouera
			SizeT totalShift = bitPosition * expVal;

			CustomInteger result = allocIntegerFromValue(1, false, true);
			BitshiftPtr(&result, totalShift, LEFT, true);

			if (base.isNegative && (getBit(exp, 0) == 1)) {
				result.isNegative = true;
			}
			return result;
		}
	}

	// --- 2. ALGORITHME GÉNÉRAL (Square and Multiply - SCAN VERSION) ---

	CustomInteger result = allocIntegerFromValue(1, false, true);
	CustomInteger baseAccumulator = copyIntegerToNew(base); // Représente base^(2^i)

	// Détermination de la limite de bits à scanner
	// On cherche le bit le plus significatif de l'exposant pour ne pas boucler sur des zéros
	SizeT maxBits = exp.size * 8;
	// Petite boucle pour réduire maxBits au strict nécessaire (optionnel mais propre)
	// On pourrait utiliser une fonction getMostSignificantBitIndex()

	for (SizeT i = 0; i < maxBits; i++) {
		// Lecture directe du bit sans modifier l'exposant (O(1))
		if (getBit(exp, i) == 1) {
			CustomInteger newResult = multiplyInteger(result, baseAccumulator);
			freeInteger(&result);
			result = newResult;
		}

		// On prépare la base pour le prochain bit (base = base^2)
		// On ne fait le carré que s'il reste des bits à traiter
		// (Optimisation : éviter la dernière multiplication inutile au dernier tour)
		if (i < maxBits - 1) {
			// Petite vérification pour éviter de continuer si exp n'a plus de bits actifs
			// (Implique de connaître le MSB de exp, sinon on continue jusqu'à exp.size*8)

			CustomInteger newBase = multiplyInteger(baseAccumulator, baseAccumulator);
			freeInteger(&baseAccumulator);
			baseAccumulator = newBase;
		}
	}

	// Nettoyage final
	freeInteger(&baseAccumulator);

	// Gestion du signe : si base négative et exposant impair
	result.isNegative = base.isNegative && (getBit(exp, 0) == 1);

	return result;
}
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