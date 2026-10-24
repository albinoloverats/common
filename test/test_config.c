#include "test_support.h"

static void test_config_requires_initialization(void **state)
{
	(void)state;
	pid_t child = fork();
	assert_true(child >= 0);
	if (!child)
	{
		FILE *null = freopen("/dev/null", "w", stderr);
		(void)null;
		char *argv[] = { "common-test" };
		_exit(config_parse_aux(1, argv, NULL, NULL, NULL, false) == -1
			? EXIT_SUCCESS : EXIT_FAILURE);
	}
	int status = 0;
	assert_int_equal(waitpid(child, &status, 0), child);
	assert_true(WIFEXITED(status));
	assert_int_equal(WEXITSTATUS(status), EXIT_SUCCESS);
}


static void test_config_metadata_helpers(void **state)
{
	(void)state;
	config_named_s first = { .short_option = 'a' };
	config_named_s second = { .short_option = 'b' };
	assert_true(config_named_compare(&first, &second) < 0);
	config_unnamed_s left = { .description = "alpha" };
	config_unnamed_s right = { .description = "beta" };
	assert_true(config_unnamed_compare(&left, &right) < 0);

	config_named_s *named = calloc(1, sizeof *named);
	assert_non_null(named);
	named->response.type = CONFIG_ARG_LIST_STRING;
	named->response.value.list = list_default();
	assert_true(list_append(named->response.value.list, m_strdup("owned")));
	config_named_free(named);

	config_unnamed_s *unnamed = calloc(1, sizeof *unnamed);
	assert_non_null(unnamed);
	unnamed->response.type = CONFIG_ARG_STRING;
	unnamed->response.value.string = m_strdup("owned");
	config_unnamed_free(unnamed);
}


