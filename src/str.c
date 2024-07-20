/* str.c -- blametroi's common utility functions -- */

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

#include <stdlib.h>
#include <string.h>

#include "../inc/str.h"

/*
 * split a string `str` into substrings at any of the characters in
 * `sep`. unlike some split implementations, adjacent separators do
 * not result in empty substrings.
 *
 * internally, a copy of `str` is made with dup_string and then is marked
 * by strtok.
 *
 * returns an array of string pointers. the first entry is a pointer
 * to the copy the original string, and subsequent entries up until a
 * NULL entry are the substrings within that copy.
 *
 * the caller is responsible for releasing the copy of `str` and the
 * pointer array.
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
      results[0] = dup_string(str);
      results[1] = results[0];
      return results;
   }

   /* empty separators, same as empty string */
   if (sep == NULL || strlen(sep) == 0) {
      results = calloc(3, sizeof(char *));
      results[0] = dup_string(str);
      results[1] = results[0];
      return results;
   }

   /*
    * we have work to do:
    */

   /* a copy of the whole string, strtok will break into multiple
      substrings with NULs. */

   char *cpy = dup_string(str);

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

/*
 * free the block returned by split_string
 */

void
free_split(
   const char **splits        /* splits[0] & splits are both malloced */
) {
   free((void *)splits[0]);
   free(splits);
}

/*
 * i just learned that strdup is not standard. another silly seeming
 * omission in c.
 */

char *
dup_string(
   const char *str
) {
   size_t len = strlen(str) + 1;
   char *dup = malloc(len);
   strcpy(dup, str);
   return dup;
}

int
count_char(
   const char *str,
   char c
) {
   int n = 0;
   while (*str) {
      if (*str == c) {
         n += 1;
      }
      str += 1;
   }
   return n;
}

int
pos_char(
   const char *str,
   int pos,
   char c
) {
   if (pos > strlen(str)) {
      return -1;
   }
   while (str[pos]) {
      if (str[pos] == c) {
         return pos;
      }
      pos += 1;
   }
   return -1;
}
