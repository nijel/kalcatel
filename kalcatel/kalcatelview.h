/*
 * kalcatelview.h
 *
 * code for displaying kalcateldoc
 *
 * Copyright (c) 2002 by Michal Cihar <cihar@email.cz>
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * In addition to GNU GPL this code may be used also in non GPL programs but
 * if and only if programmer/distributor of that code receives written
 * permission from author of this code.
 *
 */
/* $Id$ */

#ifndef KALCATELVIEW_H
#define KALCATELVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for Qt
//#include <qvbox.h>

#include <qwidget.h>
//#include <kjanuswidget.h>

#include "alcatool/alcatel.h"

/** Listing type, used for creting headers of list views
  */
enum AlcListType {
    alc_contacts,
    alc_contacts_sim,
    alc_contacts_mobile,
    alc_contacts_mobile_cat,
    alc_todos,
    alc_todos_cat,
    alc_calendar,
    alc_calls,
    alc_calls_type,
    alc_messages,
    alc_messages_in,
    alc_messages_out
    };

class KAlcatelDoc;
class AlcatelMessage;
class AlcatelContact;
class AlcatelCalendar;
class AlcatelTodo;
class AlcatelCall;

class QListViewItem;
class QSplitter;
class QWidgetStack;
class QLabel;
class QVBox;

class KListView;
class KTextBrowser;

class KHTMLPart;

class QGridLayout;

class KAlcatelTreeViewItem;

#define ID_KALCATEL         0
#define ID_TODOS            1
#define ID_CALENDAR         2
#define ID_CONTACTS         3
#define ID_CONTACTS_SIM     4
#define ID_CONTACTS_MOBILE  5
#define ID_CONTACTS_PC      6
#define ID_CALLS            7
#define ID_CALLS_OUTGOING   8
#define ID_CALLS_MISSED     9
#define ID_CALLS_RECEIVED   10
#define ID_MESSAGES         11
#define ID_MESSAGES_SENT    12
#define ID_MESSAGES_UNSENT  13
#define ID_MESSAGES_READ    14
#define ID_MESSAGES_UNREAD  15
#define ID_TODOS_CAT        1000
#define ID_CONTACTS_CAT     2000
#define ID_CONTACTS_PC_CAT  3000


/** The KAlcatelView class provides the view widget for the KAlcatelApp instance.	
 * The View instance inherits QWidget as a base class and represents the view object of a KTMainWindow. As KAlcatelView is part of the
 * docuement-view model, it needs a reference to the document object connected with it by the KAlcatelApp class to manipulate and display
 * the document structure provided by the KAlcatelDoc class.
 * 	
 * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
 * @version KDevelop version 0.4 code generation
 */
class KAlcatelView : public QWidget {
  Q_OBJECT
  public:
    /** Constructor for the main view */
    KAlcatelView(QWidget *parent = 0, const char *name=0);
    /** Destructor for the main view */
    ~KAlcatelView();

    /** returns a pointer to the document connected to the view instance. Mind that this method requires a KAlcatelApp instance as a parent
     * widget to get to the window document pointer by calling the KAlcatelApp::getDocument() method.
     *
     * @see KAlcatelApp#getDocument
     */
    KAlcatelDoc *getDocument() const;

    /** contains the implementation for printing functionality
    */
    void print(QPrinter *pPrinter);
  public slots:
    void repaint();

    void slotUrlClick(const QString &url);
    void slotMailClick(const QString &name, const QString &address);

    void slotTreeChanged(QListViewItem *item);
    void slotSetTitle( int num );

    void slotMessageChanged(QListViewItem *item);
    void slotTodoChanged(QListViewItem *item);
    void slotCalendarChanged(QListViewItem *item);
    void slotContactChanged(QListViewItem *item);
    void slotCallChanged(QListViewItem *item);

    void slotShowCall(AlcatelCall *what);
    void slotShowMessage(AlcatelMessage *what);
    void slotShowContact(AlcatelContact *what);
    void slotShowTodo(AlcatelTodo *what);
    void slotShowCalendar( AlcatelCalendar *what);
	
  private:
    QSplitter *vsplitter, *hsplitter;
    QVBox *listwidget;
    QWidgetStack *widgetstack;
    QLabel *titlelabel;
    KListView *tree;
    KTextBrowser *textview;

    KHTMLPart *kalcatel_html;
    QString *html_text;
    KTextBrowser *kalcatelview;

    KAlcatelTreeViewItem *kalcatel_item,
        *todo_item,
        *calendar_item,
        *contacts_item, *contacts_sim_item, *contacts_mobile_item, *contacts_pc_item,
        *calls_item, *calls_outgoing_item, *calls_missed_item, *calls_received_item,
        *messages_item, *messages_read_item, *messages_unread_item, *messages_sent_item, *messages_unsent_item;

    KListView *todo_list, *todo_cat_list[ALC_MAX_CATEGORIES],
        *calendar_list,
        *contacts_list, *contacts_sim_list, *contacts_mobile_list, *contacts_cat_list[ALC_MAX_CATEGORIES],
        *contacts_pc_list, *contacts_pc_cat_list[ALC_MAX_CATEGORIES],
        *calls_list, *calls_outgoing_list, *calls_missed_list, *calls_received_list,
        *messages_list, *messages_read_list, *messages_unread_list, *messages_sent_list, *messages_unsent_list;


    KListView *createListView(QWidget *parent, AlcListType type);

    int docVersion;
    int messagesVersion;
    int callsVersion;
    int contactsVersion;
    int calendarVersion;
    int todosVersion;

    QString KAlcatelView::makeURL(QString which);

};

#endif // KALCATELVIEW_H
