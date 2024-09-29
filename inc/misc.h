/* misc.h -- blametroi's common utility functions -- */

/*
 * this is a header only implementation of various bits of code that i
 * keep repeating in my hobby programming that i want around without
 * the hassle of managing library dependencies. all functions are
 * small and i think pretty obvious.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * return an array of the factors of n.
 */

long *
factors_of(long n);

/*
 * various min/max functions. the typed functions are wrapped by
 * generic macros. the macros only type check the first argument to
 * determine which function to call. it's the client's responsibility
 * to ensure that the arguments compatible.
 */

int             i_max(int, int);
int             i_min(int, int);
long            l_max(long, long);
long            l_min(long, long);
unsigned int    ui_max(unsigned int, unsigned int);
unsigned int    ui_min(unsigned int, unsigned int);
unsigned long   ul_max(unsigned long, unsigned long);
unsigned long   ul_min(unsigned long, unsigned long);
float           f_max(float, float);
float           f_min(float, float);
double          d_max(double, double);
double          d_min(double, double);

#define max(X, Y) _Generic((X), \
	double: d_max, \
	float: f_max, \
	int: i_max, \
	long: l_max, \
	unsigned int: ui_max, \
	unsigned long: ul_max, \
	default: i_max \
	)(X, Y)

#define min(X, Y) _Generic((X), \
	double: d_min, \
	float: f_min, \
	int: i_min, \
	long: l_min, \
	unsigned int: ui_min, \
	unsigned long: ul_min, \
	default: i_min \
	)(X, Y)

/*
 * common predicates.
 */

bool
is_even(
	long
);
bool
is_odd(
	long
);

bool
is_digit(
	char
);
bool
is_word_char(
	char
);

bool
is_whitespace(
	char
);

bool
is_control(
	char
);

bool
is_punctuation(
	char
);

bool
is_bracketing(
	char
);

bool
is_lowercase(
	char
);

bool
is_uppercase(
	char
);

/*
 * how many bits are on in an unsigned long?
 */

int
one_bits_in(
	unsigned long
);

/*
 * sum the integers from 1 to n.
 */

int
sum_one_to(
	int n
);

/*
 * some common comparator functions for things like qsort.
 */

int
fn_cmp_int_asc(
	const void *,
	const void *
);

int
fn_cmp_int_dsc(
	const void *,
	const void *
);

/*
 * pack and unpack hex digits.
 */

uint8_t *
hex_pack(
	uint8_t *hex,        /* address of output buffer */
	int hexlen,          /* maximum length of output buffer, must be at least 1/2 of chrlen */
	char *chr,           /* address of string of hex digits */
	int chrlen           /* number of digits to pack, must be even */
);

char *
hex_unpack(
	char *chr,           /* address of output buffer */
	int chrlen,          /* maximum length of output buffer, must be at least 2*hexlen+1 */
	uint8_t *hex,        /* address of first byte to unpack */
	int hexlen           /* number of bytes to unpack */
);


#ifdef __cplusplus
}
#endif /* __cplusplus */
/* misc.h ends here */
