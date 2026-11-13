/*
 * Common code for dealing with tag, length, value arrays.
 * Copyright © 2009-2026, albinoloverats ~ Software Development
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

#if !defined _COMMON_TLV_H_
#define _COMMON_TLV_H_

/*!
 * \file    tlv.h
 * \author  albinoloverats ~ Software Development
 * \date    2009-2026
 * \brief   Common TLV code shared between projects
 *
 * Common tag/length/value code, for creating, importing and exporting
 * TLV values.
 */

#include <stdint.h> /*!< Necessary include as c99 standard integer types are referenced in this header */
#include <stdbool.h> /*!< Necessary include as c99 standard boolean type is referenced in this header */

#include "list.h"

typedef void * tlv_t; /*!< Opaque handle for a TLV collection */

/*!
 * \brief  A TLV structure
 *
 * A tag/length/value entry. The TLV collection makes a copy of the value
 * when an entry is appended.
 */
typedef struct
{
	uint8_t  tag;    /*!< The tag value */
	uint16_t length; /*!< The length of the value */
	void    *value;  /*!< The actual data */
}
tlv_entry_s;

/*!
 * \brief         Create a new TLV array
 * \return        A new TLV array
 *
 * Create a new TLV array instance; all further operations are then
 * performed against this handle. Returns NULL on error.
 */
extern tlv_t tlv_init(void) __attribute__((malloc));

/*!
 * \brief         Destroy a TLV array
 * \param[in]  h  TLV collection handle
 *
 * Destroy a previously created collection and free its entries,
 * copied values, and any cached export buffer. The handle is passed by
 * value and the caller's variable is not cleared.
 */
extern void tlv_deinit(tlv_t h);

/*!
 * \brief         Append a new TLV triple to the array
 * \param[in]  h  A pointer to the TLV
 * \param[in]  t  The TLV triple to add
 * \return        True if the entry was added
 *
 * Add an entry unless its tag already exists. The entry's value bytes
 * are copied into collection-owned storage; the caller retains ownership
 * of t.value.
 */
extern bool tlv_append(tlv_t h, tlv_entry_s t) __attribute__((nonnull(1)));

/*!
 * \brief         Remove a TLV triple from the array
 * \param[in]  h  A pointer to the TLV
 * \param[in]  t  The TLV triple to remove
 *
 * Remove the entry with t.tag and return it, or NULL if absent. The
 * returned entry and its value are no longer owned by the collection
 * and must be freed by the caller.
 */
extern const tlv_entry_s *tlv_remove(tlv_t h, tlv_entry_s t) __attribute__((nonnull(1)));

/*!
 * \brief         Remove a TLV triple from the array
 * \param[in]  h  A pointer to the TLV
 * \param[in]  t  The tag to remove
 *
 * Remove the entry with tag t and return it, or NULL if absent. The
 * returned entry and its value must be freed by the caller.
 */
extern const tlv_entry_s * tlv_remove_tag(tlv_t h, uint8_t t) __attribute__((nonnull(1)));

/*!
 * \brief         Get TLV structure for tag
 * \param[in]  h  The TLV array to check
 * \param[in]  t  The tag value to look for
 * \return        The individual TLV structure
 *
 * Return the collection-owned entry for tag t, or NULL if absent. Do
 * not modify or free the returned entry or its value.
 */
extern const tlv_entry_s *tlv_get(tlv_t h, uint8_t t) __attribute__((nonnull(1)));

/*!
 * \brief         Check whether a TLV array has a particular tag
 * \param[in]  h  The TLV array to check
 * \param[in]  t  The tag value to look for
 * \return        Whether the tag is used
 *
 * Check whether the TLV array has the tag given value.
 */
extern bool tlv_has_tag(tlv_t h, uint8_t t) __attribute__((nonnull(1)));

#define TLV_VALUE_OF_ARGS_COUNT(...) TLV_VALUE_OF_ARGS_COUNT2(__VA_ARGS__, 3, 2, 1) /*!< Function overloading argument count (part 1) */
#define TLV_VALUE_OF_ARGS_COUNT2(_1, _2, _3, _, ...) _                              /*!< Function overloading argument count (part 2) */

