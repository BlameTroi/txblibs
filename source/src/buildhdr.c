/* buildhdr.c -- blametroi's c single file header library packager -- troy brumley*/

/*
 * buildhdr is my take on Apoorva Joshi's single_header_packer.py. i
 * use almost the same command line format but i let the shell do the
 * file globbing so files are delimited by spaces instead of commas.
 *
 * buildhdr --macro MACRO_PREFIX
 *         [--intro <files>]
 *          --pub <files>
 *         [--priv <files>]
 *         [--outro <files>]
 *         [--fix-prefix FIX_PREFIX]
 *
 * the component files are wrapped with a header and implementation
 * guards and writtent ot standard output.
 *
 * released to the public domain, troy brumley, june 2024
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <assert.h>
#include <sys/errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <unistd.h>

/*
 * a bit of chicken and egg here.
 */

#define TXBSTR_IMPLEMENTATION
#include "../inc/str.h"
#include "../src/str.c"

#define TXBMISC_IMPLEMENTATION
#include "../inc/misc.h"
#include "../src/misc.c"
#define TXBPAT_IMPLEMENTATION
#include "../inc/pat.h"
#include "../src/pat.c"

/*
 * sometime globals are the right answer, but grouping them in a typedef namespace
 * keeps it clearly visible.
 */

typedef struct ctx_t {
	int argc;                         /* from main */
	const char **argv;
	const char *macro_prefix;         /* --macro NAME */
	const char *fix_prefix;           /* --fix-prefix */
	int intro_start;                  /* --intro _here_ */
	int intro_count;                  /*         count to next -- */
	int pub_start;                    /* --pub */
	int pub_count;
	int priv_start;                   /* --priv */
	int priv_count;
	int outro_start;                  /* --outro */
	int outro_count;
} ctx_t;

ctx_t ctx = {
	0,
	NULL,
	NULL,
	NULL,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1
};

/*
 * common predicates and helper functions.
 */

/*
 * lazy initializers for match patterns.
 */

/*
 * macros should be uppercase.
 */

const cpat *
pat_macro_prefix(
	void
) {
	static const cpat *pat = NULL;
	if (pat == NULL) {
		pat = compile_pattern("^[A-Z][A-Z_0-9]*$");
		assert(pat &&
			"could not compile macro prefix pattern");
	}
	return pat;
}

/*
 * file names should be lowercase.
 */

const cpat *
pat_fix_prefix(
	void
) {
	static const cpat *pat = NULL;
	if (pat == NULL) {
		pat = compile_pattern("^[a-z][a-z_0-9]*$");
		assert(pat &&
			"could not compile fix prefix pattern");
	}
	return pat;
}

/*
 * any include
 */

const cpat *
pat_include_prefix(
	void
) {
	static const cpat *pat = NULL;
	if (pat == NULL) {
		pat = compile_pattern("^ *#include +[<\"].+[>\"]");
		assert(pat &&
			"could not compile include prefix pattern");
	}
	return pat;
}

/*
 * non system include
 */

const cpat *
pat_fixable_prefix(
	void
) {
	static const cpat *pat = NULL;
	if (pat == NULL) {
		pat = compile_pattern("^ *#include +\"\\.\\./");
		assert(pat &&
			"could not compile fixable prefix pattern");
	}
	return pat;
}

/*
 * is a string a valid macro prefix?
 */

bool
is_valid_macro_prefix(const char *str) {
	return match(str, pat_macro_prefix());
}

/*
 * or fix prefix?
 */

bool
is_valid_fix_prefix(const char *str) {
	return match(str, pat_fix_prefix());
}

/*
 * is a string a possible long option name (--text)?
 */
bool
is_longopt(
	const char *str
) {
	return str && strlen(str) > 2 && str[0] == '-' && str[1] == '-';
}

/*
 * get filename (sans extension) from the end of a path.
 */

const
char *
get_filename_path(
	const char *str
) {
	const char *result = NULL;
	const char **tokens = split_string(str, "/\\:");
	int i = 1;
	while (tokens[i])
		i += 1;
	result = strdup(tokens[i-1]);
	free_split(tokens);
	return result;
}

/*
 * get the filename and extension from the end of an #include directive.
 */

