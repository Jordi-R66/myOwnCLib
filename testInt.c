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
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

// --- Outils de vérification ---

/**
 * Convertit un CustomInteger en int64_t (pour vérification simple)
 * Attention : ne fonctionne que si le nombre tient sur 64 bits.
 */
int64_t customToInt64(CustomInteger ci) {
	int64_t res = 0;
	SizeT limit = ci.size > 2 ? 2 : ci.size;
	for (SizeT i = 0; i < limit; i++) {
		res |= ((uint64_t)ci.value[i]) << (i * 32);
	}

	if (ci.isNegative) {
		res = -res;
	}
	return res;
}

/**
 * Affiche le résultat d'un test unitaire classique
 */
void assertTest(const char* opName, int64_t a, int64_t b, int64_t expected, CustomInteger actualCI) {
	int64_t actual = customToInt64(actualCI);

	if (actual == expected) {
		printf("  %s %s : %" PRId64 " op %" PRId64 " = %" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n",
			COLOR_BLUE, opName, a, b, actual);
	}
	else {
		printf("  %s %s : %" PRId64 " op %" PRId64 " " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, opName, a, b);
		printf("     Attendu : %" PRId64 "\n", expected);
		printf("     Reçu    : %" PRId64 " " COLOR_RESET "\n", actual);
	}
}

// Oracle pour le PGCD (Algorithme d'Euclide standard sur int64)
int64_t gcd_oracle(int64_t a, int64_t b) {
	if (a < 0) a = -a;
	if (b < 0) b = -b;
	while (b != 0) {
		int64_t temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

// --- Tests Arithmétiques de Base ---

void runAdditionTests(int64_t valA, int64_t valB) {
	printf("\n=== TEST ADDITION (+/- %" PRId64 " et +/- %" PRId64 ") ===\n", valA, valB);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			int64_t curA = i ? -valA : valA;
			int64_t curB = j ? -valB : valB;
			int64_t expected = curA + curB;

			CustomInteger cA = allocIntegerFromValue(valA, i, true);
			CustomInteger cB = allocIntegerFromValue(valB, j, true);
			CustomInteger res = addInteger(cA, cB);

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
			CustomInteger res = multiplyInteger(cA, cB);

			assertTest("MUL", curA, curB, expected, res);
			freeInteger(&cA); freeInteger(&cB); freeInteger(&res);
		}
	}
}

void runDivisionTests(int64_t valA, int64_t valB) {
	printf("\n=== TEST DIVISION EUCLIDIENNE (Python Style) (+/- %" PRId64 " et +/- %" PRId64 ") ===\n", valA, valB);
	if (valB == 0) return;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			int64_t curA = i ? -valA : valA;
			int64_t curB = j ? -valB : valB;

			int64_t expectedQ = curA / curB;
			int64_t expectedR = curA % curB;

			// Ajustement Python
			if (expectedR != 0 && ((curA < 0) ^ (curB < 0))) {
				expectedQ--;
				expectedR += curB;
			}

			CustomInteger cA = allocIntegerFromValue(valA, i, true);
			CustomInteger cB = allocIntegerFromValue(valB, j, true);
			EuclideanDivision res = euclideanDivInteger(cA, cB);

			int64_t actualQ = customToInt64(res.quotient);
			int64_t actualR = customToInt64(res.remainder);

			if (actualQ == expectedQ && actualR == expectedR) {
				printf("  %s DIV : %" PRId64 " / %" PRId64 " = (Q=%" PRId64 ", R=%" PRId64 ") " COLOR_GREEN "[OK]" COLOR_RESET "\n",
					COLOR_BLUE, curA, curB, actualQ, actualR);
			}
			else {
				printf("  %s DIV : %" PRId64 " / %" PRId64 " " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, curA, curB);
				printf("     Attendu : Q=%" PRId64 ", R=%" PRId64 "\n", expectedQ, expectedR);
				printf("     Reçu    : Q=%" PRId64 ", R=%" PRId64 "\n", actualQ, actualR);
			}

			freeInteger(&cA); freeInteger(&cB);
			freeInteger(&res.quotient); freeInteger(&res.remainder);
		}
	}
}

// --- Tests Arithmétiques Avancés ---

void runPowerTests(int64_t base, int64_t exp) {
	printf("\n=== TEST EXPONENTIATION (+/- %" PRId64 " ^ %" PRId64 ") ===\n", base, exp);
	for (int i = 0; i < 2; i++) {
		int64_t curBase = i ? -base : base;

		int64_t expected = 1;
		for (int k = 0; k < exp; k++) expected *= curBase;

		CustomInteger cBase = allocIntegerFromValue(base, i, true);
		CustomInteger cExp = allocIntegerFromValue(exp, false, true);
		CustomInteger res = powInteger(cBase, cExp);

		assertTest("POW", curBase, exp, expected, res);
		freeInteger(&cBase); freeInteger(&cExp); freeInteger(&res);
	}
}

