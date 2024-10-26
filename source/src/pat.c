/* pat.c -- blametroi's regular expression and pattern matching -- */

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
 * a match string expression is compiled into a pattern buffer. the
 * buffer is a one dimensional array of unsigned integers. a pattern
 * item occupies at least one item in the array. the item code is
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "txbmisc.h"
#include "txbstr.h"

#include "../inc/abort_if.h"

#include "../inc/pat.h"

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
 * programming notes:
 *
 * tokenizing and matching may be the poster child for the switch
 * statement, and i started out using it, but i've been removing it
 * where i can. the formatting is flow breaking, and you can't define
 * variables under a case: clause unless you enclose the code in {}.
 *
 * it's just too irregular. and then there's the mistaken urge to use
 * case: drop through to "save space." storage is cheap, and
 * forgetting a break statement is annoying.
 *
 * i use early exits and continues to avoid many run on if/else if/...
 * constructs, but there are places where it makes sense to group code
 * in one. its a hint to the reader that these blocks are very
 * related.
 *
 * matching is done on characters, which in my world means 8-bit
 * bytes, but i don't try to save much space. the compiled pattern is
 * stored as a list of unsigned int values, 'wasting' anywhere from
 * one to seven bytes depending on your platform. off boundary fetches
 * are a thing, but so is in cache reads. the efficiency isn't a big
 * deal either way, and this approach should handle switching to
 * wchar_t based strings instead of char without much difficulty.
 */

/*
 * these are debugging and testing helpers. they are not included in
 * the external declarations for txbpat but they aren't marked static
 * so you can add a declaration for them in client code if needed.
 */

/* for printing */

typedef struct match_code_t {
	char meta;
	cpat code;
	char *text;
} match_code_t;

static const match_code_t match_codes[] = {
	{ 0, PAT_BEG, ">>>BEGIN PATTERN" },
	{ 0, PAT_END, "<<<END PATTERN" },
	{ 0, PAT_FF, "\\f FORM FEED" },
	{ 0, PAT_LF, "\\n LINE FEED" },
	{ 0, PAT_LIT, "   LITERAL" },
	{ 0, PAT_TAB, "\\t TAB" },
	{ META_BOL, PAT_BOL, "^  BOL" },
	{ META_CCLASS, PAT_CCLASS, "[  BEGIN CLASS" },
	{ META_DIG, PAT_DIG, "\\d DIGIT" },
	{ META_END_CLASS, PAT_END_OF, "]  END CLASS" },
	{ META_END_GROUP, PAT_END_GROUP, ")  END GROUP" },
	{ META_EOL, PAT_EOL, "$  EOL" },
	{ META_GROUP, PAT_GROUP, "(  GROUP" },
	{ META_NCCLASS, PAT_NOT_CCLASS, "[^ BEGIN NEGATED CLASS" },
	{ META_NOT_DIG, PAT_NOT_DIG, "\\D NOT DIGIT" },
	{ META_NOT_WC, PAT_NOT_WC, "\\D NOT WORD CHARACTER" },
	{ META_NOT_WS, PAT_NOT_WS, "\\S NOT WHITESPACE" },
	{ META_OR, PAT_OR, "|  OR" },
	{ META_REP01, PAT_REP01, "?  ZERO OR ONE" },
	{ META_REP0M, PAT_REP0M, "*  REPEAT ZERO OR MORE" },
	{ META_REP1M, PAT_REP1M, "+  REPEAT ONE OR MORE" },
	{ META_REP_COUNT, PAT_REP_COUNT, "{  REPEAT COUNT" },
	{ META_REP_END_COUNT, PAT_END_REP, "}  END REPEAT COUNT" },
	{ META_WC, PAT_WC, "\\w WORD CHARACTER" },
	{ META_WILD, PAT_WILD, ".  WILD" },
	{ META_WS, PAT_WS, "\\s WHITESPACE" },
	{ 0, 0, NULL },
};

/*
 * given a pattern item code, return a pointer to a displayable
 * representation.
 */

static
const char *
displayable_match_code(
	cpat code
) {
	int i = 0;
	while (match_codes[i].text != NULL) {
		if (code == match_codes[i].code)
			break;
		i += 1;
	}
	if (match_codes[i].text == NULL)
		return "!!!ERROR!!!";
	return match_codes[i].text;
}

/*
 * miscellaneous predicates and helpers.
 */

/*
 * is_quantifier
 *
 * is the current pattern buffer item a quantifier?
 *
 *     in: a cpat item
 *
 * return: boolean
 */

static
bool
is_quantifier(
	cpat p
) {
	return p == PAT_REP0M ||   /* * */
	p == PAT_REP1M ||   /* + */
	p == PAT_REP01 ||   /* ? */
	p == PAT_REP_COUNT; /* {,} */
}

/*
 * pattern_length
 *
 * get position of next piece of the pattern, or the length of the
 * current piece of the pattern.
 *
 *     in: a cpat array
 *
 *     in: int index of the pattern in the array
 *
 * return: int length of the current pattern in the cpat buffer
 */

