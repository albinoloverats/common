#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <errno.h>
#include <assert.h>
#include <limits.h>

#include <dirent.h>
#include <sys/stat.h>

#include <time.h>

#include "error.h"
#include "config.h"
#include "cli.h"
#include "list.h"
#include "tlv.h"
#include "dir.h"

static config_about_t about =
{
	"test",
	"T1",
	NULL,
	"./test.conf"
};

static int list_test_comp(const void *a, const void *b)
{
	return *((const int *)a) - *((const int *)b);
}

static void list_tests(int i)
{
	if (i < 1)
	{
		errno = EINVAL;
		cli_eprintf("Cannot run list tests with %d items!\n", i);
		return;
	}
	cli_eprintf("Running list tests with %d items\n", i);

	cli_eprintf("  Simple list\n");
	LIST l = list_default();
	assert(l != NULL);
	for (int j = 0; j < i; j++)
	{
		int *k = malloc(sizeof( int ));
		*k = (int)lrand48();
		list_append(l, k);
	}
	assert(list_size(l) == (size_t)i);
	ITER t = list_iterator(l);
	assert(t != NULL);
	for (int j = 0; list_has_next(t); j++)
	{
		const int *k = list_get_next(t);
		cli_printf("    Item [%2d] = %08x\n", j + 1, *k);
	}
	free(t);
	list_deinit(l, free);

	cli_eprintf("  Sorted list\n");
	l = list_init(list_test_comp, true, true);
	for (int j = 0; j < i; j++)
	{
		int *k = malloc(sizeof( int ));
		*k = (int)lrand48();
		list_append(l, k);
	}
	int prev = INT_MIN;
	for (int j = 0; j < i; j++)
	{
		const int *k = list_get(l, j);
		assert(prev <= *k);
		cli_printf("    Item [%2d] = %08x\n", j + 1, *k);
		prev = *k;
	}
	list_deinit(l, free);

	cli_eprintf("  Unique list (will attempt to insert the same value %d times)\n", i);
	l = list_init(list_test_comp, false, false);
	for (int j = 0; j < i; j++)
	{
		int k = i;
		list_append(l, &k);
	}
	assert(list_size(l) == 1);
	t = list_iterator(l);
	assert(t != NULL);
	for (int j = 0; list_has_next(t); j++)
	{
		const int *k = list_get_next(t);
		cli_printf("    Item [%2d] = %08x\n", j + 1, *k);
	}
	free(t);
	list_deinit(l);

	cli_eprintf("  Remove from list (creating initial list of %d)\n", i * 2);
	l = list_default();
	assert(l != NULL);
	for (int j = 0; j < i * 2; j++)
	{
		int *k = malloc(sizeof( int ));
		*k = (int)lrand48();
		list_append(l, k);
	}
	assert(list_size(l) == (size_t)i * 2);
	for (int j = 0; j < i; j++)
	{
		int r = lrand48() % list_size(l);
		free((void *)list_remove_index(l, r));
	}
	assert(list_size(l) == (size_t)i);
	t = list_iterator(l);
	assert(t != NULL);
	for (int j = 0; list_has_next(t); j++)
	{
		const int *k = list_get_next(t);
		cli_printf("    Item [%2d] = %08x\n", j + 1, *k);
	}
	free(t);
	list_deinit(l, free);

	return;
}

void tlv_tests(int i)
{
	if (i < 1)
	{
		errno = EINVAL;
		cli_eprintf("Cannot run TLV tests with %d items!\n", i);
		return;
	}
	cli_eprintf("Running TLV tests with %d items\n", i);

	cli_eprintf("  Simple TLV\n");
	TLV t = tlv_init();
	assert(t != NULL);
	for (int j = 0; j < i; j++)
	{
		tlv_t v;
		v.tag    = j + 1;
		v.length = (lrand48() % 9) + 2;
		v.value  = calloc(v.length, 1);
		for (int k = 0; k < v.length - 1; k++)
			((char *)v.value)[k] = (lrand48() % 26) + 'A';
		tlv_append(t, v);
		free(v.value);
	}
	assert(tlv_count(t) == (size_t)i);
	for (int j = 0; j < tlv_count(t); j++)
	{
		const tlv_t *v = tlv_get(t, j + 1);
		cli_printf("    Tag  [%2d] (%2d) = %s\n", v->tag, v->length, (char *)v->value);
	}
	tlv_deinit(t);

	cli_eprintf("  Remove TLV list (creating initial list of %d)\n", i * 2);
	t = tlv_init();
	assert(t != NULL);
	for (int j = 0; j < i * 2; j++)
	{
		tlv_t v;
		v.tag    = j + 1;
		v.length = (lrand48() % 9) + 2;
		v.value  = calloc(v.length, 1);
		for (int k = 0; k < v.length - 1; k++)
			((char *)v.value)[k] = (lrand48() % 26) + 'A';
		tlv_append(t, v);
		free(v.value);
	}
	assert(tlv_count(t) == (size_t)i * 2);
	for (int j = 0; j < i; j++)
	{
		int r = 0;
		do
		{
			r = lrand48() % (i * 2 + 1);
		}
		while (!tlv_has_tag(t, r));
		const tlv_t *v = tlv_remove_tag(t, r);
		free(v->value);
		free((void *)v);
	}
	assert(tlv_count(t) == (size_t)i);
	ITER r = tlv_iterator(t);
	assert(r != NULL);
	while (tlv_has_next(r))
	{
		const tlv_t *v = tlv_get_next(r);
		assert(v != NULL);
		cli_printf("    Tag  [%2d] (%2d) = %s\n", v->tag, v->length, (char *)v->value);
	}
	free(r);
	tlv_deinit(t);

	return;
}

