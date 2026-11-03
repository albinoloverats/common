/*
 * Common code for hanlding command line arguments and config files.
 * Copyright © 2005-2025, albinoloverats ~ Software Development
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

#ifndef _COMMON_CONFIG_H_
#define _COMMON_CONFIG_H_

#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"
#include "non-gnu.h"
#include "pair.h"
#include "list.h"


#define CONF_TRUE     "true"
#define CONF_ON       "on"
#define CONF_ENABLED  "enabled"
#define CONF_YES      "yes"
#define CONF_ONE      "1"

#define CONF_FALSE    "false"
#define CONF_OFF      "off"
#define CONF_DISABLED "disabled"
#define CONF_NO       "no"
#define CONF_ZERO     "0"


#define CONFIG_ARG_REQUIRED 0x80000000 // 1000
#define CONFIG_ARG_LIST     0x40000000 // 0100
#define CONFIG_ARG_PAIR     0xa0000000 // 0010

/*!
 * \brief Encoded type and modifiers for a parsed command-line/config value
 *
 * The low bits select the value type; the flag bits describe required
 * arguments, comma-separated lists, or paired values. CONFIG_ARG_OPT_*
 * and CONFIG_ARG_REQ_* aliases distinguish whether a value is optional
 * or mandatory.
 */
typedef enum
{
	CONFIG_ARG_BOOLEAN = 1, /*!< Boolean payload */
	CONFIG_ARG_INTEGER = 2, /*!< Signed integer payload */
	CONFIG_ARG_DECIMAL = 4, /*!< Long double payload */
	CONFIG_ARG_STRING  = 8, /*!< String payload */
	/* Optional-value aliases for readability in option declarations. */
	CONFIG_ARG_OPT_BOOLEAN      = CONFIG_ARG_BOOLEAN,
	CONFIG_ARG_OPT_INTEGER      = CONFIG_ARG_INTEGER,
	CONFIG_ARG_OPT_DECIMAL      = CONFIG_ARG_DECIMAL,
	CONFIG_ARG_OPT_STRING       = CONFIG_ARG_STRING,

	/* Required single values. */
	CONFIG_ARG_REQ_BOOLEAN      = (CONFIG_ARG_OPT_BOOLEAN | CONFIG_ARG_REQUIRED),
	CONFIG_ARG_REQ_INTEGER      = (CONFIG_ARG_OPT_INTEGER | CONFIG_ARG_REQUIRED),
	CONFIG_ARG_REQ_DECIMAL      = (CONFIG_ARG_OPT_DECIMAL | CONFIG_ARG_REQUIRED),
	CONFIG_ARG_REQ_STRING       = (CONFIG_ARG_OPT_STRING  | CONFIG_ARG_REQUIRED),

	CONFIG_ARG_PAIR_BOOLEAN     = (CONFIG_ARG_REQ_BOOLEAN | CONFIG_ARG_PAIR), /*!< Required pair of booleans */
	CONFIG_ARG_PAIR_INTEGER     = (CONFIG_ARG_REQ_INTEGER | CONFIG_ARG_PAIR),
	CONFIG_ARG_PAIR_DECIMAL     = (CONFIG_ARG_REQ_DECIMAL | CONFIG_ARG_PAIR),
	CONFIG_ARG_PAIR_STRING      = (CONFIG_ARG_REQ_STRING  | CONFIG_ARG_PAIR),

	CONFIG_ARG_LIST_BOOLEAN     = (CONFIG_ARG_REQ_BOOLEAN | CONFIG_ARG_LIST), /*!< Required list of booleans */
	CONFIG_ARG_LIST_INTEGER     = (CONFIG_ARG_REQ_INTEGER | CONFIG_ARG_LIST),
	CONFIG_ARG_LIST_DECIMAL     = (CONFIG_ARG_REQ_DECIMAL | CONFIG_ARG_LIST),
	CONFIG_ARG_LIST_STRING      = (CONFIG_ARG_REQ_STRING  | CONFIG_ARG_LIST),

	CONFIG_ARG_LIST_PAIR_STRING = (CONFIG_ARG_LIST_STRING | CONFIG_ARG_PAIR_STRING)
}
config_arg_e;

/*!
 * \brief Storage for one parsed configuration value
 *
 * Read the member selected by the corresponding config_arg_e type.
 */
typedef union
{
	bool boolean;       /*!< Boolean value */
	int64_t integer;    /*!< Signed integer value */
	//__float128 decimal;
	long double decimal; /*!< Decimal value */
	char *string;        /*!< Allocated string value */
	pair_u pair;          /*!< Two values of the configured pair type */
	list_t *list;         /*!< List handle for a list-valued option */
}
config_arg_u;

/*!
 * \brief Parsed configuration value and its declared type
 */
typedef struct
{
	config_arg_e type;  /*!< Declared value type and modifiers */
	config_arg_u value; /*!< Parsed value in the member selected by type */
}
config_arg_s;

/*!
 * \brief  A named command line parameter.
 *
 * A named command-line option, with its accepted spelling, help text,
 * expected value type, current value, and parsing state.
 */
