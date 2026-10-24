#include "test_support.h"

int compare_integer(const void *left, const void *right)
{
	int a = *(const int *)left;
	int b = *(const int *)right;
	return (a > b) - (a < b);
}

int compare_string(const void *left, const void *right)
{
	return strcmp(left, right);
}

size_t visited_items;

void count_list_item(const void *item)
{
	visited_items += (size_t)*(const int *)item;
}

void count_tlv_item(uint8_t tag, uint16_t length, const void *value)
{
	(void)value;
	visited_items += tag + length;
}

void count_map_item(const void *key, const void *value)
{
	(void)key;
	(void)value;
	visited_items++;
}

void *copy_integer(const void *item)
{
	int *copy = malloc(sizeof *copy);
	assert_non_null(copy);
	*copy = *(const int *)item;
	return copy;
}

config_named_s *add_config_option(list_t args, char short_name, char *long_name,
	config_arg_e type, config_arg_u value)
{
	config_named_s *option = calloc(1, sizeof *option);
	assert_non_null(option);
	option->short_option = short_name;
	option->long_option = long_name;
	option->description = long_name;
	option->response.type = type;
	option->response.value = value;
	assert_true(list_append(args, option));
	return option;
}

void free_pair_string(void *item)
{
	pair_string_s *pair = item;
	free(pair->s1);
	free(pair->s2);
	free(pair);
}
