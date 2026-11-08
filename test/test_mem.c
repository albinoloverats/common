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

#ifndef __APPLE__
typedef enum
{
	FAIL_NONE,
	FAIL_MALLOC,
	FAIL_CALLOC,
	FAIL_REALLOC,
	FAIL_STRDUP,
	FAIL_STRNDUP,
	FAIL_VASPRINTF_AOD,
	FAIL_VASPRINTF_FOD
}
allocator_failure_e;

static allocator_failure_e fail_next_allocator;

extern void *__real_malloc(size_t);
extern void *__real_calloc(size_t, size_t);
extern void *__real_realloc(void *, size_t);
extern char *__real_strdup(const char *);
extern char *__real_strndup(const char *, size_t);
extern int __real_vasprintf(char **, const char *, va_list);

void *__wrap_malloc(size_t size)
{
	if (fail_next_allocator == FAIL_MALLOC)
	{
		fail_next_allocator = FAIL_NONE;
		errno = ENOMEM;
		return NULL;
	}
	return __real_malloc(size);
}

void *__wrap_calloc(size_t count, size_t size)
{
	if (fail_next_allocator == FAIL_CALLOC)
	{
		fail_next_allocator = FAIL_NONE;
		errno = ENOMEM;
		return NULL;
	}
	return __real_calloc(count, size);
}

void *__wrap_realloc(void *ptr, size_t size)
{
	if (fail_next_allocator == FAIL_REALLOC)
	{
		fail_next_allocator = FAIL_NONE;
		errno = ENOMEM;
		return NULL;
	}
	return __real_realloc(ptr, size);
}

char *__wrap_strdup(const char *str)
{
	if (fail_next_allocator == FAIL_STRDUP)
	{
		fail_next_allocator = FAIL_NONE;
		errno = ENOMEM;
		return NULL;
	}
	return __real_strdup(str);
}

char *__wrap_strndup(const char *str, size_t size)
{
	if (fail_next_allocator == FAIL_STRNDUP)
	{
		fail_next_allocator = FAIL_NONE;
		errno = ENOMEM;
		return NULL;
	}
	return __real_strndup(str, size);
}

int __wrap_vasprintf(char **strp, const char *format, va_list args)
{
	if (fail_next_allocator == FAIL_VASPRINTF_AOD || fail_next_allocator == FAIL_VASPRINTF_FOD)
	{
		fail_next_allocator = FAIL_NONE;
		errno = ENOMEM;
		return -1;
	}
	return __real_vasprintf(strp, format, args);
}
#endif

/*!
 * \brief Verifies allocation, duplication, formatting, and resize helpers.
 */
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

	resized = mem_rod(__FILE__, __LINE__, __func__, NULL, 4);
	memcpy(resized, "xyz", 4);
	resized = mem_rod(__FILE__, __LINE__, __func__, resized, 8);
	assert_string_equal(resized, "xyz");
	free(resized);
}

#ifndef __APPLE__
/*!
 * \brief Verifies allocation wrappers exit with ENOMEM when allocation fails.
 */
static void test_memory_wrappers_exit_on_allocation_failure(void **state)
{
	(void)state;
	/* Each failure runs in a child because the wrappers terminate through die(). */
	for (allocator_failure_e failure = FAIL_MALLOC; failure <= FAIL_VASPRINTF_FOD; failure++)
	{
		pid_t child = fork();
		assert_true(child >= 0);
		if (!child)
		{
			FILE *null = freopen("/dev/null", "w", stderr);
			(void)null;
			fail_next_allocator = failure;
			switch (failure)
			{
				case FAIL_MALLOC:
					mem_mod(__FILE__, __LINE__, __func__, 8);
					break;
				case FAIL_CALLOC:
					mem_cod(__FILE__, __LINE__, __func__, 2, 4);
					break;
				case FAIL_REALLOC:
				{
					void *ptr = malloc(4);
					if (!ptr)
						_exit(EXIT_FAILURE);
					fail_next_allocator = failure;
					mem_rod(__FILE__, __LINE__, __func__, ptr, 8);
					break;
				}
				case FAIL_STRDUP:
					mem_sod(__FILE__, __LINE__, __func__, "value");
					break;
				case FAIL_STRNDUP:
					mem_nod(__FILE__, __LINE__, __func__, "value", 3);
					break;
				case FAIL_VASPRINTF_AOD:
				{
					char *ptr = NULL;
					mem_aod(__FILE__, __LINE__, __func__, &ptr, "%s", "value");
					break;
				}
				case FAIL_VASPRINTF_FOD:
				{
					mem_fod(__FILE__, __LINE__, __func__, "%s", "value");
					break;
				}
				default:
					_exit(EXIT_FAILURE);
			}
			_exit(EXIT_FAILURE);
		}
		int status = 0;
		assert_int_equal(waitpid(child, &status, 0), child);
		assert_true(WIFEXITED(status));
		assert_int_equal(WEXITSTATUS(status), ENOMEM);
	}
}
#endif

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_memory_helpers),
#ifndef __APPLE__
	cmocka_unit_test(test_memory_wrappers_exit_on_allocation_failure)
#endif
};

REGISTER_TESTS(mem, tests)
