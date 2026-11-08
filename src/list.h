/*
 * Common code for dealing with linked lists.
 * Copyright © 2021-2026, albinoloverats ~ Software Development
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

#ifndef _COMMON_LIST_H_
#define _COMMON_LIST_H_

/*!
 * \file    list.h
 * \author  albinoloverats ~ Software Development
 * \date    2021-2026
 * \brief   Common linked list code shared between projects
 *
 * Common linked list implementation.
 */

#include <stddef.h>
#include <stdbool.h>

#include "common.h"

typedef void * list_t; /*!< Opaque handle for a list */

typedef void * iter_t; /*!< Opaque handle for a list or map iterator */

#if 0
#define LIST_INIT_COUNT(...) LIST_INIT_COUNT2(__VA_ARGS__, 3, 2, 1) /*!< Function overloading argument count (part 1) */
#define LIST_INIT_COUNT2(_1, _2, _3, _, ...) _                      /*!< Function overloading argument count (part 2) */
// doesn't (yet) work with zero arguments
#define list_init_0()         list_init_aux(NULL, true, false) /*<! Call list_init_aux with NULL and true for parameters    */
#define list_init_1(A)        list_init_aux(A, true, false)    /*<! Call list_init_aux with true for second parameter       */
#define list_init_2(A, B)     list_init_aux(A, B, false)       /*<! Call list_init_aux with both user supplied parameters   */
#define list_init_3(A, B, C)  list_init_aux(A, B, C)           /*<! Call list_init_aux with both user supplied parameters   */
#define list_init(...) CONCAT(list_init_, LIST_INIT_COUNT(__VA_ARGS__))(__VA_ARGS__) /*!< Decide how to call list_init */
#endif

/*!
 * \brief         Create a new linked list
 * \return        A new linked list
 *
 * Create a new linked list instance; all further operations are then
 * performed against this handle. Returns NULL on error.
 */
#define list_default() list_init(NULL, true, false)

/*!
 * \brief         Create a new linked list
 * \return        A new linked list
 *
 * Create a new linked list instance that is intended to contain integer
 * values. It allows duplicates and will sort the list according to
 * their value. Returns NULL on error.
 */
#define list_integer() list_init((void *)list_compare_integer, true, true)

/*!
 * \brief         Create a new linked list
 * \return        A new linked list
 *
 * Create a new linked list instance that is intended to contain decimal
 * values. It allows duplicates and will sort the list according to
 * their values. Returns NULL on error.
 */
#define list_decimal() list_init((void *)list_compare_decimal, true, true)

/*!
 * \brief         Create a new linked list
 * \return        A new linked list
 *
 * Create a new linked list instance that is intended to contain string
 * values. It allows duplicates and will sort the list according to
 * strcmp. Returns NULL on error.
 */
#define list_string() list_init((void *)strcmp, true, true)

/*!
 * \brief         Create a new linked list
 * \param[in]  c  A function to compare items within the list (can be NULL)
 * \param[in]  d  Whether to allow duplicates in the list
 * \param[in]  s  Whether items in the list should be sorted
 * \return        A new linked list
 *
 * Create a new linked list instance; all further operations are then
 * performed against this handle. Returns NULL on error. The comparator
 * is used when searching and removing items to check if the correct
 * item has been found or whether the item can be deleted. Use
 * list_default() if you are happy with no comparator and allowing
 * duplicates in your list.
 */
extern list_t list_init(int c(const void *, const void *), bool d, bool s) __attribute__((malloc));

#define LIST_DEINIT_ARGS_COUNT(...) LIST_DEINIT_ARGS_COUNT2(__VA_ARGS__, 2, 1) /*!< Function overloading argument count (part 1) */
#define LIST_DEINIT_ARGS_COUNT2(_1, _2, _, ...) _                              /*!< Function overloading argument count (part 2) */

#define list_deinit_1(A)     list_deinit_aux(A, NULL)  /*<! Call list_deinit_aux with false for second parameter */
#define list_deinit_2(A, B)  list_deinit_aux(A, B)     /*<! Call list_deinit_aux with both user supplied parameters */
#define list_deinit(...) CONCAT(list_deinit_, LIST_DEINIT_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__) /*!< Decide how to call list_deinit */

/*!
 * \brief         Destroy a linked list
 * \param[in]  h  A pointer to a linked list to destroy
 * \param[in]  f  A function to free data items within the list
 *
 * Destroy a previously created list and its internal nodes. The handle
 * is passed by value, so the caller's variable is not cleared. If f is
 * NULL, the pointed-to data items are not freed.
 */
extern void list_deinit_aux(list_t h, void f(void *));

/*!
 * \brief         Copy an existing list
 * \param[in]  h  A pointer to an existing list
 * \param[in]  c  A function to copy the item data
 * \return        A new list
 *
 * Create a new list with the same comparator, duplicate policy, and
 * ordering policy as the source. c is called for each item to produce
 * the copied item; the caller owns the returned list and copied items.
 */
extern list_t list_copy(list_t h, void *c(const void *)) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Get the number of items in the list
 * \param[in]  h  A pointer to the list
 * \return        The number of items in the list
 *
 * Return the number of items in the list in constant time.
 */
extern size_t list_size(list_t h) __attribute__((nonnull(1)));

//#define LIST_SIZE(l) (((ptrdiff_t)l)+4)