static
int
pattern_length(
	const cpat *pat,
	const int pp
) {
	int pl = 0;
	if (pat[pp] == PAT_CCLASS || pat[pp] == PAT_NOT_CCLASS || pat[pp] == PAT_LIT)
		pl = 2 + pat[pp+1];

	else if (pat[pp] == PAT_REP_COUNT)
		pl = 2;

	else if (pat[pp] == PAT_BEG)
		pl = 2 + pat[pp+1];

	else
		pl = 1;
	abort_if(pl < 1, "pat pattern_length error calculating pattern length");
	return pl;
}

/*
 * next_pattern
 *
 * return the index of the next pattern in the cpat array.
 * current position + length(current positioned item)
 *
 *     in: a cpat array
 *
 *     in: int the index of the current pattern
 *
 * return: int index of next pattern
 */

static
int
next_pattern(
	const cpat *pat,
	const int pp
) {
	return pp + pattern_length(pat, pp);
}

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
	const cpat *pat
) {
	if (pat == NULL || *pat != PAT_BEG)
		return "not a valid pattern";
	return (char *)(pat+2);
}

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
	const cpat *pat
) {
	if (pat == NULL || *pat != PAT_BEG)
		return strdup("not a valid pattern");
	return strdup("decompile_pattern not implemented");
}

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
) {
	printf("compiled pattern: \n");
	int i = 0;
	while (pat[i] != 0) {
		printf("%3d %s", i, displayable_match_code(pat[i]));

		switch (pat[i]) {

		case PAT_BEG:
		case PAT_BOL:
		case PAT_EOL:
		case PAT_WILD:
		case PAT_END:
		case PAT_END_OF:
		case PAT_DIG:
		case PAT_NOT_DIG:
		case PAT_WC:
		case PAT_NOT_WC:
		case PAT_WS:
		case PAT_NOT_WS:
		case PAT_REP0M:
		case PAT_REP1M:
		case PAT_REP01:
			printf("\n");
			break;

		case PAT_CCLASS:
		case PAT_NOT_CCLASS:
		case PAT_LIT:
			printf(" %d ", pat[i+1]);
			for (int j = 0; j < pat[i+1]; j++) {
				char c = (char)pat[i+2+j];
				if (c >= ' ')
					printf("%c", (char)pat[i+2+j]);

				else if (c == '\n')
					printf("\\n");

				else if (c == '\t')
					printf("\\t");

				else if (c == '\f')
					printf("\\f");

				else
					printf("?? %x ??", c);
			}
			printf("\n");
			break;

		default:
			abort_if(true,
				"pat print_compiled_pattern error detected in compiled pattern buffer");
		}
		i = next_pattern(pat, i);

	}
}

/*
 * validate_compiled_pattern
 *
 * validate_compiled_pattern provides a way to compare a pattern
 * against an expected value.
 *
 * a compiled pattern is an array of unsigned ints and it is compared
 * against an array of signed ints with -1 as an ending sentinel.
 *
 *     in: a compiled pattern
 *
 *     in: an array of ints, terminated by -1, with the
 *         expected compiled pattern
 *
 * return: bool true or false
 *
 * this function is externally visible but it is not declared
 * in pat.h
 */

bool
validate_compiled_pattern(
	const cpat *pat,
	int *val
) {
	if (pat == NULL || *pat != PAT_BEG)
		return false;

	/* begin and end are assumed */
	pat += next_pattern(pat, 0);
	for (int i = 0; val[i]!= -1; i++)
		if (pat[i] != (cpat)val[i])
			return false;

	return true;
}

/*
 * expand_range
 *
 * create a new copy of the pattern string with any embedded ranges in
 * character classes expanded.
 *
 * so [0..9] becomes [0123456789].
 *
 * TODO this isn't very robust.
 *
 *     in: string original pattern
 *
 * return: string pattern with ranges expanded
 */

static
char *
expand_range(
	const char *raw
) {
	int exp_max = max(64, strlen(raw) * 2);
	char *exp = malloc(exp_max);
	int pr = 0;
	int pe = 0;
	bool in_of = false;
	while (raw[pr]) {
		if (pe > exp_max * 3 / 4) {
			exp_max += exp_max + 32;
			exp = realloc(exp, exp_max);
		}
		if (raw[pr] == '\\') {
			exp[pe] = raw[pr];
			exp[pe+1] = raw[pr+1];
			pe += 2;
			pr += 2;
			continue;
		}
		if (in_of && raw[pr] == '-') {
			int c = raw[pr-1] + 1;
			int d = raw[pr+1];
			while (c < d) {
				exp[pe] = (char)c;
				c += 1;
				pe += 1;
			}
			pr += 1;
			continue;
		}
		if (in_of && raw[pr] == ']')
			in_of = false;
		if (!in_of && raw[pr] == '[')
			in_of = true;
		exp[pe] = raw[pr];
		pe += 1;
		pr += 1;
	}
	exp[pe] = '\0';
	return exp;
}

