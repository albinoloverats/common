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

#ifndef _COMMON_MAP_H_
#define _COMMON_MAP_H_

/*!
 * \file    map.h
 * \author  albinoloverats ~ Software Development
 * \date    2022-2022
 * \brief   Common map code shared between projects
 *
 * Common map implementation.
 */

#include <stddef.h>
#include <stdbool.h>

#include "common.h"
#include "list.h"

typedef void * MAP; /*!< The user visible MAP type */

/*!
 * \brief         Create a new map
 * \return        A new map
 *
 * Create a new map instance; all further operations are then performed
 * against this handle. Returns NULL on error.
 */
#define map_default() map_init(NULL, true, false)

extern MAP map_init(int c(const void *, const void *), bool f, bool s);

/*!
 * \brief         Destroy a map
 * \param[in]  h  A pointer to a map to destroy
 *
 * Destroy a previously created map when it is no longer needed. Free
 * the memory and sets h to NULL so all subsequent calls to MAP
 * functions will not result in undefined behaviour. If called with f as
 * false then keys and values items will not be freed.
 */
extern void map_deinit(MAP h) __attribute__((nonnull(1)));

extern size_t map_size(MAP h);

extern bool map_add(MAP h, const void *k, const void *v);

extern const void *map_get(MAP h, const void *k);

extern bool map_contains(MAP h, const void *k);

extern const void *map_remove(MAP h, const void *k);

extern LIST map_keys(MAP h);

#endif /* _COMMON_MAP_H_ */
