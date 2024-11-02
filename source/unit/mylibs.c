/* mylibs.c -- combined single header libraries -- troy brumley */

/* just compiling into one object file for testing */

/* including both txbkv and txbsb is throwing an error in eglot/clangd
 * somewhere. it's bogus, as this file compiles cleanly with with the
 * same compile_commands.json created by cmake/ninja using clang. */

#define TXBDA_IMPLEMENTATION
#include "txbda.h"

#define TXBDL_IMPLEMENTATION
#include "txbdl.h"

#define TXBKL_IMPLEMENTATION
#include "txbkl.h"

#define TXBKV_IMPLEMENTATION
#include "txbkv.h"

#define TXBLL_IMPLEMENTATION
#include "txbll.h"

#define TXBMD5_IMPLEMENTATION
#include "txbmd5.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define TXBPAT_IMPLEMENTATION
#include "txbpat.h"

#define TXBPMUTE_IMPLEMENTATION
#include "txbpmute.h"

#define TXBPQ_IMPLEMENTATION
#include "txbpq.h"

#define TXBQU_IMPLEMENTATION
#include "txbqu.h"

#define TXBRAND_IMPLEMENTATION
#include "txbrand.h"

#define TXBRS_IMPLEMENTATION
#include "txbrs.h"

#define TXBSB_IMPLEMENTATION
#include "txbsb.h"

#define TXBST_IMPLEMENTATION
#include "txbst.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

/* mylibs.c ends here */