static void test_config_command_line_values(void **state)
{
	(void)state;
	config_init((config_about_s){ "common-test", "1", NULL, NULL });
	list_t args = list_init(config_named_compare, false, false);
	config_named_s *enabled = add_config_option(args, 'e', "enabled", CONFIG_ARG_OPT_BOOLEAN,
		(config_arg_u){ .boolean = false });
	config_named_s *count = add_config_option(args, 'c', "count", CONFIG_ARG_REQ_INTEGER,
		(config_arg_u){ .integer = 0 });
	config_named_s *ratio = add_config_option(args, 'r', "ratio", CONFIG_ARG_REQ_DECIMAL,
		(config_arg_u){ .decimal = 0 });
	config_named_s *name = add_config_option(args, 'n', "name", CONFIG_ARG_REQ_STRING,
		(config_arg_u){ .string = NULL });
	config_named_s *flags = add_config_option(args, 'b', "flags", CONFIG_ARG_LIST_BOOLEAN,
		(config_arg_u){ .list = NULL });
	config_named_s *numbers = add_config_option(args, 'i', "numbers", CONFIG_ARG_LIST_INTEGER,
		(config_arg_u){ .list = NULL });
	config_named_s *ratios = add_config_option(args, 'd', "ratios", CONFIG_ARG_LIST_DECIMAL,
		(config_arg_u){ .list = NULL });
	config_named_s *labels = add_config_option(args, 's', "labels", CONFIG_ARG_LIST_STRING,
		(config_arg_u){ .list = NULL });
	config_named_s *optional_integer = add_config_option(args, 'o', "optional-integer", CONFIG_ARG_OPT_INTEGER,
		(config_arg_u){ .integer = 0 });
	config_named_s *optional_decimal = add_config_option(args, 'p', "optional-decimal", CONFIG_ARG_OPT_DECIMAL,
		(config_arg_u){ .decimal = 0 });
	config_named_s *optional_string = add_config_option(args, 'x', "optional-string", CONFIG_ARG_OPT_STRING,
		(config_arg_u){ .string = NULL });
	config_named_s *optional_no_value = add_config_option(args, 'y', "optional-no-value", CONFIG_ARG_OPT_INTEGER,
		(config_arg_u){ .integer = 11 });

	config_unnamed_s positional = { "file", { CONFIG_ARG_STRING, { .string = NULL } }, false, false };
	config_unnamed_s extra_boolean = { "boolean", { CONFIG_ARG_BOOLEAN, { .boolean = false } }, false, false };
	config_unnamed_s extra_integer = { "integer", { CONFIG_ARG_INTEGER, { .integer = 0 } }, false, false };
	config_unnamed_s extra_decimal = { "decimal", { CONFIG_ARG_DECIMAL, { .decimal = 0 } }, false, false };
	list_t extra = list_default();
	assert_true(list_append(extra, &extra_boolean));
	assert_true(list_append(extra, &extra_integer));
	assert_true(list_append(extra, &extra_decimal));
	assert_true(list_append(extra, &positional));
	char *argv[] =
	{
		"common-test", "--enabled", "--count=2K", "-r1.5", "--name=alice",
		"--flags=true,false", "--numbers=1K,2", "--ratios=1.5,2", "--labels=a,b",
		"--optional-integer=6", "--optional-decimal=2.5", "--optional-string=text",
		"--optional-no-value",
		"--enabled=true", "--flags=yes", "--numbers=3", "--ratios=3.5", "--labels=c",
		"yes", "42K", "2.5", "input.txt"
	};
	assert_int_equal(config_parse(22, argv, args, extra, NULL, false), 16);
	assert_true(enabled->seen);
	assert_true(enabled->response.value.boolean);
	assert_int_equal(count->response.value.integer, 2 * 1024);
	assert_true(fabsl(ratio->response.value.decimal - 1.5L) < 0.0001L);
	assert_string_equal(name->response.value.string, "alice");
	assert_int_equal(list_size(flags->response.value.list), 3);
	assert_true(*(const bool *)list_get(flags->response.value.list, 0));
	assert_false(*(const bool *)list_get(flags->response.value.list, 1));
	assert_true(*(const bool *)list_get(flags->response.value.list, 2));
	assert_int_equal(*(const int64_t *)list_get(numbers->response.value.list, 0), 1024);
	assert_int_equal(*(const int64_t *)list_get(numbers->response.value.list, 1), 2);
	assert_int_equal(*(const int64_t *)list_get(numbers->response.value.list, 2), 3);
	assert_true(fabsl(*(const long double *)list_get(ratios->response.value.list, 0) - 1.5L) < 0.0001L);
	assert_true(fabsl(*(const long double *)list_get(ratios->response.value.list, 2) - 3.5L) < 0.0001L);
	assert_string_equal(list_get(labels->response.value.list, 0), "a");
	assert_string_equal(list_get(labels->response.value.list, 1), "b");
	assert_string_equal(list_get(labels->response.value.list, 2), "c");
	assert_int_equal(optional_integer->response.value.integer, 6);
	assert_true(fabsl(optional_decimal->response.value.decimal - 2.5L) < 0.0001L);
	assert_string_equal(optional_string->response.value.string, "text");
	assert_true(optional_no_value->seen);
	assert_int_equal(optional_no_value->response.value.integer, 11);
	assert_true(extra_boolean.seen && extra_boolean.response.value.boolean);
	assert_true(extra_integer.seen && extra_integer.response.value.integer == 42 * 1024);
	assert_true(extra_decimal.seen && fabsl(extra_decimal.response.value.decimal - 2.5L) < 0.0001L);
	assert_true(positional.seen);
	assert_string_equal(positional.response.value.string, "input.txt");

	free(positional.response.value.string);
	list_deinit(args, config_named_free);
	list_deinit(extra);
}


