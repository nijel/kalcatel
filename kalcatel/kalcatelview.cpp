/*
 * kalcatelview.cpp
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

// include files for Qt
#include <qprinter.h>
#include <qpainter.h>
#include <qsplitter.h>
#include <qwidgetstack.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qheader.h>
#include <qlistview.h>
#include <qfont.h>
#include <qstring.h>
#include <qgrid.h>

#include <qtextstream.h>
#include <qfile.h>

// include files for KDE
#include <klistview.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <ktextbrowser.h>

#include <kapp.h>

#include <kstddirs.h>

#include <khtml_part.h>
#include <khtmlview.h>

// application specific includes
#include "kalcatelview.h"
#include "kalcateldoc.h"
#include "kalcatel.h"
#include "kalcateltreeviewitem.h"
#include "kalcatelviewitem.h"
#include "alcatelclasses.h"
#include "alcatool/sms.h"
#include "alcatool/logging.h"

KAlcatelView::KAlcatelView(QWidget *parent, const char *name) : QWidget(parent, name) {
    int i;

    QGridLayout *layout = new QGridLayout( this );

    hsplitter = new QSplitter( Qt::Horizontal, this );
    layout->addWidget( hsplitter, 0, 0);
    tree = new KListView( hsplitter );
    tree->setSorting(-1);
    tree->addColumn(QString());
    tree->header()->hide();

    connect( tree, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotTreeChanged(QListViewItem *) ) );

    vsplitter = new QSplitter( Qt::Vertical, hsplitter );

    listwidget = new QVBox( vsplitter);

    titlelabel = new QLabel( QString(), listwidget );
    QFont font = titlelabel->font();
    font.setBold(true);
    titlelabel->setFont(font);
    titlelabel->setMargin(3);
    listwidget->setStretchFactor( titlelabel, -1);

    QFrame *line = new QFrame( listwidget );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    listwidget->setStretchFactor( line, -1);

    widgetstack = new QWidgetStack( listwidget );
    widgetstack->setMargin(3);
    connect(widgetstack, SIGNAL(aboutToShow ( int )), this, SLOT( slotSetTitle( int )));
    listwidget->setStretchFactor( widgetstack, 100);

    textview = new KTextBrowser( vsplitter );
    vsplitter->setResizeMode( textview, QSplitter::FollowSizeHint );
    textview->setBackgroundMode( PaletteBase );

    for (i=0; i<ALC_MAX_CATEGORIES; i++) {
        contacts_cat_list[i] =  NULL;
        todo_cat_list[i] =  NULL;
    }

    kalcatel_item = new KAlcatelTreeViewItem(tree, i18n("KAlcatel"), SmallIcon("kalcatel-mobile.png"), ID_KALCATEL );

    kalcatel_html = new KHTMLPart(widgetstack);

    QString html = KApplication::kApplication()->dirs()->findResource("data", "kalcatel/info/index.html");

    if (html.isNull()) {
        kalcatel_html->begin();
        kalcatel_html->write(QString("<b>%1</b>").arg(i18n("Template for document info view not found!")));
        kalcatel_html->end();
    } else {
        QString html_dir = KApplication::kApplication()->dirs()->findResourceDir("data", "kalcatel/info/index.html");
        html_dir.append("kalcatel/info/");
        QFile file(html);
        file.open(IO_ReadOnly);
        html_text = new QString(QTextStream(&file).read());
        file.close();
        html_text->replace( QRegExp("%URL%"), html_dir);
    }

    kalcatel_html->view()->repaint();

    widgetstack->addWidget( kalcatel_html->view(), ID_KALCATEL );

    widgetstack->addWidget( messages_list = createListView( widgetstack, alc_messages ), ID_MESSAGES );
    connect( messages_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotMessageChanged(QListViewItem *) ) );
    messages_item = new KAlcatelTreeViewItem(kalcatel_item, i18n("Messages"), SmallIcon("kalcatel-message.png"), ID_MESSAGES );

    widgetstack->addWidget( messages_unsent_list = createListView( widgetstack, alc_messages_out ), ID_MESSAGES_UNSENT );
    connect( messages_unsent_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotMessageChanged(QListViewItem *) ) );
    messages_unsent_item = new KAlcatelTreeViewItem(messages_item, i18n("Unsent"), SmallIcon("kalcatel-message-unsent.png"), ID_MESSAGES_UNSENT );

    widgetstack->addWidget( messages_sent_list = createListView( widgetstack, alc_messages_out ), ID_MESSAGES_SENT );
    connect( messages_sent_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotMessageChanged(QListViewItem *) ) );
    messages_sent_item = new KAlcatelTreeViewItem(messages_item, i18n("Sent"), SmallIcon("kalcatel-message-sent.png"), ID_MESSAGES_SENT );

    widgetstack->addWidget( messages_read_list = createListView( widgetstack, alc_messages_in ), ID_MESSAGES_READ );
    connect( messages_read_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotMessageChanged(QListViewItem *) ) );
    messages_read_item = new KAlcatelTreeViewItem(messages_item, i18n("Read"), SmallIcon("kalcatel-message-read.png"), ID_MESSAGES_READ );

    widgetstack->addWidget( messages_unread_list = createListView( widgetstack, alc_messages_in ), ID_MESSAGES_UNREAD );
    connect( messages_unread_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotMessageChanged(QListViewItem *) ) );
    messages_unread_item = new KAlcatelTreeViewItem(messages_item, i18n("Unread"), SmallIcon("kalcatel-message-unread.png"), ID_MESSAGES_UNREAD );

    widgetstack->addWidget( calls_list = createListView( widgetstack, alc_calls ), ID_CALLS );
    connect( calls_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotCallChanged(QListViewItem *) ) );
    calls_item = new KAlcatelTreeViewItem(kalcatel_item, i18n("Calls"), SmallIcon("kalcatel-call.png"), ID_CALLS );

    widgetstack->addWidget( calls_outgoing_list = createListView( widgetstack, alc_calls_type ), ID_CALLS_OUTGOING );
    connect( calls_outgoing_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotCallChanged(QListViewItem *) ) );
    calls_outgoing_item = new KAlcatelTreeViewItem(calls_item, i18n("Outgoing"), SmallIcon("kalcatel-call-outgoing.png"), ID_CALLS_OUTGOING );

    widgetstack->addWidget( calls_received_list = createListView( widgetstack, alc_calls_type ), ID_CALLS_RECEIVED );
    connect( calls_received_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotCallChanged(QListViewItem *) ) );
    calls_received_item = new KAlcatelTreeViewItem(calls_item, i18n("Received"), SmallIcon("kalcatel-call-received.png"), ID_CALLS_RECEIVED );

    widgetstack->addWidget( calls_missed_list = createListView( widgetstack, alc_calls_type ), ID_CALLS_MISSED );
    connect( calls_missed_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotCallChanged(QListViewItem *) ) );
    calls_missed_item = new KAlcatelTreeViewItem(calls_item, i18n("Missed"), SmallIcon("kalcatel-call-missed.png"), ID_CALLS_MISSED );

    widgetstack->addWidget( contacts_list = createListView( widgetstack, alc_contacts ), ID_CONTACTS );
    connect( contacts_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactChanged(QListViewItem *) ) );
    contacts_item = new KAlcatelTreeViewItem(kalcatel_item, i18n("Contacts"), SmallIcon("kalcatel-contact.png"), ID_CONTACTS );

    widgetstack->addWidget( contacts_mobile_list = createListView( widgetstack, alc_contacts_mobile ), ID_CONTACTS_MOBILE );
    connect( contacts_mobile_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactChanged(QListViewItem *) ) );
    contacts_mobile_item = new KAlcatelTreeViewItem(contacts_item, i18n("Mobile"), SmallIcon("kalcatel-contact-mobile.png"), ID_CONTACTS_MOBILE );

    widgetstack->addWidget( contacts_sim_list = createListView( widgetstack, alc_contacts_sim ), ID_CONTACTS_SIM );
    connect( contacts_sim_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactChanged(QListViewItem *) ) );
    contacts_sim_item = new KAlcatelTreeViewItem(contacts_item, i18n("SIM"), SmallIcon("kalcatel-contact-sim.png"), ID_CONTACTS_SIM );

    widgetstack->addWidget( calendar_list = createListView( widgetstack, alc_calendar ), ID_CALENDAR );
    connect( calendar_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotCalendarChanged(QListViewItem *) ) );
    calendar_item = new KAlcatelTreeViewItem(kalcatel_item, i18n("Calendar"), SmallIcon("kalcatel-calendar.png"), ID_CALENDAR );

    widgetstack->addWidget( todo_list = createListView( widgetstack, alc_todos ), ID_TODOS );
    connect( todo_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotTodoChanged(QListViewItem *) ) );
    todo_item = new KAlcatelTreeViewItem(kalcatel_item, i18n("Todos"), SmallIcon("kalcatel-todo.png"), ID_TODOS );

    repaint();

    /* just force to show view after init correctly */
    tree->setCurrentItem(kalcatel_item);
    slotTreeChanged(kalcatel_item);
    slotSetTitle(ID_KALCATEL);
}

