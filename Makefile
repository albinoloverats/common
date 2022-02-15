.PHONY: clean distclean

APP       = test-app

SRC       = test/test.c src/error.c src/list.c src/tlv.c src/version.c src/config.c src/cli.c
MISC      = misc.h

CFLAGS   += -O0 -ggdb -Wall -Wextra -std=gnu99 -pipe -Wrestrict -Wformat=2 -Wno-unused-result
CPPFLAGS += -Isrc -D__DEBUG__ -DMALLOC_CHECK_=1 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DGIT_COMMIT=\"none\" -DBUILD_OS=\"$(shell grep PRETTY_NAME /etc/os-release | cut -d= -f2)\"

LIBS      = -lpthread -lcurl

all:
	 @echo "#define ALL_CFLAGS   \"$(strip $(subst \",\',"${CFLAGS}  "))\""  > ${MISC}
	 @echo "#define ALL_CPPFLAGS \"$(strip $(subst \",\',"${CPPFLAGS}"))\"" >> ${MISC}
	 @${CC} ${CFLAGS} ${CPPFLAGS} ${SRC} ${LIBS} -o ${APP}
	-@echo -e "built ‘`echo -e ${SRC} | sed 's/ /’\n      ‘/g'`’ → ‘${APP}’"

clean:
	 @rm -fv ${MISC}
	 @rm -fv gmon.out

distclean: clean
	 @rm -fv ${APP}
