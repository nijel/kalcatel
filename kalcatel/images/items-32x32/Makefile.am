####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = configure.png mobile.png merge.png 

install-data-local:
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/actions/
	$(INSTALL_DATA) $(srcdir)/configure.png $(kde_icondir)/hicolor/32x32/actions/kalcatel-configure.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/actions/
	$(INSTALL_DATA) $(srcdir)/mobile.png $(kde_icondir)/hicolor/32x32/actions/kalcatel-mobile.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/actions/
	$(INSTALL_DATA) $(srcdir)/merge.png $(kde_icondir)/hicolor/32x32/actions/kalcatel-merge.png

uninstall-local:
	-rm -f $(kde_icondir)/hicolor/32x32/actions/kalcatel-configure.png
	-rm -f $(kde_icondir)/hicolor/32x32/actions/kalcatel-mobile.png
	-rm -f $(kde_icondir)/hicolor/32x32/actions/kalcatel-merge.png

####### kdevelop will overwrite this part!!! (end)############