KAlcatelView::~KAlcatelView() {
}

KListView *KAlcatelView::createListView(QWidget *parent, AlcListType type) {
    KListView *list;
    list = new KListView(parent);

    list->setAllColumnsShowFocus(true);
    list->setShowSortIndicator(true);

    switch (type) {
        case alc_contacts:
        case alc_contacts_sim:
            list->addColumn(i18n("Name"));
            list->addColumn(i18n("Phone"));
            list->addColumn(i18n("Position"));
            break;
        case alc_contacts_mobile:
            list->addColumn(i18n("Last name"));
            list->addColumn(i18n("First name"));
            list->addColumn(i18n("Category"));
            list->addColumn(i18n("Mobile"));
            list->addColumn(i18n("Work"));
            list->addColumn(i18n("Main"));
            list->addColumn(i18n("Email"));
            list->addColumn(i18n("Position"));
            break;
        case alc_contacts_mobile_cat:
            list->addColumn(i18n("Last name"));
            list->addColumn(i18n("First name"));
            list->addColumn(i18n("Mobile"));
            list->addColumn(i18n("Work"));
            list->addColumn(i18n("Main"));
            list->addColumn(i18n("Email"));
            list->addColumn(i18n("Position"));
            break;
        case alc_todos:
            list->addColumn(i18n("Completed"));
            list->addColumn(i18n("Priority"));
            list->addColumn(i18n("Due date"));
            list->addColumn(i18n("Subject"));
            list->addColumn(i18n("Category"));
            list->addColumn(i18n("Position"));
            break;
        case alc_todos_cat:
            list->addColumn(i18n("Completed"));
            list->addColumn(i18n("Priority"));
            list->addColumn(i18n("Due date"));
            list->addColumn(i18n("Subject"));
            list->addColumn(i18n("Position"));
            break;
        case alc_calendar:
            list->addColumn(i18n("Date"));
            list->addColumn(i18n("Start"));
            list->addColumn(i18n("End"));
            list->addColumn(i18n("Type"));
            list->addColumn(i18n("Subject"));
            list->addColumn(i18n("Alarm"));
            list->addColumn(i18n("Repeat"));
            list->addColumn(i18n("Position"));
            break;
        case alc_calls_type:
            list->addColumn(i18n("Number"));
            list->addColumn(i18n("Name"));
            list->addColumn(i18n("Contact"));
            list->addColumn(i18n("Position"));
            break;
        case alc_calls:
            list->addColumn(i18n("Number"));
            list->addColumn(i18n("Name"));
            list->addColumn(i18n("Contact"));
            list->addColumn(i18n("Type"));
            list->addColumn(i18n("Position"));
            break;
        case alc_messages:
            list->addColumn(i18n("Number"));
            list->addColumn(i18n("Name"));
            list->addColumn(i18n("Status"));
            list->addColumn(i18n("Date"));
            list->addColumn(i18n("Time"));
            list->addColumn(i18n("Text"));
            list->addColumn(i18n("Position"));
            break;
        case alc_messages_in:
            list->addColumn(i18n("From"));
            list->addColumn(i18n("From (name)"));
            list->addColumn(i18n("Date"));
            list->addColumn(i18n("Time"));
            list->addColumn(i18n("Text"));
            list->addColumn(i18n("Position"));
            break;
        case alc_messages_out:
            list->addColumn(i18n("To"));
            list->addColumn(i18n("To (name)"));
            list->addColumn(i18n("Date"));
            list->addColumn(i18n("Time"));
            list->addColumn(i18n("Text"));
            list->addColumn(i18n("Position"));
            break;
    }

    return list;
}

