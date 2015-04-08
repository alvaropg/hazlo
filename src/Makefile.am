EXTRA_DIST =
CLEANFILES =

bin_PROGRAMS = hazlo

hazlo_SOURCES = \
	hzl-application.c \
	hzl-application.h \
	hzl-application-window.c \
	hzl-application-window.h \
	hzl-main.c \
	hzl-task.c \
	hzl-task.h \
	hzl-task-list-row.c \
	hzl-task-list-row.h \
	hzl-tasks-list.c \
	hzl-tasks-list.h \
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

# GResource

hazlo_SOURCES += resources.c

resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/hazlo.gresource.xml)
resources.c: hazlo.gresource.xml $(resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source --c-name hazlo $(srcdir)/hazlo.gresource.xml

EXTRA_DIST += hazlo.gresource.xml
CLEANFILES += resources.c

-include $(top_srcdir)/git.mk
