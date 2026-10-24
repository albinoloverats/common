#include "test_support.h"

static void test_list_sorted_add_and_unique_items(void **state)
{
	(void)state;
	list_t list = list_init(compare_integer, false, true);
	assert_non_null(list);

	int values[] = { 7, 2, 5, 2 };
	assert_true(list_add(list, &values[0]));
	assert_true(list_add(list, &values[1]));
	assert_true(list_add(list, &values[2]));
	assert_false(list_add(list, &values[3]));
	assert_int_equal(list_size(list), 3);

	assert_ptr_equal(list_get(list, 0), &values[1]);
	assert_ptr_equal(list_get(list, 1), &values[2]);
	assert_ptr_equal(list_get(list, 2), &values[0]);
	assert_null(list_get(list, 3));

	list_deinit(list);
}


static void test_builtin_list_comparators(void **state)
{
	(void)state;
	list_t integers = list_integer();
	int64_t iv[] = { INT64_MAX, INT64_MIN, 0 };
	for (size_t i = 0; i < 3; i++)
		assert_true(list_add(integers, &iv[i]));
	assert_ptr_equal(list_get(integers, 0), &iv[1]);
	assert_ptr_equal(list_get(integers, 1), &iv[2]);
	assert_ptr_equal(list_get(integers, 2), &iv[0]);
	list_deinit(integers);

	list_t decimals = list_decimal();
	long double dv[] = { 1.5L, 1.0L, 1.25L };
	for (size_t i = 0; i < 3; i++)
		assert_true(list_add(decimals, &dv[i]));
	assert_ptr_equal(list_get(decimals, 0), &dv[1]);
	assert_ptr_equal(list_get(decimals, 1), &dv[2]);
	assert_ptr_equal(list_get(decimals, 2), &dv[0]);
	list_deinit(decimals);
}


static void test_list_copy_sort_and_remove_item(void **state)
{
	(void)state;
	list_t list = list_default();
	list_sort(list);
	visited_items = 0;
	list_for_each(list, count_list_item);
	assert_int_equal(visited_items, 0);
	int values[] = { 3, 1, 2 };
	for (size_t i = 0; i < 3; i++)
		assert_true(list_append(list, &values[i]));

	list_t copy = list_copy(list, copy_integer);
	assert_int_equal(list_size(copy), 3);
	assert_ptr_not_equal(list_get(copy, 0), &values[0]);
	assert_int_equal(*(const int *)list_get(copy, 0), 3);
	list_add_comparator(copy, compare_integer);
	list_sort(copy);
	assert_int_equal(*(const int *)list_get(copy, 0), 1);
	assert_int_equal(*(const int *)list_get(copy, 2), 3);
	const void *middle = list_get(copy, 1);
	assert_ptr_equal(list_remove_item(copy, middle), middle);
	assert_int_equal(list_size(copy), 2);
	visited_items = 0;
	list_for_each(copy, count_list_item);
	assert_int_equal(visited_items, 4);

	list_deinit(copy, free);
	list_deinit(list);
}


static void test_list_insert_remove_and_iteration(void **state)
{
	(void)state;
	list_t list = list_default();
	assert_non_null(list);

	int values[] = { 1, 2, 3, 4 };
	assert_true(list_append(list, &values[0]));
	assert_true(list_append(list, &values[2]));
	assert_true(list_insert(list, 1, &values[1]));
	assert_true(list_insert(list, 99, &values[3]));
	assert_int_equal(list_size(list), 4);
	assert_ptr_equal(list_remove_index(list, 1), &values[1]);
	assert_ptr_equal(list_remove_index(list, 99), NULL);

	iter_t iter = list_iterator(list);
	assert_non_null(iter);
	assert_true(list_has_next(iter));
	assert_ptr_equal(list_get_next(iter), &values[0]);
	assert_ptr_equal(list_get_next(iter), &values[2]);
	assert_ptr_equal(list_get_next(iter), &values[3]);
	assert_false(list_has_next(iter));
	free(iter);
	list_deinit(list);
}


static void test_list_append_after_removing_tail(void **state)
{
	(void)state;
	list_t list = list_default();
	int first = 1;
	int second = 2;
	assert_true(list_append(list, &first));
	assert_ptr_equal(list_remove_item(list, &first), &first);
	assert_true(list_append(list, &second));
	assert_ptr_equal(list_get(list, 0), &second);
	assert_ptr_equal(list_remove_index(list, 0), &second);
	assert_true(list_append(list, &first));
	assert_ptr_equal(list_get(list, 0), &first);
	list_deinit(list);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_list_sorted_add_and_unique_items),
	cmocka_unit_test(test_builtin_list_comparators),
	cmocka_unit_test(test_list_copy_sort_and_remove_item),
	cmocka_unit_test(test_list_insert_remove_and_iteration),
	cmocka_unit_test(test_list_append_after_removing_tail)
};

REGISTER_TESTS(list, tests)