static void test_config_display_actions_exit_cleanly(void **state)
{
	(void)state;
	const char *actions[] = { "--help", "--version", "--licence" };
	for (size_t i = 0; i < sizeof actions / sizeof actions[0]; i++)
	{
		pid_t child = fork();
		assert_true(child >= 0);
		if (!child)
		{
			FILE *null = freopen("/dev/null", "w", stderr);
			(void)null;
			config_init((config_about_s){ "common-test", "1", "https://example.invalid", NULL });
			list_t args = list_init(config_named_compare, false, false);
			config_arg_u default_value = { .string = "default" };
			config_named_s *normal = add_config_option(args, 'n', "normal",
				CONFIG_ARG_OPT_STRING, default_value);
			normal->option_type = "value";
			normal->advanced = true;
			config_named_s *boolean = add_config_option(args, 'b', "boolean", CONFIG_ARG_OPT_BOOLEAN,
				(config_arg_u){ .boolean = true });
			boolean->description = "boolean option with a default";
			config_named_s *integer = add_config_option(args, 'i', "integer", CONFIG_ARG_REQ_INTEGER,
				(config_arg_u){ .integer = 42 });
			integer->description = "integer option with a default";
			config_named_s *decimal = add_config_option(args, 'd', "decimal", CONFIG_ARG_OPT_DECIMAL,
				(config_arg_u){ .decimal = 1.5L });
			decimal->description = "decimal option with a default";
			config_named_s *required = add_config_option(args, 'r', "required", CONFIG_ARG_REQ_STRING,
				(config_arg_u){ .string = NULL });
			required->description = "a required option with a descriptive explanation long enough to wrap across multiple lines";
			config_named_s *hidden = add_config_option(args, 'h', "hidden", CONFIG_ARG_OPT_BOOLEAN,
				(config_arg_u){ .boolean = false });
			hidden->hidden = true;
			config_unnamed_s extra_arg = { "path", { CONFIG_ARG_STRING, { .string = NULL } }, true, false };
			list_t extra = list_default();
			assert_true(list_append(extra, &extra_arg));
			list_t notes = list_string();
			assert_true(list_append(notes, "a note that is deliberately long enough to wrap over more than one line in the notes section"));
			char *argv[] = { "common-test", (char *)actions[i] };
			config_parse(2, argv, args, extra, notes, false);
			_exit(EXIT_FAILURE);
		}
		int status = 0;
		assert_int_equal(waitpid(child, &status, 0), child);
		assert_true(WIFEXITED(status));
		assert_int_equal(WEXITSTATUS(status), EXIT_SUCCESS);
	}

	pid_t child = fork();
	assert_true(child >= 0);
	if (!child)
	{
		FILE *null = freopen("/dev/null", "w", stderr);
		(void)null;
		config_init((config_about_s){ "common-test", "1", NULL, NULL });
		list_t args = list_init(config_named_compare, false, false);
		config_named_s *required = add_config_option(args, 'r', "required", CONFIG_ARG_REQ_STRING,
			(config_arg_u){ .string = NULL });
		required->required = true;
		char *argv[] = { "common-test" };
		config_parse(1, argv, args, NULL, NULL, true);
		_exit(EXIT_FAILURE);
	}
	int status = 0;
	assert_int_equal(waitpid(child, &status, 0), child);
	assert_true(WIFEXITED(status));
	assert_int_equal(WEXITSTATUS(status), EXIT_SUCCESS);
}


