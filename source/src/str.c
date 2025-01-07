/* txbstr.c -- String helpers -- Brumley BlameTroi@gmail.com */

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
#include <stdlib.h>
#include <string.h>

#include "../inc/str.h"

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
	const char *str,
	const char *sep
) {

	/* the list of tokens, an array, NULL terminated. these all point
	   into a copy of `str` if one is made. */

	const char **results = NULL;

	/*
	 * guards for special cases:
	 */

	/* no string, nothing but NULLs back */
	if (str == NULL) {
		results = calloc(3, sizeof(char *));
		return results;
	}

	/* empty string, a copy of same */
	if (strlen(str) == 0) {
		results = calloc(3, sizeof(char *));
		results[0] = strdup(str);
		results[1] = results[0];
		return results;
	}

	/* empty separators, same as empty string */
	if (sep == NULL || strlen(sep) == 0) {
		results = calloc(3, sizeof(char *));
		results[0] = strdup(str);
		results[1] = results[0];
		return results;
	}

	/*
	 * we have work to do:
	 */

	/* a copy of the whole string, strtok will break into multiple
	   substrings with NULs. */

	char *cpy = strdup(str);

	/* how many tokens can we have at a maximum? adjacent separators
	   count as one separator, but we don't care at this point. over
	   allocation of the token list isn't a concern.

	   a single word with no separators requires two slots, and we
	   allocate space an ending NULL item and a pointer to the copy
	   of the original string. */

	int maxTokens = 2 + 1 + 1;
	char *pos = cpy;
	while (*pos) {
		const char *c = sep;
		while (*c) {
			if (*pos == *c) {
				maxTokens += 1;
				break;
			}
			c += 1;
		}
		pos += 1;
	}

	/* reserve space and stash the address of the copied string
	   for future freeing. */

	results = calloc(maxTokens, sizeof(char *));
	results[0] = cpy;

	/* split and remember in the list. */

	char *tok;                      /* current token */
	for (int i = 0; i < maxTokens; i++) {
		tok = strtok((i == 0 ? cpy : NULL), sep);
		if (tok == NULL)
			break;
		if (strlen(tok)) {
			results[i + 1] = tok;      /* +1 to skip past cpy */
		}
	}

	/* returns the array which is big enough to hold

	   `cpy | tok1 | tok2 | ... | tokn | NULL`

	   but may have a few extra NULL pointers tacked on the end. */

	return results;
}

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
) {
	free((void *)splits[0]);
	free(splits);
}

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
) {
	int n = 0;
	while (*str) {
		if (*str == c)
			n += 1;
		str += 1;
	}
	return n;
}

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
) {
	if (pos > strlen(str))
		return -1;
	while (str[pos]) {
		if (str[pos] == c)
			return pos;
		pos += 1;
	}
	return -1;
}

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
) {
	return a != NULL && b != NULL && strcmp(a, b) == 0;
}

bool
less_than_string(
	const char *a,
	const char *b
) {
	return a != NULL && b != NULL && strcmp(a, b) < 0;
}

bool
greater_than_string(
	const char *a,
	const char *b
) {
	return a != NULL && b != NULL && strcmp(a, b) > 0;
}

/* txbstr.c ends here */
