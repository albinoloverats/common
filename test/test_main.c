/*
 * Unit tests.
 * Copyright © 2026, albinoloverats ~ Software Development
 * email: webmaster@albinoloverats.net
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "test_support.h"

#define MAX_TESTS 48

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
