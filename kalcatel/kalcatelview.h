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
 * if and only if programmer/distributor of that code recieves written
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
#include <qvbox.h>

//#include <qwidget.h>
#include <kjanuswidget.h>

#include "alcatool/alcatel.h"

typedef enum {
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
    } AlcListType;

class KAlcatelDoc;
class KTextBrowser;
class AlcatelSMS;
class AlcatelContact;
class AlcatelCalendar;
class AlcatelTodo;

/** The KAlcatelView class provides the view widget for the KAlcatelApp instance.	
 * The View instance inherits QWidget as a base class and represents the view object of a KTMainWindow. As KAlcatelView is part of the
 * docuement-view model, it needs a reference to the document object connected with it by the KAlcatelApp class to manipulate and display
 * the document structure provided by the KAlcatelDoc class.
 * 	
 * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
 * @version KDevelop version 0.4 code generation
 */
class KAlcatelView : public KJanusWidget
{
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

    /** contains the implementation for printing functionality */
//    void print(QPrinter *pPrinter);
  public slots:
    void repaint();
    void slotMessageChanged(QListViewItem *item);
    void slotReadMessageChanged(QListViewItem *item);
    void slotUnreadMessageChanged(QListViewItem *item);
    void slotSentMessageChanged(QListViewItem *item);
    void slotUnsentMessageChanged(QListViewItem *item);
    void slotTodoChanged(QListViewItem *item);
    void slotTodoCatChanged(QListViewItem *item);
    void slotCalendarChanged(QListViewItem *item);
    void slotContactChanged(QListViewItem *item);
    void slotContactSimChanged(QListViewItem *item);
    void slotContactMobileChanged(QListViewItem *item);
    void slotContactMobileCatChanged(QListViewItem *item);
    void slotShowMessage(KTextBrowser *where, AlcatelSMS *what);
    void slotShowContact(KTextBrowser *where, AlcatelContact *what);
    void slotShowTodo(KTextBrowser *where, AlcatelTodo *what);
    void slotShowCalendar(KTextBrowser *where, AlcatelCalendar *what);
	
  private:
    QVBox *todo, *calendar,
        *contacts, *contacts_sim, *contacts_mobile,
        *calls, *calls_ld, *calls_mc, *calls_rc,
        *messages, *msg_read, *msg_unread, *msg_sent, *msg_unsent;
    KListView *todo_list, *todo_cat_list[ALC_MAX_CATEGORIES],
        *calendar_list,
        *contacts_list, *contacts_sim_list, *contacts_mobile_list, *contacts_cat_list[ALC_MAX_CATEGORIES],
        *calls_list, *calls_ld_list, *calls_mc_list, *calls_rc_list,
        *messages_list, *msg_read_list, *msg_unread_list, *msg_sent_list, *msg_unsent_list;

    KTextBrowser *message, *message_read, *message_unread, *message_sent, *message_unsent,
        *todo_view, *calendar_view, *todo_cat_view[ALC_MAX_CATEGORIES],
        *contact_view, *contact_sim_view, *contact_mobile_view, *contacts_cat_view[ALC_MAX_CATEGORIES];

    KListView *createListView(QWidget *parent, AlcListType type);

    int docVersion;
    int smsVersion;
    int callVersion;
    int contactVersion;
    int calendarVersion;
    int todoVersion;
};

#endif // KALCATELVIEW_H