KAlcatelDoc *KAlcatelView::getDocument() const {
    KAlcatelApp *theApp=(KAlcatelApp *) parentWidget();
    return theApp->getDocument();
}

void KAlcatelView::repaint() {
    KAlcatelDoc *doc = getDocument();
    int unread_sms = 0;
    if (doc->getVersion() != docVersion) {
        docVersion = doc->getVersion();

        if (doc->getMessagesVersion() != messagesVersion || doc->getContactsVersion() != contactsVersion) {
            /* we should update this also when contacts change, because there are names of sender */
            messagesVersion = doc->getMessagesVersion();
            AlcatelMessageList::Iterator it;
            KListView *list;
            QString type;

            messages_list->clear();
            messages_unread_list->clear();
            messages_read_list->clear();
            messages_unsent_list->clear();
            messages_sent_list->clear();

            for( it = doc->messages->begin(); it != doc->messages->end(); ++it ) {
                type =  MessageTypes[(* it).Status];
                switch ((* it).Status) {
                    case MESSAGE_UNREAD:
                        list = messages_unread_list;
                        unread_sms++;
                        break;
                    case MESSAGE_READ:
                        list = messages_read_list;
                        break;
                    case MESSAGE_UNSENT:
                        list = messages_unsent_list;
                        break;
                    case MESSAGE_SENT:
                        list = messages_sent_list;
                        break;
                }
                AlcatelContact *cont = getContactByPhone(doc->contacts, &((* it).Sender), &(((KAlcatelApp *)parent())->phone_prefix));
                new KAlcatelMessageViewItem (messages_list, &(* it),
                        QString((* it).Sender),
                        cont == NULL? i18n("Unknown") : cont->Name(),
                        type,
                        (* it).Date.date().toString(),
                        (* it).Date.time().toString(),
                        QString((* it).Text),
                        QString("%1").arg((* it).Id));

                new KAlcatelMessageCatViewItem (list, &(* it),
                        QString((* it).Sender),
                        cont == NULL? i18n("Unknown") : cont->Name(),
                        (* it).Date.date().toString(),
                        (* it).Date.time().toString(),
                        QString((* it).Text),
                        QString("%1").arg((* it).Id));
            } /* for cycle over sms */
            if (unread_sms)
                tree->setCurrentItem(messages_unread_item);
            else
                tree->setCurrentItem(messages_item);
        } /* change in sms */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (doc->getTodosVersion() != todosVersion|| doc->getContactsVersion() != contactsVersion) {
            /* we should update this also when contacts change, because there are names of contact */
            todosVersion = doc->getTodosVersion();
            AlcatelCategoryList::Iterator c_it;
            int i;

            todo_list->clear();

            for (i=0; i<ALC_MAX_CATEGORIES; i++)
                if (todo_cat_list[i] !=  NULL) {
                    widgetstack->removeWidget(todo_cat_list[i]);
                    todo_cat_list[i] = NULL;
                }

            QListViewItem * nextChild = todo_item->firstChild();
            QListViewItem * myChild;
            while( (myChild=nextChild)!=NULL ) {
                nextChild = myChild->nextSibling();
                ((KAlcatelTreeViewItem *)myChild)->~KAlcatelTreeViewItem();
            }

            for( c_it = doc->todo_cats->begin(); c_it != doc->todo_cats->end(); ++c_it ) {
                widgetstack->addWidget(todo_cat_list[(*c_it).Id] = createListView(widgetstack, alc_todos_cat), ID_TODOS_CAT + (*c_it).Id );
                new KAlcatelTreeViewItem(todo_item, (*c_it).Name, SmallIcon("kalcatel-todo.png"), ID_TODOS_CAT + (*c_it).Id );
                connect( todo_cat_list[(*c_it).Id], SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotTodoChanged(QListViewItem *) ) );
                ::message(MSG_DEBUG, "Created category listview %d", (*c_it).Id);
            }

            AlcatelTodoList::Iterator it;
            for( it = doc->todos->begin(); it != doc->todos->end(); ++it ) {
                QString catname;
                if ((* it).Category >= 0) {
                    if ((todo_cat_list[(* it).Category] != NULL) && ((* it).Category < ALC_MAX_CATEGORIES)) {
                        new KAlcatelTodoCatViewItem (todo_cat_list[(* it).Category], &(*it),
                                (* it).Completed == -1 ? QString(""): (* it).Completed ? i18n("Yes") : i18n("No"),
                                (* it).Priority == -1 ? QString("") : Priorities[(* it).Priority],
                                (* it).DueDate.isNull()?i18n("None"):(* it).DueDate.toString(),
                                (* it).Subject,
                                QString("%1").arg((* it).Id));
                    } else {
                        ::message(MSG_WARNING, "Can not insert to category list (%d)", (* it).Category);
                    }

                    AlcatelCategory *cat = getCategoryById(doc->todo_cats, (*it).Category, StorageMobile);
                    if (cat == NULL) {
                        catname = i18n("Unknown (id=%1)").arg((* it).Category);
                    } else {
                        catname = cat->Name;
                    }
                } else {
                    catname = i18n("None");
                }

                new KAlcatelTodoViewItem (todo_list, &(*it),
                        (* it).Completed == -1 ? QString(""): (* it).Completed ? i18n("Yes") : i18n("No"),
                        (* it).Priority == -1 ? QString("") : Priorities[(* it).Priority],
                        (* it).DueDate.isNull()?i18n("None"):(* it).DueDate.toString(),
                        (* it).Subject,
                        catname,
                        QString("%1").arg((* it).Id));

            } /* for cycle over todos */

            tree->setCurrentItem(todo_item);
        } /* change in todos */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (doc->getCalendarVersion() != calendarVersion || doc->getContactsVersion() != contactsVersion) {
            /* we should update this also when contacts change, because there are names of contact */
            calendarVersion = doc->getCalendarVersion();

            calendar_list->clear();

            AlcatelCalendarList::Iterator it;
            for( it = doc->calendar->begin(); it != doc->calendar->end(); ++it ) {
                new KAlcatelCalendarViewItem (calendar_list, &(*it),
                        (* it).EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : (* it).Date.isNull()?i18n("None"):(* it).Date.toString(),
                        (* it).EventType == ALC_CALENDAR_BIRTHDAY || (* it).EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : (* it).StartTime.toString(),
                        (* it).EventType == ALC_CALENDAR_BIRTHDAY || (* it).EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : (* it).EndTime.toString(),
                        (* it).EventType!=-1?CalendarTypes[(* it).EventType]:i18n("Unknown"),
                        (* it).Subject,
                        (* it).Alarm.isNull()?i18n("None"):(* it).Alarm.toString(),
                        (* it).Repeating(),
                        QString("%1").arg((* it).Id));
            } /* for cycle over items */

            tree->setCurrentItem(calendar_item);
        } /* change in calendar */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (doc->getCallsVersion() != callsVersion || doc->getContactsVersion() != contactsVersion) {
            /* we should update this also when contacts change, because there are names of contact */
             int missed = 0;
            callsVersion = doc->getCallsVersion();

            calls_list->clear();
            calls_outgoing_list->clear();
            calls_missed_list->clear();
            calls_received_list->clear();

            AlcatelCallList::Iterator it;
            for( it = doc->calls->begin(); it != doc->calls->end(); ++it ) {
                AlcatelContact *cont = getContactByPhone(doc->contacts, &((* it).Number), &(((KAlcatelApp *)parent())->phone_prefix));
                new KAlcatelCallViewItem (calls_list, &(* it),
                        (* it).Number,
                        (* it).Name,
                        cont == NULL? QString("") : cont->Name(),
                        CallTypes[(* it).Type],
                        QString("%1").arg((* it).Id));
                switch ((* it).Type) {
                    case CallDialled:
                        new KAlcatelCallCatViewItem (calls_outgoing_list, &(* it),
                                (* it).Number,
                                (* it).Name,
                                cont == NULL? QString("") : cont->Name(),
                                QString("%1").arg((* it).Id));
                        break;
                    case CallMissed:
                        missed++;
                        new KAlcatelCallCatViewItem (calls_missed_list, &(* it),
                                (* it).Number,
                                (* it).Name,
                                cont == NULL? QString("") : cont->Name(),
                                QString("%1").arg((* it).Id));
                        break;
                    case CallReceived:
                        new KAlcatelCallCatViewItem (calls_received_list, &(* it),
                                (* it).Number,
                                (* it).Name,
                                cont == NULL? QString("") : cont->Name(),
                                QString("%1").arg((* it).Id));
                        break;
                }
            } /* for cycle over calls */

            if (missed>0)
                tree->setCurrentItem(calls_missed_item);
            else
                tree->setCurrentItem(calls_item);
        } /* change in calls */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (doc->getContactsVersion() != contactsVersion) {
            contactsVersion = doc->getContactsVersion();
            AlcatelCategoryList::Iterator c_it;
            int i;

            contacts_list->clear();
            contacts_sim_list->clear();
            contacts_mobile_list->clear();

            for (i=0; i<ALC_MAX_CATEGORIES; i++)
                if (contacts_cat_list[i] !=  NULL) {
                    widgetstack->removeWidget(todo_cat_list[i]);
                    contacts_cat_list[i] = NULL;
                }

            QListViewItem * nextChild = contacts_mobile_item->firstChild();
            QListViewItem * myChild;
            while( (myChild=nextChild)!=NULL ) {
                nextChild = myChild->nextSibling();
                ((KAlcatelTreeViewItem *)myChild)->~KAlcatelTreeViewItem();
            }

            for( c_it = doc->contact_cats->begin(); c_it != doc->contact_cats->end(); ++c_it ) {
                widgetstack->addWidget(contacts_cat_list[(*c_it).Id] = createListView(widgetstack, alc_contacts_mobile_cat), ID_CONTACTS_CAT + (*c_it).Id );
                new KAlcatelTreeViewItem(contacts_mobile_item, (*c_it).Name, SmallIcon("kalcatel-contact-mobile.png"), ID_CONTACTS_CAT + (*c_it).Id );
                connect( contacts_cat_list[(*c_it).Id], SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactChanged(QListViewItem *) ) );
                ::message(MSG_DEBUG, "Created category listview %d", (*c_it).Id);
            }

            AlcatelContactList::Iterator it;
            for( it = doc->contacts->begin(); it != doc->contacts->end(); ++it ) {
                new KAlcatelContactViewItem (contacts_list, &(*it),
                        (* it).Name(),
                        QString(!(* it).MainNumber.isEmpty()?(* it).MainNumber: /* try to find non-empty phone */
                                    !(* it).MobileNumber.isEmpty()?(* it).MobileNumber:
                                        !(* it).WorkNumber.isEmpty()?(* it).WorkNumber:
                                            !(* it).HomeNumber.isEmpty()?(* it).HomeNumber:
                                                !(* it).OtherNumber.isEmpty()?(* it).OtherNumber:
                                                    !(* it).FaxNumber.isEmpty()?(* it).FaxNumber:
                                                        (* it).PagerNumber
                            ),
                        QString("%1 %2").
                            arg(StorageTypes[(* it).Storage]).arg((* it).Id)
                        );

                if ((* it).Storage == StorageMobile) {
                    QString catname;
                    if ((* it).Category >= 0) {
                        if ((contacts_cat_list[(* it).Category] != NULL) && ((* it).Category < ALC_MAX_CATEGORIES)) {
                            new KAlcatelContactMobileCatViewItem (contacts_cat_list[(* it).Category], &(*it),
                                    QString((* it).LastName),
                                    QString((* it).FirstName),
                                    QString((* it).MobileNumber),
                                    QString((* it).WorkNumber),
                                    QString((* it).MainNumber),
                                    QString((* it).Email1),
                                    QString("%1").arg((* it).Id));
                        } else {
                            ::message(MSG_WARNING, "Can not insert to category list (%d)", (* it).Category);
                        }
                        AlcatelCategory *cat = getCategoryById(doc->contact_cats, (* it).Category, StorageMobile);
                        if (cat == NULL) {
                            catname = i18n("Unknown (id=%1)").arg((* it).Category);
                        } else {
                            catname = cat->Name;;
                        }
                    } else {
                        catname = i18n("None");
                    }
                    new KAlcatelContactMobileViewItem (contacts_mobile_list, &(*it),
                            QString((* it).LastName),
                            QString((* it).FirstName),
                            catname,
                            QString((* it).MobileNumber),
                            QString((* it).WorkNumber),
                            QString((* it).MainNumber),
                            QString((* it).Email1),
                            QString("%1").arg((* it).Id));
                } /* storage=mobile */
                else {
                    new KAlcatelContactSIMViewItem (contacts_sim_list, &(*it),
                            QString((* it).LastName),
                            QString((* it).MainNumber),
                            QString("%1").arg((* it).Id));
                } /* storage=sim */
            } /* for cycle over contacts */

            tree->setCurrentItem(contacts_item);
        } /* change in contacts */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (unread_sms)
            tree->setCurrentItem(messages_unread_item);

        QString info = i18n("<h2>Document statistics:</h2>\n"
            "<table border=1 cellspacing=0 cellpadding=2>\n"
            "<tr><th>\n"
            "Todos\n"
            "</th><td>\n"
            "%1\n"
            "</td></tr>\n"
            "<tr><th>\n"
            "Calenar events\n"
            "</th><td>\n"
            "%2\n"
            "</td></tr>\n"
            "<tr><th>\n"
            "Contacts\n"
            "</th><td>\n"
            "%3\n"
            "</td></tr>\n"
            "<tr><th>\n"
            "Calls\n"
            "</th><td>\n"
            "%4\n"
            "</td></tr>\n"
            "<tr><th>\n"
            "Messages\n"
            "</th><td>\n"
            "%5\n"
            "</td></tr>\n"
            "</table>").
            arg(doc->todos->count()).
            arg(doc->calendar->count()).
            arg(doc->contacts->count()).
            arg(doc->calls->count()).
            arg(doc->messages->count());

        kalcatel_html->begin();
        kalcatel_html->write(html_text->arg(i18n("Application to manage Alcatel One Touch 501 & 701 mobile")).arg(info));
        kalcatel_html->end();

    } /* any change */
    hsplitter->setResizeMode( tree, QSplitter::FollowSizeHint );
    QWidget::repaint();
}

