/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../inc/str.h"
#include "../inc/abort.h"
#include "../inc/misc.h"
#include "../inc/pat.h"

/*
 * the validate_compiled_pattern is a 'hidden' function in txbpat. it
 * can be used to test the construction of various patterns. it
 * compares the pattern items between PAT_BEG and PAT_END in the
 * pattern buffer against the array of expected values. the expected
 * values array needs an ending sentinel value of -1.
 */

bool
validate_compiled_pattern(const cpat_t *, int *);

/*
 * a sample directory for glob testing:
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

/*
 * reset and release context for tests.
 */

void
test_setup(void) {
   debug_off(NULL);
}

void
test_teardown(void) {
   debug_off(NULL);
}


/*
 * test the compilation of literals, runs of one or more characters.
 * meta characters can passed as literals by escaping them or if they
 * are out of position. an example is the begin and end of line
 * anchors, which can only be the first and last characters of a
 * pattern string. anywhere else, and they are treated as literals.
 */

MU_TEST(test_compile_literals) {
   const cpat_t *pat = NULL;
   int *val = NULL;

   /* various length literals */

   pat = compile_pattern("a");
   val = (int[]) {
      PAT_LIT, 1, 'a', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   debug_off(NULL);
   pat = compile_pattern("ab");
   val = (int[]) {
      PAT_LIT, 1, 'a', PAT_LIT, 1, 'b', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);
   debug_off(NULL);
   pat = compile_pattern("simple");
   val = (int[]) {
      PAT_LIT, 1, 's',
      PAT_LIT, 1, 'i',
      PAT_LIT, 1, 'm',
      PAT_LIT, 1, 'p',
      PAT_LIT, 1, 'l',
      PAT_LIT, 1, 'e', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* c escape characters in literals */

   pat = compile_pattern("a\ts\nd\ff");
   val = (int[]) {
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, '\t',
      PAT_LIT, 1, 's',
      PAT_LIT, 1, '\n',
      PAT_LIT, 1, 'd',
      PAT_LIT, 1, '\f',
      PAT_LIT, 1, 'f', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* escaping meta characters */

   pat = compile_pattern("\\[^abcd\\]");
   val = (int[]) {
      PAT_LIT, 1, '[',
      PAT_LIT, 1, '^',
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, 'b',
      PAT_LIT, 1, 'c',
      PAT_LIT, 1, 'd',
      PAT_LIT, 1, ']', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* some meta characters lose their meaning in different positions */

   pat = compile_pattern("a^$b");
   val = (int[]) {
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, '^',
      PAT_LIT, 1, '$',
      PAT_LIT, 1, 'b', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("$^");
   val = (int[]) {
      PAT_LIT, 1, '$',
      PAT_LIT, 1, '^', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* meta bol and eol out of position so should be literals */

   pat = compile_pattern("as^df$q$");
   val = (int[]) {
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, 's',
      PAT_LIT, 1, '^',
      PAT_LIT, 1, 'd',
      PAT_LIT, 1, 'f',
      PAT_LIT, 1, '$',
      PAT_LIT, 1, 'q',
      PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* and you have to work to get a backslash literal */

   pat = compile_pattern("\\\\");
   val = (int[]) {
      PAT_LIT, 1, '\\', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);
}

/*
 * there are quite a few meta characters. the syntax used is a subset
 * of modern regular expressions.
 */

MU_TEST(test_compile_metas) {
   const cpat_t *pat = NULL;
   int *val = NULL;

   /* line anchors */

   pat = compile_pattern("^");
   val = (int[]) {
      PAT_BOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("$");
   val = (int[]) {
      PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("^$");
   val = (int[]) {
      PAT_BOL, PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("^.$");
   val = (int[]) {
      PAT_BOL, PAT_WILD, PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("^a$");
   val = (int[]) {
      PAT_BOL, PAT_LIT, 1, 'a', PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* any one character */

   pat = compile_pattern(".");
   val = (int[]) {
      PAT_WILD, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("..");
   val = (int[]) {
      PAT_WILD, PAT_WILD, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("\\..");
   val = (int[]) {
      PAT_LIT, 1, '.', PAT_WILD, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* one of, none of */

   pat = compile_pattern("[asdf]");
   val = (int[]) {
      PAT_CCLASS, 4, 'a', 's', 'd', 'f',  -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("[^asdf]");
   val = (int[]) {
      PAT_NOT_CCLASS, 4, 'a', 's', 'd', 'f',  -1
   };
   free((void *)pat);

   /* special characters within character class groups don't have
      special meaning, but rememder that backslash gums up the works
      anyway. */

   pat = compile_pattern("[^^?$.#(){}\\\\<>]");
   val = (int[]) {
      PAT_NOT_CCLASS, 12, '^', '?', '$', '.',  '#', '(', ')', '{', '}', '\\', '<', '>',  -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* escape a group closing ] ? */

   pat = compile_pattern("[a\\]]");
   val = (int[]) {
      PAT_CCLASS, 2, 'a', ']',  -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("[\\]]");
   val = (int[]) {
      PAT_CCLASS, 1, ']',  -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* multiple x of groups alternating with literal runs */

   pat = compile_pattern("[Mm]ac[Oo]s");
   val = (int[]) {
      PAT_CCLASS, 2, 'M', 'm',
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, 'c',
      PAT_CCLASS, 2, 'O', 'o',
      PAT_LIT, 1, 's', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* two adjacent one ofs */

   pat = compile_pattern("^[ab][cd]e$");
   val = (int[]) {
      PAT_BOL, PAT_CCLASS, 2, 'a', 'b',  PAT_CCLASS, 2, 'c', 'd',  PAT_LIT, 1, 'e', PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);
}

/*
 * one of and none of groupings can contain range specifications to
 * reduce typing. <first char>-<last char> is expanded in the
 * grouping, so 'a-f' becomes 'abcdef'.
 */

MU_TEST(test_compile_ranges) {
   const cpat_t *pat = NULL;
   int *val = NULL;

   /* a big range */

   pat = compile_pattern("[a-z]");
   val = (int[]) {
      PAT_CCLASS, 26, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
      'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* multiple ranges in an of */

   pat = compile_pattern("as[d-f0-3][qw]erty");
   val = (int[]) {
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, 's',
      PAT_CCLASS, 7, 'd', 'e', 'f', '0', '1', '2', '3',
      PAT_CCLASS, 2, 'q', 'w',
      PAT_LIT, 1, 'e',
      PAT_LIT, 1, 'r',
      PAT_LIT, 1, 't',
      PAT_LIT, 1, 'y',
      -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* two ranges, one not needed */

   pat = compile_pattern("[abc-fghi-jk]");
   val = (int[]) {
      PAT_CCLASS, 11, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',  -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* bad range */

   pat = compile_pattern("[^abc-b]");
   val = (int[]) {
      PAT_NOT_CCLASS, 4, 'a', 'b', 'c', 'b',  -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);
}

/*
 * character class metas are shortcuts for ranged one/none of
 * groupings. so \d means [0-9], \D means [^0-9]. similarly \w and \W
 * are for word characters (latin alphabet and _), and \s and \S are
 * for whitespace characters.
 */

MU_TEST(test_compile_class_metas) {
   const cpat_t *pat = NULL;
   int *val = NULL;

   pat = compile_pattern("ibm s/3\\d0");
   val = (int[]) {
      PAT_LIT, 1, 'i',
      PAT_LIT, 1, 'b',
      PAT_LIT, 1, 'm',
      PAT_LIT, 1, ' ',
      PAT_LIT, 1, 's',
      PAT_LIT, 1, '/',
      PAT_LIT, 1, '3',
      PAT_DIG,
      PAT_LIT, 1, '0', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("\\w = \\w [+\\-*/] \\w");
   val = (int[]) {
      PAT_WC,
      PAT_LIT, 1, ' ',
      PAT_LIT, 1, '=',
      PAT_LIT, 1, ' ',
      PAT_WC,
      PAT_LIT, 1, ' ',
      PAT_CCLASS, 4, '+', '-', '*', '/',
      PAT_LIT, 1, ' ',
      PAT_WC, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("\\W\\w\\S\\s\\D\\d");
   val = (int[]) {
      PAT_NOT_WC, PAT_WC, PAT_NOT_WS, PAT_WS, PAT_NOT_DIG, PAT_DIG, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);
}

/*
 * compile with quantifiers. this library does not support match groups, so you
 * can't match something like asdfasdfasdf with (asdf)* or (asdf)+, but a
 * quantifier can be applid to a character class meta (\d*, \W?, etc), a one of
 * or none of grouping ([a-z][a-z0-9_]* would match a basic variable name),
 * or the _last_ character of a literal run (asdf* would match asd, asdf,
 * asdffffff, and so on).
 */

MU_TEST(test_compile_quantifiers) {
   const cpat_t *pat = NULL;
   int *val = NULL;

   /* a line which may or may not be empty */

   pat = compile_pattern("^.*$");
   val = (int[]) {
      PAT_BOL, PAT_REP0M, PAT_WILD, PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   /* a line which has at least one non whitespace character */

   pat = compile_pattern("^\\S+$");
   val = (int[]) {
      PAT_BOL, PAT_REP1M, PAT_NOT_WS, PAT_EOL, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   debug_off(NULL);

   pat = compile_pattern("a+bc");
   val = (int[]) {
      PAT_REP1M, PAT_LIT, 1, 'a',
      PAT_LIT, 1, 'b',
      PAT_LIT, 1, 'c', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);


   pat = compile_pattern("asdf*");
   val = (int[]) {
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, 's',
      PAT_LIT, 1, 'd',
      PAT_REP0M, PAT_LIT, 1, 'f', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("asdf*ijkl");
   val = (int[]) {
      PAT_LIT, 1, 'a',
      PAT_LIT, 1, 's',
      PAT_LIT, 1, 'd',
      PAT_REP0M, PAT_LIT, 1, 'f',
      PAT_LIT, 1, 'i',
      PAT_LIT, 1, 'j',
      PAT_LIT, 1, 'k',
      PAT_LIT, 1, 'l',
      -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("[a-e]*");
   val = (int[]) {
      PAT_REP0M, PAT_CCLASS, 5, 'a', 'b', 'c', 'd', 'e', -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern("\\d*");
   val = (int[]) {
      PAT_REP0M, PAT_DIG, -1
   };
   mu_should(validate_compiled_pattern(pat, val));
   free((void *)pat);

   pat = compile_pattern(" *\\d+$");
   val = (int[]) {
      PAT_REP0M, PAT_LIT, 1, ' ',
      PAT_REP1M, PAT_DIG,
      PAT_EOL,
      -1
   };
   free((void *)pat);

   /* more to come */
}

/*
 * match quantifiers, trying to hit edge cases.
 */
MU_TEST(test_match_star) {
   const cpat_t *pat = NULL;

   pat = compile_pattern("^a*b");
   mu_should(pat);
   mu_should(match("b", pat));
   mu_should(match("ab", pat));
   mu_should(match("aaabc", pat));
   mu_shouldnt(match("aa", pat));
   free((void *)pat);

   pat = compile_pattern("a*b");
   mu_should(match("ab", pat));
   mu_should(match("aab", pat));
   mu_should(match("aaaaaaaaaaaaaaab", pat));
   mu_should(match("b", pat));
   mu_should(match("betty", pat));
   mu_should(match("xaaaaaaaab", pat));
   free((void *)pat);

   pat = compile_pattern("a*b*");
   mu_should(match("ab", pat));
   mu_should(match("a", pat));
   mu_should(match("b", pat));
   mu_should(match("cab", pat));
   free((void *)pat);

   pat = compile_pattern("^a*b*");
   mu_should(match("ab", pat));
   mu_should(match("a", pat));
   mu_should(match("b", pat));
   mu_should(match("cab", pat));
   free((void *)pat);

   pat = compile_pattern("a*b$");
   mu_should(match("b", pat));
   mu_should(match("aab", pat));
   mu_should(match("asdfb", pat));
   free((void *)pat);

   pat = compile_pattern("123*37*8");
   mu_should(match("12378", pat));
   free((void *)pat);
}

MU_TEST(test_match_quantifiers) {
   const cpat_t *pat = NULL;

   pat = compile_pattern("a+bc");
   mu_should(match("aabc", pat));
   mu_should(match("abc", pat));
   mu_should(match("abcd", pat));
   mu_should(match("aaaaaaaaaaaaaaaaaaaaaabc", pat));
   mu_shouldnt(match("aaba", pat));
   free((void *)pat);

   pat = compile_pattern("a+b+c");
   mu_should(match("abc", pat));
   mu_should(match("aaaaaaaabc", pat));
   mu_should(match("abbbbbbbbc", pat));
   mu_should(match("aaaaabbbbbbcccccc", pat));
   mu_shouldnt(match("aacb", pat));
   mu_shouldnt(match("bbbc", pat));
   free((void *)pat);

   pat = compile_pattern("a+ab");
   mu_should(match("aab", pat));
   mu_shouldnt(match("ab", pat));
   mu_should(match("aaaaaab", pat));
   free((void *)pat);

   pat = compile_pattern("a+b+c+");
   mu_should(match("abc", pat));
   mu_should(match("abcccccd", pat));
   mu_should(match("abcd", pat));
   mu_shouldnt(match("abdc", pat));
   free((void *)pat);

   pat = compile_pattern("a*b+");
   mu_should(match("abcd", pat));
   mu_should(match("bcda", pat));
   mu_should(match("aabbcd", pat));
   mu_should(match("able", pat));
   mu_should(match("abba", pat));
   mu_shouldnt(match("acdc", pat));
   free((void *)pat);

   pat = compile_pattern("^[a-e]*a\\d");
   mu_should(match("abba9", pat));
   mu_should(match("baba817", pat));
   mu_shouldnt(match("babc1", pat));
   free((void *)pat);

   pat = compile_pattern("^[a-e]\\d .?asdf*$");
   mu_should(match("a9 xasdf", pat));
   mu_should(match("e0 asd", pat));
   free((void *)pat);

   pat = compile_pattern("asdf+");
   mu_shouldnt(match("asd", pat));
   mu_should(match("asdf", pat));
   mu_should(match("asdff", pat));
   mu_should(match("asdfff ghijklmnop", pat));
   mu_shouldnt(match("asd f", pat));
   free((void *)pat);

   pat = compile_pattern("asdf?");
   mu_should(match("asdf", pat));
   mu_should(match("asdfff g", pat));
   mu_should(match("asd f", pat));
   free((void *)pat);

   pat = compile_pattern("asdf?gh");
   mu_shouldnt(match("asdfg", pat));
   mu_shouldnt(match("asdgkh", pat));
   mu_should(match("asdfgh", pat));
   mu_should(match("asdgh", pat));
   free((void *)pat);

}

/*
 * a pattern with no anchors and one wildcard character.
 */

MU_TEST(test_match_any) {
   const cpat_t *pat = NULL;

   pat = compile_pattern("as.f");
   mu_should(match("asdf", pat));
   mu_should(match("ijklasdf", pat));
   mu_shouldnt(match("asfd", pat));
   free((void *)pat);
}

/*
 * a simple literal matching at various locations in a string.
 */

MU_TEST(test_match_literals) {
   const cpat_t *pat = NULL;

   pat = compile_pattern("this");
   mu_should(match("what is this?", pat));
   mu_shouldnt(match("what was that?", pat));
   mu_shouldnt(match("what was thi", pat));
   mu_shouldnt(match("x", pat));

   /* dup_string shows we aren't using a bad pointer elsewhere */

   char *dup = dup_string("this");
   mu_should(match(dup, pat));
   free(dup);
   free((void *)pat);
}

/*
 * exercise the class metas \d, \w, \s and their negations.
 */

MU_TEST(test_match_class_metas) {
   const cpat_t *pat = NULL;

   pat = compile_pattern("\\d60");
   mu_should(match("460", pat));
   mu_shouldnt(match("a60", pat));
   free((void *)pat);

   pat = compile_pattern("\\D60");
   mu_shouldnt(match("460", pat));
   mu_should(match("a60", pat));
   free((void *)pat);

   pat = compile_pattern("\\s60");
   mu_should(match(" 60", pat));
   mu_shouldnt(match("360", pat));
   free((void *)pat);

   pat = compile_pattern("\\S60");
   mu_should(match("360", pat));
   mu_shouldnt(match(" 60", pat));
   free((void *)pat);

   pat = compile_pattern("\\w60");
   mu_should(match("w60", pat));
   mu_should(match("W60", pat));
   mu_should(match("_60", pat));
   mu_shouldnt(match("+60", pat));
   free((void *)pat);

   pat = compile_pattern("\\W60");
   mu_shouldnt(match("w60", pat));
   mu_shouldnt(match("W60", pat));
   mu_shouldnt(match("_60", pat));
   mu_should(match("+60", pat));
   free((void *)pat);

   pat = compile_pattern("^\\s");
   mu_should(match("  asdf", pat));
   mu_shouldnt(match("", pat));
   mu_shouldnt(match("a sdf", pat));
   mu_should(match("\tasdf", pat));
   free((void *)pat);

   pat = compile_pattern("\\d\\d\\d");
   mu_should(match("000", pat));
   mu_should(match("123", pat));
   mu_should(match("987", pat));
   mu_shouldnt(match(" 12", pat));
   free((void *)pat);

   pat = compile_pattern("\\D\\D\\D\\D");
   mu_should(match("asdf", pat));
   mu_shouldnt(match("asd9", pat));
   mu_shouldnt(match("a", pat));
   free((void *)pat);

   pat = compile_pattern("\\S\\s\\D\\d\\W\\w");
   mu_should(match("? a9+_", pat));
   mu_shouldnt(match(" ?9a_+", pat));
   free((void *)pat);
}

/*
 * some matching edge cases.
 */

MU_TEST(test_match_edgecases) {
   const cpat_t *pat = NULL;

   /* empty pattern */

   pat = compile_pattern("");
   mu_should(match("anything", pat));
   mu_shouldnt(match("", pat));
   free((void *)pat);

   /* single character pattern */

   pat = compile_pattern("a");
   mu_should(match("a", pat));
   mu_should(match("ba", pat));
   mu_shouldnt(match("", pat));
   free((void *)pat);

   /* single any char */

   pat = compile_pattern(".");
   mu_should(match("a", pat));
   mu_should(match(".", pat));
   mu_shouldnt(match("", pat));
   free((void *)pat);
}

/*
 * matching against the beginning or ending of a line.
 */

MU_TEST(test_match_anchors) {
   const cpat_t *pat = NULL;

   /* asdf at the start of a line */

   pat = compile_pattern("^asdf");
   mu_should(match("asdf", pat));
   mu_should(match("asdfijkl", pat));
   mu_shouldnt(match("^asdf", pat));
   free((void *)pat);

   /* asdf at the end of a line */

   pat = compile_pattern("asdf$");
   mu_should(match("the password might be asdf", pat));
   mu_shouldnt(match("ijklasdf$", pat));
   mu_should(match("asdf\n", pat));
   free((void *)pat);

   /* line holding only asdf */

   pat = compile_pattern("^asdf$");
   mu_should(match("asdf", pat));
   mu_shouldnt(match(" asdf ", pat));
   mu_shouldnt(match("", pat));
   mu_should(match("asdf\n", pat));
   free((void *)pat);
}

/*
 * groups match one character against one of (or none of) the enclosed
 * characters. ^[0-9] would match a basic line number, while [^0-9]
 * would not.
 */

MU_TEST(test_match_classes) {
   const cpat_t *pat = NULL;

   /* any of a b c d in first position */

   pat = compile_pattern("[abcd]");
   mu_should(match("asdf", pat));
   mu_should(match("this is billy", pat));
   mu_shouldnt(match("qwerty", pat));
   mu_should(match("a", pat));
   mu_shouldnt(match("e", pat));
   free((void *)pat);

   /* none of a b c d in first position */

   pat = compile_pattern("[^abcd]");
   mu_shouldnt(match("aabcda", pat));
   mu_should(match("this is billy", pat));
   mu_should(match("qwerty", pat));
   mu_shouldnt(match("a", pat));
   mu_should(match("e", pat));
   free((void *)pat);

   /* adjacent groups */

   pat = compile_pattern("[ab][cd]e");
   mu_should(match("ace", pat));
   mu_should(match("bde", pat));
   mu_should(match("xyz ace", pat));
   mu_shouldnt(match("abcde", pat));
   mu_shouldnt(match("acd", pat));
   mu_shouldnt(match("bd", pat));
   mu_should(match("ade", pat));
   mu_shouldnt(match("ACE", pat));
   free((void *)pat);

   /* group in last position */

   pat = compile_pattern("[+_&$]$");
   mu_should(match("$", pat));
   mu_shouldnt(match("$ ", pat));
   mu_should(match("this is a long line that should match &", pat));
   free((void *)pat);
}

/*
 * file name globbing uses a subset of the meta characters for regular
 * expressions, but their meanings are slightly different. this library
 * does not provided a full implementation of globs but will convert a
 * glob to an almost exact pattern.
 *
 * the difficulties are path separators and the use of a leading . for
 * hidden files in unix like systems.
 */

MU_TEST(test_convert_globs) {
   const char *str = NULL;

   debug_on("glob convert");

   /* glob searches don't return hidden files by default in unix like systems */
   str = convert_glob("*.*");
   mu_should(strcmp(str, "^.*\\..*$") == 0);
   free((void *)str);

   str = convert_glob("dir/file.ext");
   mu_should(strcmp(str, "^dir/file\\.ext$") == 0);
   free((void *)str);

   debug_off(NULL);
}

MU_TEST(test_match_globs) {
   const char *str = NULL;
   const cpat_t *pat = NULL;

   debug_on("glob match");

   str = convert_glob("*.*");
   pat = compile_pattern(str);
   mu_should(glob_match("README.org", pat));
   mu_shouldnt(glob_match(".gitignore", pat));
   mu_should(glob_match("a.out", pat));
   free((void *)str);
   free((void *)pat);

   str = convert_glob(".*.*");
   pat = compile_pattern(str);
   mu_shouldnt(glob_match(".gitignore", pat));
   mu_shouldnt(glob_match("txbpat.h", pat));
   mu_should(glob_match(".asdf.txt", pat));
   free((void *)str);
   free((void *)pat);

   str = convert_glob(".*");
   pat = compile_pattern(str);
   mu_should(glob_match(".gitignore", pat));
   mu_shouldnt(glob_match("txbpat.h", pat));
   /* actually, the following probably doesn't match on windows
    * file systems, but it does on my mac and i imagine other
    * unix like systems as well, so i'm treating this match
    * as if it is correct. */
   mu_should(glob_match(".asdf.txt", pat));

   debug_off(NULL);
}



/*
 * pull in stuff you want to breakpoint debug.
 */

MU_TEST(test_breakpoint) {
   const cpat_t *pat = NULL;
   pat = compile_pattern("asdf?gh");
   /* put whatever you want to breakpoint here */
   free((void *)pat);
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

   /* test compiling a search pattern string into a pattern buffer.
    * these rely on an exposed pattern buffer comparator. */

   MU_RUN_TEST(test_compile_literals);
   MU_RUN_TEST(test_compile_metas);
   MU_RUN_TEST(test_compile_ranges);
   MU_RUN_TEST(test_compile_class_metas);
   MU_RUN_TEST(test_compile_quantifiers);

   /* test various match syntax to verify that they work as
    * intended. */

   MU_RUN_TEST(test_match_literals);
   MU_RUN_TEST(test_match_class_metas);
   MU_RUN_TEST(test_match_edgecases);
   MU_RUN_TEST(test_match_anchors);
   MU_RUN_TEST(test_match_any);
   MU_RUN_TEST(test_match_classes);
   MU_RUN_TEST(test_match_quantifiers);

   MU_RUN_TEST(test_match_star);

   /* file name globbing */

   MU_RUN_TEST(test_convert_globs);
   MU_RUN_TEST(test_match_globs);

   /* breakpointing */
   MU_RUN_TEST(test_breakpoint);

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
