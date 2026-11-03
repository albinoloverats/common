/*
 * Common code for dealing with maps.
 * Copyright © 2022-2024, albinoloverats ~ Software Development
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

#ifndef _COMMON_MAP_H_
#define _COMMON_MAP_H_

/*!
 * \file    map.h
 * \author  albinoloverats ~ Software Development
 * \date    2022-2024
 * \brief   Common map code shared between projects
 *
 * Common map implementation.
 */

#include <stddef.h>
#include <stdbool.h>

#include "common.h"
#include "list.h"
#include "pair.h"

typedef void * map_t; /*!< The user visible MAP type */

/*!
 * \brief         Create a new map
 * \param[in]  c  Optional key comparator, also used to order entries
 * \param[in]  f  Whether the map owns and frees keys and values on destruction
 * \param[in]  s  Whether entries are kept sorted by key
 * \param[in]  o  Whether adding an existing key replaces its value
 * \return        A new map
 *
 * Create a new map instance; all further operations are then performed
 * against this handle. Returns NULL on error.
 *
 * When o is true, replacing an entry does not free the old value unless f
 * is true. When f is true, the map takes ownership of inserted keys and
 * values.
 */
#define map_default() map_init(NULL, true, false, false)

extern map_t map_init(int c(const void *, const void *), bool f, bool s, bool o);

/*!
 * \brief         Destroy a map
 * \param[in]  h  A pointer to a map to destroy
 *
 * Destroy a previously created map and its internal storage. This
 * function receives the handle by value and cannot clear the caller's
 * variable. If map_init was called with f false, key and value objects
 * are not freed.
 */
extern void map_deinit(map_t h);

/*!
 * \brief         Get the number of entries in the map
 * \param[in]  h  A pointer to the map
 * \return        The number of entries in the map
 *
 * Get the number of entries in the map.
 */
extern size_t map_size(map_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Add an entry to the map
 * \param[in]  h  A pointer to the map
 * \param[in]  k  The key of the entry to add
 * \param[in]  v  The value of the entry to add
 * \return        Whether the entry was added or replaced
 *
 * Add a new entry to the map. If the map is sorted then the entry will
 * be inserted based on the results of the compare function, otherwise
 * it will be appended. If the key already exists, whether it is
 * overwritten depends on how the map was initialised. Returns true if
 * the key/value are added/replaced, false otherwise.
 */
extern bool map_add(map_t h, const void *k, const void *v) __attribute__((nonnull(1, 2, 3)));

/*!
 * \brief         Get a value from the map
 * \param[in]  h  Map handle
 * \param[in]  k  The key of the value to get
 * \return        The value for the given key
 *
 * Retrieve the value for the given key from within the map.
 */
extern const void *map_get(map_t h, const void *k) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Check if the map contains the key
 * \param[in]  h  Map handle
 * \param[in]  k  The key to check for
 * \return        True if the map contains a value for the given key,
 *                false otherwise
 *
 * Return whether map_get() returns a non-NULL value for k. Consequently,
 * a key mapped to NULL is indistinguishable from an absent key through
 * this function.
 */
extern bool map_contains(map_t h, const void *k) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Remove a key/value from the map
 * \param[in]  h  Map handle
 * \param[in]  k  The key of the entry to remove
 * \return        The value that was removed, or NULL
 *
 * Remove the given key/value from the map. Returns NULL if the key was
 * not found. The key is freed when the map owns entries; the returned
 * value is not freed and is transferred to the caller. Key comparison
 * follows the comparator configured for the map.
 */
extern const void *map_remove(map_t h, const void *k) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Set the map up for iterating
 * \param[in]  h  Map handle
 * \return        An iterator for the map
 *
 * Create an iterator over map entries. Free the iterator with free()
 * after use; entries returned by it are borrowed from the map.
 */
extern iter_t map_iterator(map_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Get the next entryin the map
 * \param[in]  h  Map iterator
 * \return        The next entry in the map
 *
 * Return the next key/value pair, or NULL when iteration is exhausted.
 * The pair is owned by the map and must not be modified or freed.
 */
extern const pair_object_s *map_get_next(iter_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Indicates if there is another item in the map
 * \param[in]  h  Map iterator
 * \return        Returns true if there is another entry
 *
 * Return whether the iterator has another entry.
 */
extern bool map_has_next(iter_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Call the given function for each item in the map
 * \param[in]  h  Map handle
 * \param[in]  f  The function to call
 *
 * Call f once for each key/value pair in the map.
 */
extern void map_for_each(map_t h, void f(const void *, const void *)) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Add comparator to the map
 * \param[in]  h  A pointer to the map
 * \param[in]  c  The comparator to add
 *
 * Set the comparator used to compare map-entry keys. This does not
 * reorder entries already present; configure the comparator in map_init
 * when sorted insertion or comparator-based lookup is required.
 */
extern void map_add_comparator(map_t h, int c(const void *, const void *)) __attribute__((nonnull(1, 2)));

#endif /* _COMMON_MAP_H_ */
