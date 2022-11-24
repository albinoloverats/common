/*
 * Common code for dealing with maps.
 * Copyright © 2022-2022, albinoloverats ~ Software Development
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

#include <stdlib.h>

#include <stdbool.h>

#include "common.h"
#include "error.h"
#include "map.h"

typedef struct
{
	LIST entries;
	LIST keys; // keeping a list of keys might help in the future (I don't know yet)
	int (*compare)(const void *, const void *); /*!< How to compare entries (keys) in the map */
	bool free:1;
	bool overwrite:1;
}
map_private_t;

typedef struct
{
	map_private_t *parent; // reference to parent gives compare function in entry compare
	const void *key;
	const void *value;
}
entry_t;

static int map_compare(const void *a, const void *b);
static void map_free(void *map);

extern MAP map_init(int c(const void *, const void *), bool f, bool s, bool o)
{
	map_private_t *m = calloc(sizeof( map_private_t ), sizeof( byte_t ));
	if (!m)
		die(_("Out of memory @ %s:%d:%s [%zu]"), __FILE__, __LINE__, __func__, sizeof( map_private_t ));
	m->entries = list_init(map_compare, false, s);
	m->keys = list_init(c, false, s);
	m->compare = c;
	m->free = f;
	m->overwrite = o;
	return m;
}

extern void map_deinit(MAP ptr)
{
	map_private_t *map = (map_private_t *)ptr;
	if (!map)
		return;
	list_deinit(map->keys);
	list_deinit(map->entries, map_free);
	free(map);
	return;
}

extern size_t map_size(MAP ptr)
{
	if (!ptr)
		return -1;
	return list_size(((map_private_t *)ptr)->entries);
}

extern bool map_add(MAP ptr, const void *k, const void *v)
{
	map_private_t *map = (map_private_t *)ptr;
	if (!map)
		return false;
	bool exists = list_contains(map->keys, k);
	if (exists && !map->overwrite)
		return false; // already exists and not overwritting
	if (!list_append(map->keys, k))
		return false;
	entry_t *e;
	if (exists && map->overwrite)
	{
		e = (entry_t *)list_remove_item(map->entries, &e);
		if (map->free)
			free((void *)e->value);
	}
	else
	{
		e = calloc(sizeof( entry_t ), sizeof( byte_t ));
		e->parent = map;
		e->key = k;
	}
	e->value = v;
	if (list_append(map->entries, e))
		return true;
	list_remove_item(map->keys, k);
	free(e);
	return false;
}

extern const void *map_get(MAP ptr, const void *k)
{
	map_private_t *map = (map_private_t *)ptr;
	if (!map)
		return NULL;
	if (!list_contains(map->keys, k))
		return NULL;
	entry_t e = { map, k, NULL };
	const entry_t *r = list_contains(map->entries, &e);
	return r->value;
}

extern bool map_contains(MAP ptr, const void *k)
{
	return map_get(ptr, k);
}

extern const void *map_remove(MAP ptr, const void *k)
{
	map_private_t *map = (map_private_t *)ptr;
	if (!map)
		return NULL;
	if (!list_contains(map->keys, k))
		return NULL;
	entry_t e = { map, k, NULL };
	list_remove_item(map->keys, k);
	const entry_t *r = list_remove_item(map->entries, &e);
	if (map->free)
		free((void *)r->key);
	const void *x = r->value;
	free((void *)r);
	return x;
}

extern LIST map_keys(MAP ptr)
{
	return ((map_private_t *)ptr)->keys;
}

/*
 * TODO implement an iterator
 */

static int map_compare(const void *a, const void *b)
{
	const entry_t *x = a;
	const entry_t *y = b;
	if (x->parent->compare)
		return x->parent->compare(x->key, y->key);
	return x->key - y->key;
}

static void map_free(void *m)
{
	entry_t *e = (entry_t *)m;
	if (e->parent->free) {
		free((void *)e->key);
		free((void *)e->value);
	}
	free(e);
	return;
}
