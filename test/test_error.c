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
 * \brief Verifies error initialization and die() exit-status behavior.
 */
static void test_error_exit_paths(void **state)
{
	(void)state;
#ifndef _WIN32
	pid_t child = fork();
	assert_true(child >= 0);
	if (!child)
	{
		error_init();
		error_init();
		void (*handler)(int) = signal(SIGILL, SIG_IGN);
		_exit(handler != SIG_DFL ? EXIT_SUCCESS : EXIT_FAILURE);
	}
	int status = 0;
	assert_int_equal(waitpid(child, &status, 0), child);
	assert_true(WIFEXITED(status));
	assert_int_equal(WEXITSTATUS(status), EXIT_SUCCESS);

	child = fork();
	assert_true(child >= 0);
	if (!child)
	{
		FILE *null = freopen("/dev/null", "w", stderr);
		(void)null;
		errno = EINVAL;
		die("expected error %d", 17);
	}
	assert_int_equal(waitpid(child, &status, 0), child);
	assert_true(WIFEXITED(status));
	assert_int_equal(WEXITSTATUS(status), EINVAL);

	child = fork();
	assert_true(child >= 0);
	if (!child)
	{
		FILE *null = freopen("/dev/null", "w", stderr);
		(void)null;
		errno = 0;
		die("expected success exit");
	}
	assert_int_equal(waitpid(child, &status, 0), child);
	assert_true(WIFEXITED(status));
	assert_int_equal(WEXITSTATUS(status), EXIT_SUCCESS);
#endif
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_error_exit_paths)
};

REGISTER_TESTS(error, tests)