void KAlcatelView::slotMessageChanged(QListViewItem *item) {
    if (item != NULL) slotShowMessage((AlcatelMessage *)(((KAlcatelDataItem *) item)->alcatelData));
}

void KAlcatelView::slotShowMessage(AlcatelMessage *what) {
    if (what == NULL) {
        textview->setText( i18n("Failed reading message!"));
        return;
    }
    AlcatelContact *cont = getContactByPhone(getDocument()->contacts, &(what->Sender), &(((KAlcatelApp *)parent())->phone_prefix));
    textview->setText( i18n(
        "<b>From:</b> %1 (%2)<br>"
        "<b>Date:</b> %3<br>"
        "<b>Time:</b> %4<br>"
        "<b>SMSC:</b> %5<br>"
        "<b>Position:</b> %6<br>"
        "<b>Status:</b> %7<br><br>"
        "%8").
        arg(cont == NULL? QString("") : cont->Name()).
        arg(what->Sender).
        arg(what->Date.date().toString()).
        arg(what->Date.time().toString()).
        arg(what->SMSC).
        arg(what->Id).
        arg(MessageTypes[what->Status]).
        arg(what->Text));
    textview->setMinimumHeight(textview->contentsHeight()); /* resize to show all contents*/
    vsplitter->setResizeMode( textview, QSplitter::FollowSizeHint );
}

