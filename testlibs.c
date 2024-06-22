/* testlibs.c -- troy brumley */

/* test the header only library packaging. */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TXBMISC_H_IMPLEMENTATION
#include "txbmisc.h"
#define TXBSTR_H_IMPLEMENTATION
#include "txbstr.h"
#define TXBPMUTE_H_IMPLEMENTATION
#include "txbpmute.h"


/* logging */

void
print_current(int *iter, int n, int *ints) {
   *iter += 1;
   printf("%6d: ", *iter);
   for (int i = 0; i < n; i++) {
      printf("%1d", ints[i]);
   }
   printf("\n");
}


/* permutation driver, iterative */

void
permute(const int n) {

   int *ints = (int *)calloc(n, sizeof(int));
   assert(ints);

   for (int i = 0; i < n; i++) {
      ints[i] = i;
   }

   int iter = 0;
   print_current(&iter, n, ints);
   while (permute_next(n, ints)) {
      print_current(&iter, n, ints);
   }

   free(ints);

}



/***main function***/

int
main() {

   printf("permuting 8\n");
   permute(8);

   printf("the max of 1,2 is %d\n", max(1, 2));
   printf("the min of 2,1 is %d\n", min(2, 1));

   char *s = "this is a test string";
   printf("splitting '%s'\n", s);

   const char **splits = split_string(s, " ");
   int i = 1;
   while (splits[i]) {
      printf("'%s'\n", splits[i]);
      i += 1;
   }

   free((void *)splits[0]);
   free(splits);

   return 0;

}