static void test_config_file_values_and_update(void **state)
{
	(void)state;
	char root[] = "/tmp/common-config-XXXXXX";
	assert_non_null(mkdtemp(root));
	char cwd[PATH_MAX];
	assert_non_null(getcwd(cwd, sizeof cwd));
	assert_int_equal(chdir(root), 0);
	FILE *file = fopen("test.conf", "w");
	assert_non_null(file);
	assert_true(fputs("# ignored\nenabled yes\ncount 4K\nname configured value\n"
		"bounds 5 9\nnumbers 1,2,3\n", file) >= 0);
	assert_int_equal(fclose(file), 0);

	config_init((config_about_s){ "common-test", "1", NULL, "test.conf" });
	list_t args = list_init(config_named_compare, false, false);
	config_named_s *enabled = add_config_option(args, 'e', "enabled", CONFIG_ARG_OPT_BOOLEAN,
		(config_arg_u){ .boolean = false });
	config_named_s *count = add_config_option(args, 'c', "count", CONFIG_ARG_REQ_INTEGER,
		(config_arg_u){ .integer = 0 });
	config_named_s *name = add_config_option(args, 'n', "name", CONFIG_ARG_REQ_STRING,
		(config_arg_u){ .string = NULL });
	config_named_s *bounds = add_config_option(args, 'b', "bounds", CONFIG_ARG_PAIR_INTEGER,
		(config_arg_u){ 0 });
	config_named_s *numbers = add_config_option(args, 'i', "numbers", CONFIG_ARG_LIST_INTEGER,
		(config_arg_u){ .list = NULL });
	char *argv[] = { "common-test" };
	assert_int_equal(config_parse(1, argv, args, NULL, NULL, false), 5);
	assert_true(enabled->response.value.boolean);
	assert_int_equal(count->response.value.integer, 4 * 1024);
	assert_string_equal(name->response.value.string, "configured value");
	assert_int_equal(bounds->response.value.pair.integer.i1, 5);
	assert_int_equal(bounds->response.value.pair.integer.i2, 9);
	assert_int_equal(list_size(numbers->response.value.list), 3);
	assert_int_equal(*(const int64_t *)list_get(numbers->response.value.list, 2), 3);

	char *old_home = getenv("HOME") ? m_strdup(getenv("HOME")) : NULL;
	assert_int_equal(setenv("HOME", root, 1), 0);
	update_config("count", "8");
	update_config("added", "new value");
	if (old_home)
	{
		assert_int_equal(setenv("HOME", old_home, 1), 0);
		free(old_home);
	}
	else
		assert_int_equal(unsetenv("HOME"), 0);

	file = fopen("test.conf", "r");
	assert_non_null(file);
	char contents[256] = { 0 };
	assert_non_null(fread(contents, 1, sizeof contents - 1, file));
	fclose(file);
	assert_non_null(strstr(contents, "count 8\n"));
	assert_non_null(strstr(contents, "enabled yes\n"));
	assert_non_null(strstr(contents, "added new value\n"));

	list_deinit(args, config_named_free);
	assert_int_equal(unlink("test.conf"), 0);
	assert_int_equal(chdir(cwd), 0);
	assert_int_equal(rmdir(root), 0);
}


