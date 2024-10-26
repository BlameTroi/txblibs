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
 * these functions provide a limited implementation of regular
 * expressions and file name globbing expressions.
 *
 * the initial shape of the implementation was inspired by Kernighan
 * and Plaugher's chapter on _Text Patterns_ in _Software Tools in
 * Pascal_, but the code is my own.
 *
 * diagnostics are limited and the code takes a "when in doubt, abort
 * execution" approach to error handling. non fatal syntax errors in
 * a pattern string might cause an infinite loop or invalid result,
 * but should not access violate.
 *
 * the suppported pattern language is a subset of the common regular
 * expression syntax:
 *
 * any character can be escaped via a backslash, removing its special
 * meaning.
 *
 * token  meaning
 *
 * .      matches any single character except \n
 * *      repeat the preceeding item zero or more times
 * ?      repeat the preceeding item zero or one time
 * +      repeat the preceeding item one or more times
 * ^      start of line when in first position, otherwise the
 *        literal character ^
 * $      end of line when in last position, otherwise the
 *        literal character $
 * \      remove the special meaning from the following character
 *        except as follows:
 * \s     match a single whitespace character
 * \S                    non whitespace character
 * \w                    word character
 * \W                    non word character
 * \d                    digit character
 * \D                    non digit character
 * \n \t \f retain their common meanings
 * [      start a class of characters, matches any one character
 *        in the class
 * [^     start a class of characters, matches any one character
 *        _not_ in the class
 * ]      end a class of characters
 * -      when in a class of characters and not the first or last
 *        character in the class, defines a range all the characters
 *        between the preceeding and following characters in ascending
 *        order
 *
 * the following are not yet implemented but must be escaped if they
 * are meant as literals:
 *
 * (      group one or more expression items.
 * )      end of a group
 * |      or or choice, one or the other but not both.
 * {      start a specific repeat quantifier, + is equivalent to {1,}
 *        and ? is equivalent to {,1}. more explicitly:
 *        {n} for exactly n times
 *        {min,} at least min times
 *        {,max} up to max times
 *        {min,max}
 * }      close the repeat quantifier.
 *        not in the group
 *
 * any character or token not listed above is treated as a literal to
 * be matched exactly.
 */

/*
 * a match string expression is compiled into a pattern buffer. this
 * buffer is a one dimensional array of unsigned integers. a pattern
 * item occupies at least one slot of the array. the item code is
 * symbolically identified in code by the macros PAT_???.
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
 * common character class matches for digits, letters, and whitespace
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
 * the following definitions are not strictly needed by client code.
 * they are exposed here for ease of unit testing.
 *
 * the naming convention is PAT for the item compiled into a pattern
 * buffer, and META for the character in the pattern source string.
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
 * compile_pattern
 *
 * given a null terminated match string, return a compiled pattern
 * buffer.
 *
 *     in: string
 *
 * return: a compiled pattern
 */

const cpat *
compile_pattern(
	const char *str
);

/*
 * pattern_source
 *
 * retrieve a copy of the source of the compiled pattern.
 *
 * a copy of the original pattern source string is carried in the
 * pat_beg item at the start of the compiled pattern buffer.
 *
 *     in: a compiled pattern
 *
 * return: a copy of the original pattern
 */

const char *
pattern_source(
	const cpat *
);

/*
 * decompile_pattern
 *
 * decompile the pattern buffer, which might be different than the
 * stored pattern source. this is helpful for debugging.
 *
 *     in: a compiled pattern
 *
 * return: string
 */

char *
decompile_pattern(
	const cpat *
);

/*
 * print_compiled_pattern
 *
 * print out the compiled pattern in what is hopefully a clear and
 * readable format, one that is less terse than the raw pattern source
 * string.
 *
 * this is printed on stdout.
 *
 *     in: a compiled pattern
 *
 * return: nothing
 */

void
print_compiled_pattern(
	cpat *pat
);

/*
 * convert_glob
 *
 * converts a DOS like glob pattern for filenames into a match string
 * that can then be compiled to a pattern.
 *
 *     in: string, a glob pattern
 *
 * return: string, the glob as a regex
 */

const char *
convert_glob(
	const char *str
);

/*
 * match
 *
 * match a string, generally assumed to be a line of text, against a
 * compiled pattern.
 *
 *     in: the string to match against
 *
 *     in: the compiled pattern
 *
 * return: boolean true if a match was found
 *
 * match takes a string and a compiled pattern and returns true if the
 * pattern is found at least once in the string. the match is
 * attempted repeatedly until the string is exhaused or a match is
 * found.
 */

bool
match(
	const char *str,
	const cpat *pat
);

/*
 * glob_match
 *
 * match a string, generally assumed to be a file name, against a
 * compiled pattern, while honoring some globbing restrictions for
 * path separators and hidden files.
 *
 *     in: the string to match against
 *
 *     in: the compiled pattern
 *
 * return: boolean true if a match was found
 *
 * glob_match takes a string and a compiled pattern and returns true
 * if the pattern matches the string within the rules for globbing.
 * you should use convert_glob to produce a regular expression style
 * search string and then compile it with compile_pattern.
 *
 * globbing wants the whole string to be a filename, so the match has
 * to be from the start of the string. currently not checking to see
 * if the whole string is consumed yet.
 */

bool
glob_match(
	const char *str,
	const cpat *pat
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* pat.h ends here */
