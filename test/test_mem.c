#include "test_support.h"

static void test_memory_helpers(void **state)
{
	(void)state;
	char *copy = m_strdup("common");
	assert_string_equal(copy, "common");
	free(copy);

	copy = m_strndup("common", 3);
	assert_string_equal(copy, "com");
	free(copy);

	char *formatted = m_strdupf("%s-%d", "item", 7);
	assert_string_equal(formatted, "item-7");
	free(formatted);

	char *allocated = NULL;
	assert_int_equal(m_asprintf(&allocated, "%s:%u", "value", 9), 7);
	assert_string_equal(allocated, "value:9");
	free(allocated);

	char *resized = m_malloc(4);
	memcpy(resized, "abc", 4);
	resized = m_realloc(resized, 8);
	assert_string_equal(resized, "abc");
	free(resized);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_memory_helpers)
};

REGISTER_TESTS(mem, tests)