static void test_config_file_pairs_and_list_types(void **state)
{
	(void)state;
	char root[] = "/tmp/common-config-types-XXXXXX";
	assert_non_null(mkdtemp(root));
	char cwd[PATH_MAX];
	assert_non_null(getcwd(cwd, sizeof cwd));
	assert_int_equal(chdir(root), 0);
	FILE *file = fopen("types.conf", "w");
	assert_non_null(file);
	assert_true(fputs("boolean no\ndecimal 1.25\nbool-list true,false\n"
		"decimal-list 1.5,2.5\nstring-list alpha,beta\n"
		"bool-single false\ninteger-single 7\ndecimal-single 3.5\nstring-single one\n"
		"bool-pair true false\ndecimal-pair 1.25 2.5\n"
		"string-pair left right\npair-list key value\n", file) >= 0);
	assert_int_equal(fclose(file), 0);

	config_init((config_about_s){ "common-test", "1", NULL, "types.conf" });
	list_t args = list_init(config_named_compare, false, false);
	config_named_s *boolean = add_config_option(args, 'b', "boolean", CONFIG_ARG_REQ_BOOLEAN,
		(config_arg_u){ .boolean = true });
	config_named_s *decimal = add_config_option(args, 'd', "decimal", CONFIG_ARG_REQ_DECIMAL,
		(config_arg_u){ .decimal = 0 });
	config_named_s *bool_list = add_config_option(args, 'B', "bool-list", CONFIG_ARG_LIST_BOOLEAN,
		(config_arg_u){ .list = NULL });
	config_named_s *decimal_list = add_config_option(args, 'D', "decimal-list", CONFIG_ARG_LIST_DECIMAL,
		(config_arg_u){ .list = NULL });
	config_named_s *string_list = add_config_option(args, 'S', "string-list", CONFIG_ARG_LIST_STRING,
		(config_arg_u){ .list = NULL });
	config_named_s *bool_single = add_config_option(args, 'u', "bool-single", CONFIG_ARG_LIST_BOOLEAN,
		(config_arg_u){ .list = NULL });
	config_named_s *integer_single = add_config_option(args, 'v', "integer-single", CONFIG_ARG_LIST_INTEGER,
		(config_arg_u){ .list = NULL });
	config_named_s *decimal_single = add_config_option(args, 'w', "decimal-single", CONFIG_ARG_LIST_DECIMAL,
		(config_arg_u){ .list = NULL });
	config_named_s *string_single = add_config_option(args, 'x', "string-single", CONFIG_ARG_LIST_STRING,
		(config_arg_u){ .list = NULL });
	config_named_s *bool_pair = add_config_option(args, 'p', "bool-pair", CONFIG_ARG_PAIR_BOOLEAN,
		(config_arg_u){ 0 });
	config_named_s *decimal_pair = add_config_option(args, 'q', "decimal-pair", CONFIG_ARG_PAIR_DECIMAL,
		(config_arg_u){ 0 });
	config_named_s *string_pair = add_config_option(args, 'r', "string-pair", CONFIG_ARG_PAIR_STRING,
		(config_arg_u){ 0 });
	config_named_s *pair_list = add_config_option(args, 'P', "pair-list", CONFIG_ARG_LIST_PAIR_STRING,
		(config_arg_u){ .list = NULL });
	char *argv[] = { "common-test" };
	assert_int_equal(config_parse(1, argv, args, NULL, NULL, false), 13);
	assert_false(boolean->response.value.boolean);
	assert_true(fabsl(decimal->response.value.decimal - 1.25L) < 0.0001L);
	assert_int_equal(list_size(bool_list->response.value.list), 2);
	assert_true(*(const bool *)list_get(bool_list->response.value.list, 0));
	assert_false(*(const bool *)list_get(bool_list->response.value.list, 1));
	assert_true(fabsl(*(const long double *)list_get(decimal_list->response.value.list, 1) - 2.5L) < 0.0001L);
	assert_string_equal(list_get(string_list->response.value.list, 0), "alpha");
	assert_false(*(const bool *)list_get(bool_single->response.value.list, 0));
	assert_int_equal(*(const int64_t *)list_get(integer_single->response.value.list, 0), 7);
	assert_true(fabsl(*(const long double *)list_get(decimal_single->response.value.list, 0) - 3.5L) < 0.0001L);
	assert_string_equal(list_get(string_single->response.value.list, 0), "one");
	assert_true(bool_pair->response.value.pair.boolean.b1);
	assert_false(bool_pair->response.value.pair.boolean.b2);
	assert_true(fabsl(decimal_pair->response.value.pair.decimal.d2 - 2.5L) < 0.0001L);
	assert_string_equal(string_pair->response.value.pair.string.s1, "left");
	assert_string_equal(string_pair->response.value.pair.string.s2, "right");
	assert_int_equal(list_size(pair_list->response.value.list), 1);
	const pair_string_s *entry = list_get(pair_list->response.value.list, 0);
	assert_string_equal(entry->s1, "key");
	assert_string_equal(entry->s2, "value");

	free(string_pair->response.value.pair.string.s1);
	free(string_pair->response.value.pair.string.s2);
	string_pair->response.value.pair.string.s1 = NULL;
	string_pair->response.value.pair.string.s2 = NULL;
	list_deinit(pair_list->response.value.list, free_pair_string);
	pair_list->response.value.list = NULL;
	list_deinit(args, config_named_free);
	assert_int_equal(unlink("types.conf"), 0);
	assert_int_equal(chdir(cwd), 0);
	assert_int_equal(rmdir(root), 0);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_config_requires_initialization),
	cmocka_unit_test(test_config_metadata_helpers),
	cmocka_unit_test(test_config_command_line_values),
	cmocka_unit_test(test_config_display_actions_exit_cleanly),
	cmocka_unit_test(test_config_file_values_and_update),
	cmocka_unit_test(test_config_file_pairs_and_list_types)
};

REGISTER_TESTS(config, tests)