void runGCDTests(int64_t valA, int64_t valB) {
	printf("\n=== TEST PGCD (GCD) (+/- %" PRId64 ", +/- %" PRId64 ") ===\n", valA, valB);
	// Le PGCD est toujours positif mathématiquement, on teste que la lib gère bien les entrées négatives
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			int64_t curA = i ? -valA : valA;
			int64_t curB = j ? -valB : valB;

			int64_t expected = gcd_oracle(curA, curB);

			CustomInteger cA = allocIntegerFromValue(valA, i, true);
			CustomInteger cB = allocIntegerFromValue(valB, j, true);
			CustomInteger res = gcdInteger(cA, cB);

			// Affichage spécifique pour GCD
			int64_t actual = customToInt64(res);
			if (actual == expected) {
				printf("  %s GCD(%" PRId64 ", %" PRId64 ") = %" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n",
					COLOR_BLUE, curA, curB, actual);
			}
			else {
				printf("  %s GCD(%" PRId64 ", %" PRId64 ") " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, curA, curB);
				printf("     Attendu : %" PRId64 "\n", expected);
				printf("     Reçu    : %" PRId64 "\n", actual);
			}

			freeInteger(&cA); freeInteger(&cB); freeInteger(&res);
		}
	}
}

void runExtendedEuclideTests(int64_t a, int64_t b) {
	printf("\n=== TEST EUCLIDE ÉTENDU (Bézout) (%" PRId64 ", %" PRId64 ") ===\n", a, b);

	// On vérifie que a*u + b*v = GCD
	CustomInteger cA = allocIntegerFromValue(abs(a), a < 0, true);
	CustomInteger cB = allocIntegerFromValue(abs(b), b < 0, true);

	Euclide res = ExtendedEuclide(cA, cB);

	int64_t r_act = customToInt64(res.gcd);
	int64_t u_act = customToInt64(res.u);
	int64_t v_act = customToInt64(res.v);

	// Vérification de l'équation de Bézout: a*u + b*v
	int64_t bezout_check = (a * u_act) + (b * v_act);
	int64_t expected_gcd = gcd_oracle(a, b);

	// Vérification du PGCD lui-même
	if (r_act == expected_gcd && bezout_check == r_act) {
		printf("  %s ExtEuclid(%" PRId64 ", %" PRId64 ") -> r=%" PRId64 ", u=%" PRId64 ", v=%" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n",
			COLOR_BLUE, a, b, r_act, u_act, v_act);
		printf("     Check: %" PRId64 "*%" PRId64 " + %" PRId64 "*%" PRId64 " = %" PRId64 "\n", a, u_act, b, v_act, bezout_check);
	}
	else {
		printf("  %s ExtEuclid(%" PRId64 ", %" PRId64 ") " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, a, b);
		printf("     Attendu GCD : %" PRId64 "\n", expected_gcd);
		printf("     Reçu GCD    : %" PRId64 "\n", r_act);
		printf("     Bézout Check: %" PRId64 " (devrait être égal au GCD)\n", bezout_check);
		printf("     u=%" PRId64 ", v=%" PRId64 "\n", u_act, v_act);
	}

	freeInteger(&cA); freeInteger(&cB);
	freeInteger(&res.gcd); freeInteger(&res.u); freeInteger(&res.v);
}

