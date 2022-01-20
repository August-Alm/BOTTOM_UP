/* ***** ***** */

#include <CUnit/Basic.h>

#include "../src/memory.h"
#include "../src/heap.h"
#include "../src/uplink.h"
#include "../src/leaf.h"
#include "../src/single.h"
#include "../src/branch.h"
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

static
int church_to_int(node_t nd)
{
    int sid, zid;
    int acc = 0;
    if (get_node_kind(nd) == SINGLE_NODE) {
        sid = get_leaf_id(get_leaf(nd));
        nd = get_child(nd);
        if (get_node_kind(nd) == SINGLE_NODE) {
            zid = get_leaf_id(get_leaf(nd));
            nd = get_child(nd);
        }
        else {
            return -1;
        }
    }
    else {
        return -1;
    }

    while(true) {
        switch (get_node_kind(nd)) {
        case LEAF_NODE:
            if (get_leaf_id(nd) == zid) return acc;
            return -1;
        case BRANCH_NODE: {
            node_t lch = get_lchild(nd);
            if ((get_node_kind(lch) == LEAF_NODE) && 
                (get_leaf_id(lch) == sid)) {
                ++acc;
                nd = get_rchild(nd);
                continue;
            }
            return -1;
        }
        default:
            return -1;
        }
    }
}

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

const char *test2_desc = "2: parse and print input \"\\f.\\x.(f x)\"";
void test2(void)
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

const char *test3_desc = "3: parse and print input \"@ f = \\x.x; (f f)\"";
void test3(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup(
        "@ f = \\x.x; (f f)"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t result = parse_node(ih);
    fprintf_node(stdout, result);

    free_string_handle(sh);
    memory_free();
}

const char *test4_desc = "4: normalize \
    \"@ two = \\f.\\z.(s (s z)); (two two)\"";
void test4(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup(
       "@ two = \\f.\\x.(f (f x)); (two two)"
    ));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t result = parse_node(ih);
    normalize(&result);
    CU_ASSERT_EQUAL(church_to_int(result), 4);
    free_string_handle(sh);
    memory_free();
}

const char *test5_desc = "t: article example"
    "\"\\u.\\t.(\\x. @f = \\y.(x (u y));((x f) f) t)\"";
void test5(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup(
       "\\u.\\t.(\\x. @f = \\y.(x (u y));((x f) f) t)"
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

const char *test6_desc = "6: normalize church ((2*5)^2)^2 = 10k.";
void test6(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup(
        "@two = \\s.\\z.(s (s z)); \
        @ five = \\s.\\z.(s (s (s (s (s z))))); \
        @ mul = \\m.\\n.\\s.(m (n s)); \
        @ ten = ((mul two) five); \
        @ hundred = ((mul ten) ten); \
        @ tenk = ((mul hundred) hundred); \
        tenk"));
        //@ oneM = ((mul tenk) tenk); \
        //oneM"));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    CU_ASSERT_PTR_NOT_NULL(ih);
    node_t result = parse_node(ih);
    normalize(&result);
    CU_ASSERT_EQUAL(church_to_int(result), 10000); //1000000);

    free_string_handle(sh);
    memory_free();
}

const char *test7_desc = "7: normalisation of factorial of eight.";
void test7(void)
{
    memory_setup();

    struct string_handle *sh = new_string_handle(strdup(
        "@one = \\s.\\z.(s z); \
        @ oneone = \\g.((g one) one); \
        @ snd = \\a.\\b.b; \
        @ F = \\p.(p \\a.\\b.\\g.((g \\s.\\z.(s ((a s) z))) \\s.(a (b s)))); \
        @ fact = \\k.(((k F) oneone) snd); \
        @ eight = \\s.\\z.(s (s (s (s (s (s (s (s z)))))))); \
        (fact eight)"
    ));
    CU_ASSERT_PTR_NOT_NULL(sh);
    struct input_handle *ih = input_from_string(sh);
    node_t result = parse_node(ih);
    normalize(&result);
    CU_ASSERT_EQUAL(church_to_int(result), 40320);

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