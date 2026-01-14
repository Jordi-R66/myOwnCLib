#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include "variableSizeInt/customInteger.h"

// Codes couleurs pour le terminal
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

// --- Outils de vérification ---

/**
 * Convertit un CustomInteger en int64_t (pour vérification simple)
 * Attention : ne fonctionne que si le nombre tient sur 64 bits.
 */
int64_t customToInt64(CustomInteger ci) {
	int64_t res = 0;
	// On suppose Little Endian pour l'architecture x86/ARM courante
	// et que ci.value est aussi stocké en Little Endian (index 0 = poids faible)

	// Reconstruction de la valeur absolue
	SizeT limit = ci.size > 8 ? 8 : ci.size;
	for (SizeT i = 0; i < limit; i++) {
		res |= ((uint64_t)ci.value[i]) << (i * 8);
	}

	if (ci.isNegative) {
		res = -res;
	}
	return res;
}

/**
 * Affiche le résultat d'un test unitaire
 */
void assertTest(const char* opName, int64_t a, int64_t b, int64_t expected, CustomInteger actualCI) {
	int64_t actual = customToInt64(actualCI);

	if (actual == expected) {
		printf("  %s %s : %" PRId64 " op %" PRId64 " = %" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n",
			COLOR_BLUE, opName, a, b, actual);
	}
	else {
		printf("  %s %s : %" PRId64 " op %" PRId64 "\n", COLOR_RED, opName, a, b);
		printf("     Attendu : %" PRId64 "\n", expected);
		printf("     Reçu    : %" PRId64 " " COLOR_RESET "\n", actual);

		// Affichage debug hexa
		printf("     Dump Hex Reçu: ");
		printInteger(actualCI, HEX, true);
	}
}

// --- Tests Spécifiques ---

void runAdditionTests(int64_t valA, int64_t valB) {
	printf("\n=== TEST ADDITION (+/- %" PRId64 " et +/- %" PRId64 ") ===\n", valA, valB);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			// Génération des signes
			int64_t curA = i ? -valA : valA;
			int64_t curB = j ? -valB : valB;

			// 1. Calcul attendu (CPU standard)
			int64_t expected = curA + curB;

			// 2. Calcul CustomInteger
			CustomInteger cA = allocIntegerFromValue(valA, i, true);
			CustomInteger cB = allocIntegerFromValue(valB, j, true);

			CustomInteger res = addInteger(cA, cB);

			// 3. Vérification
			assertTest("ADD", curA, curB, expected, res);

			freeInteger(&cA); freeInteger(&cB); freeInteger(&res);
		}
	}
}

void runSubtractionTests(int64_t valA, int64_t valB) {
	printf("\n=== TEST SOUSTRACTION (+/- %" PRId64 " et +/- %" PRId64 ") ===\n", valA, valB);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			int64_t curA = i ? -valA : valA;
			int64_t curB = j ? -valB : valB;

			int64_t expected = curA - curB;

			CustomInteger cA = allocIntegerFromValue(valA, i, true);
			CustomInteger cB = allocIntegerFromValue(valB, j, true);

			CustomInteger res = subtractInteger(cA, cB);

			assertTest("SUB", curA, curB, expected, res);

			freeInteger(&cA); freeInteger(&cB); freeInteger(&res);
		}
	}
}

void runMultiplicationTests(int64_t valA, int64_t valB) {
	printf("\n=== TEST MULTIPLICATION (+/- %" PRId64 " et +/- %" PRId64 ") ===\n", valA, valB);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			int64_t curA = i ? -valA : valA;
			int64_t curB = j ? -valB : valB;

			int64_t expected = curA * curB;

			CustomInteger cA = allocIntegerFromValue(valA, i, true);
			CustomInteger cB = allocIntegerFromValue(valB, j, true);

			// Note: Karatsuba sera utilisé automatiquement si la taille est suffisante,
			// ou multiplyNaive sinon. Avec des petits nombres ici, c'est souvent Naive.
			// Pour tester Karatsuba, il faudrait des valeurs > 2^256, mais on teste la logique mathématique ici.
			CustomInteger res = multiplyInteger(cA, cB);

			assertTest("MUL", curA, curB, expected, res);

			freeInteger(&cA); freeInteger(&cB); freeInteger(&res);
		}
	}
}

