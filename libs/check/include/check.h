/*-*- mode:C; -*- */
/*
 * Check: a unit test framework for C
 * Copyright (C) 2001, 2002, Arien Malec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CHECK_H
#define CHECK_H

#include <okl4/arch/config.h>

/* Check: a unit test framework for C

   Check is a unit test framework for C. It features a simple
   interface for defining unit tests, putting little in the way of the
   developer. Tests are run in a separate address space, so Check can
   catch both assertion failures and code errors that cause
   segmentation faults or other signals. The output from unit tests
   can be used within source code editors and IDEs.

   Unit tests are created with the START_TEST/END_TEST macro
   pair. The fail_unless and fail macros are used for creating
   checks within unit tests; the mark_point macro is useful for
   trapping the location of signals and/or early exits.


   Test cases are created with tcase_create, unit tests are added
   with tcase_add_test


   Suites are created with suite_create; test cases are added
   with suite_add_tcase

   Suites are run through an SRunner, which is created with
   srunner_create. Additional suites can be added to an SRunner with
   srunner_add_suite. An SRunner is freed with srunner_free, which also
   frees all suites added to the runner. 

   Use srunner_run_all to run a suite and print results.

*/

/* CK_WITHPAGER
 * This is an additional forking mode added specifically for L4
 * It will create a second thread in the privileged address space and run the
 * unit tests on that thread.  The initial thread will act as a pager and
 * exception handler for all created threads.
 *
 * This allows tests to be easily adapted to run in different address spaces
 *
 * The pager maps everything 1 to 1 with the roottasks address space, with no 
 * permission enforcement.  It will treat all accesses to addresses below 8k
 * as null pointer dereference errors.
 */

#ifdef __cplusplus
#define CK_CPPSTART extern "C" {
CK_CPPSTART
#endif

#ifdef __ARMv__
#if (__ARMv__ == 5)
#define NO_UTCB_RELOCATE
#endif
#endif /* __ARMv__ */

#include <okl4/kernel.h>
#include <okl4/types.h>

/* check version numbers */

#define CHECK_MAJOR_VERSION (0)
#define CHECK_MINOR_VERSION (9)
#define CHECK_MICRO_VERSION (1)

extern int check_major_version;
extern int check_minor_version;
extern int check_micro_version;

/* lib check capid */
extern okl4_kcap_t libcheck;

/* opaque type for a test case

   A TCase represents a test case.  Create with tcase_create, free
   with tcase_free.  For the moment, test cases can only be run
   through a suite
*/
  
typedef struct TCase TCase; 

/* type for a test function */
typedef void (*TFun) (void);

/* type for a setup/teardown function */
typedef void (*SFun) (void);
 
/* Opaque type for a test suite */
typedef struct Suite Suite;
 
/* Creates a test suite with the given name
*/
Suite *suite_create (const char *name);

/* Add a test case to a suite */
void suite_add_tcase (Suite *s, TCase *tc);

/* Create a test case */
TCase *tcase_create (const char *name);

/* Add a test function to a test case
  (macro version) */
#define tcase_add_test(tc,tf) _tcase_add_test(tc,tf,"" # tf "")

/* Add a test function to a test case
  (function version -- use this when the macro won't work */
void _tcase_add_test (TCase *tc, TFun tf, const char *fname);

/*
 * Add unchecked fixture setup/teardown functions to a test case
 *
 * Unchecked fixtures are run once at the beginning and end of each set of unit
 * tests. If a memory access error occurs, it will bring down the
 * entire test suite.
 */
void tcase_add_unchecked_fixture (TCase *tc, SFun setup, SFun teardown);

/*
 * Add fixture setup/teardown functions to a test case.
 *
 * These setup/teardown functions are carried out by a new thread,
 * and are run at the start and end of each individual unit test.
 */
void tcase_add_checked_fixture (TCase *tc, SFun setup, SFun teardown);

/* Internal function to mark the start of a test function */
void tcase_fn_start (const char *fname, const char *file, int line);

/* Start a unit test with START_TEST(unit_name), end with END_TEST
   One must use braces within a START_/END_ pair to declare new variables */