const
char *
get_filename_include(
	const char *str
) {
	const char *result = NULL;
	const char **tokens = split_string(str, "/\\:#\"\n");
	int i = 1;
	while (tokens[i])
		i++;
	result = strdup(tokens[i-1]);
	free_split(tokens);
	return result;
}

/*
 * is a string a possible end of argment flag (--)?
 */

bool
is_endarg(
	const char *str
) {
	return str && strlen(str) == 2 && str[0] == '-' && str[1] == '-';
}

/*
 * does this line begin with a formfeed?
 */

bool
is_formfeed(
	const char *str
) {
	return str && str[0] == '\f' && strlen(str) > 1;
}

/*
 * if the line is an #include directive, does it reference one of the
 * files in --priv? if so, it should be suppressed.
 */

bool
is_suppressable_header(
	char *str
) {
	if (!match(str, pat_include_prefix()))
		return false;
	for (int i = 0; i < ctx.pub_count; i++) {
		/* the .h in the filename will treat the . as a wildcard, but that's
		 * good enough for our purposes. */
		const char *fname = get_filename_include(ctx.argv[ctx.pub_start + 1 + i]);
		const cpat *pat = compile_pattern(fname);
		free((void *)fname);
		assert(pat &&
			"could not compile suppressable header file name pattern");
		if (match(str, pat))
			return true;
		free((void *)pat);
	}
	return false;
}

/*
 * if the line is an #include directive, does it reference a local include?
 * "../inc/xx.h"? if so, replace "../inc/" with ctx.macro_prefix
 */

bool
is_fixable_header(
	char *str
) {
	if (!match(str, pat_fixable_prefix()))
		return false;
	return true;
}

char
to_lowercase(
	char c
) {
	return c + ('a' - 'A');
}

char *
fix_header(
	char *str
) {
	if (ctx.fix_prefix == NULL) {
		fprintf(stderr, "no fix-prefix found, defaulting to start of macro-prefix\n");
		/* this leak doesn't matter */
		char *leaks = strdup(ctx.macro_prefix);
		if (strlen(leaks) > 3)
			leaks[3] = '\0';
		int i = 0;
		while (leaks[i]) {
			if (is_uppercase(leaks[i]))
				leaks[i] = to_lowercase(leaks[i]);
			i += 1;
		}
		ctx.fix_prefix = leaks;
	}
	const char *name = get_filename_include(str);
	size_t over_allocate = strlen(str) * 2 + strlen(name) * 2;
	char *buffer = malloc(over_allocate);
	memset(buffer, 0, over_allocate);
	snprintf(buffer, over_allocate, "#include \"%s%s\"\n", ctx.fix_prefix,
		name);
	free((void *)name);
	return buffer;
}

/*
 * return the position of the long option argument str in the
 * argument list. advance through the list with get_next_optval to get
 * each value that follows the option marker.
 *
 * long arguments are "--optname". you could pass any string to seek
 * in the argument list, but don't. scanning of the argument list
 * stops at the end argument marker "--".
 *
 * returns -1 if the option is not found.
 */

int
get_longopt(
	const char *str
) {
	if (ctx.argv == NULL || str == NULL || *str == '\0' || !is_longopt(str))
		return -1;
	int i = 0;
	while (ctx.argv[i] && !is_endarg(ctx.argv[i])) {
		if (equal_string(str, ctx.argv[i]))
			return i;
		i += 1;
	}
	return -1;
}

/*
 * get the index of the next option value listed after a long option
 * named found with get_longopt. returns the index in the arguments
 * list or -1 if no more values are available.
 *
 * "./prog --input a.txt b.txt --output --error"
 *
 * for "--input" indices for a.txt and b.txt are returned. the next
 * call will return -1 as a new long option name is detected. for
 * "--output" there are no option values before the next long option
 * name, so -1 is returned immediately.
 */

int
get_next_optval(
	int i
) {
	if (ctx.argv[i] == NULL ||
	ctx.argv[i+1] == NULL ||
				is_longopt(ctx.argv[i+1]) ||
				is_endarg(ctx.argv[i+1])) {
		return -1;
	}
	return i+1;
}