void KAlcatelView::slotTodoChanged(QListViewItem *item) {
    if (item != NULL) slotShowTodo((AlcatelTodo *)(((KAlcatelDataItem *) item)->alcatelData));
}

void KAlcatelView::slotShowTodo(AlcatelTodo *what) {
    QString text;
    if (what == NULL) {
        textview->setText( i18n("Failed reading todo!"));
        return;
    }

    AlcatelContact *cont=getContactById(getDocument()->contacts, what->ContactID, StorageMobile);
    if (!what->Subject.isEmpty()) text.append(i18n("<b>Subject:</b> %1<br>").arg(what->Subject));
    if (!what->DueDate.isNull()) text.append(i18n("<b>DueDate:</b> %1<br>").arg(what->DueDate.toString()));
    if (!what->Alarm.isNull()) text.append(i18n("<b>Alarm:</b> %1<br>").arg(what->Alarm.toString()));
    if (what->Private != -1) text.append(i18n("<b>Private:</b> %1<br>").arg(what->Private == 1?i18n("Yes"):i18n("No")));
    if (what->Completed != -1) text.append(i18n("<b>Completed:</b> %1<br>").arg(what->Completed == 1?i18n("Yes"):i18n("No")));
    if (what->Priority != -1) text.append(i18n("<b>Priority:</b> %1<br>").arg(Priorities[what->Priority]));
    if (what->ContactID != -1 && what->ContactID != 0) text.append(i18n("<b>Contact:</b> %1<br>").arg(cont==NULL?QString("id=%1").arg(what->ContactID):cont->Name()));
    if (what->Category != -1) {
        AlcatelCategory *cat = getCategoryById(getDocument()->todo_cats, what->Category, StorageMobile);
        if (cat == NULL) {
            text.append(i18n("<b>Category:</b> %1<br>").arg(i18n("Unknown (id=%1))").arg(what->Category)));
        } else {
            text.append(i18n("<b>Category:</b> %1<br>").arg(cat->Name));
        }
    }
    text.append(i18n("<b>Position:</b> %1").arg(what->Id));
    textview->setText(text);
    textview->setMinimumHeight(textview->contentsHeight()); /* resize to show all contents*/
    vsplitter->setResizeMode( textview, QSplitter::FollowSizeHint );
}

