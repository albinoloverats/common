.PHONY: clean distclean

APP       = test-app

SRC       = test/test.c src/error.c src/list.c src/tlv.c src/map.c src/dir.c src/version.c src/config.c src/cli.c
MISC      = src/misc.h

# -fsanitize=address
CFLAGS   += -O0 -ggdb -pg -Wall -Wextra -Werror -std=gnu99 -pipe -Wformat=2 -Wno-unused-result
CPPFLAGS += -Isrc -D__DEBUG__ -DMALLOC_CHECK_=1 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DGIT_COMMIT=\"$(shell git log | head -n1 | cut -f2 -d' ')\" -DBUILD_OS=\"$(shell grep PRETTY_NAME /etc/os-release | cut -d= -f2)\"

LIBS      = -lpthread -lcurl

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

#extra:
#	# -fanalyzer-verbosity=4
#	 @echo "#define ALL_CFLAGS   \"$(strip $(subst \",\',"${CFLAGS}  "))\""  > ${MISC}
#	 @echo "#define ALL_CPPFLAGS \"$(strip $(subst \",\',"${CPPFLAGS}"))\"" >> ${MISC}
#	 @${CC} -fanalyzer ${CFLAGS} ${CPPFLAGS} ${SRC} ${LIBS} -o ${APP}
#	-@echo -e "built ‘`echo -e ${SRC} | sed 's/ /’\n      ‘/g'`’ → ‘${APP}’"

clean:
	 @rm -fv ${MISC}
	 @rm -fv gmon.out

distclean: clean
	 @rm -fv ${APP}-gcc ${APP}-clang