/*
 * the macro prefix for header guards should be preceeded by "--macro"
 * in arguments and should begin with an uppercase letter followed by
 * uppercase letters and underscores.
 */

const char *
get_macro_prefix(
	void
) {
	int i = get_longopt("--macro");
	if (i == -1 || ctx.argv[i] == NULL)
		return NULL;
	i = get_next_optval(i);
	if (i == -1 || !is_valid_macro_prefix(ctx.argv[i]))
		return NULL;
	return ctx.argv[i];
}

/*
 * get_fix_prefix
 *
 * fix up includes of other headers in development mode and
 * convert them to release format. in other words:
 *
 * #include "../inc/<not-this-pub-filename>.h"
 *
 * becomes:
 *
 * #include "<fix-prefix><not-this-pub-filename>.h"
 *
 * placement of #defines for implementation generation
 * is left for the user.
 */
const char *
get_fix_prefix(
	void
) {
	int i = get_longopt("--fix-prefix");
	if (i == -1 || ctx.argv[i] == NULL)
		return NULL;
	i = get_next_optval(i);
	if (i == -1 || !is_valid_fix_prefix(ctx.argv[i]))
		return NULL;
	return ctx.argv[i];
}

/*
 * usage and error reporting.
 */

void
usage(
	FILE *where
) {
	if (where == NULL) {
		where = stdout;
	}
	fprintf(where,
		"usage: %s --macro MACRO_PREFIX [--intro <files>] --pub <files> [--priv <files>] [--outro <files>] [--fix-prefix] <lowercase macro prefix>\n\n",
		get_filename_path(ctx.argv[0]));
	fprintf(where,
		"Combines one or more files to create a C single file header library.\n\n");
	fprintf(where,
		" --macro       required  is a prefix for header guard macros.\n");
	fprintf(where,
		" --intro       optional  one or more plain text files to include in a doc\n");
	fprintf(where,
		"                         block at the start of the output file.\n");
	fprintf(where,
		" --pub         required  one or more C files containing externally visible\n");
	fprintf(where,
		"                         declarations to be compiled in an #ifdef MACRO_PREFIX_H\n");
	fprintf(where, "                         block.\n");
	fprintf(where,
		" --priv        optional  one or more C files containing executable code to\n");
	fprintf(where,
		"                         be compiled in an #ifdef MACRO_PREFIX_H_IMPLEMENTATION\n");
	fprintf(where, "                         block.\n");
	fprintf(where,
		" --outro       optional  one or more plain text files to include in a doc\n");
	fprintf(where,
		"                         block at the end of the output file.\n");
	fprintf(where,
		" --fix-prefix  optional  if your public .c file includes dependencies\n");
	fprintf(where,
		"                         on other in library sources that have not been\n");
	fprintf(where,
		"                         packaged (as in \"#include \"..\\inc\\other.h\"\n");
	fprintf(where,
		"                         this prefix will be used to create an include\n");
	fprintf(where,
		"                         for the packaged library as \"<prefix>other.h\"\n\n");
}

/*
 * deal with arguments, get context set up.
 */

/*
 * -h, -?, and --help are synonyms people...
 */

bool
wants_help(
	void
) {
	/* there are many ways to ask for help, respond to the ones we know. */
	for (int i = 1; i < ctx.argc; i++)
		if (equal_string("-?", ctx.argv[i]) ||
			equal_string("-h", ctx.argv[i]) ||
			equal_string("--help", ctx.argv[i]))
			return true;
	return false;
}

/*
 * parse arguments into ctx, report any errors found. all file
 * arguments must be readable, and the macro prefix and at least one
 * public file are required. if not, return error.
 */

