#include "test_support.h"

/*!
 * \brief Verifies TLV insertion, lookup, iteration, length, and export.
 */
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
	/* Export can also encode 16-bit lengths without network-byte-order conversion. */
	const uint8_t little_endian[] =
	{
		7, 2, 0, 0x11, 0x22,
		2, 1, 0, 0x33
	};
	assert_memory_equal(tlv_export(tlv, false), little_endian, sizeof little_endian);
	tlv_deinit(tlv);
}


/*!
 * \brief Verifies removing a tag and iterating over the remaining entries.
 */
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

/*!
 * \brief Verifies NULL/empty TLV behavior and exact tag-and-length removal.
 */
static void test_tlv_empty_null_and_exact_removal(void **state)
{
	(void)state;
	tlv_t null_tlv = NULL;
	uint8_t value = 0x5a;
	assert_false(tlv_append(null_tlv, (tlv_entry_s){ 1, 1, &value }));
	assert_null(tlv_remove(null_tlv, (tlv_entry_s){ 1, 0, NULL }));
	assert_null(tlv_remove_tag(null_tlv, 1));
	assert_null(tlv_get(null_tlv, 1));
	assert_false(tlv_has_tag(null_tlv, 1));
	assert_null(tlv_value_of(null_tlv, 1));
	assert_null(tlv_value_of(null_tlv, 1, &value));
	assert_int_equal(tlv_length_of(null_tlv, 1), 0);
	assert_null(tlv_export(null_tlv));
	assert_int_equal(tlv_length(null_tlv), 0);
	assert_null(tlv_iterator(null_tlv));
	assert_null(tlv_get_next(null_tlv));
	assert_false(tlv_has_next(null_tlv));
	tlv_for_each(null_tlv, count_tlv_item);
	tlv_deinit(null_tlv);

	tlv_t tlv = tlv_init();
	assert_non_null(tlv_export(tlv));
	assert_int_equal(tlv_length(tlv), 0);
	assert_null(tlv_get(tlv, 1));
	assert_false(tlv_has_tag(tlv, 1));
	assert_int_equal(tlv_length_of(tlv, 1), 0);

	assert_true(tlv_append(tlv, (tlv_entry_s){ 1, 1, &value }));
	assert_true(tlv_append(tlv, (tlv_entry_s){ 2, 1, &value }));
	const tlv_entry_s *removed = tlv_remove(tlv, (tlv_entry_s){ 2, 0, NULL });
	assert_non_null(removed);
	assert_int_equal(removed->tag, 2);
	free(removed->value);
	free((void *)removed);
	assert_null(tlv_remove(tlv, (tlv_entry_s){ 3, 0, NULL }));

	const uint8_t expected[] = { 1, 0, 1, 0x5a };
	assert_memory_equal(tlv_export(tlv), expected, sizeof expected);
	tlv_deinit(tlv);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_tlv_add_lookup_and_export),
	cmocka_unit_test(test_tlv_remove_and_iteration),
	cmocka_unit_test(test_tlv_empty_null_and_exact_removal)
};

REGISTER_TESTS(tlv, tests)