/*!
 * \brief         Add an item to the end of the list
 * \param[in]  h  A pointer to the list
 * \param[in]  d  The item to add to the list
 * \return        Whether the item was added
 *
 * Append an item, unless the list is sorted, in which case it is
 * inserted according to the comparator. Returns false for a duplicate
 * when duplicates are disabled. The list stores the pointer, not a copy.
 */
extern bool list_append(list_t h, const void *d) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Insert an item into the list
 * \param[in]  h  A pointer to the list
 * \param[in]  i  The index where to insert the item
 * \param[in]  d  The item to be inserted
 * \return        Whether the item was added
 *
 * Insert before index i, or append when i is at or beyond the list
 * length. Sorted lists ignore i and insert by comparator. Returns false
 * for a duplicate when duplicates are disabled.
 */
extern bool list_insert(list_t h, size_t i, const void *d) __attribute__((nonnull(1, 3)));

/*!
 * \brief         Add an item to the sorted list
 * \param[in]  h  A pointer to the list
 * \param[in]  d  The item to add to the list
 * \return        Whether the item was added
 *
 * Insert by comparator when sorted; otherwise append. Returns false for
 * a duplicate when duplicates are disabled. The list stores the pointer,
 * not a copy.
 */
extern bool list_add(list_t h, const void *d) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Add all items to the list
 * \param[in]  h  A pointer to the list
 * \param[in]  o  The other list to add from
 * \param[in]  c  Function to copy item data
 * \return        Number of items added, or -1 if either list handle is NULL
 *
 * Call c for each source item and append the returned pointer to the
 * destination. The return value counts successful additions; rejected
 * duplicates are not counted. The caller manages ownership of copied
 * items that were not added.
 */
extern int list_add_all(list_t h, list_t o, void *c(const void *)) __attribute__((nonnull(1, 2, 3)));

/*!
 * \brief         Check if the list contains the item
 * \param[in]  h  A pointer to the list
 * \param[in]  d  The item to check for
 * \return        A pointer to the item already in the list, NULL
 *                otherwise
 *
 * Check whether the list contains the item. If a comparator was set
 * during initialisation then that is used instead of just comparing the
 * pointer. Returns a pointer to the existing item or NULL if it doesn't
 * exist in the list.
 */
extern const void *list_contains(list_t h, const void *d) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Remove an item from within the list
 * \param[in]  h  A pointer to the list
 * \param[in]  d  The item to remove
 * \return        The item that was removed, or NULL
 *
 * Remove every matching item (pointer equality or comparator equality)
 * and return the last removed data pointer, or NULL if none matched.
 * Removed data is not freed.
 */
extern const void *list_remove_item(list_t h, const void *d) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Remove an item from within the list
 * \param[in]  h  A pointer to the list
 * \param[in]  i  The index of the item to remove
 * \return        The removed item, or NULL if i is out of range
 *
 * Remove the item at the given index from the list. The item is
 * returned to allow the user to free it if necessary.
 */
extern const void *list_remove_index(list_t h, size_t i) __attribute__((nonnull(1)));

/*!
 * \brief         Get an item from the list
 * \param[in]  h  A pointer to the list
 * \param[in]  i  The index of the item to get
 * \return        The item at the give index
 *
 * Return the item at index i, or NULL if i is out of range. The item
 * remains owned by the caller/list according to its existing ownership
 * convention and is not copied.
 */
extern const void *list_get(list_t h, size_t i) __attribute__((nonnull(1)));

/*!
 * \brief         Set the list up for iterating
 * \param[in]  h  A pointer to the list
 * \return        An iterator for the list
 *
 * Create an iterator over the current list nodes. Free the iterator
 * with free() after use; returned item pointers are not copied.
 */
extern iter_t list_iterator(list_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Get the next item in the list
 * \param[in]  h  A pointer to the iterator
 * \return        The next item in the list
 *
 * Return the next item and advance the iterator, or NULL when exhausted.
 */
extern const void *list_get_next(iter_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Indicates if there is another item in the list
 * \param[in]  h  A pointer to the iterator
 * \return        Returns true if there is another item
 *
 * Return whether the iterator has another item.
 */
extern bool list_has_next(iter_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Call the given function for each item in the list
 * \param[in]  h  A pointer to the list
 * \param[in]  f  The function to call
 *
 * Iterate through the list, calling the given function for each item.
 */
extern void list_for_each(list_t h, void f(const void *)) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Sort the given list
 * \param[in]  h  A pointer to the list
 *
 * Sort the list using its comparator and keep it sorted for subsequent
 * insertions. If no comparator is configured, the list is unchanged.
 */
extern void list_sort(list_t h) __attribute__((nonnull(1)));

//extern void list_tidy(list_t h) __attribute__((nonnull(1)));

/*!
 * \brief         Add comparator to the list
 * \param[in]  h  A pointer to the list
 * \param[in]  c  The comparator to add
 *
 * Set the comparator used for equality checks and sorting. This does
 * not immediately reorder items or enable sorted insertion on a list
 * that was initialized as unsorted.
 */
extern void list_add_comparator(list_t h, int c(const void *, const void *)) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Compare two signed 64-bit integers
 * \param[in]  a  Pointer to the first int64_t value
 * \param[in]  b  Pointer to the second int64_t value
 * \return        Negative, zero, or positive according to their ordering
 */
extern int list_compare_integer(const void *a, const void *b);

/*!
 * \brief         Compare two long double values
 * \param[in]  a  Pointer to the first long double value
 * \param[in]  b  Pointer to the second long double value
 * \return        Negative, zero, or positive according to their ordering
 */
extern int list_compare_decimal(const void *a, const void *b);

#endif
