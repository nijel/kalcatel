####### kdevelop will overwrite this part!!! (begin)##########

SUBDIRS = items-16x16 items-22x22 items-32x32 battery signal 

EXTRA_DIST = icon-16x16.png icon-22x22.png icon-32x32.png icon-48x48.png 

install-data-local:
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/icon-16x16.png $(kde_icondir)/hicolor/16x16/apps/kalcatel.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/22x22/apps/
	$(INSTALL_DATA) $(srcdir)/icon-22x22.png $(kde_icondir)/hicolor/22x22/apps/kalcatel.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/icon-32x32.png $(kde_icondir)/hicolor/32x32/apps/kalcatel.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/48x48/apps/
	$(INSTALL_DATA) $(srcdir)/icon-48x48.png $(kde_icondir)/hicolor/48x48/apps/kalcatel.png

uninstall-local:
	-rm -f $(kde_icondir)/hicolor/16x16/apps/kalcatel.png
	-rm -f $(kde_icondir)/hicolor/22x22/apps/kalcatel.png
	-rm -f $(kde_icondir)/hicolor/32x32/apps/kalcatel.png
	-rm -f $(kde_icondir)/hicolor/48x48/apps/kalcatel.png

####### kdevelop will overwrite this part!!! (end)############
