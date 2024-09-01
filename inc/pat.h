/* pat.h -- blametroi's common utility functions -- */

/*
 * this is a header only implementation of a subset of a regular
 * expression parser and pattern matcher.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

/*
 * a match string expression is compiled into pattern buffer. the
 * buffer is a one dimensional array of unsigned integers. a pattern
 * item occupies at least one entry in the array. the item code is
 * symbolically identified by the macros PAT_???.
 *
 * the first item in the array is always PAT_BEG, and the last is
 * always PAT_END. PAT_END is guaranteed to be followed by an
 * unsigned integer zero.
 *
 * pattern items occupy a variable number of slots.
 *
 * many single character match string specifications take only
 * one slot:
 *
 * ^ for start of line, $ for end of line, and . for any single
 * character matches all take one slot.
 *
 * common character type matches for digits, letters, and whitespace
 * all take one slot (\d, \w, \s) as do their negations.
 *
 * quantifiers (*, ?, +) all take only one slot.
 *
 * character classes or groups, where any one character can be matched
 * or excluded from a list ([], [^]) occupy three slots plus one slot
 * for each character in the group.
 *
 * runs of characters to match exactly also occupy three slots plus
 * one slot for each character in the run.
 *
 * while literals are stored and processed as runs in the pattern
 * buffer, a quantifier following a literal run actually only refers
 * to the _last_ character of the run. [ab]cde* matches acd, bcd,
 * bcdeeeee, and so on, not acdecde.
 *
 * as yet there is no match grouping so (abc)* does not mean zero or
 * more repetitions of abc, and instead will match (abc, abc), abc))
 * and so on.
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * externally visible declarations/forwards:
 *
 * the naming convention here is PAT for the item compiled into a
 * pattern buffer, and META for the character in the pattern source
 * string.
 */

/*
 * compiled pattern item codes.
 *
 * some are not implemented yet but are included in hopes that they
 * will be.
 *
 * the ordering of the PAT item codes and apparent grouping by tens is
 * not significant.
 */

#define PAT_BEG        1
#define PAT_BOL        11
#define PAT_EOL        12
#define PAT_CCLASS     21
#define PAT_NOT_CCLASS 22
#define PAT_END_OF     23
#define PAT_GROUP      25
#define PAT_END_GROUP  26
#define PAT_LIT        31
#define PAT_WILD       32
#define PAT_LF         33
#define PAT_TAB        34
#define PAT_FF         35
#define PAT_REP0M      41
#define PAT_REP1M      42
#define PAT_REP01      43
#define PAT_REP_COUNT  44
#define PAT_END_REP    49
#define PAT_OR         51
#define PAT_ESC        81
#define PAT_WS         82
#define PAT_NOT_WS     83
#define PAT_WC         84
#define PAT_NOT_WC     85
#define PAT_DIG        86
#define PAT_NOT_DIG    87
#define PAT_END        99

/*
 * pattern meta characters:
 *
 * these are the language of pattern matching expressions.
 *
 * these generally do what you would expect if you are familiar with
 * regular expression syntax. this implementation is not complete, but
 * strives to be a useful subset.
 *
 * some meta characters are only meta in certain positions. for
 * example, ^ means beginning of line if it is the first character of
 * a match string, negation if it is the first character in a grouping
 * ([^...]), otherwise it's a literal ^.
 *
 * the backslash escape is listed as a meta character below but it
 * supercedes them.
 */

#define META_BOL        '^'
#define META_EOL        '$'
#define META_CCLASS     '['
#define META_NCCLASS    '^'
#define META_END_CLASS  ']'
#define META_WILD       '.'
#define META_ESC        '\\'
#define META_REP0M      '*'
#define META_REP1M      '+'
#define META_REP01      '?'
#define META_RANGE      '-'
#define META_WS         's'
#define META_NOT_WS     'S'
#define META_WC         'w'
#define META_NOT_WC     'W'
#define META_DIG        'd'
#define META_NOT_DIG    'D'
#define META_OR         '|'
#define META_GROUP      '('
#define META_END_GROUP  ')'
#define META_REP_COUNT  '{'
#define META_REP_END_COUNT '}'

/*
 * a compiled pattern buffer is a zero terminated list of unsigned
 * integers. the pattern item code is symbolically identfied by the
 * PAT_??? macros defined earlier and a more complete description of
 * the pattern buffer is at the head of this include file.
 *
 * the buffer is dynamically allocated and the client code is
 * responsible for releasing it.
 *
 * as an aid to debugging, the original pattern definition string
 * is available in the pattern buffer.
 */

typedef unsigned int cpat;

/*
 * these functions can be used to turn on various undocumented debug
 * prints for pattern analysis.
 */

void
debug_on(char *optional_print);

void
debug_off(char *optional_print);

/*
 * given a null terminated match string, return a compiled pattern
 * buffer.
 */

const cpat *
compile_pattern(const char *str);

/*
 * retrieve a copy of the source of the compiled pattern.
 */

const char *
pattern_source(const cpat *);

/*
 * decompile the pattern buffer, which might be different than the
 * stored pattern source.
 */

char *
decompile_pattern(const cpat *);

/*
 * converts a DOS like glob pattern for filenames into a match string
 * that can then be compiled to a pattern.
 */

const char *
convert_glob(
	const char *str
);

/*
 * match a string, generally assumed to be a line of text, against a
 * compiled pattern.
 */

bool
match(
	const char *str,
	const cpat *pat
);

/*
 * match a string, generally assumed to be a file name, against a
 * compiled pattern, while honoring some globbing restrictions for
 * path separators and hidden files.
 */

bool
glob_match(
	const char *str,
	const cpat *pat
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
