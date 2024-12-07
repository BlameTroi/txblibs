/* mylibs.c -- combined single header libraries -- troy brumley */

/* just compiling into one object file for testing */

/* including both txbkv and txbsb is throwing an error in eglot/clangd
 * somewhere. it's bogus, as this file compiles cleanly with with the
 * same compile_commands.json created by cmake/ninja using clang. */

#define TXBALLOC_IMPLEMENTATION
#include "txballoc.h"

#define TXBALIST_IMPLEMENTATION
#include "txbalist.h"

#define TXBLOG2_IMPLEMENTATION
#include "txblog2.h"

#define TXBDL_IMPLEMENTATION
#include "txbdl.h"

#define TXBKL_IMPLEMENTATION
#include "txbkl.h"

#define TXBKV_IMPLEMENTATION
#include "txbkv.h"

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

#define TXBRAND_IMPLEMENTATION
#include "txbrand.h"

#define TXBRS_IMPLEMENTATION
#include "txbrs.h"

#define TXBSB_IMPLEMENTATION
#include "txbsb.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

/* mylibs.c ends here */
