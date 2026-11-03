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
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
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
