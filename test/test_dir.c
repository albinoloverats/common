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
 * \brief Verifies basename, password, parent, depth, and component helpers.
 */
static void test_directory_path_helpers(void **state)
{
	(void)state;
	char *value = dir_get_name("/tmp/archive/file.txt:password", '.');
	assert_string_equal(value, "file");
	free(value);
	value = dir_get_name("/tmp/archive/file:password", ':');
	assert_string_equal(value, "file");
	free(value);
	value = dir_get_name("file.txt", '.');
	assert_string_equal(value, "file");
	free(value);
	value = dir_get_name("file", '.');
	assert_string_equal(value, "file");
	free(value);
	value = dir_get_pass("/tmp/archive/file:password");
	assert_string_equal(value, "password");
	free(value);
	value = dir_get_pass("/tmp/archive/file");
	assert_string_equal(value, "");
	free(value);
	value = dir_get_path("/tmp/archive/file");
	assert_string_equal(value, "/tmp/archive");
	free(value);
	value = dir_get_path("/file");
	assert_string_equal(value, "/");
	free(value);
	value = dir_get_path("file");
	assert_string_equal(value, "");
	free(value);
	assert_int_equal(dir_get_deep("/a/b/c"), 3);
	value = dir_get_part("/a/b/c", 0);
	assert_string_equal(value, "/");
	free(value);
	value = dir_get_part("/a/b/c", 2);
	assert_string_equal(value, "b");
	free(value);
	value = dir_get_part("/a/b/c", 4);
	assert_string_equal(value, "");
	free(value);
}


/*!
 * \brief Verifies recursive directory creation and entry-type tree filtering.
 */
static void test_directory_tree_and_recursive_mkdir(void **state)
{
	(void)state;
	char root[] = "/tmp/common-unit-XXXXXX";
	assert_non_null(mkdtemp(root));
	char nested[256], empty[256], file[256], link_path[256], fifo[256], socket_path[256];
	snprintf(nested, sizeof nested, "%s/a/b", root);
	dir_mk_recursive(nested, 0700);
	snprintf(empty, sizeof empty, "%s/empty", root);
	assert_int_equal(mkdir(empty, 0700), 0);
	list_t empty_tree = dir_get_tree(empty, DIR_ALL);
	assert_int_equal(list_size(empty_tree), 0);
	list_deinit(empty_tree, free);
	char nested_slash[sizeof nested + 1];
	snprintf(nested_slash, sizeof nested_slash, "%s/", nested);
	dir_mk_recursive(nested_slash, 0700);
	dir_mk_recursive("", 0700);
	snprintf(file, sizeof file, "%s/a/b/item", root);
	FILE *stream = fopen(file, "w");
	assert_non_null(stream);
	assert_true(fputs("data", stream) >= 0);
	assert_int_equal(fclose(stream), 0);
#if !defined _WIN32
	snprintf(link_path, sizeof link_path, "%s/link", root);
	assert_int_equal(symlink(file, link_path), 0);
	snprintf(fifo, sizeof fifo, "%s/pipe", root);
	assert_int_equal(mkfifo(fifo, 0600), 0);
	snprintf(socket_path, sizeof socket_path, "%s/socket", root);
	int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	assert_true(socket_fd >= 0);
	struct sockaddr_un address = { .sun_family = AF_UNIX };
	assert_true(strlen(socket_path) < sizeof address.sun_path);
	memcpy(address.sun_path, socket_path, strlen(socket_path) + 1);
	assert_int_equal(bind(socket_fd, (struct sockaddr *)&address, sizeof address), 0);
	assert_int_equal(close(socket_fd), 0);
#endif
	/* Traversal always descends into directories, but returns only selected types. */
	list_t files = dir_get_tree(root, DIR_FILE);
	list_t folders = dir_get_tree(root, DIR_FOLDER);
	list_t links = dir_get_tree(root, DIR_LINK);
	list_t pipes = dir_get_tree(root, DIR_PIPE);
	list_t sockets = dir_get_tree(root, DIR_SOCKET);
	list_t everything = dir_get_tree(root, DIR_ALL);
	list_t none = dir_get_tree(root, DIR_NONE);
	assert_int_equal(list_size(files), 1);
	assert_int_equal(list_size(folders), 3);
	assert_int_equal(list_size(links), 1);
	assert_int_equal(list_size(pipes), 1);
	assert_int_equal(list_size(sockets), 1);
	assert_int_equal(list_size(everything), 7);
	assert_int_equal(list_size(none), 0);
	assert_string_equal(list_get(files, 0), file);
	assert_string_equal(list_get(links, 0), link_path);
#if defined __sun
	list_t character_devices = dir_get_tree("/devices", DIR_CHAR);
	assert_non_null(list_contains(character_devices, "/devices/pseudo/mm@0:null"));
#else
	list_t character_devices = dir_get_tree("/dev", DIR_CHAR);
	assert_non_null(list_contains(character_devices, "/dev/null"));
#endif
	list_deinit(character_devices, free);
	list_deinit(files, free);
	list_deinit(folders, free);
	list_deinit(links, free);
	list_deinit(pipes, free);
	list_deinit(sockets, free);
	list_deinit(everything, free);
	list_deinit(none, free);

	assert_int_equal(unlink(fifo), 0);
	assert_int_equal(unlink(socket_path), 0);
	assert_int_equal(unlink(link_path), 0);
	assert_int_equal(unlink(file), 0);
	char parent[256];
	snprintf(parent, sizeof parent, "%s/a/b", root);
	assert_int_equal(rmdir(parent), 0);
	snprintf(parent, sizeof parent, "%s/a", root);
	assert_int_equal(rmdir(parent), 0);
	assert_int_equal(rmdir(empty), 0);
	assert_int_equal(rmdir(root), 0);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_directory_path_helpers),
	cmocka_unit_test(test_directory_tree_and_recursive_mkdir)
};

REGISTER_TESTS(dir, tests)