/*
 * add_pattern_item
 *
 * called by compile_pattern to build the encoded
 * pattern buffer from a pattern match string.
 *
 *     in: a compiled pattern from the search string
 *
 * in/out: int current position in pattern
 *
 *     in: int maximum position in pattern
 *
 *     in: int pattern match code
 *
 *     in: string pattern being compiled
 *
 * in/out: int current position pattern string
 *
 * return: no return value but the position in pat
 *         and position in string arguments are
 *         updated as the string is consumed.
 *
 * the encoded pattern is an array of slots (unsigned ints in the
 * current implementation). each character (meta or literal) in the
 * incoming match string will require at least one slot for a marker.
 * anchors such as begin of line and end of line require only one
 * slot, while others may require more.
 *
 * examples:
 *
 * "a" -> literal character, 1, 'a'
 * "^this" -> bol, literal character, 1, 't', literal character, 'h',
 *            literal character, 'i', literal character, 's'
 * "[abc]" -> a one of the following grouping, 3, 'a', 'b', 'c'
 *
 * as characters are consumed from the match string and placed in the
 * compiled pattern, the current positions within each are updated.
 *
 * as an aid to compilation, the position of the last item is stored
 * in the next available position. this is set to zero when the
 * pattern buffer is marked complete.
 */

static
void
add_pattern_item(
	cpat *pat,                 /* a compiled pattern from the search string */
	int *pos,                  /* in-out arg: current position in pat */
	int max,                   /* maximum position in pat */
	int item,                  /* pattern match code */
	const char *str,           /* the input search string */
	int *from                  /* in-out arg: current position in str */
) {
	abort_if(*pos + 8 >= max, "pat add_pattern_item pattern buffer overflow");
	int this_item = *pos;
	int last_item = *pos ? pat[*pos] : 0;

	pat[*pos] = item;

	if (item == PAT_BEG) {
		pat[*pos+1] = (strlen(str)+1) / sizeof(cpat) + 2;
		pat[*pos+2] = 0;
		strcpy((char *)(pat+*pos+2), str);
		*pos = 2 + pat[*pos+1];

	} else if (item == PAT_END)
		*pos += 1;


	else if (item == PAT_BOL || item == PAT_EOL || item == PAT_WILD) {
		*pos += 1;
		*from += 1;

	} else if (item == PAT_REP0M || item == PAT_REP1M || item == PAT_REP01) {
		*pos += 1;
		*from += 1;

	} else if (item == PAT_CCLASS || item == PAT_NOT_CCLASS) {
		if (pat[last_item] == item) {
			pat[last_item+1] += 1;
			pat[*pos] = (cpat)str[*from];
			*pos += 1;
			this_item =
				last_item; /* this pointer stays at last until we get a new pattern code */
			*from += 1;
		} else {
			pat[*pos] = item;
			pat[*pos+1] = 0;
			*pos += 2;
			*from += (item == PAT_CCLASS ? 1 : 2);
		}

	} else if (item == PAT_END_OF) {
		*pos += 1;
		*from += 1;

	} else if (item == PAT_WS || item == PAT_NOT_WS ||
	item == PAT_DIG || item == PAT_NOT_DIG ||
	item == PAT_WC || item == PAT_NOT_WC) {
		*pos += 1;
		*from += 2; /* \char */

	} else if (item == PAT_LIT) {

		/* an optimizatino would be to combine literals into a single
		 * item or string for comparison, but the use of quantifiers
		 * (repetition counts) made that more trouble than it was worth.
		 * literals still carry a length field but only one character is
		 * stored per literal at this time. */

		pat[*pos] = item;
		pat[*pos+1] = 1;
		pat[*pos+2] = (cpat)str[*from];
		*pos += 3;
		*from += 1;

	} else

		abort_if(true,
			"pat add_pattern_item illegal item type in add_pattern_item");


	/* remember the prior code position for character class groups and
	 * any other multi slot items so we can just add the next character
	 * instead of a whole new token. */

	if (item == PAT_END)
		pat[*pos] = 0;

	else
		pat[*pos] = this_item;
}

/*
 * reorganize_pattern_buffer
 *
 * reorganize the pattern buffer. this is done to put quantifiers
 * ahead of the item they refer to which simplifies the match
 * functions. pattern compilation helpers such as grouping end markers
 * are deleted, and the buffer allocation is a better fit than the
 * over allocated buffer for part one of compilation.
 *
 *     in: a compiled pattern
 *
 * return: a new compiled pattern
 */

