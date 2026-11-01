#include "test_support.h"

static void *same_list_item(const void *item)
{
	return (void *)item;
}

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

static void test_list_empty_and_bulk_operations(void **state)
{
	(void)state;
	list_t null_list = NULL;
	assert_int_equal(list_size(null_list), 0);
	assert_null(list_copy(null_list, copy_integer));
	assert_false(list_append(null_list, &null_list));
	assert_false(list_insert(null_list, 0, &null_list));
	assert_false(list_add(null_list, &null_list));
	assert_int_equal(list_add_all(null_list, null_list, copy_integer), -1);
	assert_null(list_contains(null_list, &null_list));
	assert_null(list_remove_item(null_list, &null_list));
	assert_null(list_remove_index(null_list, 0));
	assert_null(list_get(null_list, 0));
	assert_null(list_iterator(null_list));
	assert_null(list_get_next(null_list));
	assert_false(list_has_next(null_list));
	list_for_each(null_list, count_list_item);
	list_sort(null_list);
	list_add_comparator(null_list, compare_integer);
	list_deinit(null_list);

	list_t empty = list_default();
	list_t empty_copy = list_copy(empty, copy_integer);
	assert_non_null(empty_copy);
	assert_int_equal(list_size(empty_copy), 0);
	assert_null(list_remove_item(empty, &null_list));
	list_deinit(empty_copy);

	int values[] = { 3, 1, 2 };
	list_t source = list_default();
	for (size_t i = 0; i < 3; i++)
		assert_true(list_append(source, &values[i]));
	int four = 4, five = 5, six = 6;
	list_t editable = list_init(compare_integer, false, false);
	for (size_t i = 0; i < 3; i++)
		assert_true(list_append(editable, &values[i]));
	assert_true(list_insert(editable, 0, &four));
	assert_true(list_insert(editable, 3, &five));
	assert_false(list_insert(editable, 1, &values[0]));
	assert_true(list_add(editable, &six));
	assert_int_equal(list_size(editable), 6);
	assert_ptr_equal(list_remove_index(editable, 3), &five);
	list_deinit(editable);

	list_t sorted = list_init(compare_integer, true, true);
	assert_int_equal(list_add_all(sorted, source, copy_integer), 3);
	assert_int_equal(*(const int *)list_get(sorted, 0), 1);
	assert_int_equal(*(const int *)list_get(sorted, 2), 3);
	int *four_copy = malloc(sizeof *four_copy);
	assert_non_null(four_copy);
	*four_copy = four;
	assert_true(list_insert(sorted, 0, four_copy));
	assert_int_equal(*(const int *)list_get(sorted, 0), 1);
	assert_true(list_add(sorted, four_copy));
	assert_int_equal(list_size(sorted), 5);
	assert_int_equal(list_add_all(sorted, empty, copy_integer), 0);
	assert_int_equal(list_add_all(empty, null_list, copy_integer), -1);
	assert_null(list_contains(source, &four));
	assert_null(list_remove_item(source, &four));
	list_t unique = list_init(compare_integer, false, false);
	assert_true(list_append(unique, &values[0]));
	assert_int_equal(list_add_all(unique, unique, same_list_item), 0);
	assert_int_equal(list_size(unique), 1);
	list_deinit(unique);
	assert_ptr_equal(list_remove_item(sorted, four_copy), four_copy);
	free(four_copy);
	assert_int_equal(list_size(sorted), 3);
	iter_t empty_iter = list_iterator(empty);
	assert_null(list_get_next(empty_iter));
	free(empty_iter);
	list_deinit(sorted, free);
	list_deinit(source);
	list_t sorted_empty = list_init(compare_integer, true, true);
	list_sort(sorted_empty);
	assert_int_equal(list_size(sorted_empty), 0);
	list_deinit(sorted_empty);
	list_deinit(empty);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_list_sorted_add_and_unique_items),
	cmocka_unit_test(test_builtin_list_comparators),
	cmocka_unit_test(test_list_copy_sort_and_remove_item),
	cmocka_unit_test(test_list_insert_remove_and_iteration),
	cmocka_unit_test(test_list_append_after_removing_tail),
	cmocka_unit_test(test_list_empty_and_bulk_operations)
};

REGISTER_TESTS(list, tests)