static void dir_recurse(LIST files, const char *dir, int t)
{
	struct dirent **eps = NULL;
	int n = 0;
	for (int j = 0; j < t; j++)
		cli_eprintf("  ");
	char *name = dir_get_name(dir);
	cli_eprintf("  Scanning %s\n", name);
	free(name);
	if ((n = scandir(dir, &eps, NULL, alphasort)))
	{
		for (int i = 0; i < n; i++)
		{
			if (!strcmp(".", eps[i]->d_name) || !strcmp("..", eps[i]->d_name))
				continue;
			for (int j = 0; j < t; j++)
				cli_eprintf("  ");
			cli_eprintf("    Found %s\n", eps[i]->d_name);
			uint64_t l = strlen(eps[i]->d_name);
			char *full_path = NULL;
			if (!asprintf(&full_path, "%s/%s", dir, eps[i]->d_name))
				die(_("Out of memory @ %s:%d:%s [%" PRIu64 "]"), __FILE__, __LINE__, __func__, strlen(dir) + l + 2);

			struct stat s;
			stat(full_path, &s);
			switch (s.st_mode & S_IFMT)
			{
				case S_IFDIR:
					dir_recurse(files, full_path, t + 1);
					break;
				case S_IFREG:
					list_add(files, strdup(full_path));
					break;
			}
			free(full_path);
		}
	}
	for (int i = 0; i < n; i++)
		free(eps[i]);
	free(eps);
	return;
}

static void fs_tests(char *root)
{
	LIST files = list_string();

	if (!root)
		root = getcwd(NULL, 0);
	cli_eprintf("Running FS tests on %s\n", root);

	dir_recurse(files, root, 0);

	ITER i = list_iterator(files);
	while (list_has_next(i))
		cli_eprintf("  %s\n", (const char *)list_get_next(i));
	free(i);

	free(root);

	list_deinit(files, free);
	return;
}

static int item_count = 10;

int main(int argc, char **argv)
{
	srand48(time(NULL));

	config_init(about);

	LIST args = list_init(config_arg_comp, false, false);
	list_add(args, &((config_named_t){ 's', "list", "number", "Run ‘LIST’ tests, with the given number of items (default 10)",    CONFIG_ARG_OPT_NUMBER, { .number = item_count }, false, false, false, false }));
	list_add(args, &((config_named_t){ 't', "tlv",  "number", "Run ‘TLV’ tests, with the given number of items (default 10)",     CONFIG_ARG_OPT_NUMBER, { .number = item_count }, false, false, false, false }));
	list_add(args, &((config_named_t){ 'f', "fs",   "path",   "Run ‘FS’ tests, on the given path (default is current directory)", CONFIG_ARG_OPT_STRING, { .string = NULL       }, false, false, false, false }));

	LIST notes = list_default();
	list_add(notes, "Not specifying any tests is the same as specifying all tests.");

	bool all = !config_parse(argc, argv, args, NULL, notes);

	errno = EXIT_SUCCESS;

	if (all || ((config_named_t *)list_get(args, 0))->seen)
		list_tests(((config_named_t *)list_get(args, 0))->response_value.number);
	if (all || ((config_named_t *)list_get(args, 1))->seen)
		tlv_tests(((config_named_t *)list_get(args, 1))->response_value.number);
	if (all || ((config_named_t *)list_get(args, 2))->seen)
		fs_tests(((config_named_t *)list_get(args, 2))->response_value.string);

	list_deinit(args);
	list_deinit(notes);

	return errno;
}
