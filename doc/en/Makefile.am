####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = index.docbook index.cache.bz2 

install-data-local:
	$(mkinstalldirs) $(kde_docdir)/en/kalcatel/
	$(INSTALL_DATA) $(srcdir)/index.docbook $(kde_docdir)/en/kalcatel/index.docbook
	$(mkinstalldirs) $(kde_docdir)/en/kalcatel/
	$(INSTALL_DATA) $(srcdir)/index.cache.bz2 $(kde_docdir)/en/kalcatel/index.cache.bz2

uninstall-local:
	-rm -f $(kde_docdir)/en/kalcatel/index.docbook
	-rm -f $(kde_docdir)/en/kalcatel/index.cache.bz2

####### kdevelop will overwrite this part!!! (end)############

KDE_LANG = en
KDE_DOCS = kalcatel

