/* txbstr.h -- String helpers -- Brumley BlameTroi@gmail.com */

/*
 * this is a header only implementation of a split string function
 * that works the way i think such a function should. other bits of
 * character and string manipulation support will be added as
 * needed.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * split_string
 *
 * split a string into substrings at runs of any of the separator
 * characters. unlike some split implementations, adjacent separators
 * do not result in empty substrings.
 *
 *     in: a string to split
 *
 *     in: a string. each character in this string is considered a
 *         separator
 *
 * return: a NULL terminated array of strings
 *
 * strdup is used to make a copy of the string and that string is
 * then written over by strtok to find and mark the splits.
 *
 * returns the copy and splits in an array which is big enough to hold
 * a pointer to the copy, pointers to each split, and a trailing NULL
 * pointer:
 *
 *    `copy | tok1 | tok2 | ... | tokn | NULL`
 *
 * there can be a few extra NULL pointers tacked on the end but you
 * should not count on their presence. this memory should be released
 * when no longer needed in two steps:
 *
 * free(ret[0]);          free the string copy
 * free(ret);             free the spit pointers
 *
 * or better yet, call free_split(ret).
 */

const char **
split_string(
	const char *str,           /* string to split */
	const char *sep            /* string of possible separator character */
);

/*
 * free_split
 *
 * free the block returned by split_string.
 *
 *     in: the string array returned by split_string
 *
 * return: nothing
 */

void
free_split(
	const char **splits        /* splits[0] & splits are both malloced */
);

/*
 * count_char
 *
 * count how many times a character occurs in a string.
 *
 *     in: string
 *
 *     in: character
 *
 * return: int
 */

int
count_char(
	const char *str,
	char c
);

/*
 * pos_char
 *
 * returns the position of the next occurance of a character in a
 * string starting from some position in the string.
 *
 *     in: string to scan
 *
 *     in: int start scanning from this index
 *
 *     in: char to find
 *
 * return: index of character found or -1
 */

int
pos_char(
	const char *str,
	int pos,
	char c
);

/*
 * equal_string less_than_string greater_than_string
 *
 * wrappers for strcmp, i find strcmp <=> 0 to be flow breaking.
 *
 *     in: string
 *
 *     in: string
 *
 * return: bool is the first string equal/less than/greater than
 * second string
 */

bool
equal_string(
	const char *a,
	const char *b
);

bool
less_than_string(
	const char *a,
	const char *b
);

bool
greater_than_string(
	const char *a,
	const char *b
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* txbstr.h ends here */
