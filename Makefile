.PHONY: clean distclean test coverage

APP       = test-app
UNIT_TEST = test-unit

SRC       = test/test.c src/error.c src/mem.c src/list.c src/tlv.c src/map.c src/dir.c src/version.c src/config.c src/cli.c
UNIT_SRC  = $(wildcard test/test_*.c) src/error.c src/mem.c src/list.c src/tlv.c src/map.c src/ecc.c src/dir.c src/cli.c src/config.c src/ccrypt.c src/version.c
MISC      = src/misc.h

# -fsanitize=address -pg
CFLAGS   += -O0 -ggdb -Wall -Wextra -Werror -std=gnu99 -pipe -Wformat=2 -Wno-unused-result
CPPFLAGS += -Isrc -D__DEBUG__ -DMALLOC_CHECK_=1 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DGIT_COMMIT=\"$(shell git log | head -n1 | cut -f2 -d' ')\" -DBUILD_OS=\"$(shell grep PRETTY_NAME /etc/os-release | cut -d= -f2)\"

LIBS      = -lpthread -lcurl
TEST_LIBS = $(shell pkg-config --libs cmocka)
UNIT_LIBS = ${TEST_LIBS} $(shell pkg-config --libs libgcrypt libcurl) -lpthread -lm

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
	 @gcc $(CFLAGS) $(CPPFLAGS) $(shell pkg-config --cflags cmocka libgcrypt libcurl) ${UNIT_SRC} ${UNIT_LIBS} -o ${UNIT_TEST}
	 @./${UNIT_TEST}

coverage:
	 @rm -f test-unit-*.gcda test-unit-*.gcno *.gcov
	 @gcc $(CFLAGS) $(CPPFLAGS) $(shell pkg-config --cflags cmocka libgcrypt libcurl) --coverage ${UNIT_SRC} ${UNIT_LIBS} -o ${UNIT_TEST}
	 @./${UNIT_TEST}
	 @gcov -r -b -c test-unit-error.gcno test-unit-mem.gcno test-unit-list.gcno test-unit-tlv.gcno test-unit-map.gcno test-unit-ecc.gcno test-unit-dir.gcno test-unit-cli.gcno test-unit-config.gcno test-unit-ccrypt.gcno test-unit-version.gcno

#extra:
#	# -fanalyzer-verbosity=4
#	 @echo "#define ALL_CFLAGS   \"$(strip $(subst \",\',"${CFLAGS}  "))\""  > ${MISC}
#	 @echo "#define ALL_CPPFLAGS \"$(strip $(subst \",\',"${CPPFLAGS}"))\"" >> ${MISC}
#	 @${CC} -fanalyzer ${CFLAGS} ${CPPFLAGS} ${SRC} ${LIBS} -o ${APP}
#	-@echo -e "built ‘`echo -e ${SRC} | sed 's/ /’\n      ‘/g'`’ → ‘${APP}’"

clean:
	 @rm -fv ${MISC}
	 @rm -fv gmon.out ${UNIT_TEST} *.gcda *.gcno *.gcov

distclean: clean
	 @rm -fv ${APP}-gcc ${APP}-clang
