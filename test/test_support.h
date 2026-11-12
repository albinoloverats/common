/*
 * Unit tests.
 * Copyright © 2026, albinoloverats ~ Software Development
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

#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <sys/wait.h>
#endif
#include <sys/stat.h>
#include <unistd.h>

#include "ccrypt.h"
#include "cli.h"
#include "config.h"
#include "dir.h"
#include "ecc.h"
#include "error.h"
#include "list.h"
#include "map.h"
#include "mem.h"
#include "tlv.h"
#include "version.h"

extern size_t visited_items; /*!< Shared accumulator used by iteration tests */

/*! Compare pointers to int values. */
extern int compare_integer(const void *, const void *);
/*! Compare two string pointers with strcmp(). */
extern int compare_string(const void *, const void *);
/*! Add the pointed-to int value to visited_items. */
extern void count_list_item(const void *);
/*! Add the TLV tag and length to visited_items. */
extern void count_tlv_item(uint8_t, uint16_t, const void *);
/*! Increment visited_items for a map callback. */
extern void count_map_item(const void *, const void *);
/*! Allocate and return a copy of an integer value. */
extern void *copy_integer(const void *);
/*! Create and append a configured named-option test fixture. */
extern config_named_s *add_config_option(list_t, char, char *, config_arg_e, config_arg_u);
/*! Free a test pair of strings and its containing allocation. */
extern void free_pair_string(void *);

/*! Append a test group's registered cases to an aggregate test array. */
#define REGISTER_TESTS(name, tests) \
    void register_##name##_tests(struct CMUnitTest *all_tests, size_t *count) \
    { \
        for (size_t i = 0; i < sizeof(tests) / sizeof((tests)[0]); i++) \
            all_tests[(*count)++] = (tests)[i]; \
    }

#endif