static
cpat *
reorganize_pattern_buffer(
	const cpat *pat
) {

	int res_size = 0;
	cpat *res = NULL;

	/* the size of the optimized pattern buffer will be large enough to
	 * hold all existing items. any end of class markers are deleted
	 * since they are an aid to compilation and have no bearing on
	 * match processing. i don't try for an exact fit, but there is
	 * much less padding than in the first pass. */

	int i = 0;
	res_size = 1;
	while (pat[i] != PAT_END) {
		int n = next_pattern(pat, i);
		if (is_quantifier(pat[i]))
			res_size += 4;

		else if (pat[i] == PAT_END_OF) {
			res_size += 0; /* just being explicit here that we drop these in the final */
		} else
			res_size += n - i;
		i = n;
	}
	res_size += 2; /* allow for end marker and trailing 0 */

	/* allocate and clear the new pattern buffer. */
	res = malloc(res_size *sizeof(cpat));
	memset(res, 0, res_size *sizeof(cpat));

	/* position in original (pp) and new (pr) buffers */
	int pp = 0;
	int pr = 0;

	while (pat[pp] != PAT_END) {

		/* quantifiers should follow the item they refer to up to this
		 * point. if we see one here we've made an error. */
		abort_if(is_quantifier(pat[pp]),
			"pat reorganize_pattern_buffer optimize error, should not see a quantifier here.");

		/* look ahead, are we followed by a quantifier? remember that
		 * during pattern compilation character class groups have an end
		 * marker to simplify compilation. we can remove that here. */

		int pn = next_pattern(pat, pp);

		bool was_end_of = false;
		if (pat[pn] == PAT_END_OF) {
			was_end_of = true;
			pn = next_pattern(pat, pn);
		}

		/* is the real next item a quantifier? if so, copy it to output
		 * before we copy the current item. */

		if (is_quantifier(pat[pn])) {
			res[pr] = pat[pn];
			pr += 1;
		}

		/* now copy the current item over. */

		while (pp < pn) {
			res[pr] = pat[pp];
			pr += 1;
			pp += 1;
		}

		/* if there was an end marker for the current item, it is no
		 * longer needed so back it off. */

		if (was_end_of)
			pr -= 1;

		/* if next item is a quantifier, we've handled it already so we
		 * can skip it. */

		if (is_quantifier(pat[pn]))
			pn = next_pattern(pat, pn);

		/* handle next */
		pp = pn;
	}

	/* properly end the buffer and return */

	res[pr] = PAT_END;
	res[pr+1] = 0;

	return res;
}

/*
 * compile_pattern
 *
 * given a null terminated match string, return a compiled pattern
 * buffer.
 *
 *     in: string
 *
 * return: a compiled pattern
 *
 * compile_pattern takes a match string and creates an encoded pattern
 * for use by match.
 *
 * in part one, a working buffer for the pattern is over-allocated
 * with a worst case scenario estimate. this buffer will be more
 * realistically sized when it is finally returned to the caller.
 *
 * releasing this buffer is the callers responsibility.
 *
 * before compilation the match string is checked for character
 * classes within one of [] or none of [^] specifications. any ranges
 * found are expanded, so [abc-f] becomes [abcdef].
 *
 * the worse case scenario buffer size estimate:
 *
 * a single literal takes three slots in the pattern buffer:
 *
 * - one slot for the literal character marker
 * - one slot for the number of characters to follow (1 right now)
 *   one slot for the actual
 * - character value
 *
 * a character class group takes two slots to begin the group, a
 * slot for the number of members of the class, one slot for
 * each character in the class, and one slot to end the class.
 *
 * the initial allocation is three slots per each character in the
 * match string (actual length or sixteen characters, whichever is
 * more) plus three additional slots for begin and end of pattern
 * markers and a terminal nil item.
 *
 * once the raw search string has been compiled, the pattern buffer
 * is reorganized and resized to better fit the actual pattern.
 */

