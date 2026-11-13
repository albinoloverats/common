/*
 * Common code for working with directory trees.
 * Copyright © 2007-2026, albinoloverats ~ Software Development
 * email: stegfs@albinoloverats.net
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

#if !defined _DIR_H_
#define _DIR_H_

/*!
 * \file    dir.h
 * \author  albinoloverats ~ Software Development
 * \date    2009-2026
 * \brief   Directory parsing functions
 *
 * String-returning path helpers allocate their results. Directory-tree
 * enumeration returns a list of allocated path strings.
 */

#include <inttypes.h>

#include "common.h"
#include "list.h"

#if !defined _WIN32
	#define DIR_SEPARATOR "/"
	#define DIR_SEPARATOR_CHAR '/'
#else
	#define DIR_SEPARATOR "\\"
	#define DIR_SEPARATOR_CHAR '\\'
#endif

#define path_equals(X, Y)       (X && Y && !strcmp(X, Y)) /*!< True when both non-NULL paths are identical */
#define path_starts_with(X, Y)  (X && Y && !strncmp(X, Y, strlen(X))) /*!< True when Y begins with non-NULL prefix X */

#define DIR_GET_NAME_ARGS_COUNT(...) DIR_GET_NAME_ARGS_COUNT2(__VA_ARGS__, 2, 1)
#define DIR_GET_NAME_ARGS_COUNT2(_1, _2, _, ...) _

#define dir_get_name_1(A)     dir_get_name_aux(A, '\0')
#define dir_get_name_2(A, B)  dir_get_name_aux(A, B)
#define dir_get_name(...) CONCAT(dir_get_name_, DIR_GET_NAME_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define DIR_SCAN_TOP                                                            \
	struct dirent **eps = NULL;                                             \
	int n = 0;                                                              \
	if ((n = scandir(path, &eps, NULL, alphasort)))                         \
	{                                                                       \
		for (int dxxi = 0; dxxi < n; ++dxxi)                            \
		{                                                               \
			char *nm = eps[dxxi]->d_name;                           \
			if (!strcmp(".", nm) || !strcmp("..", nm))              \
				continue;                                       \
			char *file = m_strdupf("%s/%s", path, eps[dxxi]->d_name)

#define DIR_SCAN_END                                                            \
			free(file);                                             \
		}                                                               \
	}                                                                       \
	for (int dxxi = 0; dxxi < n; ++dxxi)                                    \
		free(eps[dxxi]);                                                \
	free(eps)


/*!
 * \brief Bit flags selecting filesystem entry types
 */
typedef enum
{
	DIR_NONE    = 0x0000, /*!< Select no entry types */

	DIR_FOLDER  = 0x0001, /*!< Directories */
	DIR_FILE    = 0x0002, /*!< Regular files */
	DIR_LINK    = 0x0004, /*!< Symbolic links */
	DIR_BLOCK   = 0x0008, /*!< Block devices */
	DIR_CHAR    = 0x0010, /*!< Character devices */
	DIR_SOCKET  = 0x0020, /*!< Sockets */
	DIR_PIPE    = 0x0040, /*!< FIFOs */

	DIR_ALL     = 0x00FF  /*!< All supported entry types */
}
dir_type_e;

/*!
 * \brief         Extract the name part of the /path/file:password
 * \param[in]  p  Path containing the entry name
 * \param[in]  s  Delimiter whose suffix should be omitted; '\0' keeps the full basename
 * \return        Newly allocated string: name
 *
 * Return the basename, stopping at the last occurrence of sep in the
 * input path when sep is nonzero.
 */
extern char *dir_get_name_aux(const char * const p, const char s) __attribute__((nonnull(1)));

/*!
 * \brief         Determine how deep down the hierarchy path goes
 * \param[in]  p  Path to count separators in
 * \return        Number of directory separator characters in path
 *
 * Count directory separators, including a leading separator and any
 * separator before a filename.
 */
extern uint16_t dir_get_deep(const char * const p) __attribute__((nonnull(1)));

/*!
 * \brief         Extract the numbered part of the /path/file:password
 * \param[in]  p  Path to split
 * \param[in]  i  Zero for the root separator, then one-based path components
 * \return        Newly allocated path component
 *
 * Return the indexed component of the path. Index zero returns the root
 * separator; subsequent indexes return components from left to right.
 */
extern char *dir_get_part(const char * const p, const uint16_t i) __attribute__((nonnull(1)));

/*!
 * \brief         Extract the password part of the /path/file:password
 * \param[in]  p  Path that may include a colon-delimited password
 * \return        Newly allocated password suffix, or an empty string
 *
 * Return the text following the final colon, or an empty string if no
 * colon is present.
 */
extern char *dir_get_pass(const char * const restrict p) __attribute__((nonnull(1)));

/*!
 * \brief         Extract only the path part of the /path/file:password
 * \param[in]  p  Path whose final component should be removed
 * \return        Newly allocated parent path
 *
 * Return the parent directory path, the root separator for a root-level
 * absolute path, or an empty string when the input has no separator.
 */
extern char *dir_get_path(const char * const restrict p) __attribute__((nonnull(1)));


/*!
 * \brief         Create a new directory recursively
 * \param[in]  p  Directory path to create
 * \param[in]  m  New directories mode
 *
 * Attempts to create the requested directory and any missing parents.
 * An empty path is ignored. This function does not report creation errors.
 */
extern void dir_mk_recursive(const char *p, mode_t m) __attribute__((nonnull(1)));

/*!
 * \brief         Get all entries in a directory tree
 * \param[in]  p  Directory tree to scan
 * \param[in]  t  Bitmask of entry types to include
 * \return        List of newly allocated path strings; caller frees
 *                the strings and destroys the list
 *
 * Recursively scan the tree and return paths whose entry types match
 * types. Directories are traversed regardless of whether DIR_FOLDER is
 * selected.
 */
extern list_t dir_get_tree(const char *p, dir_type_e t) __attribute__((nonnull(1)));

#endif /* _DIR_H_ */
