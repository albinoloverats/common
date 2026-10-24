#include "test_support.h"

static void test_tlv_add_lookup_and_export(void **state)
{
	(void)state;
	tlv_t tlv = tlv_init();
	assert_non_null(tlv);
	visited_items = 0;
	tlv_for_each(tlv, count_tlv_item);
	assert_int_equal(visited_items, 0);
	const uint8_t first[] = { 0x11, 0x22 };
	const uint8_t second[] = { 0x33 };

	assert_true(tlv_append(tlv, (tlv_entry_s){ 7, sizeof first, (void *)first }));
	assert_true(tlv_append(tlv, (tlv_entry_s){ 2, sizeof second, (void *)second }));
	assert_false(tlv_append(tlv, (tlv_entry_s){ 7, sizeof second, (void *)second }));
	assert_true(tlv_has_tag(tlv, 7));
	assert_false(tlv_has_tag(tlv, 9));
	assert_int_equal(tlv_length_of(tlv, 7), sizeof first);
	assert_memory_equal(tlv_value_of(tlv, 7), first, sizeof first);
	assert_ptr_equal(tlv_value_of(tlv, 9, (uint8_t *)"fallback"), "fallback");
	assert_int_equal(tlv_size(tlv), 2);
	assert_int_equal(tlv_length(tlv), 9);
	visited_items = 0;
	tlv_for_each(tlv, count_tlv_item);
	assert_int_equal(visited_items, 12);

	const uint8_t expected[] =
	{
		7, 0, 2, 0x11, 0x22,
		2, 0, 1, 0x33
	};
	assert_memory_equal(tlv_export(tlv), expected, sizeof expected);
	const uint8_t little_endian[] =
	{
		7, 2, 0, 0x11, 0x22,
		2, 1, 0, 0x33
	};
	assert_memory_equal(tlv_export(tlv, false), little_endian, sizeof little_endian);
	tlv_deinit(tlv);
}


static void test_tlv_remove_and_iteration(void **state)
{
	(void)state;
	tlv_t tlv = tlv_init();
	assert_non_null(tlv);
	uint8_t value_a = 0xa;
	uint8_t value_b = 0xb;
	assert_true(tlv_append(tlv, (tlv_entry_s){ 1, 1, &value_a }));
	assert_true(tlv_append(tlv, (tlv_entry_s){ 2, 1, &value_b }));

	const tlv_entry_s *removed = tlv_remove_tag(tlv, 1);
	assert_non_null(removed);
	assert_int_equal(removed->tag, 1);
	free(removed->value);
	free((void *)removed);
	assert_false(tlv_has_tag(tlv, 1));
	assert_int_equal(tlv_size(tlv), 1);

	iter_t iter = tlv_iterator(tlv);
	assert_non_null(iter);
	assert_true(tlv_has_next(iter));
	const tlv_entry_s *entry = tlv_get_next(iter);
	assert_int_equal(entry->tag, 2);
	assert_int_equal(*(const uint8_t *)entry->value, value_b);
	assert_false(tlv_has_next(iter));
	free(iter);
	tlv_deinit(tlv);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_tlv_add_lookup_and_export),
	cmocka_unit_test(test_tlv_remove_and_iteration)
};

REGISTER_TESTS(tlv, tests)
