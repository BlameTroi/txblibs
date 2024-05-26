/* txbstr.h -- blametroi's common utility functions -- */
#ifndef TXBSTR_H
#define TXBSTR_H


/*
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this is a header only implementation of various bits of code that i
 * keep repeating in my hobby programming that i want around without
 * the hassle of managing library dependencies.
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */


#ifdef TXBSTR_H_IMPLEMENTATION
#include <string.h>
#endif /* TXBSTR_H_IMPLEMENTATION */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * forward definitions
 */

/* split a string at (runs of) separators

   strdup is used to make a copy of the string and that string is
   then written over by strtok to find and mark the splits.

   returns the copy and splits in an array which is big enough to hold
   a pointer to the copy, pointers to each split, and a trailing NULL
   pointer:

      `copy | tok1 | tok2 | ... | tokn | NULL`

   there can have a few extra NULL pointers tacked on the end but you
   should not count on their presence. this memory should be released
   when no longer needed in two steps:

   free(ret[0]);          free the string copy
   free(ret);             free the spit pointers */

char **
split_string(
   const char *str,           /* string to split */
   const char *sep            /* string of possible separator character */
);

#ifdef TXBSTR_H_IMPLEMENTATION


/*
 * split a string `str` into substrings at any of the characters in
 * `sep`. unlike some split implementations, adjacent separators do
 * not result in empty substrings.
 *
 * internally, a copy of `str` is made with strdup and then is marked
 * by strtok.
 *
 * returns an array of string pointers. the first entry is a pointer
 * to the copy the original string, and subsequent entries up until a
 * NULL entry are the substrings within that copy.
 *
 * the caller is responsible for releasing the copy of `str` and the
 * pointer array.
 */

char **
split_string(const char *str, const char *sep) {

   /* the list of tokens, an array, NULL terminated. these all point
      into a copy of `str` if one is made. */

   char **results = NULL;

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
      allocate space an ending NULL entry and a pointer to the copy
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
      if (tok == NULL) {
         break;
      }
      if (strlen(tok)) {
         results[i + 1] = tok;      /* +1 to skip past cpy */
      }
   }

   /* returns the array which is big enough to hold

      `cpy | tok1 | tok2 | ... | tokn | NULL`

      but may have a few extra NULL pointers tacked on the end. */

   return results;
}

#endif /* TXBSTR_H_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TXBSTR_H */