const
cpat *
compile_pattern(
	const char *raw
) {

	/* do preprocessing for ranges and any other tweaks to the
	 * pattern search string. this local copy may be modified
	 * to deal with escaped characters. */

	char *str = expand_range(raw);

	/* allocate a compiled pattern buffer. initialzed to 0xde
	 * for 0xdead as eye catcher. */

	int max_pat = 5 * max(strlen(str), 16) + 3;
	cpat *pat = malloc(max_pat * sizeof(cpat));
	abort_if(!pat, "pat compile_pattern could not allocate pattern buffer");
	memset(pat, 0xde, max_pat * sizeof(cpat));

	/* the current position within the string (ps) and pattern buffer
	 * (pp). these are updated mostly within add_pattern_item, but
	 * there are a couple of instances where it's easier to bump the ps
	 * value here in compile_pattern: dealing with some c style
	 * character excapes, and the digraph token for "none of the
	 * following" which is "[^". */

	int ps = 0;  /* position in string */
	int pp = 0;  /* position in pattern buffer */

	/* the behavior inside character classes ([] or [^]) is different
	 * enough that it is broken out. once a class is compiling,
	 * characters are added until the end of group token is seen. */

	bool in_class = false;
	cpat class_pattern_code = 0;

	/* place begining of pattern marker in the buffer. it includes the
	 * raw match pattern that is being compiled. adding the beginning
	 * marker does not adjust ps. */

	add_pattern_item(pat, &pp, max_pat, PAT_BEG, raw, &ps);

	/* scan through the pattern search string mostly character by character
	 * until we hit the end of string marker. */

	while (str[ps]) {

		/* even if not escaped, other meta characters are not meta
		 * characters when inside a (n)one of. the only exception is
		 * that a ] must be escaped if it is part of a class. the \n,
		 * \f, and \t escapes are supported, but otherwise the backslash
		 * is consumed and the following character is passed to the
		 * grouping unaltered. */

		if (in_class) {
			if (str[ps] != META_END_CLASS) {
				if (str[ps] == META_ESC) {

					/* update position in string here to consume the
					 * escape, the escaped character itself will be
					 * addressed in add_pattern_item. */
					ps += 1;

					/* remember that str is a local copy of the raw string,
					 * so it is safe to modify here. the most common c
					 * escapes are supported. */
					if (str[ps] == 't')
						str[ps] = '\t';

					else if (str[ps] == 'n')
						str[ps] = '\n';

					else if (str[ps] == 'f')
						str[ps] = '\f';
				}
				add_pattern_item(pat, &pp, max_pat, class_pattern_code, str, &ps);

			} else {

				/* a grouping end marker ']' has been read. mark the end
				 * of the grouping (which acts as a nop in matching but
				 * provides a separator when compiling) and reset flags so
				 * we do normal processing. */

				add_pattern_item(pat, &pp, max_pat, PAT_END_OF, str, &ps);
				in_class = false;
				class_pattern_code = 0;
			}

			continue;
		}

		/* outside of a grouping, each character is evaluated. several
		 * meta characters are available and items for each are added
		 * via add_pattern_item. if the character has no special
		 * meaning, it is a literal. */

		switch (str[ps]) {

		case META_BOL:

			/* the beginning of line meta is only a meta if it is the
			 * first character of the search string, otherwise treat it
			 * as a literal. */

			if (ps == 0)
				add_pattern_item(pat, &pp, max_pat, PAT_BOL, str, &ps);

			else
				add_pattern_item(pat, &pp, max_pat, PAT_LIT, str, &ps);
			break;

		case META_EOL:

			/* as for the beginning, so it is for the ending */

			if (str[ps+1] == '\0')
				add_pattern_item(pat, &pp, max_pat, PAT_EOL, str, &ps);

			else
				add_pattern_item(pat, &pp, max_pat, PAT_LIT, str, &ps);
			break;

		case META_CCLASS:

			/* character classes are indicated by an open square brace [.
			 * a class matches a single character with any character in
			 * the grouping. if the [ is immediately followed by a caret
			 * ^, the behavior of the class is to match any character
			 * _not_ in the class.
			 *
			 * an empty class is a fatal error.
			 *
			 * the in_class flag is set and the class is started in the
			 * pattern buffer. the special class processing at the head
			 * of the main loop will add characters to the class until
			 * the end marker ] is seen. */

			in_class = true;
			if (str[ps+1] == META_NCCLASS) {
				abort_if(str[ps+2] == ']',
					"pat compile_pattern empty character class found in source string");
				class_pattern_code = PAT_NOT_CCLASS;
			} else {
				abort_if(str[ps+1] == ']',
					"pat compile_pattern empty character class found in source string");
				class_pattern_code = PAT_CCLASS;
			}

			add_pattern_item(pat, &pp, max_pat, class_pattern_code, str, &ps);
			break;

		case META_ESC:

			/* handle a c style backslash character escape. what we do
			 * depends on the next character. there are some character
			 * classification metas to support, then a few standard c
			 * escapes, and finally when in doubt just eat the escape
			 * character and pass the following character through as a
			 * literal. */

			abort_if(str[ps+1] == '\0',
				"pat compile_pattern backslash escape can not be the last character of a search string");

			/* character class escapes have their own pattern items. */

			if (str[ps+1] == 's')
				add_pattern_item(pat, &pp, max_pat, PAT_WS, str, &ps);

			else if (str[ps+1] == 'S')
				add_pattern_item(pat, &pp, max_pat, PAT_NOT_WS, str, &ps);

			else if (str[ps+1] == 'w')
				add_pattern_item(pat, &pp, max_pat, PAT_WC, str, &ps);

			else if (str[ps+1] == 'W')
				add_pattern_item(pat, &pp, max_pat, PAT_NOT_WC, str, &ps);

			else if (str[ps+1] == 'd')
				add_pattern_item(pat, &pp, max_pat, PAT_DIG, str, &ps);

			else if (str[ps+1] == 'D')
				add_pattern_item(pat, &pp, max_pat, PAT_NOT_DIG, str, &ps);

			else {

				/* consume the escape \ here by advancing ps, fix up the
				 * next character in the string if it is supported c
				 * escape, and then pass whatever the next character is
				 * through as a literal. */

				ps += 1;
				if (str[ps] == 'n')
					str[ps] = '\n';
				if (str[ps] == 't')
					str[ps] = '\t';
				if (str[ps] == 'f')
					str[ps] = '\f';
				add_pattern_item(pat, &pp, max_pat, PAT_LIT, str, &ps);
			}
			break;

		case META_END_CLASS:

			/* the close class token should not be seen here. this is for
			 * completeness. */

			abort_if(true,
				"pat compile_pattern error parsing pattern unexpected close class ]");
			break;

		case META_WILD:

			/* any character can match this. */

			add_pattern_item(pat, &pp, max_pat, PAT_WILD, str, &ps);
			break;

		case META_REP0M:

			/* the three quantifier tokens are *, +, and ?. */

			add_pattern_item(pat, &pp, max_pat, PAT_REP0M, str, &ps);
			break;

		case META_REP1M:

			add_pattern_item(pat, &pp, max_pat, PAT_REP1M, str, &ps);
			break;

		case META_REP01:

			add_pattern_item(pat, &pp, max_pat, PAT_REP01, str, &ps);
			break;

		case META_OR:

			abort_if(true,
				"pat compile_pattern or | not yet implemented.");
			break;

		case META_REP_COUNT:
		case META_REP_END_COUNT:

			abort_if(true,
				"pat compile_pattern repeat counts {m,n} not yet implemented.");
			break;

		case META_GROUP:
		case META_END_GROUP:

			abort_if(true,
				"pat compile_pattern grouping via () not yet implemented.");
			break;

		default:

			/* anything else is a literal */

			add_pattern_item(pat, &pp, max_pat, PAT_LIT, str, &ps);
			break;
		}
	}

	/* place an end marker in the pattern. */
	add_pattern_item(pat, &pp, max_pat, PAT_END, str, &ps);

	/* now reorganize the pattern by moving quantifiers in front of the
	 * items they refer to, and other minor optimizations if there are
	 * any.
	 *
	 * quantifiers are more naturally specified _after_ the item they are
	 * applied to, but it's easier to program if they come _before_ the
	 * item. if a one was seen, we'll want to reorder the items in the
	 * pattern buffer. */

	cpat *temp = reorganize_pattern_buffer(pat);
	free(pat);
	pat = temp;

	/* str is a local copy of the raw input and must be freed here. */
	free(str);

	return pat;
}

