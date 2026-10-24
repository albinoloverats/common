#include "test_support.h"

static void test_map_add_lookup_remove(void **state)
{
	(void)state;
	map_t map = map_init(compare_string, false, true, false);
	assert_non_null(map);

	const char *keys[] = { "charlie", "alpha", "bravo" };
	const char *values[] = { "3", "1", "2" };
	for (size_t i = 0; i < 3; i++)
		assert_true(map_add(map, keys[i], values[i]));
	assert_false(map_add(map, "alpha", "duplicate"));
	assert_int_equal(map_size(map), 3);
	assert_true(map_contains(map, "bravo"));
	assert_string_equal(map_get(map, "bravo"), "2");
	assert_null(map_get(map, "missing"));
	assert_string_equal(map_remove(map, "alpha"), "1");
	assert_null(map_remove(map, "missing"));
	assert_int_equal(map_size(map), 2);

	map_deinit(map);
}


static void test_map_overwrite_existing_value(void **state)
{
	(void)state;
	map_t map = map_init(compare_string, false, true, true);
	assert_non_null(map);
	assert_true(map_add(map, "key", "old"));
	assert_true(map_add(map, "key", "new"));
	assert_int_equal(map_size(map), 1);
	assert_string_equal(map_get(map, "key"), "new");
	map_deinit(map);
}


static void test_map_iteration_and_owned_overwrite(void **state)
{
	(void)state;
	map_t map = map_init(compare_string, true, false, true);
	assert_non_null(map);
	visited_items = 0;
	map_for_each(map, count_map_item);
	assert_int_equal(visited_items, 0);
	char *key = m_strdup("owned-key");
	char *old_value = m_strdup("old");
	char *new_value = m_strdup("new");
	assert_true(map_add(map, key, old_value));
	assert_true(map_add(map, "owned-key", new_value));
	assert_int_equal(map_size(map), 1);
	assert_string_equal(map_get(map, "owned-key"), "new");

	iter_t iter = map_iterator(map);
	assert_true(map_has_next(iter));
	const pair_object_s *entry = map_get_next(iter);
	assert_string_equal(entry->p1, "owned-key");
	assert_string_equal(entry->p2, "new");
	assert_false(map_has_next(iter));
	free(iter);
	visited_items = 0;
	map_for_each(map, count_map_item);
	assert_int_equal(visited_items, 1);

	assert_string_equal(map_remove(map, "owned-key"), "new");
	free(new_value);
	assert_int_equal(map_size(map), 0);
	assert_true(map_add(map, m_strdup("another"), m_strdup("owned")));
	map_deinit(map);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_map_add_lookup_remove),
	cmocka_unit_test(test_map_overwrite_existing_value),
	cmocka_unit_test(test_map_iteration_and_owned_overwrite)
};

REGISTER_TESTS(map, tests)
