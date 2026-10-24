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

extern size_t visited_items;
extern int compare_integer(const void *, const void *);
extern int compare_string(const void *, const void *);
extern void count_list_item(const void *);
extern void count_tlv_item(uint8_t, uint16_t, const void *);
extern void count_map_item(const void *, const void *);
extern void *copy_integer(const void *);
extern config_named_s *add_config_option(list_t, char, char *, config_arg_e, config_arg_u);
extern void free_pair_string(void *);

#define REGISTER_TESTS(name, tests) \
    void register_##name##_tests(struct CMUnitTest *all_tests, size_t *count) \
    { \
        for (size_t i = 0; i < sizeof(tests) / sizeof((tests)[0]); i++) \
            all_tests[(*count)++] = (tests)[i]; \
    }

#endif
