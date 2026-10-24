#include "test_support.h"

static void test_error_exit_paths(void **state)
{
	(void)state;
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
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_error_exit_paths)
};

REGISTER_TESTS(error, tests)