void KAlcatelView::slotCalendarChanged(QListViewItem *item) {
    if (item != NULL) slotShowCalendar((AlcatelCalendar *)(((KAlcatelDataItem *) item)->alcatelData));
}

void KAlcatelView::slotShowCalendar(AlcatelCalendar *what) {
    QString text;
    if (what == NULL) {
        textview->setText( i18n("Failed reading calendar item!"));
        return;
    }

    AlcatelContact *cont=getContactById(getDocument()->contacts, what->ContactID, StorageMobile);

    if (!what->Subject.isEmpty()) text.append(i18n("<b>Subject:</b> %1<br>").arg(what->Subject));
    if (!what->Date.isNull()) text.append(i18n("<b>Date:</b> %1<br>").arg(what->Date.toString()));
    if (what->EventType != ALC_CALENDAR_BIRTHDAY && what->EventType != ALC_CALENDAR_ALARM) {
        text.append(i18n("<b>StartTime:</b> %1<br>").arg(what->StartTime.toString()));
        text.append(i18n("<b>EndTime:</b> %1<br>").arg(what->EndTime.toString()));
    }

    text.append(i18n("<b>EventType:</b> %1<br>").arg((what->EventType!=-1)?CalendarTypes[what->EventType]:i18n("Unknown")));
    text.append(i18n("<b>Repeating:</b> %1<br>").arg(what->RepeatingDetail()));

    if (!what->Alarm.isNull()) text.append(i18n("<b>Alarm:</b> %1<br>").arg(what->Alarm.toString()));
    if (!what->Alarm2.isNull()) text.append(i18n("<b>Alarm2:</b> %1<br>").arg(what->Alarm2.toString()));
    if (what->Private != -1) text.append(i18n("<b>Private:</b> %1<br>").arg(what->Private == 1?i18n("Yes"):i18n("No")));
    if (what->ContactID != -1 && what->ContactID != 0) text.append(i18n("<b>Contact:</b> %1<br>").arg(cont==NULL?QString("id=%1").arg(what->ContactID):cont->Name()));

    text.append(i18n("<b>Position:</b> %1").arg(what->Id));
    textview->setText(text);
    textview->setMinimumHeight(textview->contentsHeight()); /* resize to show all contents*/
    vsplitter->setResizeMode( textview, QSplitter::FollowSizeHint );
}

void KAlcatelView::slotContactChanged(QListViewItem *item) {
    if (item != NULL) slotShowContact((AlcatelContact *)(((KAlcatelDataItem *) item)->alcatelData));
}

