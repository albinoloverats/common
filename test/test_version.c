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

/*!
 * \brief Verifies build metadata and asynchronous update-version reporting.
 */
static void test_version_build_information(void **state)
{
	(void)state;
	char *info = version_build_info();
	assert_non_null(info);
	assert_non_null(strstr(info, "built on"));
	assert_non_null(strstr(info, "compiler"));
	assert_non_null(strstr(info, "runtime"));
	free(info);

	char path[] = "/tmp/common-version-XXXXXX";
	int fd = mkstemp(path);
	assert_true(fd >= 0);
	assert_int_equal(write(fd, "2.0\n", 4), 4);
	assert_int_equal(close(fd), 0);
	char url[PATH_MAX + 8];
	snprintf(url, sizeof url, "file://%s", path);
	version_new_available = false;
	version_is_checking = false;
	/* The local file URL keeps the asynchronous update check independent of the network. */
	version_check_for_update("1.0", url, NULL);
	version_check_for_update("ignored", url, NULL);
	for (int i = 0; i < 500 && version_is_checking; i++)
		usleep(10000);
	assert_false(version_is_checking);
	assert_true(version_new_available);
	assert_string_equal(version_available, "2.0");
	FILE *output = tmpfile();
	assert_non_null(output);
	int saved = dup(STDERR_FILENO);
	assert_true(saved >= 0);
	fflush(stderr);
	assert_int_equal(dup2(fileno(output), STDERR_FILENO), STDERR_FILENO);
	version_print("common-test", "1.0", "download-url");
	fflush(stderr);
	assert_int_equal(dup2(saved, STDERR_FILENO), STDERR_FILENO);
	close(saved);
	assert_int_equal(fseek(output, 0, SEEK_SET), 0);
	char message[4096] = { 0 };
	assert_true(fread(message, 1, sizeof message - 1, output) > 0);
	assert_non_null(strstr(message, "Version 2.0"));
	assert_non_null(strstr(message, "download-url"));
	fclose(output);

	version_new_available = false;
	version_check_for_update("3.0", url, NULL);
	for (int i = 0; i < 500 && version_is_checking; i++)
		usleep(10000);
	assert_false(version_is_checking);
	assert_false(version_new_available);
	assert_int_equal(unlink(path), 0);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_version_build_information)
};

REGISTER_TESTS(version, tests)