bool
arguments_ok(
	void
) {

	bool bad_args = false;

	ctx.macro_prefix = get_macro_prefix();
	if (ctx.macro_prefix == NULL) {
		fprintf(stderr, "macro name required, must be all upper case\n");
		bad_args = true;
	}

	ctx.fix_prefix = get_fix_prefix();


	int i = 1;
	while (ctx.argv[i]) {
		if (is_longopt(ctx.argv[i])) {
			if (equal_string("--macro", ctx.argv[i])) {
				i += 2;
				continue;
			} else if (equal_string("--intro", ctx.argv[i]) && ctx.intro_start == -1) {
				ctx.intro_start = i;
			} else if (equal_string("--pub", ctx.argv[i]) && ctx.pub_start == -1) {
				ctx.pub_start = i;
			} else if (equal_string("--priv", ctx.argv[i]) && ctx.priv_start == -1) {
				ctx.priv_start = i;
			} else if (equal_string("--outro", ctx.argv[i]) && ctx.outro_start == -1) {
				ctx.outro_start = i;
			} else if (equal_string("--fix-prefix", ctx.argv[i])) {
				i += 2;
				continue;
			} else {
				fprintf(stderr, "unknown or redundant option %s\n", ctx.argv[i]);
				bad_args = true;
			}
		} else if (access(ctx.argv[i], R_OK) != 0) {
			fprintf(stderr, "can not read file: %s\n", ctx.argv[i]);
			bad_args = true;
		}
		i += 1;
	}

	if (bad_args || ctx.pub_start == -1)
		fprintf(stderr, "missing or invalid arguments\n");

	/* make sure we have at least one public file */

	i = get_next_optval(ctx.pub_start);
	if (i == -1) {
		bad_args = true;
		fprintf(stderr, "no --pub files provided\n");
	}

	/* count files for each --type */

	if (ctx.intro_start > 0) {
		i = ctx.intro_start + 1;
		ctx.intro_count = 0;
		while (ctx.argv[i] && !is_endarg(ctx.argv[i]) && !is_longopt(ctx.argv[i])) {
			i += 1;
			ctx.intro_count += 1;
		}
		if (ctx.intro_count == 0) {
			bad_args = true;
			fprintf(stderr, "--intro specified but no files provided\n");
		}
	}

	if (ctx.pub_start > 0) {
		i = ctx.pub_start + 1;
		ctx.pub_count = 0;
		while (ctx.argv[i] && !is_endarg(ctx.argv[i]) && !is_longopt(ctx.argv[i])) {
			i += 1;
			ctx.pub_count += 1;
		}
		if (ctx.pub_count == 0) {
			bad_args = true;
			fprintf(stderr, "--pub specified but no files provided\n");
		}
	}

	if (ctx.priv_start > 0) {
		i = ctx.priv_start + 1;
		ctx.priv_count = 0;
		while (ctx.argv[i] && !is_endarg(ctx.argv[i]) && !is_longopt(ctx.argv[i])) {
			i += 1;
			ctx.priv_count += 1;
		}
		if (ctx.priv_count == 0) {
			bad_args = true;
			fprintf(stderr, "--priv specified but no files provided\n");
		}
	}

	if (ctx.outro_start > 0) {
		i = ctx.outro_start + 1;
		ctx.outro_count = 0;
		while (ctx.argv[i] && !is_endarg(ctx.argv[i]) && !is_longopt(ctx.argv[i])) {
			i += 1;
			ctx.outro_count += 1;
		}
		if (ctx.outro_count == 0) {
			bad_args = true;
			fprintf(stderr, "--outro specified but no files provided\n");
		}
	}

	return !bad_args;
}

/*
 * copy file line by line to ouptut, typically stdout. there is minimal
 * error handling. lines are arbitrarily maxed at 4k.
 */

void
print_file(
	FILE *where,
	const char *name
) {
	FILE *there = fopen(name, "r");
	if (there == NULL) {
		fprintf(stderr, "can not open file %s\n", name);
		exit(EXIT_FAILURE);
	}
	const int MAX_LINE = 4095;
	char *line_buffer = malloc(MAX_LINE+1);
	while (fgets(line_buffer, MAX_LINE, there)) {
		if (is_formfeed(line_buffer)) {
			fputs(line_buffer+1, where);
			continue;
		}
		fputs(line_buffer, where);
	}
	if (ferror(there)) {
		fprintf(stderr, "error %d reading file %s\n", errno, name);
		exit(EXIT_FAILURE);
	}
	fclose(there);
}

/*
 * a version of print_file that will suppress any #include
 * directives that appear to reference files in --priv.
 */

