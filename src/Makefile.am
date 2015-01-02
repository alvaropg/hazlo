bin_PROGRAMS = hazlo

hazlo_SOURCES = \
	hzl-application.c \
	hzl-application.h \
	hzl-application-window.c \
	hzl-application-window.h \
	hzl-main.c \
	hzl-task.c \
	hzl-task.h \
	hzl-utils.c \
	hzl-utils-h \
	$(NULL)

hazlo_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	-DPACKAGE_DATA_DIR=\""${datadir}"\" \
	$(AM_CPPFLAGS) \
	$(NULL)

hazlo_CFLAGS = \
	$(WARN_CFLAGS) \
	$(HZL_MODULES_CFLAGS) \
	$(AM_CFLAGS) \
	$(NULL)

hazlo_LDFLAGS =

hazlo_LDADD = \
	$(HZL_MODULES_LIBS) \
	$(NULL)

-include $(top_srcdir)/git.mk