void runDivisionTests(int64_t valA, int64_t valB) {
	printf("\n=== TEST DIVISION EUCLIDIENNE (Python Style) (+/- %" PRId64 " et +/- %" PRId64 ") ===\n", valA, valB);

	if (valB == 0) {
		printf("Skip division by zero check in automated loop.\n");
		return;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			int64_t curA = i ? -valA : valA;
			int64_t curB = j ? -valB : valB;

			// --- Oracle Python (Division Plancher) ---
			// En C natif : division tronquée vers 0 (ex: -77/12 = -6)
			// En Python  : division plancher vers -inf (ex: -77//12 = -7)

			int64_t expectedQ = curA / curB;
			int64_t expectedR = curA % curB;

			// Ajustement pour correspondre à la logique Python implémentée dans la lib
			// Si le reste n'est pas nul et que les opérandes ont des signes opposés...
			if (expectedR != 0 && ((curA < 0) ^ (curB < 0))) {
				expectedQ--;
				expectedR += curB;
			}

			// --- Test Library ---
			CustomInteger cA = allocIntegerFromValue(valA, i, true);
			CustomInteger cB = allocIntegerFromValue(valB, j, true);

			EuclideanDivision res = euclideanDivInteger(cA, cB);

			// Vérification Quotient
			int64_t actualQ = customToInt64(res.quotient);
			if (actualQ == expectedQ) {
				printf("  %s DIV(Q) : %" PRId64 " / %" PRId64 " = %" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n",
					COLOR_BLUE, curA, curB, actualQ);
			}
			else {
				printf("  %s DIV(Q) : %" PRId64 " / %" PRId64 " " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, curA, curB);
				printf("     Attendu : %" PRId64 "\n", expectedQ);
				printf("     Reçu    : %" PRId64 "\n", actualQ);
			}

			// Vérification Reste
			int64_t actualR = customToInt64(res.remainder);
			if (actualR == expectedR) {
				printf("  %s DIV(R) : %" PRId64 " %% %" PRId64 " = %" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n",
					COLOR_BLUE, curA, curB, actualR);
			}
			else {
				printf("  %s DIV(R) : %" PRId64 " %% %" PRId64 " " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, curA, curB);
				printf("     Attendu : %" PRId64 "\n", expectedR);
				printf("     Reçu    : %" PRId64 "\n", actualR);
			}

			freeInteger(&cA); freeInteger(&cB);
			freeInteger(&res.quotient); freeInteger(&res.remainder);
		}
	}
}

void runPowerTests(int64_t base, int64_t exp) {
	printf("\n=== TEST EXPONENTIATION (+/- %" PRId64 " ^ %" PRId64 ") ===\n", base, exp);

	// Note : On ne teste que des exposants positifs car powInteger(a, -n) retourne 0 pour les entiers.
	// On teste Base Positive et Base Négative.

	for (int i = 0; i < 2; i++) {
		int64_t curBase = i ? -base : base;

		// Oracle
		int64_t expected = 1;
		for (int k = 0; k < exp; k++) expected *= curBase;

		// Library
		CustomInteger cBase = allocIntegerFromValue(base, i, true);
		CustomInteger cExp = allocIntegerFromValue(exp, false, true);

		CustomInteger res = powInteger(cBase, cExp);

		assertTest("POW", curBase, exp, expected, res);

		freeInteger(&cBase); freeInteger(&cExp); freeInteger(&res);
	}
}

// --- Main ---

int main(void) {
	printf("=== DÉBUT DES TESTS UNITAIRES CUSTOMINTEGER ===\n");

	// 1. Tests Basiques (Nombres petits)
	runAdditionTests(42, 17);
	runSubtractionTests(42, 50); // Résultat négatif attendu
	runMultiplicationTests(12, 12);

	// 2. Tests Division (Cas critiques Python)
	// 77 / 12 (Cas classique)
	runDivisionTests(77, 12);
	// 10 / 3 (Cas classique)
	runDivisionTests(10, 3);

	// 3. Tests Puissance (Valeurs raisonnables)
	// 2^10 = 1024
	runPowerTests(2, 10);
	// 5^3 = 125
	runPowerTests(5, 3);
	// Cas limite : Exposant 0
	runPowerTests(1234, 0);

	// 4. Test "Edge Case" Karatsuba Threshold
	// On prend des valeurs assez grandes pour être intéressantes, 
	// mais convertibles en int64 pour la vérification automatique.
	// Max int64 est ~9e18.
	// 1 milliard * 2 milliard tient dans int64.
	runMultiplicationTests(1000000000, 2000000000);

	// 5. Test Spécifique Puissance de 2 (Optimisation Shift)
	// 2^5 = 32. L'optimisation doit marcher.
	runPowerTests(2, 5);
	// 4^3 = 64 (Base puissance de 2 : 4 = 2^2 -> (2^2)^3 = 2^6)
	runPowerTests(4, 3);

	printf("\n=== FIN DES TESTS ===\n");
	return 0;
}