void
print_file_suppress_headers(
	FILE *where,
	const char *name
) {
	FILE *there = fopen(name, "r");
	if (there == NULL) {
		fprintf(stderr, "can not open file %s\n", name);
		exit(EXIT_FAILURE);
	}
	const int MAX_LINE = 4095;
	char *line_buffer = malloc(MAX_LINE+1);
	while (fgets(line_buffer, MAX_LINE, there)) {
		if (is_suppressable_header(line_buffer)) {
			continue;
		}
		if (is_formfeed(line_buffer)) {
			fputs(line_buffer+1, where);
			continue;
		}
		if (is_fixable_header(line_buffer)) {
			char *fixed = fix_header(line_buffer);
			fputs(fixed, where);
			free(fixed);
			continue;
		}
		fputs(line_buffer, where);
	}
	if (ferror(there)) {
		fprintf(stderr, "error %d reading file %s\n", errno, name);
		exit(EXIT_FAILURE);
	}
	free(line_buffer);
	fclose(there);
}

/*
 * write each section, --intro, --pub, --priv, and finally --outro.
 *
 * --intro and --outro are expected to be plain text files and their
 * entire contents are bracketed within \/\* and \*\/.
 *
 * --pub and --priv are expected to be valid C source files.
 */

void
write_intro(
	void
) {
	fprintf(stdout, "/*\n * single file header generated via:\n");
	for (int i = 0; i < ctx.argc; i++)
		if (i == 0)
			fprintf(stdout, " * %s ", get_filename_path(ctx.argv[i]));
		else
			fprintf(stdout, "%s ", ctx.argv[i]);

	fprintf(stdout, "\n */\n");
	if (ctx.intro_count > 0) {
		fprintf(stdout, "/* *** begin intro ***\n");
		for (int i = 0; i < ctx.intro_count; i++)
			print_file(stdout, ctx.argv[ctx.intro_start + 1 + i]);
		fprintf(stdout, "   *** end intro ***\n");
		fprintf(stdout, " */\n");
	}
}

void
write_pub(
	void
) {
	fprintf(stdout, "\n#ifndef %s_SINGLE_HEADER\n", ctx.macro_prefix);
	fprintf(stdout, "#define %s_SINGLE_HEADER\n", ctx.macro_prefix);
	if (ctx.pub_count > 0) {
		fprintf(stdout, "/* *** begin pub *** */\n");
		for (int i = 0; i < ctx.pub_count; i++)
			print_file(stdout, ctx.argv[ctx.pub_start + 1 + i]);
		fprintf(stdout, "/* *** end pub *** */\n");
	}
	fprintf(stdout, "\n#endif /* %s_SINGLE_HEADER */\n", ctx.macro_prefix);
}

void
write_priv(
	void
) {
	fprintf(stdout, "\n#ifdef %s_IMPLEMENTATION\n", ctx.macro_prefix);
	fprintf(stdout, "#undef %s_IMPLEMENTATION\n", ctx.macro_prefix);
	if (ctx.priv_count > 0) {
		fprintf(stdout, "/* *** begin priv *** */\n");
		for (int i = 0; i < ctx.priv_count; i++)
			print_file_suppress_headers(stdout, ctx.argv[ctx.priv_start + 1 + i]);
		fprintf(stdout, "/* *** end priv *** */\n");
	}
	fprintf(stdout, "\n#endif /* %s_IMPLEMENTATION */\n", ctx.macro_prefix);
}

void
write_outro(
	void
) {
	if (ctx.outro_count > 0) {
		fprintf(stdout, "/* *** begin outro ***\n");
		for (int i = 0; i < ctx.intro_count; i++)
			print_file(stdout, ctx.argv[ctx.intro_start + 1 + i]);
		fprintf(stdout, "   *** end outro ***\n");
		fprintf(stdout, " */\n");
	}
}

/*
 * a typical mainline ...
 */

int
main(
	int argc,
	const char **argv
) {
	ctx.argc = argc;
	ctx.argv = argv;

	if (wants_help()) {
		usage(stdout);
		return EXIT_SUCCESS;
	}

	if (!arguments_ok()) {
		usage(stderr);
		return EXIT_FAILURE;
	}

	/* write files ... */

	write_intro();
	write_pub();
	write_priv();
	write_outro();

	return EXIT_SUCCESS;
}
