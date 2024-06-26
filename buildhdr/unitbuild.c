/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "minunit.h"

#include "txbabort.h"

#include "txbmisc.h"

#define TXBSTR_H_IMPLEMENTATION
#include "txbstr.h"

#include "txbpat.h"

#include "buildhdr.h"

/*
 * common test data
 */

char *filenames[] = {
   "README.org",                  /* test name.* */
   "README.md",
   "unitteset.c",                 /* for various, like *.c */
   "lib.h",                       /* ???.h */
   "lib.c",
   "mem.h",
   "mem.c",                       /* ???.c */
   "makefile",
   "LICENSE",
   "file01.c",
   "file02.c",
   "file11.c",
   "file12.c",
   ".gitignore",                  /* .* */
   NULL
};

char *somecmd = "./a.out --macro TXBTEST --intro boilerpalte/proglog.txt licenses/unlicense.txt --pub defs/*.h $QAHOME/debug.h --priv defs/*.c $QAHOME/debug.c --outro boilerplate/epilog.txt";

char *someargv[] = {
   "./a.out",
   "--macro",
   "TXBTEST",
   "--intro",
   "boilerplate/prolog.txt",
   "licenses/unlicense.txt",
   "--pub",
   "defs/a.h",
   "defs/b.h",
   "defs/c.h",
   "/Users/qa/debug.h",
   "--priv",
   "defs/a.c",
   "defs/b.c",
   "defs/c.c",
   "/Users/qa/debug.c",
   "--outro",
   "boilerplate/epilog.txt",
   NULL,
};

int someargc = sizeof(someargv) / sizeof(char *) - 1;

/*
 * minunit infrastructure
 */

void
test_setup(void) {

}

void
test_teardown(void) {

}

/*
 * tests
 */

MU_TEST(test_stuff) {
   DIR *dirp = opendir(".");
   if (dirp == NULL) {
      printf("could not open '.'\n");
   }
   bool found = false;
   const char *glob = "*.c";
   const char *gexp = convert_glob(glob);
   const cpat_t *gpat = compile_pattern(gexp);
   struct dirent *dp;
   while ((dp = readdir(dirp)) != NULL) {
      printf("%s\n", dp->d_name);
      if (glob_match(dp->d_name, gpat)) {
         printf("found %s for glob %s regexp %s\n", dp->d_name, glob, pattern_source(gpat));
         found = true;
      }
   }
   closedir(dirp);
   mu_assert_int_eq(true, found);
}

/*
 * test list of files functions
 */

bool
is_glob(const char *str) {
   return str ? strchr(str, '*') || strchr(str, '?') || strchr(str, '[') : false;
}

bool
is_abs_path(const char *str) {
   return  str ? str[0] == '/' || str[0] == '$' || str[0] == '~' : false;
}

bool
is_dashed(const char *str) {
   return str[0] == '-' && str[1] == '-';
}

#define PATH_SEP '/'
#define MAX_CHUNKS 16

/*
 * is a string a possible long option name (--text)?
 */

bool
is_longopt(const char *str) {
   return str && strlen(str) > 2 && str[0] == '-' && str[1] == '-';
}

/*
 * is a string a possible end of argment flag (--)?
 */

bool
is_endarg(const char *str) {
   return str && strlen(str) == 2 && str[0] == '-' && str[1] == '-';
}

/*
 * return the position of the first value after a long option argument
 * in the arguments array. long option must be specified as --optname.
 * scanning stops and the end argument marker --.
 *
 * returns -1 if the option is not found.
 *
 * can return the index of the NULL at the end of argv if the last
 * argument is --optname.
 */

int
get_longopt(const char *argv[], const char *str) {
   if (argv == NULL || str == NULL || *str == '\0' || !is_longopt(str)) {
      return -1;
   }
   int i = 0;
   while (argv[i] && !is_endarg(argv[i])) {
      if (strcmp(str, argv[i]) == 0) {
         return i+1;
      }
      i += 1;
   }
   return -1;
}









MU_TEST(test_parse_args) {
   for (int i = 0; i < someargc; i++) {
      printf("%2d\t%s\n", i, someargv[i]);
   }

   mu_assert_int_eq(2, get_longopt((const char **)someargv, "--macro"));
   mu_assert_string_eq("TXBTEST", someargv[get_longopt((const char **)someargv, "--macro")]);

}


MU_TEST(test_get_masks) {
   const cpat_t *pat = NULL;
   const char *filearg = NULL;
   const char **masks = NULL;

   filearg = strdup("hdr/*.h,src/*.c,main.c,data.c,../examples/*.h,$SRC/*.h");
   masks = split_string(filearg, ",");

   mu_assert_string_eq("hdr/*.h", masks[1]);
   mu_assert_string_eq("src/*.c", masks[2]);
   mu_assert_string_eq("main.c", masks[3]);
   mu_assert_string_eq("data.c", masks[4]);
   mu_assert_string_eq("../examples/*.h", masks[5]);
   mu_assert_string_eq("$SRC/*.h", masks[6]);
   mu_shouldnt(masks[7]);

   mu_should(is_glob(masks[1]));
   mu_should(is_glob(masks[2]));
   mu_shouldnt(is_glob(masks[3]));
   mu_shouldnt(is_glob(masks[4]));

   mu_shouldnt(is_abs_path(masks[1]));
   mu_shouldnt(is_abs_path(masks[2]));



   free((void *)pat);
   free((void *)filearg);
   free((void *)masks[0]);
   free((void *)masks);
}

/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */

MU_TEST_SUITE(test_suite) {

   /* always have a setup and teardown, even if they do nothing. */

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */

   MU_RUN_TEST(test_stuff);
   MU_RUN_TEST(test_get_masks);
   MU_RUN_TEST(test_parse_args);
}

/*
 * master control:
 */

int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