/*
 * convert_glob
 *
 * converts a DOS like glob pattern for filenames into a match string
 * that can then be compiled to a pattern.
 *
 *     in: string, a glob pattern
 *
 * return: string, the glob as a regex
 *
 * periods are half assed anchors given their use as filename
 * separators in dos, but they aren't in unix like systems. i'm
 * basically ignoring their specialness outside of the first
 * position of the string.
 *
 * the library client is responsible for releasing the string's
 * storage when it is no longer needed.
 */

const char *
convert_glob(
	const char *glob
) {

	/* no input should return a match almost anything */

	if (glob == NULL || strlen(glob) == 0)
		return strdup("^[^.]*$");

	/* pattern buffer is 32 or twice the size of the incoming
	 * glob string */

	int str_max = max(32, strlen(glob) * 2);
	int pg = 0;
	int ps = 0;
	char *str = malloc(str_max);
	str[ps] = '^';
	ps += 1;

	/* TODO decision point here based on leading . in glob */

	while (glob[pg]) {

		/* if buffer down to 25%, increase by 16 characters */
		if (ps > str_max  * 3 / 4) {
			str_max += 16;
			str = realloc(str, str_max);
		}

		/* a glob * means zero or more of anything */
		if (glob[pg] == '*') {
			str[ps] = '.';
			str[ps+1] = '*';
			ps += 2;
			pg += 1;
			continue;
		}

		/* a glob ? means one of anything */
		if (glob[pg] == '?') {
			str[ps] = '.';
			ps += 1;
			pg += 1;
			continue;
		}

		/* a glob . is really a period, it must be escaped in the
		 * pattern */
		if (glob[pg] == '.') {
			str[ps] = '\\';
			str[ps+1] = '.';
			ps += 2;
			pg += 1;
			continue;
		}

		/* a [] grouping is a one of and we pass it straight through,
		 * but watch for escapes so a \] doesn't prematurely end the
		 * grouping. */
		if (glob[pg] == '[') {
			while (glob[pg] && glob[pg] != ']') {
				if (glob[pg] == '\\') {
					abort_if(glob[pg+1] == '\0',
						"pat convert_glob improperly constructed [] in glob string");
					str[ps] = glob[pg];
					ps += 1;
					pg += 1;
				}
				str[ps] = glob[pg];
				ps += 1;
				pg += 1;
			}
			continue;
		}

		/* pass non special characters straight through */
		str[ps] = glob[pg];
		ps += 1;
		pg += 1;
	}

	/* close the pattern and return */
	str[ps] = '$';
	str[ps+1] = '\0';

	return str;
}

/*
 * match_this_item
 * is the string at the current position a match for the current
 * pattern buffer item?
 *
 * if it does, advance the string position to consume the portion
 * matched.
 *
 *     in: the entire string to match against
 *
 * in/out: int current position in the match string
 *
 *     in: direct pointer to the current pattern item in
 *         the compiled pattern
 *
 * return: bool did this match, also updates the position
 *         in the string to consume the matched portion
 */

