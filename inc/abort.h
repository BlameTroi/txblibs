/* abort.h -- blametroi's common utility functions -- */

/*
 * a header only implementation of abort and report helper functions
 * and macros.
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

/* TODO variadic arguments and better error messages */

/*
 * these are called via macro wrappers abort and abort_if, in an
 * attempt to provide better diagnostics than straight asserts.
 */

void
do_abort(
	char *msg,
	char *func,
	char *file,
	int line
);

void
do_abort_if(
	char *cond,
	char *msg,
	char *func,
	char *file,
	int line
);

#define abort(msg) do {\
   do_abort(msg, __func__, __FILE__, __LINE__);\
   } while(0)

#define abort_if(cond, msg) do {\
   if (cond) {\
   do_abort_if(#cond, msg, __func__, __FILE__, __LINE__);\
   }\
   } while(0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* abort.h ends here */
