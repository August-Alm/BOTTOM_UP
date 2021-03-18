#include "../src/heap.h"
#include "../src/parse.h"
#include "../src/name.h"
#include "../src/malcheck.h"

#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>

extern char * strdup(const char *);

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

const char *test1_desc = "1: Check if starting and destroying the heap works\n";
void test1(void)
{
    CU_ASSERT_PTR_NULL(heap);
    heap_setup();
    CU_ASSERT_PTR_NOT_NULL(heap);
    memory_free();
}

const char *test2_desc = "2: Read from file and produce input struct\n";
void test2(void)
{
    FILE *fp = fopen("test/TEST_INPUT.txt", "r");

    CU_ASSERT_PTR_NOT_NULL(fp);

    struct input *read = read_file(fp);
    CU_ASSERT_PTR_NOT_NULL(read);

    fclose(fp);

    CU_ASSERT_STRING_EQUAL(read->data, "CORRECT\n");
    CU_ASSERT_EQUAL(read->max, 8);
    free(read->data);
    free(read);
}

const char *test3_desc = "3: Try peek() and get() from input struct\n";
void test3(void)
{
    struct input *inp = malloc(sizeof(struct input));
    char *str = "hello";
    *inp = (struct input) {.data = strdup(str), .index = 0, .peek_index = 0, .max = 5};

    CU_ASSERT_EQUAL(str[0], parse_peek(inp));
    CU_ASSERT_EQUAL(str[0], parse_get(inp));
    CU_ASSERT_EQUAL(str[1], parse_peek(inp));
    CU_ASSERT_EQUAL(str[1], parse_get(inp));
    CU_ASSERT_EQUAL(str[2], parse_peek(inp));
    CU_ASSERT_EQUAL(str[2], parse_get(inp));
    CU_ASSERT_EQUAL(str[3], parse_peek(inp));
    CU_ASSERT_EQUAL(str[3], parse_peek(inp));
    CU_ASSERT_EQUAL(str[3], parse_get(inp));

    free(inp->data);
    free(inp);
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