void KAlcatelView::slotShowContact(AlcatelContact *what) {
    QString text;
    if (what == NULL) {
        textview->setText( i18n("Failed reading contact!"));
        return;
    }
    if (what->Storage == StorageSIM) {
        if (!what->LastName.isEmpty()) text.append(i18n("<b>Name:</b> %1<br>").arg(what->LastName));
        if (!what->MainNumber.isEmpty()) text.append(i18n("<b>Phone:</b> %1<br>").arg(what->MainNumber));
        text.append(i18n("<b>Storage:</b> %1<br>").arg(i18n("SIM card")));
    } else {
        if (!what->FirstName.isEmpty()) text.append(i18n("<b>FirstName:</b> %1<br>").arg(what->FirstName));
        if (!what->LastName.isEmpty()) text.append(i18n("<b>LastName:</b> %1<br>").arg(what->LastName));

        if (what->Category != -1) {
            AlcatelCategory *cat = getCategoryById(getDocument()->contact_cats, what->Category, StorageMobile);
            if (cat == NULL) {
                text.append(i18n("<b>Category:</b> %1<br>").arg(i18n("Unknown (id=%1))").arg(what->Category)));
            } else {
                text.append(i18n("<b>Category:</b> %1<br>").arg(cat->Name));
            }
        }

        if (!what->Company.isEmpty()) text.append(i18n("<b>Company:</b> %1<br>").arg(what->Company));
        if (!what->JobTitle.isEmpty()) text.append(i18n("<b>JobTitle:</b> %1<br>").arg(what->JobTitle));
        if (!what->WorkNumber.isEmpty()) text.append(i18n("<b>WorkNumber:</b> %1<br>").arg(what->WorkNumber));
        if (!what->MainNumber.isEmpty()) text.append(i18n("<b>MainNumber:</b> %1<br>").arg(what->MainNumber));
        if (!what->FaxNumber.isEmpty()) text.append(i18n("<b>FaxNumber:</b> %1<br>").arg(what->FaxNumber));
        if (!what->OtherNumber.isEmpty()) text.append(i18n("<b>OtherNumber:</b> %1<br>").arg(what->OtherNumber));
        if (!what->PagerNumber.isEmpty()) text.append(i18n("<b>PagerNumber:</b> %1<br>").arg(what->PagerNumber));
        if (!what->MobileNumber.isEmpty()) text.append(i18n("<b>MobileNumber:</b> %1<br>").arg(what->MobileNumber));
        if (!what->HomeNumber.isEmpty()) text.append(i18n("<b>HomeNumber:</b> %1<br>").arg(what->HomeNumber));
        if (!what->Email1.isEmpty()) text.append(i18n("<b>Email1:</b> <a href=\"%1\">%2</a><br>").arg(what->Email1).arg(what->Email1));
        if (!what->Email2.isEmpty()) text.append(i18n("<b>Email2:</b> <a href=\"%1\">%2</a><br>").arg(what->Email2).arg(what->Email2));
        if (!what->Address.isEmpty()) text.append(i18n("<b>Address:</b> %1<br>").arg(what->Address));
        if (!what->City.isEmpty()) text.append(i18n("<b>City:</b> %1<br>").arg(what->City));
        if (!what->State.isEmpty()) text.append(i18n("<b>State:</b> %1<br>").arg(what->State));
        if (!what->Zip.isEmpty()) text.append(i18n("<b>Zip:</b> %1<br>").arg(what->Zip));
        if (!what->Country.isEmpty()) text.append(i18n("<b>Country:</b> %1<br>").arg(what->Country));
        QString custom;
        KAlcatelApp *theApp=(KAlcatelApp *) parentWidget();
        if (theApp->contact_url == 1) custom = i18n("<a href=\"%1\">%2</a>").arg(what->Custom1).arg(what->Custom1);
        else custom = what->Custom1;
        if (!what->Custom1.isEmpty()) text.append(i18n("<b>Custom1:</b> %1<br>").arg(custom));
        if (theApp->contact_url == 2) custom = i18n("<a href=\"%1\">%2</a>").arg(what->Custom1).arg(what->Custom1);
        else custom = what->Custom1;
        if (!what->Custom2.isEmpty()) text.append(i18n("<b>Custom2:</b> %1<br>").arg(custom));
        if (theApp->contact_url == 3) custom = i18n("<a href=\"%1\">%2</a>").arg(what->Custom1).arg(what->Custom1);
        else custom = what->Custom1;
        if (!what->Custom3.isEmpty()) text.append(i18n("<b>Custom3:</b> %1<br>").arg(custom));
        if (theApp->contact_url == 4) custom = i18n("<a href=\"%1\">%2</a>").arg(what->Custom1).arg(what->Custom1);
        else custom = what->Custom1;
        if (!what->Custom4.isEmpty()) text.append(i18n("<b>Custom4:</b> %1<br>").arg(custom));
        if (!what->Note.isEmpty()) text.append(i18n("<b>Note:</b> %1<br>").arg(what->Note));
        if (what->Private != -1) text.append(i18n("<b>Private:</b> %1<br>").arg(what->Private == 1?i18n("Yes"):i18n("No")));
        text.append(i18n("<b>Storage:</b> %1<br>").arg(i18n("Phone")));
    }
    text.append(i18n("<b>Position:</b> %1").arg(what->Id));
    textview->setText(text);
    textview->setMinimumHeight(textview->contentsHeight()); /* resize to show all contents*/
    vsplitter->setResizeMode( textview, QSplitter::FollowSizeHint );
}

