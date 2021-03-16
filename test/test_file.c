#include <CUnit/Basic.h>

#include "../src/heap.h"

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

const char *test1_desc = "Test 1: check if starting and destroying the heap works\n";
void test1(void)
{
    CU_ASSERT_PTR_NULL(heap);
    heap_setup();
    CU_ASSERT_PTR_NOT_NULL(heap);
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
    if (
	!CU_add_test(test_suite, test1_desc, test1)
	) {
	CU_cleanup_registry();
	return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
