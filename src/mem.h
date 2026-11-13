/*
 * Common code for memory handling (in a do-or-die kind of way).
 * Copyright © 2024-2026, albinoloverats ~ Software Development
 * email: encrypt@albinoloverats.net
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

#if !defined _COMMON_MEMORY_H_
#define _COMMON_MEMORY_H_

#include "common.h"

/*! Allocate S bytes; terminate through die() if allocation fails. */
#define m_malloc(S)            mem_mod(__FILE__, __LINE__, __func__, S)
/*! Allocate C elements of S bytes each; terminate through die() on failure. */
#define m_calloc(C, S)         mem_cod(__FILE__, __LINE__, __func__, C, S)
/*! Resize P to S bytes; terminate through die() if allocation fails. */
#define m_realloc(P, S)        mem_rod(__FILE__, __LINE__, __func__, P, S)

/*! Duplicate a NUL-terminated string; terminate through die() on failure. */
#define m_strdup(S)            mem_sod(__FILE__, __LINE__, __func__, S)
/*! Duplicate at most L bytes and append a NUL terminator. */
#define m_strndup(S, L)        mem_nod(__FILE__, __LINE__, __func__, S, L)

/*! Format an allocated string into P, replacing its previous allocation. */
#define m_asprintf(P, F, ...)  mem_aod(__FILE__, __LINE__, __func__, P, F, __VA_ARGS__)

/*! Return a newly allocated formatted string. */
#define m_strdupf(F, ...)      mem_fod(__FILE__, __LINE__, __func__, F, __VA_ARGS__)

/*!
 * \brief Allocate a block and report allocation failures with call-site information
 * \param[in] file  Source file name
 * \param[in] line  Source line number
 * \param[in] func  Function name
 * \param[in] size  Number of bytes to allocate
 * \return          Allocated memory
 */
extern void *mem_mod(const char *, int, const char *, size_t);

/*!
 * \brief Allocate zero-initialized memory and report failures with call-site information
 * \param[in] file   Source file name
 * \param[in] line   Source line number
 * \param[in] func   Function name
 * \param[in] count  Number of elements
 * \param[in] size   Size of each element in bytes
 * \return           Allocated memory
 */
extern void *mem_cod(const char *, int, const char *, size_t, size_t);

/*!
 * \brief Resize a block and report failures with call-site information
 * \param[in] file    Source file name
 * \param[in] line    Source line number
 * \param[in] func    Function name
 * \param[in] ptr     Existing allocation, or NULL
 * \param[in] newsize Requested size in bytes
 * \return            Resized allocation
 */
extern void *mem_rod(const char *, int, const char *, void *, size_t);

/*!
 * \brief         Duplicate a NUL-terminated string
 * \param[in]  file  Source file name
 * \param[in]  line  Source line number
 * \param[in]  func  Function name
 * \param[in]  str   String to duplicate
 * \return           Newly allocated string
 */
extern char *mem_sod(const char *, int, const char *, const char *);

/*!
 * \brief         Duplicate at most len bytes and NUL-terminate the result
 * \param[in]  file  Source file name
 * \param[in]  line  Source line number
 * \param[in]  func  Function name
 * \param[in]  str   String to duplicate
 * \param[in]  len   Maximum number of bytes to copy
 * \return           Newly allocated string
 */
extern char *mem_nod(const char *, int, const char *, const char *, size_t);

/*!
 * \brief         Allocate or replace a formatted string
 * \param[in]  file      Source file name
 * \param[in]  line      Source line number
 * \param[in]  func      Function name
 * \param[out] ptr       Receives the allocated string
 * \param[in]  template  printf-style format string
 * \return               Number of characters formatted, excluding NUL
 */
extern int mem_aod(const char *, int, const char *, char **, const char *, ...) __attribute__((nonnull(5), format(gnu_printf, 5, 6)));

/*!
 * \brief         Create a newly allocated formatted string
 * \param[in]  file      Source file name
 * \param[in]  line      Source line number
 * \param[in]  func      Function name
 * \param[in]  template  printf-style format string
 * \return               Newly allocated string
 */
extern char *mem_fod(const char *, int, const char *, const char *, ...) __attribute__((nonnull(4), format(gnu_printf, 4, 5)));

#if defined USE_GCRYPT

/*! Allocate secure memory; terminate through die() if allocation fails. */
#define m_gcry_malloc_secure(S)     mem_sec_mod(__FILE__, __LINE__, __func__, S)
/*! Allocate zero-initialized secure memory; terminate through die() on failure. */
#define m_gcry_calloc_secure(C, S)  mem_sec_cod(__FILE__, __LINE__, __func__, C, S)
/*! Resize secure memory; terminate through die() if allocation fails. */
#define m_gcry_realloc(P, S)        mem_sec_rod(__FILE__, __LINE__, __func__, P, S)

/*!
 * \brief Allocate secure memory and report failures with call-site information
 * \param[in] file  Source file name
 * \param[in] line  Source line number
 * \param[in] func  Function name
 * \param[in] size  Number of bytes to allocate
 * \return          Secure allocation
 */
extern void *mem_sec_mod(const char *, int, const char *, size_t);

/*!
 * \brief Allocate zero-initialized secure memory
 * \param[in] file   Source file name
 * \param[in] line   Source line number
 * \param[in] func   Function name
 * \param[in] count  Number of elements
 * \param[in] size   Size of each element in bytes
 * \return           Secure allocation
 */
extern void *mem_sec_cod(const char *, int, const char *, size_t, size_t);

/*!
 * \brief Resize secure memory and report failures with call-site information
 * \param[in] file    Source file name
 * \param[in] line    Source line number
 * \param[in] func    Function name
 * \param[in] ptr     Existing secure allocation, or NULL
 * \param[in] newsize Requested size in bytes
 * \return            Resized secure allocation
 */
extern void *mem_sec_rod(const char *, int, const char *, void *, size_t);

#endif /* USE_GCRYPT */

#endif /* _COMMON_MEMORY_H_ */
