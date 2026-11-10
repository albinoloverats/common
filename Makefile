.PHONY: clean distclean test

APP          = test-app
UNIT_TEST    = test-unit

SRC          = test/test.c src/error.c src/mem.c src/list.c src/tlv.c src/map.c src/dir.c src/version.c src/config.c src/cli.c
UNIT_SRC     = $(wildcard test/test_*.c) src/error.c src/mem.c src/list.c src/tlv.c src/map.c src/ecc.c src/dir.c src/cli.c src/config.c src/ccrypt.c src/version.c
MISC         = src/misc.h

# -fsanitize=address -pg
CFLAGS      += -O0 -ggdb -Wall -Wextra -Werror -std=gnu99 -pipe -Wformat=2 -Wno-unused-result
CPPFLAGS    += -Isrc -D__DEBUG__ -DMALLOC_CHECK_=1 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DGIT_COMMIT=\"$(shell git log | head -n1 | cut -f2 -d' ')\" -DBUILD_OS=\"$(shell grep PRETTY_NAME /etc/os-release | cut -d= -f2)\"
TEST_CFLAGS  = ${CFLAGS} $(shell pkg-config --cflags cmocka libgcrypt libcurl) --coverage

LIBS         = -lpthread -lcurl
TEST_LIBS    = $(shell pkg-config --libs cmocka)
UNIT_LIBS    = ${TEST_LIBS} $(shell pkg-config --libs libgcrypt libcurl) -lpthread -lm

ifneq ($(shell uname -s),Darwin)
ALLOCATOR_WRAP = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=strdup -Wl,--wrap=strndup -Wl,--wrap=vasprintf
endif

GCOV         = $(shell ${CC} -print-prog-name=gcov)
COVERAGE     = coverage

default: gcc

all: gcc clang

gcc:
	 @echo "#define ALL_CFLAGS   \"$(strip $(subst \",\',"${CFLAGS} -Wrestrict  "))\""  > ${MISC}
	 @echo "#define ALL_CPPFLAGS \"$(strip $(subst \",\',"${CPPFLAGS}"))\"" >> ${MISC}
	 @gcc ${CFLAGS} -Wrestrict ${CPPFLAGS} ${SRC} ${LIBS} -o ${APP}-gcc
	-@echo -e "built ‘`echo -e ${SRC} | sed 's/ /’\n      ‘/g'`’ → ‘${APP}-gcc’"

clang:
	 @echo "#define ALL_CFLAGS   \"$(strip $(subst \",\',"${CFLAGS}  "))\""  > ${MISC}
	 @echo "#define ALL_CPPFLAGS \"$(strip $(subst \",\',"${CPPFLAGS}"))\"" >> ${MISC}
	 @clang  ${CFLAGS} ${CPPFLAGS} ${SRC} ${LIBS} -o ${APP}-clang
	-@echo -e "built ‘`echo -e ${SRC} | sed 's/ /’\n      ‘/g'`’ → ‘${APP}-clang’"

test:
	 @rm -f test-unit-*.gcda test-unit-*.gcno *.gcov
	 @rm -fvr ${COVERAGE}
	 @${CC} ${TEST_CFLAGS} ${CPPFLAGS} ${UNIT_SRC} ${UNIT_LIBS} ${ALLOCATOR_WRAP} -o ${UNIT_TEST}
	-@echo -e "built ‘`echo -e ${UNIT_SRC} | sed 's/ /’\n      ‘/g'`’ → ‘${UNIT_TEST}’"
ifneq ($(strip ${CMOCKA_XML_DIR}),)
	 @mkdir -p "${CMOCKA_XML_DIR}"
	 @CMOCKA_MESSAGE_OUTPUT=XML CMOCKA_XML_FILE="$(abspath ${CMOCKA_XML_DIR})/common.xml" ./${UNIT_TEST}
else
	 @./${UNIT_TEST}
endif
	 @mkdir -p ${COVERAGE}
	 @gcovr --root . --gcov-executable "${GCOV}" --filter 'src/' --xml-pretty --output ${COVERAGE}/coverage.xml --html-details ${COVERAGE}/index.html --print-summary

#extra:
#	# -fanalyzer-verbosity=4
#	 @echo "#define ALL_CFLAGS   \"$(strip $(subst \",\',"${CFLAGS}  "))\""  > ${MISC}
#	 @echo "#define ALL_CPPFLAGS \"$(strip $(subst \",\',"${CPPFLAGS}"))\"" >> ${MISC}
#	 @${CC} -fanalyzer ${CFLAGS} ${CPPFLAGS} ${SRC} ${LIBS} -o ${APP}
#	-@echo -e "built ‘`echo -e ${SRC} | sed 's/ /’\n      ‘/g'`’ → ‘${APP}’"

clean:
	 @rm -fv ${MISC}
	 @rm -fv gmon.out ${UNIT_TEST} *.gcda *.gcno *.gcov
	 @rm -fvr ${COVERAGE}

distclean: clean
	 @rm -fv ${APP}-gcc ${APP}-clang
