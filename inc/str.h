/* str.h -- blametroi's common utility functions -- */

/*
 * this is a header only implementation of a split string function
 * that works the way i think such a functino should. other bits of
 * character and string manipulation support will be added as
 * needed.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * split a string at (runs of) separators
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
 * there can have a few extra NULL pointers tacked on the end but you
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
 * free the block returned by split_string
 */

void
free_split(
   const char **splits        /* splits[0] & splits are both malloced */
);

/*
 * strdup is not standard, wtf?
 */

char *
dup_string(
   const char *str
);

/*
 * count how many times a character occurs in a string
 */

int
count_char(
   const char *str,
   char c
);

/*
 * return the position of the next occurance of c in str starting
 * at pos.
 */

int
pos_char(
   const char *str,
   int pos,
   char c
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