#define tlv_value_of_2(A, B)     tlv_value_of_aux(A, B, NULL) /*<! Call tlv_value_of_aux with NULL for third parameter */
#define tlv_value_of_3(A, B, C)  tlv_value_of_aux(A, B, C)    /*<! Call tlv_value_of_aux with both user supplied parameters */
#define tlv_value_of(...) CONCAT(tlv_value_of_, TLV_VALUE_OF_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__) /*!< Decide how to call tlv_value_of */

/*!
 * \brief         Get value for tag
 * \param[in]  h  The TLV array to check
 * \param[in]  t  The tag value to get
 * \param[in]  d  The default to use if the tag doesn't exist
 * \return        The value of the tag
 *
 * Return the collection-owned value for tag t, or d if the tag is
 * absent. The returned pointer is borrowed and must not be modified or
 * freed. Tags are unique, so there is at most one matching entry.
 */
extern byte_t *tlv_value_of_aux(tlv_t h, uint8_t t, uint8_t *d) __attribute__((nonnull(1)));

/*!
 * \brief         Get length for tag
 * \param[in]  h  The TLV array to check
 * \param[in]  t  The tag whose value length is requested
 * \return        The length for the given tag
 *
 * Retrieve the length for the TLV entry with the given tag value.
 */
extern uint16_t tlv_length_of(tlv_t ptr, uint8_t t) __attribute__((nonnull(1)));

#define TLV_EXPORT_ARGS_COUNT(...) TLV_EXPORT_ARGS_COUNT2(__VA_ARGS__, 2, 1) /*!< Function overloading argument count (part 1) */
#define TLV_EXPORT_ARGS_COUNT2(_1, _2, _, ...) _                             /*!< Function overloading argument count (part 2) */

#define tlv_export_1(A)       tlv_export_aux(A, true) /*<! Call tlv_export_aux with value of true for second parameter */
#define tlv_export_2(A, B)    tlv_export_aux(A, B)    /*<! Call tlv_export_aux with both user supplied parameters */
#define tlv_export(...) CONCAT(tlv_export_, TLV_EXPORT_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__) /*!< Decide how to call tlv_export */

/*!
 * \brief         Export the TLV array
 * \param[in]  h  The TLV array to export
 * \param[in]  e  Whether to encode lengths in network byte order
 * \return        The TLV array as a byte array
 *
 * Serialize entries as tag, 16-bit length, and value bytes. The
 * returned buffer is owned by the collection and remains valid only
 * until the next export or collection destruction. The one-argument
 * macro selects network byte order for lengths.
 */
extern byte_t *tlv_export_aux(tlv_t h, bool e) __attribute__((nonnull(1)));

/*!
 * \brief         The number of TLV items in the array
 * \param[in]  h  The TLV array to count
 * \return        The number of TLV items
 *
 * Count the number of TLV elements in the array.
 */
extern uint16_t tlv_size(tlv_t h) __attribute__((nonnull(1)));

/*!
 * \brief         The size of TLV array
 * \param[in]  h  The TLV array to size
 * \return        The size of the TLV array
 *
 * Return the total size of the TLV array in bytes; useful when calling
 * tlv_export().
 */
extern size_t tlv_length(tlv_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Set the TLV up for iterating
 * \param[in]  h  A pointer to the TLV
 * \return        An iterator for the TLV
 *
 * Create an iterator over collection entries. Free the iterator with
 * free() after use; entries returned are borrowed from the collection.
 */
extern iter_t tlv_iterator(tlv_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Get the next item in the TLV
 * \param[in]  h  TLV iterator
 * \return        The next item in the TLV
 *
 * Return the next entry and advance the iterator, or NULL when exhausted.
 */
extern const tlv_entry_s *tlv_get_next(iter_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Indicates if there is another item in the TLV
 * \param[in]  h  TLV iterator
 * \return        Returns true if there is another item
 *
 * Return whether the iterator has another entry.
 */
extern bool tlv_has_next(iter_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Call the given function for each item in the TLV
 * \param[in]  h  A pointer to the TLV
 * \param[in]  f  The function to call
 *
 * Call f once for each entry, passing its tag, length, and borrowed
 * value pointer.
 */
extern void tlv_for_each(tlv_t h, void f(uint8_t, uint16_t, const void *)) __attribute__((nonnull(1, 2)));

#endif /* _COMMON_TLV_H_ */
