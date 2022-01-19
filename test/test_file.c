/* ***** ***** */

#include <CUnit/Basic.h>

#include "../src/memory.h"
#include "../src/heap.h"
#include "../src/uplink.h"
#include "../src/node.h"
#include "../src/name.h"
#include "../src/input.h"
#include "../src/token.h"
#include "../src/parse.h"
#include "../src/types.h"
#include "../src/print.h"
#include "../src/normalize.h"

#include <string.h>

/* ***** ***** */

extern char * strdup(const char *);

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

const char *test1_desc = "1: check if starting and destroying the heap works\n";
void test1(void)
{
    CU_ASSERT_PTR_NULL(heap);
    heap_setup();
    CU_ASSERT_PTR_NOT_NULL(heap);
    heap_free();
}

const char *test2_desc = "2: tokenizing \"\\f.\\x.(f x)\"";
void test2(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup("\\f.\\x.(f x)"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    
    struct token tok = read_token(ih);
    while (tok.tag != T_EOF) {
        fprintf_token(stdout, tok);
        tok = read_token(ih);
    }

    free_string_handle(sh);
    memory_free();
}

const char *test3_desc = "3: parse and print input \"\\f.\\x.(f x)\"";
void test3(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup("\\f.\\x.(f x)"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t result = parse_node(ih);
    fprintf_node(stdout, result);

    free_string_handle(sh);
    memory_free();
}

const char *test4_desc = "4: parse and print input \"@ f = \\x.x; (f f)\"";
void test4(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup("@ f = \\x.x; (f f)"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t result = parse_node(ih);
    fprintf_node(stdout, result);

    free_string_handle(sh);
    memory_free();
}

const char *test5_desc = "5: normalize_wh input \"(\\f.(f f) \\x.x)\"";
void test5(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup("(\\f.(f f) \\x.x)"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t result = parse_node(ih);
    normalize_wh(&result);
    fprintf_node(stdout, result);

    free_string_handle(sh);
    memory_free();
}

const char *test6_desc = "6: normalize \"@ two = \\f.\\x.(f (f x)); (two two)\"";
void test6(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup(
       "@ two = \\f.\\x.(f (f x)); (two two)"
    ));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t result = parse_node(ih);
    for (int i = 0; i < 40; ++i) {
        fprintf(stderr, "heap[%i] = %i\n", i, heap[i]);
    }
    normalize(&result);
    fprintf_node(stdout, result);
 
    free_string_handle(sh);
    memory_free();
}

const char *test7_desc = "t: article example"
    "\"\\u.\\t.(\\x. @ f = \\y.(x (u y)); \r\n ((x f) f) t)\"";
void test7(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup(
       "\\u.\\t.(\\x. @ f = \\y.(x (u y)); \r\n ((x f) f) t)"
    ));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t res = parse_node(ih);
    normalize(&res);
    fprintf_node(stdout, res);

    free_string_handle(sh);
    memory_free();
}



/* ***** ***** */

int main(void)
{
    CU_pSuite test_suite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry()) {
	    return CU_get_error();
    }

    test_suite = CU_add_suite("Tests", init_suite, clean_suite);

    if (!test_suite) {
	    CU_cleanup_registry();
	    return CU_get_error();
    }
    if (
        !CU_add_test(test_suite, test1_desc, test1) ||
        !CU_add_test(test_suite, test2_desc, test2) ||
        !CU_add_test(test_suite, test3_desc, test3) ||
        !CU_add_test(test_suite, test4_desc, test4) ||
        !CU_add_test(test_suite, test5_desc, test5) ||
        !CU_add_test(test_suite, test6_desc, test6) ||
        !CU_add_test(test_suite, test7_desc, test7)

	) {
	    CU_cleanup_registry();
	    return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

/* ***** ***** */

// end of test_file.c