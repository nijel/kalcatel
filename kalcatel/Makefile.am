####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kalcatel

## INCLUDES were found outside kdevelop specific part

kalcatel_SOURCES = selectcontactdialog.cpp timevalidator.cpp edittododialog.cpp kalcatellistview.cpp phonenumbervalidator.cpp editcontactdialog.cpp signalled.cpp signallabel.cpp editmessagedialog.cpp kalcatelmergedialog.cpp kalcatelviewitem.cpp kalcateltreeviewitem.cpp kalcatelconfigdialog.cpp alcatelclasses.cpp kalcatelview.cpp kalcateldoc.cpp kalcatel.cpp main.cpp 
kalcatel_LDADD   = ./alcatool/libalcatool.a  $(LIB_KFILE) $(LIB_KHTML) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = alcatool images info 

EXTRA_DIST = main.cpp kalcatel.cpp kalcatel.h kalcateldoc.cpp kalcateldoc.h kalcatelview.cpp kalcatelview.h kalcatelui.rc kalcatel.desktop alcatelclasses.cpp alcatelclasses.h kalcatelconfigdialog.cpp kalcatelconfigdialog.h kalcateltreeviewitem.cpp kalcateltreeviewitem.h kalcatelviewitem.cpp kalcatelviewitem.h kalcatelmergedialog.cpp kalcatelmergedialog.h editmessagedialog.cpp editmessagedialog.h signallabel.cpp signallabel.h signalled.cpp signalled.h editcontactdialog.cpp editcontactdialog.h phonenumbervalidator.cpp phonenumbervalidator.h kalcatellistview.cpp kalcatellistview.h edittododialog.cpp edittododialog.h timevalidator.cpp timevalidator.h selectcontactdialog.cpp selectcontactdialog.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/kalcatel.desktop $(kde_appsdir)/Applications/kalcatel.desktop

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/kalcatel.desktop

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
kalcatel_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/kalcatel
rc_DATA = kalcatelui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kalcatel.pot; \
	fi