void KAlcatelView::print(QPrinter *pPrinter) {
    KMessageBox::sorry((KAlcatelApp *) parent(), i18n("Printing not implemented yet..."), i18n("Sorry"));
/*
    QPainter printpainter;
    printpainter.begin(pPrinter);
  	
    // TODO: add your printing code here

    printpainter.end();*/
}

void KAlcatelView::slotTreeChanged(QListViewItem *item) {
    if (item != NULL) {
        item->setOpen(true);
        int num = ((KAlcatelTreeViewItem *)item)->showWidget;
        widgetstack->raiseWidget( num );
        QWidget *widget = widgetstack->visibleWidget();

        if ( widget->inherits("QListView") ) {
            QListView *view = (QListView *)widget;
            QListViewItem *currentitem = view->currentItem();
            textview->setText("");
            view->setFocus();

            if (num == ID_TODOS) slotTodoChanged (currentitem);
            else if (num == ID_CALENDAR)  slotCalendarChanged(currentitem);
            else if (num == ID_CONTACTS || num == ID_CONTACTS_SIM || num == ID_CONTACTS_MOBILE) slotContactChanged (currentitem);
            else if (num == ID_CALLS || num == ID_CALLS_OUTGOING || num == ID_CALLS_MISSED || num == ID_CALLS_RECEIVED)
                slotCallChanged (currentitem);
            else if (num == ID_MESSAGES || num == ID_MESSAGES_SENT || num == ID_MESSAGES_UNSENT || num == ID_MESSAGES_READ || num == ID_MESSAGES_UNREAD)
                slotMessageChanged (currentitem);
            else if (num >= ID_CONTACTS_CAT && num < ID_CONTACTS_CAT + ALC_MAX_CATEGORIES) slotContactChanged (currentitem);
            else if (num >= ID_TODOS_CAT && num < ID_TODOS_CAT + ALC_MAX_CATEGORIES) slotTodoChanged (currentitem);
        }
    }
}

void KAlcatelView::slotSetTitle( int num ) {
    if (num == ID_TODOS) titlelabel->setText(i18n("All todos"));
    else if (num == ID_CALENDAR) titlelabel->setText(i18n("Whole canedar"));
    else if (num == ID_CONTACTS) titlelabel->setText(i18n("All contacts"));
    else if (num == ID_CONTACTS_SIM) titlelabel->setText(i18n("Contacts stored on SIM card"));
    else if (num == ID_CONTACTS_MOBILE ) titlelabel->setText(i18n("Contacts stored in mobile"));
    else if (num == ID_CALLS) titlelabel->setText(i18n("All calls"));
    else if (num == ID_CALLS_OUTGOING) titlelabel->setText(i18n("Outgoing calls"));
    else if (num == ID_CALLS_MISSED) titlelabel->setText(i18n("Missed calls"));
    else if (num == ID_CALLS_RECEIVED) titlelabel->setText(i18n("Received calls"));
    else if (num == ID_MESSAGES) titlelabel->setText(i18n("All messages"));
    else if (num == ID_MESSAGES_SENT) titlelabel->setText(i18n("Sent messages"));
    else if (num == ID_MESSAGES_UNSENT) titlelabel->setText(i18n("Unsent messages"));
    else if (num == ID_MESSAGES_READ) titlelabel->setText(i18n("Read messages"));
    else if (num == ID_MESSAGES_UNREAD) titlelabel->setText(i18n("Unread messages"));
    else if (num >= ID_CONTACTS_CAT && num < ID_CONTACTS_CAT + ALC_MAX_CATEGORIES){
        AlcatelCategory *cat = getCategoryById(getDocument()->contact_cats, num - ID_CONTACTS_CAT, StorageMobile);
        if (cat == NULL)
            titlelabel->setText(i18n("Contacts in unknown category"));
        else
            titlelabel->setText(i18n("Contacts in category %1").arg(cat->Name));
    }
    else if (num >= ID_TODOS_CAT && num < ID_TODOS_CAT + ALC_MAX_CATEGORIES){
        AlcatelCategory *cat = getCategoryById(getDocument()->todo_cats, num - ID_TODOS_CAT, StorageMobile);
        if (cat == NULL)
            titlelabel->setText(i18n("Todos in unknown category"));
        else
            titlelabel->setText(i18n("Todos in category %1").arg(cat->Name));
    }
    else if (num == ID_KALCATEL) {
        titlelabel->setText(i18n("KAlcatel document statistics"));
        textview->hide();
    }
    else titlelabel->setText(i18n("KAlcatel - something wrong has happen (%1)").arg(num));
    if (num != ID_KALCATEL) textview->show();
}

void KAlcatelView::slotCallChanged(QListViewItem *item) {
    if (item != NULL) slotShowCall((AlcatelCall *)(((KAlcatelDataItem *) item)->alcatelData));
}

void KAlcatelView::slotShowCall(AlcatelCall *what) {
    if (what == NULL) {
        textview->setText( i18n("Failed reading call!"));
        return;
    }
    AlcatelContact *cont = getContactByPhone(getDocument()->contacts, &(what->Number), &(((KAlcatelApp *)parent())->phone_prefix));
    QString text;
    text.append(i18n("<b>From:</b> %1 (%2)<br>").arg(what->Name).arg(what->Number));
    if (cont != NULL) text.append(i18n("<b>Contact:</b> %3<br>").arg(cont->Name()));
    text.append(i18n( "<b>Type:</b> %4<br><b>Position:</b> %6").arg(CallTypes[what->Type]).arg(what->Id));
    textview->setText(text);
    textview->setMinimumHeight(textview->contentsHeight()); /* resize to show all contents*/
    vsplitter->setResizeMode( textview, QSplitter::FollowSizeHint );
}