static
bool
match_this_item(
	const char *str,             /* entire string to match */
	int *ps,                     /* IN-OUT position to start match at */
	const cpat *p                /* current pattern item in buffer */
) {

	/* caching the current character from the string saves a little typing
	 * and might help the compiler optimize a bit. */

	const char c = str[*ps];

	/* first up are line anchors. most regex processors treat beginning
	 * of the match string as beginning of line, and end of the match
	 * string as end of line, and handling of embedded newline
	 * characters varies.
	 *
	 * i've opted to ignore newline characters everywhere except the
	 * last position of the match string. a newline followed by a NUL
	 * is treated as end of line, as is being positioned at the NUL.
	 * beginning of line only matches at position 0.
	 *
	 * this approach to newlines seems to provide the most
	   flexibility. */

	if (c == '\0')
		return *p == PAT_EOL;

	else if (*p == PAT_EOL) {
		if (c == '\n' && str[*ps+1] == '\0') {
			/* consume the newline in the match string */
			*ps += 1;
			return true;
		}
		return false;
	} else if (*p == PAT_BOL)
		return (*ps == 0);

	/* wildcard matches any single character except newline. */

	if (*p == PAT_WILD) {
		if (c != '\n') {
			*ps += 1;
			return true;
		} else
			return false;
	}

	/* a literal character.
	 *
	 * an early but dropped optimization was to combine consecutive
	 * literals under one pattern item. right now literals are single
	 * character but the structure of the pattern item is left to
	 * allow for combining literals in the future.
	 *
	 * as with a character class group, p points to the pattern type,
	 * +1 is the number of items in the group, and +2 and onward are
	 * the items. */

	if (*p == PAT_LIT) {
		if (p[2] != c)
			return false;
		*ps += 1;
		return true;
	}

	/* there are a number of meta character class shorthands for
	 * frequently used character classes: digits, whitespace, and
	 * word characters (including _). */

	if (*p == PAT_DIG) {
		if (is_digit(c)) {
			*ps += 1;
			return true;
		} else
			return false;
	} else if (*p == PAT_NOT_DIG) {
		if (!is_digit(c)) {
			*ps += 1;
			return true;
		} else
			return false;
	} else if (*p == PAT_WS) {
		if (is_whitespace(c)) {
			*ps += 1;
			return true;
		} else
			return false;
	} else if (*p == PAT_NOT_WS) {
		if (!is_whitespace(c)) {
			*ps += 1;
			return true;
		} else
			return false;
	} else if (*p == PAT_WC) {
		if (is_word_char(c)) {
			*ps += 1;
			return true;
		} else
			return false;
	} else if (*p == PAT_NOT_WC) {
		if (!is_word_char(c)) {
			*ps += 1;
			return true;
		} else
			return false;
	}

	/* common c-ish escapes */

	if (*p == PAT_FF) {
		if (c == '\f') {
			*ps += 1;
			return true;
		} else
			return false;
	} else if (*p == PAT_LF) {
		if (c == '\n') {
			*ps += 1;
			return true;
		} else
			return false;
	} else if (*p == PAT_TAB) {
		if (c == '\t') {
			*ps += 1;
			return true;
		} else
			return false;
	}

	/* a single character compared to a character class group, either
	 * one of or none of. p points to the pattern type, +1 is the
	 * number of items in the group, +2 and onward are the items. */

	if (*p == PAT_CCLASS || *p == PAT_NOT_CCLASS) {
		bool found = false;
		for (int i = 0; i < p[1]; i++) {
			if (p[i+2] == c) {
				found = true;
				break;
			}
		}
		if (*p == PAT_CCLASS && found) {
			*ps += 1;
			return true;
		}
		if (*p == PAT_NOT_CCLASS && !found) {
			*ps += 1;
			return true;
		}
		return false;
	}

	/* we either don't understand the pattern buffer item, or we've been
	 * called with an item that shouldn't reach here (eg, quantifiers such
	 * as * are handled in match_from). report and abort. */

	char *em = calloc(256, sizeof(char));
	snprintf(em, 256, "pat match_this_item unknown pattern type code in: %d %s",
		*p, displayable_match_code(*p));
	abort_if(true, em);
	return false;
}

/*
 * match_from
 *
 *     in: string to match
 *
 *     in: int position within string
 *
 *     in: a compiled pattern
 *
 *     in: int position within compiled pattern
 *
 * return: int position of match, or -1 if there is no match
 *
 * check a full string against a pattern, or recursively to match a
 * substring against the remaining pattern when dealing with
 * quantifiers (* ? + {n,m}).
 *
 * it returns the character index in str where the pattern is found.
 * if there is no match, -1 is returned.
 *
 * it might make sense to refactor this. for non recusive calls, the
 * pp (parameter position) argument is always 0. perhaps a split of
 * match_from(str, ps, pat) and then match_from_r(str, ps, pat, pp)
 * for the recursion?
 */

