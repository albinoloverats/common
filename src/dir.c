/*
 * stegfs ~ a steganographic file system for unix-like systems
 * Copyright © 2007-2022, albinoloverats ~ Software Development
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

#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "non-gnu.h"
#include "dir.h"

extern char *dir_get_name_aux(const char * const path, const char ext)
{
	const char *file = strrchr(path, DIR_SEPARATOR_CHAR);
	if (file)
		file++;
	else
		file = path;
	const char *pass = strrchr(path, ext);
	if (!pass)
		pass = strchr(file, '\0');
	return strndup(file, pass - file);
}

extern uint16_t dir_get_deep(const char * const path)
{
	uint16_t d = 0;
	char *ptr = (char *)path;
	while ((ptr = strchr(ptr, DIR_SEPARATOR_CHAR)))
	{
		d++;
		ptr++;
	}
	return d;
}

extern char *dir_get_part(const char * const path, const uint16_t index)
{
	if (!index)
		return strdup(DIR_SEPARATOR);
	char *ptr = (char *)path;
	for (uint16_t i = 0; i < index; i++)
	{
		ptr = strchr(ptr, DIR_SEPARATOR_CHAR);
		if (!ptr)
			break;
		ptr++;
	}
	return strndup(ptr, strchrnul(ptr, DIR_SEPARATOR_CHAR) - ptr);
}

extern char *dir_get_pass(const char * const restrict path)
{
	if (!strrchr(path, ':'))
		return strdup("");
	return strdup(strrchr(path, ':') + 1);
}

extern char *dir_get_path(const char * const restrict path)
{
	char *s = strrchr(path, DIR_SEPARATOR_CHAR);
	if (!s)
		return strdup("");
	char *p = strndup(path, s - path);
	if (strlen(p))
		return p;
	free(p);
	return strdup(DIR_SEPARATOR);
}

/*
 * Taken from http://nion.modprobe.de/tmp/mkdir.c
 */
extern void dir_mk_recursive(const char *path, mode_t mode)
{
#ifdef _WIN32
	(void)mode;
#endif
	char *opath = strdup(path);
	size_t len = strlen(opath);
	if (opath[len - 1] == '/')
		opath[len - 1] = '\0';
	for (char *p = opath; *p; p++)
		if (*p == '/')
		{
			*p = '\0';
			if (access(opath, F_OK))
				mkdir(opath, mode);
			*p = '/';
		}
	if (access(opath, F_OK)) /* if path is not terminated with / */
		mkdir(opath, mode);
	free(opath);
	return;
}
