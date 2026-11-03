/*
 * Common code which is typically missing on Apple OS X.
 * Copyright © 2005-2025, albinoloverats ~ Software Development
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

#ifndef _NON_GNU_EXT_H_
#define _NON_GNU_EXT_H_

#if defined __APPLE__ || defined _WIN32
	/*
	 * Taken, once upon a time, from:
	 * https://github.com/lattera/glibc/blob/master/string/strchrnul.c
	 */
	/*!
	 * \brief         Find a character or the terminating NUL byte
	 * \param[in]  string  NUL-terminated string
	 * \param[in]  c       Character to find
	 * \return             Pointer to the first match or string's NUL terminator
	 */
	extern char *strchrnul(const char *string, int c);
#endif

#if defined __APPLE__ || defined _WIN32 || defined __FreeBSD__ || defined __sun
	/*
	 * Borrowed from:
	 * https://github.com/lattera/glibc/blob/master/string/strverscmp.c
	 */
	/*!
	 * \brief         Compare version-like strings, interpreting digit runs numerically
	 * \param[in]  s1  First string
	 * \param[in]  s2  Second string
	 * \return         Negative, zero, or positive according to ordering
	 */
	extern int strverscmp(const char *s1, const char *s2);
#endif

#if defined __sun || defined __clang__
	#define __attribute__(X)
#endif

#if defined __APPLE__ || defined _WIN32 || defined __clang__ || defined __sun
	#define __float128  double
	#define strtof128   strtod
	#define strfromf128 snprintf
#endif


/* All Windows specific from here down */

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <direct.h>

#define srand48 srand  /*!< Quietly alias srand48 to be srand on Windows */
#define lrand48 rand   /*!< Quietly alias lrand48 to be rand on Windows */
#define F_RDLCK NOTSET /*!< If value doesn’t exist on Windows, ignore it */
#define F_WRLCK NOTSET /*!< If value doesn’t exist on Windows, ignore it */
#define O_FSYNC NOTSET /*!< If value doesn’t exist on Windows, ignore it */

#ifdef S_IRUSR
	#undef S_IRUSR
	#define S_IRUSR 0
#endif
#ifdef S_IWUSR
	#undef S_IWUSR
	#define S_IWUSR 0
#endif
#ifdef S_IXUSR
	#undef S_IXUSR
	#define S_IXUSR 0
#endif

#ifndef SIGQUIT
	#define SIGQUIT SIGBREAK /*!< If value doesn’t exist on Windows, use next closest match */
#endif
#ifndef SIGSYS
	#define SIGSYS SIGILL    /*!< If value doesn’t exist on Windows, use next closest match */
#endif
#ifndef SIGBUS
	#define SIGBUS SIGSEGV   /*!< If value doesn’t exist on Windows, use next closest match */
#endif

#ifndef ECANCELED
	#define ECANCELED 125 /*!< Make sure the missing error code exists */
#endif
#define __LITTLE_ENDIAN 1234 /*!< Not defined in MinGW, so set here */
#define __BYTE_ORDER __LITTLE_ENDIAN /*!< Windows is almost always going to be LE */

#define __bswap_16(x) /*!< Define ourselves a 2-byte swap macro */ ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))

#define ntohs(x) __bswap_16(x) /*!< Make sure that network-to-host-short exists */
#define htons(x) __bswap_16(x) /*!< Make sure that host-to-network-short exists */

#define __bswap_32(x) /*!< Define ourselves a 4-byte swap macro */ ((((x) & 0xff000000ul) >> 24) | (((x) & 0x00ff0000ul) >>  8) | (((x) & 0x0000ff00ul) <<  8) | (((x) & 0x000000fful) << 24))

#define ntohl(x) __bswap_32(x) /*!< Make sure that network-to-host-long exists */
#define htonl(x) __bswap_32(x) /*!< Make sure that host-to-network-long exists */

#ifndef vsnprintf
	#define vsnprintf _vsnprintf
#endif

#define fsync(fd) _commit(fd)
#define mkdir(dir, attr) _mkdir(dir)
#define lstat(path, st) stat(path, st)
#define link(old, new) CopyFile(old, new, FALSE)
#define tmpfile temp_file

/*!
 * \brief         Read one line from a stream, allocating or resizing the buffer
 * \param[in,out] lineptr  Address of the caller's line buffer pointer
 * \param[out]    n        Receives the number of bytes read
 * \param[in]     stream   Input stream
 * \return                  Number of bytes read, or -1 at end of file
 */
extern ssize_t getline(char **lineptr, size_t *n, FILE *stream) __attribute__((nonnull(3)));

/*!
 * \brief         Duplicate at most l bytes from a string
 * \param[in]  s  Source string
 * \param[in]  l  Maximum bytes to copy
 * \return        Newly allocated NUL-terminated string
 */
extern char *strndup(const char *s, size_t l) __attribute__((nonnull(1)));

/*!
 * \brief         Scan a directory and return selected entries
 * \param[in]  path  Directory path
 * \param[out] res   Receives an allocated array of allocated dirent entries
 * \param[in]  sel   Optional selection predicate
 * \param[in]  cmp   Optional sorting comparator
 * \return           Number of entries, or -1 on failure; caller frees
 *                   each returned entry and the array
 */
extern int scandir(const char *path, struct dirent ***res, int (*sel)(const struct dirent *), int (*cmp)(const struct dirent **, const struct dirent **)) __attribute__((nonnull(1, 2)));

/*!
 * \brief         Compare directory entries by locale collation of their names
 * \param[in]  a  First dirent pointer
 * \param[in]  b  Second dirent pointer
 * \return        Result from strcoll on the entry names
 */
extern int alphasort(const struct dirent **a, const struct dirent **b);

/*! Open a temporary file in the platform's temporary directory. */
extern FILE *temp_file(void);

/*! Return a static string describing the Windows version. */
extern char *windows_version(void);

/*!
 * \brief         Print a signal description to stderr
 * \param[in]  sig  Signal number
 * \param[in]  s    Optional message prefix
 */
extern void psignal(int sig, const char *s);

#endif /* _WIN32 */

#endif /* _NON_GNU_EXT_H_ */
