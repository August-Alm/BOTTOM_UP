#include <CUnit/Basic.h>

#include "../src/heap.h"
#include "../src/name.h"
#include "../src/input.h"
#include "../src/token.h"
#include "../src/parse.h"
#include "../src/types.h"

#include <string.h>

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
    memory_free();
}

const char *test2_desc = "2: tokenizing \"\\f.\\x.(f x)\"";
void test2(void)
{
    heap_setup();
    setup_names();

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
    free_names();
    memory_free();
}

const char *test3_desc = "3: parse easy input \"\\x.x\"";
void test3(void)
{
    heap_setup();
    setup_names();

    struct string_handle *sh = new_string_handle(strdup("\\x.x"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    struct node result = parse_node(ih);
    /* Don't know what it should be, leaf, branch or what */

    free_string_handle(sh);
    free_names();
    memory_free();
}

const char *test4_desc = "4: parse easy input \"\\f.\\x.(f x)\"";
void test4(void)
{
    heap_setup();
    setup_names();

    struct string_handle *sh = new_string_handle(strdup("\\f.\\x.(f x)"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    struct node result = parse_node(ih);
    /* Don't know what it should be, leaf, branch or what */

    free_string_handle(sh);
    free_names();
    memory_free();
}

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
    if (!CU_add_test(test_suite, test1_desc, test1)
	|| !CU_add_test(test_suite, test2_desc, test2)
	|| !CU_add_test(test_suite, test3_desc, test3)
	) {
	    CU_cleanup_registry();
	    return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
