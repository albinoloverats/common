/*
 * Version checking functions (non-application specific).
 * Copyright © 2005-2026, albinoloverats ~ Software Development
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

#if !defined _COMMON_VERSION_H_
#define _COMMON_VERSION_H_

#define NEW_VERSION_AVAILABLE "Version %s is available!"
#define NEW_VERSION_OF_AVAILABLE "Version %s of %s is available!"
#define NEW_VERSION_URL "Version %s of %s is available from\n%s\n"

/*!
 * \brief         Print application version and build/runtime details
 * \param[in]  name     Application name
 * \param[in]  version  Application version string
 * \param[in]  url      Fallback URL to display when no update URL was returned
 *
 * Writes details to stderr and waits for an in-progress update check
 * before reporting an available version.
 */
extern void version_print(char *name, char *version, char *url);

/*!
 * \brief         Create a formatted build/runtime information string
 * \return        Newly allocated string; caller frees it
 */
extern char *version_build_info(void);

/*!
 * \brief         Check for a newer version in a detached background thread
 * \param[in]  current_version  Current version string
 * \param[in]  check_url        URL returning the latest version string
 * \param[in]  download_url     Optional URL format used to download/install an update
 *
 * When download_url is non-NULL and a newer version is found, the
 * implementation attempts to download and launch the update.
 */
extern void version_check_for_update(char *current_version, char *check_url, char *download_url) __attribute__((nonnull(1, 2)));

extern bool version_new_available; /*!< Set when the check reports a newer version */
extern bool version_is_checking;   /*!< Set while the background check is active */
extern char version_available[];   /*!< Buffer containing the reported newer version */
extern char new_version_url[];    /*!< Buffer containing the formatted download URL, when downloaded */

#endif /* _COMMON_VERSION_H_ */
