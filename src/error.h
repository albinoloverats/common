/*
 * Common code for error reporting.
 * Copyright © 2009-2025, albinoloverats ~ Software Development
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

#ifndef _COMMON_ERROR_H_
#define _COMMON_ERROR_H_

/*!
 * \file    error.h
 * \author  albinoloverats ~ Software Development
 * \date    2009-2025
 * \brief   Common error handling code shared between projects
 *
 * Common error handling code, currently only fatal end of execution,
 * likely due to out of memory or similar.
 */

#ifdef BUILD_GUI
	#include <gtk/gtk.h>
#endif

#define BACKTRACE_BUFFER_LIMIT 1024 /*!< Maximum number of elements in the backtrace buffer */

/*!
 * \brief Install handlers for fatal signals
 *
 * Installs the application's fatal-signal handler once.
 */
extern void error_init(void);

/*!
 * \brief         Display fatal error to user and quit application
 * \param[in]  s  String format, followed by optional additional variables
 *
 * Print the formatted message and terminate the process using the errno
 * value saved on entry as the exit status (or success when errno is zero).
 */
extern void die(const char * const restrict s, ...) __attribute__((noreturn, nonnull(1), format(printf, 1, 2)));

#ifdef BUILD_GUI
/*!
 * \brief         Initialize GUI error reporting
 * \param[in]  w  Error dialog window
 * \param[in]  m  Label used to display the error message
 */
extern void error_gui_init(GtkWidget *w, GtkWidget *m) __attribute__((nonnull(1), nonnull(2)));

/*!
 * \brief         Hide the configured error dialog
 * \param[in]  w  Signal source (unused)
 * \param[in]  d  Signal data (unused)
 * \return        TRUE, indicating the close event was handled
 */
G_MODULE_EXPORT gboolean error_gui_close(void *, void *);
#endif

#endif /* _COMMON_ERROR_H_ */