typedef struct
{
	char short_option;           /*!< Short option character, without '-' */
	char *long_option;           /*!< Long option name, without '--' */
	char *option_type;           /*!< Display label for the expected value */
	char *description;           /*!< Help text for this option */
	config_arg_s response;       /*!< Value type and parsed value */
	bool required:1;             /*!< Whether this option must be supplied */
	bool advanced:1;             /*!< Whether this option is marked advanced in usage */
	bool hidden:1;               /*!< Whether this option is omitted from usage */
	bool seen:1;                 /*!< Whether a value/occurrence was parsed */
}
config_named_s;

/*!
 * \brief  An unnamed command line parameter.
 *
 * A positional command-line argument, with its expected value type
 * and parsing state.
 */
typedef struct
{
	char *description;           /*!< A description of the argument */
	config_arg_s response;       /*!< Value type and parsed value */
	bool required:1;             /*!< Whether this positional argument is required */
	bool seen:1;                 /*!< Whether a value was parsed */
}
config_unnamed_s;

/*!
 * \brief Application metadata used by configuration and help output
 *
 * The pointed-to strings are borrowed by the configuration module and
 * must remain valid while it uses them.
 */
typedef struct
{
	char *name;    /*!< Program name */
	char *version; /*!< Program version */
	char *url;     /*!< Project or download URL */
	char *config;  /*!< Configuration filename, or NULL to disable config-file use */
}
config_about_s;


/*!
 * \brief         Set application metadata for configuration handling
 * \param[in]  about  Application metadata; strings are borrowed, not copied
 *
 * Must be called before config_parse_aux() or update_config().
 */
extern void config_init(config_about_s about);

/*!
 * \brief         Print usage and exit successfully
 * \param[in]  args   List of config_named_s options
 * \param[in]  extra  Optional list of config_unnamed_s positional arguments
 */
extern void config_show_usage(list_t args, list_t extra);

#define CONFIG_PARSE_COUNT(...) CONFIG_PARSE_COUNT2(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define CONFIG_PARSE_COUNT2(_1, _2, _3, _4, _5, _6, _, ...) _

#define config_parse_2(A, B)              config_parse_aux(A, B, NULL, NULL, NULL, true)
#define config_parse_3(A, B, C)           config_parse_aux(A, B, C, NULL, NULL, true)
#define config_parse_4(A, B, C, D)        config_parse_aux(A, B, C, D, NULL, true)
#define config_parse_5(A, B, C, D, E)     config_parse_aux(A, B, C, D, E, true)
#define config_parse_6(A, B, C, D, E, F)  config_parse_aux(A, B, C, D, E, F)
#define config_parse(...) CONCAT(config_parse_, CONFIG_PARSE_COUNT(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief             Configuration reading/parsing
 * \param[in]      argc   Number of entries in argv
 * \param[in]      argv   Command-line arguments, including the program name at index 0
 * \param[in/out]  args   List of config_named_s options to parse
 * \param[in/out]  extra  Optional list of config_unnamed_s positional arguments
 * \param[in]      notes  Optional list of additional usage notes
 * \param[in]      warn   Whether to print usage/exit for unknown or missing arguments
 * \return                Number of named and positional arguments marked seen, or -1 if config_init was not called
 *
 * Parse the configured rc file (when enabled) and then command-line
 * arguments, updating each option's response and seen fields. When
 * warn is true, usage handling exits the process for unknown arguments
 * or missing required options.
 */
extern int config_parse_aux(int c, char **v, list_t a, list_t x, list_t t, bool w);

/*!
 * \brief         Update configuration file
 * \param[in]  o  Long option name as stored in the config file
 * \param[in]  v  Text value to write
 *
 * Set or append the named option in the configured user's config file.
 */
extern void update_config(const char * const restrict o, const char * const restrict v) __attribute__((nonnull(1, 2)));


/*!
 * \brief         Compare config_named_s's
 * \param[in]  a  First config_named_s
 * \param[in]  b  Second config_named_s
 * \return        The difference between the two
 *
 * Compare two config_named_s using the short_option. Used to ensure that
 * only one of each argument is in the list_t.
 */
extern int config_named_compare(const void *a, const void *b);

/*!
 * \brief         Compare config_unnamed_s's
 * \param[in]  a  First config_unnamed_s
 * \param[in]  b  Second config_unnamed_s
 * \return        The difference between the two
 *
 * Compare two config_unnamed_s using the description.
 */
extern int config_unnamed_compare(const void *a, const void *b);

/*!
 * \brief         Free a named option
 * \param[in]  f  config_named_s pointer
 *
 * Frees the option object and the response storage handled for simple
 * string and list option types.
 */
extern void config_named_free(void *f);

/*!
 * \brief         Free a positional argument
 * \param[in]  f  config_unnamed_s pointer
 *
 * Frees the argument object and its parsed string response, when present.
 */
extern void config_unnamed_free(void *f);

#endif /* ! _COMMON_CONFIG_H_ */