void runModularInverseTests(int64_t a, int64_t m) {
	printf("\n=== TEST INVERSE MODULAIRE (inv(%" PRId64 ") mod %" PRId64 ") ===\n", a, m);

	CustomInteger cA = allocIntegerFromValue(abs(a), a < 0, true);
	CustomInteger cM = allocIntegerFromValue(abs(m), m < 0, true);

	CustomInteger res = modularInverse(cA, cM);
	int64_t actualInv = customToInt64(res);

	// Vérification: (a * inv) % m == 1
	// Attention aux overflow int64 pour la vérification, on utilise __int128 ou on cast
	int64_t check = 0;

	// Si PGCD(a, m) != 1, l'inverse n'existe pas, la fonction doit retourner 0
	int64_t gcd_val = gcd_oracle(a, m);

	if (gcd_val != 1) {
		if (actualInv == 0) {
			printf("  %s Inv(%" PRId64 ", %" PRId64 ") -> Pas d'inverse (GCD=% " PRId64 ") " COLOR_GREEN "[OK]" COLOR_RESET "\n",
				COLOR_BLUE, a, m, gcd_val);
		}
		else {
			printf("  %s Inv(%" PRId64 ", %" PRId64 ") -> Erreur, a retourné %" PRId64 " mais pas d'inverse " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, a, m, actualInv);
		}
	}
	else {
		// Vérification Mathématique
		// On fait le calcul en C standard
		check = (a * actualInv) % m;
		// En C le modulo peut être négatif, on corrige
		if (check < 0) check += m;

		if (check == 1) {
			printf("  %s Inv(%" PRId64 ", %" PRId64 ") = %" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n",
				COLOR_BLUE, a, m, actualInv);
		}
		else {
			printf("  %s Inv(%" PRId64 ", %" PRId64 ") = %" PRId64 " " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED, a, m, actualInv);
			printf("     Check: (%" PRId64 " * %" PRId64 ") %% %" PRId64 " = %" PRId64 " (Attendu: 1)\n", a, actualInv, m, check);
		}
	}

	freeInteger(&cA); freeInteger(&cM); freeInteger(&res);
}

void runModPowTests(int64_t base, int64_t exp, int64_t mod) {
	printf("\n=== TEST MOD POW (%" PRId64 " ^ %" PRId64 " %% %" PRId64 ") ===\n", base, exp, mod);

	// Oracle (boucle naïve)
	int64_t expected = 1;
	int64_t b_red = base % mod;
	if (b_red < 0) b_red += mod;

	for (int i = 0; i < exp; i++) {
		expected = (expected * b_red) % mod;
	}

	CustomInteger cB = allocIntegerFromValue(base, base < 0, true);
	CustomInteger cE = allocIntegerFromValue(exp, false, true);
	CustomInteger cM = allocIntegerFromValue(mod, false, true); // Modulo toujours positif ici

	CustomInteger res = modPowInteger(cB, cE, cM);
	int64_t actual = customToInt64(res);

	if (actual == expected) {
		printf("  %s ModPow = %" PRId64 " " COLOR_GREEN "[OK]" COLOR_RESET "\n", COLOR_BLUE, actual);
	}
	else {
		printf("  %s ModPow " COLOR_RED "[FAIL]" COLOR_RESET "\n", COLOR_RED);
		printf("     Attendu : %" PRId64 "\n", expected);
		printf("     Reçu    : %" PRId64 "\n", actual);
	}

	freeInteger(&cB); freeInteger(&cE); freeInteger(&cM); freeInteger(&res);
}

// --- Main ---

int main(void) {
	printf("=== DÉBUT DES TESTS UNITAIRES CUSTOMINTEGER ===\n");

	// 1. Tests Arithmétiques de base
	runAdditionTests(42, 17);
	runSubtractionTests(42, 50);
	runMultiplicationTests(12, 12);
	runMultiplicationTests(1000000, 2000000); // Teste Karatsuba
	runDivisionTests(77, 12);
	runDivisionTests(10, 3);
	runPowerTests(2, 10); // 1024
	runPowerTests(5, 3);  // 125

	// 2. Tests GCD
	runGCDTests(48, 18);  // 6
	runGCDTests(101, 10); // 1 (Co-premiers)
	runGCDTests(100, 25); // 25
	runGCDTests(0, 50);   // 50

	// 3. Tests Euclide Étendu (Bézout)
	// 35u + 15v = 5 -> u=1, v=-2 (35 - 30 = 5) par exemple
	runExtendedEuclideTests(35, 15);
	// 13u + 11v = 1 (Co-premiers)
	runExtendedEuclideTests(13, 11);

	// 4. Tests Inverse Modulaire
	// 3 * 4 = 12 = 1 mod 11 -> Inv(3, 11) = 4
	runModularInverseTests(3, 11);
	// 10 * x = 1 mod 17 -> Inv(10, 17) = 12 (120 = 7*17 + 1)
	runModularInverseTests(10, 17);
	// Pas d'inverse (2 et 4 ne sont pas premiers entre eux)
	runModularInverseTests(2, 4);

	// 5. Tests Mod Pow (RSA style mais petit)
	// 2^10 mod 1000 = 1024 mod 1000 = 24
	runModPowTests(2, 10, 1000);
	// 123^5 mod 19
	runModPowTests(123, 5, 19);
	// Cas limite : Modulo 1 -> Toujours 0
	runModPowTests(12345, 999, 1);
	runModPowTests(7, 28, 11);

	printf("\n=== FIN DES TESTS ===\n");
	return 0;
}