#define START_TEST(__testname)\
static void __testname (void)\
{\
  tcase_fn_start (""# __testname, __FILE__, __LINE__);

/* End a unit test */
#define END_TEST }


/* Fail the test case unless expr is true */
#define fail_unless(expr, msg)                                                \
        do {                                                                  \
            _fail_unless(expr, __FILE__, __LINE__,                            \
                    (((const char *)(msg)) != NULL) ? (msg) :                 \
                            "Assertion '"#expr"' failed");                    \
        } while (0)

/* Fail the test case if expr is true */
#define fail_if(expr, msg)                                                    \
        do {                                                                  \
            _fail_unless(!(expr), __FILE__, __LINE__,                         \
                    ((const char *)(msg) != NULL) ? (msg) :                   \
                            "Assertion '"#expr"' failed");                    \
        } while (0)

/* Always fail */
#define fail(msg) _fail_unless(0, __FILE__, __LINE__, msg)

/* Non macro version of #fail_unless, with more complicated interface */
void _fail_unless (int result, const char *file,
                   int line, const char *msg, ...);

/* Mark the last point reached in a unit test
   
   (useful for tracking down where a segfault, etc. occurs) */
#define mark_point() _mark_point(__FILE__,__LINE__)

/* Non macro version of #mark_point */
void _mark_point (const char *file, int line);

/* Result of a test */
enum test_result {
  CK_PASS, /* Test passed*/
  CK_FAILURE, /* Test completed but failed */
  CK_ERROR, /* Test failed to complete (signal or non-zero early exit) */
  CK_UNSET = -1
};

/* Specifies the how much output an SRunner should produce */
enum print_output {
  CK_SILENT, /* No output */
  CK_MINIMAL, /* Only summary output */
  CK_NORMAL, /* All failed tests */
  CK_VERBOSE, /* All tests */
  CK_ENV, /* Look at environment var */
  CK_LAST
};


/* Holds state for a running of a test suite */
typedef struct SRunner SRunner;

/* Opaque type for a test failure */
typedef struct TestResult TestResult;

/* accessors for tr fields */

 enum ck_result_ctx {
  CK_CTX_SETUP,
  CK_CTX_TEST,
  CK_CTX_TEARDOWN,
  CK_CTX_UNSET = -1,
};

/* Type of result */
int tr_rtype (TestResult *tr);
/* Context in which the result occurred */ 
enum ck_result_ctx tr_ctx (TestResult *tr); 
/* Failure message */
const char *tr_msg (TestResult *tr);
/* Line number at which failure occured */
int tr_lno (TestResult *tr);
/* File name at which failure occured */
const char *tr_lfile (TestResult *tr);
/* Test case in which unit test was run */
const char *tr_tcname (TestResult *tr);

/* Creates an SRunner for the given suite */
SRunner *srunner_create (Suite *s);

/* Adds a Suite to an SRunner */
void srunner_add_suite (SRunner *sr, Suite *s);

/* Frees an SRunner, all suites added to it and all contained test cases */
void srunner_free (SRunner *sr);

/* Test running */

/* Runs an SRunner, printing results as specified (see enum
   print_output)*/
void srunner_run_all (SRunner *sr, enum print_output print_mode);

/* Next functions are valid only after the suite has been
   completely run, of course */

/* Number of failed tests in a run suite

  Includes failures + errors */
int srunner_ntests_failed (SRunner *sr);

/* Total number of tests run in a run suite */
int srunner_ntests_run (SRunner *sr);

/* Return an array of results for all failures
  
   Number of failures is equal to srunner_nfailed_tests.  Memory for
   the array is malloc'ed and must be freed, but individual TestResults
   must not */

TestResult **srunner_failures (SRunner *sr);

/* Return an array of results for all run tests

   Number of results is equal to srunner_ntests_run, and excludes
   failures due to setup function failure.

   Memory is malloc'ed and must be freed, but individual TestResults
   must not */
  
TestResult **srunner_results (SRunner *sr);

/* a list of test names *NOT* to run */
void srunner_exceptions(SRunner * s, char * exceptions[]);

/* Printing */

/* Print the results contained in an SRunner
*/
 
void srunner_print (SRunner *sr, enum print_output print_mode);
  
  
/* Set a log file to which to write during test running.

  Log file setting is an initialize only operation -- it should be
  done immediatly after SRunner creation, and the log file can't be
  changed after being set.
 */
void srunner_set_log (SRunner *sr, const char *fname);

/* Does the SRunner have a log file?
*/
int srunner_has_log (SRunner *sr);

/* Return the name of the log file, or NULL if none
*/
const char *srunner_log_fname (SRunner *sr);

/* Set a xml file to which to write during test running.

  XML file setting is an initialize only operation -- it should be
  done immediatly after SRunner creation, and the XML file can't be
  changed after being set.
 */
void srunner_set_xml (SRunner *sr, const char *fname);

/* Does the SRunner have a log file?
*/
int srunner_has_xml (SRunner *sr);

/* Return the name of the XML file, or NULL if none
*/
const char *srunner_xml_fname (SRunner *sr);

/* Control thread creation */
enum fork_status {
  CK_FORK_UNSPECIFIED = -1,
  CK_SIMPLE = 0,
#if defined(OKL4_KERNEL_MICRO)
  CK_WITHPAGER = 1,
#endif
};
 
enum fork_status srunner_fork_status (SRunner *sr);
void srunner_set_fork_status (SRunner *sr, enum fork_status fstat); 
  
#ifdef __cplusplus 
#define CK_CPPEND }
CK_CPPEND
#endif

#endif /* CHECK_H */