static
int
match_from(
	const char *str,           /* the entire string to match */
	int from,                  /* current starting position in string */
	const cpat *pat,           /* the compiled pattern buffer */
	int pp                     /* current item position in pattern */
) {
	bool done = false;
	int res = from;
	int ps = from;

	/* skip beginning of pattern */
	if (pat[pp] == PAT_BEG)
		pp = next_pattern(pat, pp);

	while (!done && pat[pp] != PAT_END) {

		/*
		 * quantifiers (repetition) require a bit more work and possibly some
		 * recursion.
		 */

		if (is_quantifier(pat[pp])) {

			if (pat[pp] == PAT_REP0M) {

				/* the kleene *, repeat the item zero or more times */
				/* find the last position where quantified pattern matches
				 * from the current ps. */
				int pq = ps;
				int lq = -1;
				while (match_this_item(str, &pq, pat+pp+1))
					lq = pq;

				/* this is a 0 or more quantifier, so if it didn't match,
				 * it actually did match :) skip it and continue */
				if (lq == -1) {
					pp = next_pattern(pat, pp); /* the quantifier */
					pp = next_pattern(pat, pp); /* the quantified */
					continue;
				}

				/* now consume all matches of this pattern. */
				while (match_this_item(str, &pq, pat+pp+1))
					;

				/* recursively check the rest of the string against
				 * the rest of the pattern. if we get a match, it will
				 * satisfy the entire match from our earlier item so
				 * we can return.
				 *
				 * with each failed attempt, regurgitate a character
				 * consumed by this match and try again until no more
				 * characters can be reclaimed.
				 *
				 * while above we continue scanning if this item didn't
				 * consume a character, here if we roll all the way back
				 * we have already determined that the rest of the string
				 * is not matched so we can exit without checking the
				 * items again.
				 *
				 * and of course a match here completes the entire match
				 * so we can just return. */

				int pn = next_pattern(pat, pp); /* quantifier */
				pn = next_pattern(pat, pn);  /* quantified */
				while (!done && pq >= ps) {
					done = match_from(str, pq, pat, pn) > -1;
					pq -= 1;
				}
				if (!done) {
					done = true;
					res = -1;
				}
				continue;

			} else if (pat[pp] == PAT_REP01) {

				/* the ? quantifier for repeat an item zero or one
				   time.  */

				/* if it doesn't match, it's actually a match for the
				 * purposes of advancing through the search string. */
				int pq = ps;
				if (!match_this_item(str, &pq, pat+pp+1)) {
					pp = next_pattern(pat, pp); /* quantifier */
					pp = next_pattern(pat, pp); /* quantified */
					continue;
				}

				/* it did match, but maybe it shouldn't if it prevents the
				 * next item from matching.
				 *
				 * recursively check the rest of the string against the
				 * rest of the pattern. if we get a match, it will satisfy
				 * the entire match from our earlier item so we can
				 * return.
				 *
				 * if the match failed, back off the match and try
				 * continue. */
				int pn = next_pattern(pat, pp); /* quantifier */
				pn = next_pattern(pat, pn);  /* quantified */
				if (match_from(str, pq, pat, pn) > 0) {
					done = true;
					continue;
				}
				/* TODO could we always just end here with done = true,
				   res = -1? */
				pp = next_pattern(pat, pp); /* quantifier */
				pp = next_pattern(pat, pp); /* quantified */
				continue;

			} else if (pat[pp] == PAT_REP1M) {

				/* the + for repeat the item one or more times. if the
				 * string matches at least once, keep on matching and
				 * consuming until it doesn't. then, back off until we get
				 * a match on the rest of the pattern buffer or we can't
				 * get a match.
				 *
				 * for example, a+ab must match ab and aab. */

				/* we need at least one match, otherwise we're done. */
				int pq = ps;
				if (!match_this_item(str, &pq, pat+pp+1)) {
					done = true;
					res = -1;
					continue;
				}

				/* now consume all matches of this pattern. */
				while (match_this_item(str, &pq, pat+pp+1))
					;

				/* recursively check the rest of the string against
				 * the rest of the pattern. if we get a match, it will
				 * satisfy the entire match from our earlier item so
				 * we can return.
				 *
				 * with each failed attempt, regurgitate a character
				 * consumed by this match and try again until no more
				 * characters can be reclaimed.
				 *
				 * if we don't find a match, the whole match fails and
				 * we can return the failure status.
				 *
				 * a match here completes the entire match so we can
				 * just return. */

				int pn = next_pattern(pat, pp); /* quantifier */
				pn = next_pattern(pat, pn);  /* quantified */
				while (!done && pq > ps) {
					done = match_from(str, pq, pat, pn) > -1;
					pq -= 1;
				}
				if (!done) {
					done = true;
					res = -1;
				}
				continue;

			} else if (pat[pp] == PAT_REP_COUNT) {
				done = true;
				res = -1;

			} else

				abort_if(true,
					"pat match_from error unknown quantifier in pattern");


		} else if (!match_this_item(str, &ps, pat+pp)) {

			/* match failed so we're done. */

			done = true;
			res = -1;

		} else {

			/* match_this_item will have consumed the part of the
			 * string that matched, advance to the next pattern item
			 * and check it. */

			pp = next_pattern(pat, pp);
		}
	}

	return res;
}

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
) {
	int ps = 0;
	int pm = -1;

	abort_if(!str || !pat,
		"pat match missing arguments");

	while (str[ps] && pm == -1) {
		pm = match_from(str, ps, pat, 0);
		ps += 1;
	}

	return pm != -1;
}

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
) {

	int ps = 0;
	int pm = -1;

	abort_if(!str || !pat,
		"pat glob_match missing arguments");

	/* boy this is ugly */
	if (str[0] == '.') {
		int pp = 0;
		if (pat[pp] == PAT_BEG)
			pp = next_pattern(pat, pp);
		if (pat[pp] == PAT_BOL)
			pp = next_pattern(pat, pp);
		if (pat[pp] == PAT_LIT) {
			if (pat[pp+2] == '.')
				pm = match_from(str, ps, pat, 0);

			else
				pm = -1;
		}
	} else
		pm = match_from(str, ps, pat, 0);

	return pm == 0;
}

/* pat.c ends here */
