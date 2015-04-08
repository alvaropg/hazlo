resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/hazlo.gresource.xml)
hazlo.gresource: hazlo.gresource.xml $(resource_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) $<

resourcedir = $(pkgdatadir)
resource_DATA = hazlo.gresource

desktopdir = $(datadir)/applications
desktop_in_in_files = org.gnome.Hazlo.desktop.in.in
desktop_in_files = org.gnome.Hazlo.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

@INTLTOOL_XML_RULE@

EXTRA_DIST = \
	$(resource_files) \
	$(desktop_in_in_files) \
	$(NULL)

CLEANFILES = \
	hazlo.gresource \
	$(desktop_DATA) \
	$(NULL)

DISTCLEANFILES = \
	$(desktop_in_files) \
	$(NULL)

desktop-file-validate: $(desktop_DATA)
	for f in $^; do \
		$(DESKTOP_FILE_VALIDATE) $$f; \
	done

check-local: desktop-file-validate

.PHONY: desktop-file-validate

-include $(top_srcdir)/git.mk
