####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = index.docbook screenshot.png 

install-data-local:
	$(mkinstalldirs) $(kde_htmldir)/en/kalcatel/
	$(INSTALL_DATA) $(srcdir)/index.docbook $(kde_htmldir)/en/kalcatel/index.docbook
	$(mkinstalldirs) $(kde_htmldir)/en/kalcatel/
	$(INSTALL_DATA) $(srcdir)/index.cache.bz2 $(kde_htmldir)/en/kalcatel/index.cache.bz2
	$(mkinstalldirs) $(kde_htmldir)/en/kalcatel/
	$(INSTALL_DATA) $(srcdir)/screenshot.png $(kde_htmldir)/en/kalcatel/screenshot.png

uninstall-local:
	-rm -f $(kde_htmldir)/en/kalcatel/index.docbook
	-rm -f $(kde_htmldir)/en/kalcatel/index.cache.bz2
	-rm -f $(kde_htmldir)/en/kalcatel/screenshot.png

####### kdevelop will overwrite this part!!! (end)############

KDE_LANG = en
KDE_DOCS = kalcatel
