#include "test_support.h"

#define MAX_TESTS 32

void register_mem_tests(struct CMUnitTest *, size_t *);
void register_error_tests(struct CMUnitTest *, size_t *);
void register_list_tests(struct CMUnitTest *, size_t *);
void register_map_tests(struct CMUnitTest *, size_t *);
void register_tlv_tests(struct CMUnitTest *, size_t *);
void register_ecc_tests(struct CMUnitTest *, size_t *);
void register_dir_tests(struct CMUnitTest *, size_t *);
void register_cli_tests(struct CMUnitTest *, size_t *);
void register_ccrypt_tests(struct CMUnitTest *, size_t *);
void register_config_tests(struct CMUnitTest *, size_t *);
void register_version_tests(struct CMUnitTest *, size_t *);

int main(void)
{
	struct CMUnitTest tests[MAX_TESTS];
	size_t count = 0;
	register_mem_tests(tests, &count);
	register_error_tests(tests, &count);
	register_list_tests(tests, &count);
	register_map_tests(tests, &count);
	register_tlv_tests(tests, &count);
	register_ecc_tests(tests, &count);
	register_dir_tests(tests, &count);
	register_cli_tests(tests, &count);
	register_ccrypt_tests(tests, &count);
	register_config_tests(tests, &count);
	register_version_tests(tests, &count);
	assert_true(count <= MAX_TESTS);
	return _cmocka_run_group_tests("common", tests, count, NULL, NULL);
}
