#include "test_support.h"

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
}


static void test_directory_tree_and_recursive_mkdir(void **state)
{
	(void)state;
	char root[] = "/tmp/common-unit-XXXXXX";
	assert_non_null(mkdtemp(root));
	char nested[256], file[256], link_path[256], fifo[256], socket_path[256];
	snprintf(nested, sizeof nested, "%s/a/b", root);
	dir_mk_recursive(nested, 0700);
	char nested_slash[sizeof nested + 1];
	snprintf(nested_slash, sizeof nested_slash, "%s/", nested);
	dir_mk_recursive(nested_slash, 0700);
	snprintf(file, sizeof file, "%s/a/b/item", root);
	FILE *stream = fopen(file, "w");
	assert_non_null(stream);
	assert_true(fputs("data", stream) >= 0);
	assert_int_equal(fclose(stream), 0);
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

	list_t files = dir_get_tree(root, DIR_FILE);
	list_t folders = dir_get_tree(root, DIR_FOLDER);
	list_t links = dir_get_tree(root, DIR_LINK);
	list_t pipes = dir_get_tree(root, DIR_PIPE);
	list_t sockets = dir_get_tree(root, DIR_SOCKET);
	list_t everything = dir_get_tree(root, DIR_ALL);
	list_t none = dir_get_tree(root, DIR_NONE);
	assert_int_equal(list_size(files), 1);
	assert_int_equal(list_size(folders), 2);
	assert_int_equal(list_size(links), 1);
	assert_int_equal(list_size(pipes), 1);
	assert_int_equal(list_size(sockets), 1);
	assert_int_equal(list_size(everything), 6);
	assert_int_equal(list_size(none), 0);
	assert_string_equal(list_get(files, 0), file);
	assert_string_equal(list_get(links, 0), link_path);
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
	assert_int_equal(rmdir(root), 0);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_directory_path_helpers),
	cmocka_unit_test(test_directory_tree_and_recursive_mkdir)
};

REGISTER_TESTS(dir, tests)
