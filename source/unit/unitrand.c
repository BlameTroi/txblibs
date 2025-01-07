/* unitrand.c -- tests for the random numbers header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include "minunit.h"
#include "../inc/rand.h"
#include "../inc/misc.h"
#include <stdio.h>
#include <stdint.h>

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * basic functionality tests.
 */

MU_TEST(test_rand) {

	/* select and seed rand() */
	mu_should(set_random_generator(RAND_DEFAULT));
	mu_should(seed_random_generator(RAND_SEED));

	/* select and attempt to seed arc4random() */
	mu_should(set_random_generator(RAND_RANDOM));
	mu_shouldnt(seed_random_generator(RAND_SEED));

	/* just beat up the arc generator */
	int n100 = 0;
	int n1000 = 0;
	int counts[1001];
	memset(counts, 0, sizeof(counts));
	for (int i = 0; i < 100000; i++) {
		uint32_t r = random_between(100, 1000);
		if (r < 100 || r > 1000) mu_shouldnt(true);
		if (r == 100) {
			mu_should(true);
			n100 += 1;
		}
		if (r == 1000) {
			mu_should(true);
			n1000 += 1;
		}
		counts[r] += 1;
	}
	printf("\n100 -> %d     1000 -> %d\n", n100, n1000);
	printf("\n");
	for (int i = 100; i <= 1000; i++)
		printf("%4d %6d\n", i, counts[i]);

	/* now switch to rand() and see if we're back */
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
	uint32_t first[10000];
	uint32_t last[10000];
	memset(first, 0, sizeof(first));
	memset(last, 0, sizeof(last));
	for (int i = 0; i < 10000; i++)
		first[i] = random_between(100, 99999);
	seed_random_generator(RAND_SEED);
	for (int i = 0; i < 10000; i++)
		last[i] = random_between(100, 99999);
	mu_should(memcmp(first, last, sizeof(first)) == 0);
}


/*
 * #define RAND_CHAR_LOWER (1 << 0)
 * #define RAND_CHAR_UPPER (1 << 1)
 * #define RAND_CHAR_DIGIT (1 << 2)
 * #define RAND_CHAR_SPECIAL (1 << 3)
 */
#define NUM_LOWERS 26
#define NUM_UPPERS 26
#define NUM_DIGITS 10
#define NUM_SPECIALS 31
/*
 * char
 * random_lower(void);
 * char
 * random_upper(void);
 * char
 * random_digit(void);
 * char
 * random_special(void);
 * int
 * random_dice(int num, int sides);
 * char
 * random_character_from(int pool);
 */

MU_TEST(test_beater_lower) {
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);

	int lowers[26];
	for (int i = 0; i < 26; i++)
		lowers[i] = 0;

	for (int i = 0; i < 10000; i++) {
		int c = random_character_from(RAND_CHAR_LOWER);
		/* printf("%c", c); */
		lowers[c - 'a'] += 1;
	}
	int fewest = 100001;
	int mostest = -1;
	int sum = 0;
	for (int i = 0; i < 26; i++) {
		if (lowers[i] < fewest) fewest = lowers[i];
		if (lowers[i] > mostest) mostest = lowers[i];
		sum += lowers[i];
		printf("%4d %4d\n", i, lowers[i]);
	}
	printf("\nsum %d fewest %d mostest %d avg %d\n", sum, fewest, mostest,
		sum / 26);

	mu_should(true);
}

MU_TEST(test_beater_upper) {
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);

	int uppers[26];
	for (int i = 0; i < 26; i++)
		uppers[i] = 0;

	for (int i = 0; i < 10000; i++) {
		int c = random_character_from(RAND_CHAR_UPPER);
		/*printf("%c", c);*/
		uppers[c - 'A'] += 1;
	}
	int fewest = 100001;
	int mostest = -1;
	int sum = 0;
	for (int i = 0; i < 26; i++) {
		if (uppers[i] < fewest) fewest = uppers[i];
		if (uppers[i] > mostest) mostest = uppers[i];
		sum += uppers[i];
		printf("%4d %4d\n", i, uppers[i]);
	}
	printf("\nsum %d fewest %d mostest %d avg %d\n", sum, fewest, mostest,
		sum / 26);

	mu_should(true);
}

MU_TEST(test_beater_letters) {
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);

	int lowers_and_uppers[52];
	for (int i = 0; i < 52; i++)
		lowers_and_uppers[i] = 0;

	for (int i = 0; i < 10000; i++) {
		int c = random_character_from(RAND_CHAR_LOWER | RAND_CHAR_UPPER);
		/*printf("%c", c);*/
		if (is_uppercase(c))
			lowers_and_uppers[c - 'A'] += 1;
		else if (is_lowercase(c))
			lowers_and_uppers[26 + c - 'a'] += 1;
		else
			printf("\nsay what? %d\n", c);
	}
	int fewest = 100001;
	int mostest = -1;
	int sum = 0;
	for (int i = 0; i < 52; i++) {
		if (lowers_and_uppers[i] < fewest) fewest = lowers_and_uppers[i];
		if (lowers_and_uppers[i] > mostest) mostest = lowers_and_uppers[i];
		sum += lowers_and_uppers[i];
		printf("%4d %4d\n", i, lowers_and_uppers[i]);
	}
	printf("\nsum %d fewest %d mostest %d avg %d\n", sum, fewest, mostest,
		sum / 26);

	mu_should(true);
}

MU_TEST(test_abusive) {
	set_random_generator(RAND_DEFAULT);
	set_random_generator(RAND_SEED);
	int glyphs[128] = {0};
	for (int i = 0; i < 1000000; i++) {
		char g = random_character_from(RAND_CHAR_DIGIT | RAND_CHAR_LOWER |
				RAND_CHAR_UPPER | RAND_CHAR_SPECIAL);
		/*printf("%c", g);*/
		glyphs[(int)g] += 1;
	}
	printf("\n");
	for (int i = 0; i < 128; i++)
		printf("%3d %7d %2x %c\n", i, glyphs[i], i, i);
	printf("\n");
}


MU_TEST(test_extensions) {
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
	int digits[10];
	for (int i = 0; i < 10; i++)
		digits[i] = 0;
	for (int i = 0; i < 100000; i++) {
		int dig = random_digit() - '0';
		digits[dig] += 1;
	}
	printf("\n");
	int sum = 0;
	for (int i = 0; i < 10; i++) {
		printf("%2d %4d\n", i, digits[i]);
		sum += digits[i];
	}
	mu_should(sum == 100000);
	printf("%d\n", sum);
	int sides = 6; int dice = 3;
	int rolls[(sides * dice) + 1];
	sum = 0;
	for (int i = 0; i <= sides * dice; i++)
		rolls[i] = 0;
	for (int i = 0; i < 100000; i++)
		rolls[random_dice(dice, sides)] += 1;
	for (int i = 0; i <= sides * dice; i++) {
		printf("%3d %5d\n", i, rolls[i]);
		sum += rolls[i];
	}
	printf("%d\n", sum);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_rand);
	MU_RUN_TEST(test_extensions);
	MU_RUN_TEST(test_beater_lower);
	MU_RUN_TEST(test_beater_upper);
	MU_RUN_TEST(test_beater_letters);
	MU_RUN_TEST(test_abusive);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitrand.c ends here */
