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
//#include <qprinter.h>
//#include <qpainter.h>
#include <qpixmap.h>
#include <qsplitter.h>
#include <qtextview.h>

// include files for KDE
#include <kjanuswidget.h>
#include <klocale.h>
#include <klistview.h>
#include <kiconloader.h>
#include <kmessagebox.h>

// application specific includes
#include "kalcatelview.h"
#include "kalcateldoc.h"
#include "kalcatel.h"
#include "alcatelclasses.h"
#include "alcatool/sms.h"
#include "alcatool/logging.h"

KAlcatelView::KAlcatelView(QWidget *parent, const char *name) : KJanusWidget(parent, name, TreeList) {
    int i;
    QSplitter *vsplitter;
    QStringList list = QStringList();
    setShowIconsInTreeList (true);
    setTreeListAutoResize (true);

    for (i=0; i<ALC_MAX_CATEGORIES; i++) {
        contacts_cat_list[i] =  NULL;
        contacts_cat_view[i] =  NULL;
        todo_cat_list[i] =  NULL;
        todo_cat_view[i] =  NULL;
    }


    todo = addVBoxPage (i18n("Todos"), i18n("All todos"), SmallIcon("kalcatel-todo.png"));
    vsplitter = new QSplitter( Qt::Vertical, todo );
    todo_list = createListView(vsplitter, alc_todos);
    connect( todo_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotTodoChanged(QListViewItem *) ) );
    todo_view = new QTextView( vsplitter );
    todo_view->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(todo_view, QSplitter::FollowSizeHint);

    calendar = addVBoxPage (i18n("Calendar"), i18n("Whole calendar"), SmallIcon("kalcatel-calendar.png"));
    vsplitter = new QSplitter( Qt::Vertical, calendar );
    calendar_list = createListView(vsplitter, alc_calendar);
    connect( calendar_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotCalendarChanged(QListViewItem *) ) );
    calendar_view = new QTextView( vsplitter );
    calendar_view->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(calendar_view, QSplitter::FollowSizeHint);

    contacts = addVBoxPage (i18n("Contacts"), i18n("All contacts"), SmallIcon("kalcatel-contact.png"));
    vsplitter = new QSplitter( Qt::Vertical, contacts );
    contacts_list = createListView(vsplitter, alc_contacts);
    connect( contacts_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactChanged(QListViewItem *) ) );
    contact_view = new QTextView( vsplitter );
    contact_view->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(contact_view, QSplitter::FollowSizeHint);

    list.append(i18n("Contacts"));
    list.append(i18n("SIM"));
    contacts_sim = addVBoxPage (list, i18n("Contacts on SIM card"), SmallIcon("kalcatel-contact-sim.png"));
    vsplitter = new QSplitter( Qt::Vertical, contacts_sim );
    contacts_sim_list = createListView(vsplitter, alc_contacts_sim);
    list.clear();
    connect( contacts_sim_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactSimChanged(QListViewItem *) ) );
    contact_sim_view = new QTextView( vsplitter );
    contact_sim_view->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(contact_sim_view, QSplitter::FollowSizeHint);

    list.append(i18n("Contacts"));
    list.append(i18n("Mobile"));
    contacts_mobile = addVBoxPage (list, i18n("Contacts stored in mobile"), SmallIcon("kalcatel-contact-mobile.png"));
    vsplitter = new QSplitter( Qt::Vertical, contacts_mobile );
    contacts_mobile_list = createListView(vsplitter, alc_contacts_mobile);
    list.clear();
    connect( contacts_mobile_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactMobileChanged(QListViewItem *) ) );
    contact_mobile_view = new QTextView( vsplitter );
    contact_mobile_view->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(contact_mobile_view, QSplitter::FollowSizeHint);

    calls = addVBoxPage (i18n("Calls"), i18n("All calls"), SmallIcon("kalcatel-call.png"));
    calls_list = createListView(calls, alc_calls);

    list.append(i18n("Calls"));
    list.append(i18n("Missed"));
    calls_mc = addVBoxPage (list, i18n("Missed calls"), SmallIcon("kalcatel-call-missed.png"));
    calls_mc_list = createListView(calls_mc, alc_calls_type);
    list.clear();

    list.append(i18n("Calls"));
    list.append(i18n("Received"));
    calls_rc = addVBoxPage (list, i18n("Received calls"), SmallIcon("kalcatel-call-received.png"));
    calls_rc_list = createListView(calls_rc, alc_calls_type);
    list.clear();

    list.append(i18n("Calls"));
    list.append(i18n("Outgoing"));
    calls_ld = addVBoxPage (list, i18n("Outgoing calls"), SmallIcon("kalcatel-call-outgoing.png"));
    calls_ld_list = createListView(calls_ld, alc_calls_type);
    list.clear();

    messages = addVBoxPage (i18n("Messages"), i18n("All messages"), SmallIcon("kalcatel-message.png"));
    vsplitter = new QSplitter( Qt::Vertical, messages );
    messages_list = createListView(vsplitter, alc_messages);
    connect( messages_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotMessageChanged(QListViewItem *) ) );
    message = new QTextView( vsplitter );
    message->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(message, QSplitter::FollowSizeHint);


    list.append(i18n("Messages"));
    list.append(i18n("Unread"));
    msg_unread = addVBoxPage (list, i18n("Unread messages"), SmallIcon("kalcatel-message-unread.png"));
    vsplitter = new QSplitter( Qt::Vertical, msg_unread );
    msg_unread_list = createListView(vsplitter, alc_messages_in);
    list.clear();
    connect( msg_unread_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotUnreadMessageChanged(QListViewItem *) ) );
    message_unread = new QTextView( vsplitter );
    message_unread->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(message_unread, QSplitter::FollowSizeHint);

    list.append(i18n("Messages"));
    list.append(i18n("Read"));
    msg_read = addVBoxPage (list, i18n("Read messages"), SmallIcon("kalcatel-message-read.png"));
    vsplitter = new QSplitter( Qt::Vertical, msg_read );
    msg_read_list = createListView(vsplitter, alc_messages_in);
    list.clear();
    connect( msg_read_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotReadMessageChanged(QListViewItem *) ) );
    message_read = new QTextView( vsplitter );
    message_read->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(message_read, QSplitter::FollowSizeHint);

    list.append(i18n("Messages"));
    list.append(i18n("Sent"));
    msg_sent = addVBoxPage (list, i18n("Sent messages"), SmallIcon("kalcatel-message-sent.png"));
    vsplitter = new QSplitter( Qt::Vertical, msg_sent );
    msg_sent_list = createListView(vsplitter, alc_messages_out);
    list.clear();
    connect( msg_sent_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotSentMessageChanged(QListViewItem *) ) );
    message_sent = new QTextView( vsplitter );
    message_sent->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(message_sent, QSplitter::FollowSizeHint);

    list.append(i18n("Messages"));
    list.append(i18n("Unsent"));
    msg_unsent = addVBoxPage (list, i18n("Unsent messages"), SmallIcon("kalcatel-message-unsent.png"));
    vsplitter = new QSplitter( Qt::Vertical, msg_unsent );
    msg_unsent_list = createListView(vsplitter, alc_messages_out);
    list.clear();
    connect( msg_unsent_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotUnsentMessageChanged(QListViewItem *) ) );
    message_unsent = new QTextView( vsplitter );
    message_unsent->setBackgroundMode( PaletteBase );
    vsplitter->setResizeMode(message_unsent, QSplitter::FollowSizeHint);
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
            list->addColumn(i18n("Category"));
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
            list->addColumn(i18n("Position"));
            break;
        case alc_calls:
            list->addColumn(i18n("Number"));
            list->addColumn(i18n("Name"));
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

        if (doc->getSMSVersion() != smsVersion || doc->getContactVersion() != contactVersion) {
            /* we should update this also when contacts change, because there are names of sender */
            smsVersion = doc->getSMSVersion();
            AlcatelSMSList::Iterator it;
            KListView *list;
            QString type;

            messages_list->clear();
            msg_unread_list->clear();
            msg_read_list->clear();
            msg_unsent_list->clear();
            msg_sent_list->clear();

            for( it = doc->sms->begin(); it != doc->sms->end(); ++it ) {
                type =  SMSTypes[(* it).Status];
                switch ((* it).Status) {
                    case SMS_UNREAD:
                        list = msg_unread_list;
                        unread_sms++;
                        break;
                    case SMS_READ:
                        list = msg_read_list;
                        break;
                    case SMS_UNSENT:
                        list = msg_unsent_list;
                        break;
                    case SMS_SENT:
                        list = msg_sent_list;
                        break;
                }
                AlcatelContact *cont = getContactByPhone(doc->contacts, &((* it).Sender), &(((KAlcatelApp *)parent())->phone_prefix));
                new QListViewItem (messages_list,
                        QString((* it).Sender),
                        cont == NULL? QString("") : cont->Name(),
                        type,
                        (* it).Date.date().toString(),
                        (* it).Date.time().toString(),
                        QString((* it).Text),
                        QString().sprintf("%d", (* it).Id));

                new QListViewItem (list,
                        QString((* it).Sender),
                        cont == NULL? QString("") : cont->Name(),
                        (* it).Date.date().toString(),
                        (* it).Date.time().toString(),
                        QString((* it).Text),
                        QString().sprintf("%d", (* it).Id));
            } /* for cycle over sms */
            if (unread_sms)
                showPage(10);
            else
                showPage(9);
        } /* change in sms */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (doc->getTodoVersion() != todoVersion|| doc->getContactVersion() != contactVersion) {
            /* we should update this also when contacts change, because there are names of contact */
            todoVersion = doc->getTodoVersion();
            AlcatelCategoryList::Iterator c_it;
            int i;

            todo_list->clear();

            for (i=0; i<ALC_MAX_CATEGORIES; i++)
                if (todo_cat_list[i] !=  NULL) {
                    ((QVBox *)(todo_cat_list[i]->parent()->parent()))->~QVBox();
                    todo_cat_list[i] = NULL;
                }

            for( c_it = doc->todo_cats->begin(); c_it != doc->todo_cats->end(); ++c_it ) {
                QStringList list = QStringList();
                QVBox *box;

                list.append(i18n("Todos"));
                list.append((*c_it).Name);
                box = addVBoxPage (list, i18n("Todos in category %1").arg((*c_it).Name), SmallIcon("kalcatel-todo.png"));
                QSplitter *vsplitter = new QSplitter( Qt::Vertical, box );

                todo_cat_list[(*c_it).Id] = createListView(vsplitter, alc_todos_cat);

                connect( todo_cat_list[(*c_it).Id], SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotTodoCatChanged(QListViewItem *) ) );
                todo_cat_view[(*c_it).Id] = new QTextView( vsplitter );
                todo_cat_view[(*c_it).Id]->setBackgroundMode( PaletteBase );
                vsplitter->setResizeMode(todo_cat_view[(*c_it).Id], QSplitter::FollowSizeHint);

                ::message(MSG_DEBUG, "Created category listview %d", (*c_it).Id);
                list.clear();
            }
            AlcatelTodoList::Iterator it;
            for( it = doc->todo->begin(); it != doc->todo->end(); ++it ) {
                new QListViewItem (todo_list,
                        (* it).Completed == -1 ? QString(""): (* it).Completed ? i18n("Yes") : i18n("No"),
                        (* it).Priority == -1 ? QString("") : Priorities[(* it).Priority],
                        (* it).DueDate.isNull()?i18n("None"):(* it).DueDate.toString(),
                        (* it).Subject,
                        *(getCategoryName(doc->todo_cats, (*it).Category)),
                        QString("%1").arg((* it).Id));

                if (((* it).Category >= 0) && (todo_cat_list[(* it).Category] != NULL) && ((* it).Category < ALC_MAX_CATEGORIES)) {
                    new QListViewItem (todo_cat_list[(* it).Category],
                            (* it).Completed == -1 ? QString(""): (* it).Completed ? i18n("Yes") : i18n("No"),
                            (* it).Priority == -1 ? QString("") : Priorities[(* it).Priority],
                            (* it).DueDate.isNull()?i18n("None"):(* it).DueDate.toString(),
                            (* it).Subject,
                            QString("%1").arg((* it).Id));
                } else {
                    ::message(MSG_WARNING, "Can not insert to category list (%d)", (* it).Category);
                }
            } /* for cycle over todos */

            showPage(0);
        } /* change in todos */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (doc->getCalendarVersion() != calendarVersion|| doc->getContactVersion() != contactVersion) {
            /* we should update this also when contacts change, because there are names of contact */
            calendarVersion = doc->getCalendarVersion();

            calendar_list->clear();

            AlcatelCalendarList::Iterator it;
            for( it = doc->calendar->begin(); it != doc->calendar->end(); ++it ) {
                new QListViewItem (calendar_list,
                        (* it).EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : (* it).Date.isNull()?i18n("None"):(* it).Date.toString(),
                        (* it).EventType == ALC_CALENDAR_BIRTHDAY || (* it).EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : (* it).StartTime.toString(),
                        (* it).EventType == ALC_CALENDAR_BIRTHDAY || (* it).EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : (* it).EndTime.toString(),
                        (* it).EventType!=-1?CalendarTypes[(* it).EventType]:i18n("Unknown"),
                        (* it).Subject,
                        (* it).Alarm.isNull()?i18n("None"):(* it).Alarm.toString(),
                        (* it).Repeating(),
                        QString("%1").arg((* it).Id));
            } /* for cycle over items */

            showPage(1);
        } /* change in calendar */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (doc->getContactVersion() != contactVersion) {
            contactVersion = doc->getContactVersion();
            AlcatelCategoryList::Iterator c_it;
            int i;

            contacts_list->clear();
            contacts_sim_list->clear();
            contacts_mobile_list->clear();

            for (i=0; i<ALC_MAX_CATEGORIES; i++)
                if (contacts_cat_list[i] !=  NULL) {
                    ((QVBox *)(contacts_cat_list[i]->parent()->parent()))->~QVBox();
                    contacts_cat_list[i] = NULL;
                }

            for( c_it = doc->contact_cats->begin(); c_it != doc->contact_cats->end(); ++c_it ) {
                QStringList list = QStringList();
                QVBox *box;

                list.append(i18n("Contacts"));
                list.append(i18n("Mobile"));
                list.append((*c_it).Name);
                box = addVBoxPage (list, i18n("Contacts stored in mobile in category %1").arg((*c_it).Name), SmallIcon("kalcatel-contact-mobile.png"));
                QSplitter *vsplitter = new QSplitter( Qt::Vertical, box );

                contacts_cat_list[(*c_it).Id] = createListView(vsplitter, alc_contacts_mobile_cat);

                connect( contacts_cat_list[(*c_it).Id], SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotContactMobileCatChanged(QListViewItem *) ) );
                contacts_cat_view[(*c_it).Id] = new QTextView( vsplitter );
                contacts_cat_view[(*c_it).Id]->setBackgroundMode( PaletteBase );
                vsplitter->setResizeMode(contacts_cat_view[(*c_it).Id], QSplitter::FollowSizeHint);

                ::message(MSG_DEBUG, "Created category listview %d", (*c_it).Id);
                list.clear();
            }
            AlcatelContactList::Iterator it;
            for( it = doc->contacts->begin(); it != doc->contacts->end(); ++it ) {
                new QListViewItem (contacts_list,
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
                            arg(((* it).Storage == AlcatelContact::Mobile)?
                                i18n("Mobile") :
                                i18n("SIM")).arg((* it).Id,4)
                        );

                if ((* it).Storage == AlcatelContact::Mobile) {
                    if (((* it).Category >= 0) && (contacts_cat_list[(* it).Category] != NULL) && ((* it).Category < ALC_MAX_CATEGORIES)) {
                        new QListViewItem (contacts_cat_list[(* it).Category],
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
                    new QListViewItem (contacts_mobile_list,
                            QString((* it).LastName),
                            QString((* it).FirstName),
                            *(getCategoryName(doc->contact_cats, (* it).Category)),
                            QString((* it).MobileNumber),
                            QString((* it).WorkNumber),
                            QString((* it).MainNumber),
                            QString((* it).Email1),
                            QString("%1").arg((* it).Id));
                } /* storage=mobile */
                else {
                    new QListViewItem (contacts_sim_list,
                            QString((* it).LastName),
                            QString((* it).MainNumber),
                            QString("%1").arg((* it).Id));
                } /* storage=sim */
            } /* for cycle over contacts */

            showPage(2);
        } /* change in contacts */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (unread_sms)
            showPage(10); /* allways show unread messages, if any */
    } /* any change */
    KJanusWidget::repaint();
}

void KAlcatelView::slotMessageChanged(QListViewItem *item) {
    slotShowMessage(message, getSMSById(getDocument()->sms, item->text(6).toInt()));
}

void KAlcatelView::slotReadMessageChanged(QListViewItem *item) {
    slotShowMessage(message_read, getSMSById(getDocument()->sms, item->text(5).toInt()));
}

void KAlcatelView::slotUnreadMessageChanged(QListViewItem *item) {
    slotShowMessage(message_unread, getSMSById(getDocument()->sms, item->text(5).toInt()));
}

void KAlcatelView::slotSentMessageChanged(QListViewItem *item) {
    slotShowMessage(message_sent, getSMSById(getDocument()->sms, item->text(5).toInt()));
}

void KAlcatelView::slotUnsentMessageChanged(QListViewItem *item) {
    slotShowMessage(message_sent, getSMSById(getDocument()->sms, item->text(5).toInt()));
}

void KAlcatelView::slotShowMessage(QTextView *where, AlcatelSMS *what) {
    if (what == NULL) {
        where->setText( i18n("Failed reading message!"));
        return;
    }
    AlcatelContact *cont = getContactByPhone(getDocument()->contacts, &(what->Sender), &(((KAlcatelApp *)parent())->phone_prefix));
    where->setText( i18n(
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
        arg(SMSTypes[what->Status]).
        arg(what->Text));
    where->setMinimumHeight(where->contentsHeight()); /* resize to show all contents*/
}

void KAlcatelView::slotTodoChanged(QListViewItem *item) {
    slotShowTodo(todo_view, getTodoById(getDocument()->todo, item->text(5).toInt()));
}

void KAlcatelView::slotTodoCatChanged(QListViewItem *item) {
    AlcatelTodo *cont = getTodoById(getDocument()->todo, item->text(4).toInt());
    if (cont->Category >= 0) slotShowTodo(todo_cat_view[cont->Category], cont);
}

void KAlcatelView::slotShowTodo(QTextView *where, AlcatelTodo *what) {
    QString text;
    if (what == NULL) {
        where->setText( i18n("Failed reading todo!"));
        return;
    }

    AlcatelContact *cont=getContactById(getDocument()->contacts, what->ContactID);
    if (!what->Subject.isEmpty()) text.append(i18n("<b>Subject:</b> %1<br>").arg(what->Subject));
    if (!what->DueDate.isNull()) text.append(i18n("<b>DueDate:</b> %1<br>").arg(what->DueDate.toString()));
    if (!what->Alarm.isNull()) text.append(i18n("<b>Alarm:</b> %1<br>").arg(what->Alarm.toString()));
    if (what->Private != -1) text.append(i18n("<b>Private:</b> %1<br>").arg(what->Private == 1?i18n("Yes"):i18n("No")));
    if (what->Completed != -1) text.append(i18n("<b>Completed:</b> %1<br>").arg(what->Completed == 1?i18n("Yes"):i18n("No")));
    if (what->Priority != -1) text.append(i18n("<b>Priority:</b> %1<br>").arg(Priorities[what->Priority]));
    if (what->ContactID != -1 && what->ContactID != 0) text.append(i18n("<b>Contact:</b> %1<br>").arg(cont==NULL?QString("id=%1").arg(what->ContactID):cont->Name()));
    text.append(i18n("<b>Category:</b> %1<br>").arg(*(getCategoryName(getDocument()->contact_cats, what->Category))));
    text.append(i18n("<b>Position:</b> %1").arg(what->Id));
    where->setText(text);
    where->setMinimumHeight(where->contentsHeight()); /* resize to show all contents*/
}

void KAlcatelView::slotCalendarChanged(QListViewItem *item) {
    slotShowCalendar(calendar_view, getCalendarById(getDocument()->calendar, item->text(7).toInt()));
}

void KAlcatelView::slotShowCalendar(QTextView *where, AlcatelCalendar *what) {
    QString text;
    if (what == NULL) {
        where->setText( i18n("Failed reading calendar item!"));
        return;
    }
/*	QString Repeating(void);
	
*/    /* repeating events: */
/*    int DayOfWeek;
    int Day;
    int WeekOfMonth;
    int Month;
    int Frequency;
    QDate StartDate;
    QDate StopDate;*/


    AlcatelContact *cont=getContactById(getDocument()->contacts, what->ContactID);

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
    where->setText(text);
    where->setMinimumHeight(where->contentsHeight()); /* resize to show all contents*/
}

void KAlcatelView::slotContactChanged(QListViewItem *item) {
    slotShowContact(contact_view, getContactById(getDocument()->contacts, item->text(2).right(4).stripWhiteSpace().toInt()));
}

void KAlcatelView::slotContactSimChanged(QListViewItem *item) {
    slotShowContact(contact_sim_view, getContactById(getDocument()->contacts, item->text(3).toInt()));
}

void KAlcatelView::slotContactMobileChanged(QListViewItem *item) {
    slotShowContact(contact_mobile_view, getContactById(getDocument()->contacts, item->text(7).toInt()));
}

void KAlcatelView::slotContactMobileCatChanged(QListViewItem *item) {
    AlcatelContact *cont = getContactById(getDocument()->contacts, item->text(6).toInt());
    if (cont->Category >= 0) slotShowContact(contacts_cat_view[cont->Category], cont);
}

void KAlcatelView::slotShowContact(QTextView *where, AlcatelContact *what) {
    QString text;
    if (what == NULL) {
        where->setText( i18n("Failed reading contact!"));
        return;
    }
    if (what->Storage == AlcatelContact::SIM) {
        if (!what->FirstName.isEmpty()) text.append(i18n("<b>Name:</b> %1<br>").arg(what->FirstName));
        if (!what->MainNumber.isEmpty()) text.append(i18n("<b>Phone:</b> %1<br>").arg(what->MainNumber));
        text.append(i18n("<b>Storage:</b> %1<br>").arg(i18n("SIM card")));
    } else {
        if (!what->FirstName.isEmpty()) text.append(i18n("<b>FirstName:</b> %1<br>").arg(what->FirstName));
        if (!what->LastName.isEmpty()) text.append(i18n("<b>LastName:</b> %1<br>").arg(what->LastName));
        text.append(i18n("<b>Category:</b> %1<br>").arg(*(getCategoryName(getDocument()->contact_cats, what->Category))));
        if (!what->Company.isEmpty()) text.append(i18n("<b>Company:</b> %1<br>").arg(what->Company));
        if (!what->JobTitle.isEmpty()) text.append(i18n("<b>JobTitle:</b> %1<br>").arg(what->JobTitle));
        if (!what->WorkNumber.isEmpty()) text.append(i18n("<b>WorkNumber:</b> %1<br>").arg(what->WorkNumber));
        if (!what->MainNumber.isEmpty()) text.append(i18n("<b>MainNumber:</b> %1<br>").arg(what->MainNumber));
        if (!what->FaxNumber.isEmpty()) text.append(i18n("<b>FaxNumber:</b> %1<br>").arg(what->FaxNumber));
        if (!what->OtherNumber.isEmpty()) text.append(i18n("<b>OtherNumber:</b> %1<br>").arg(what->OtherNumber));
        if (!what->PagerNumber.isEmpty()) text.append(i18n("<b>PagerNumber:</b> %1<br>").arg(what->PagerNumber));
        if (!what->MobileNumber.isEmpty()) text.append(i18n("<b>MobileNumber:</b> %1<br>").arg(what->MobileNumber));
        if (!what->HomeNumber.isEmpty()) text.append(i18n("<b>HomeNumber:</b> %1<br>").arg(what->HomeNumber));
        if (!what->Email1.isEmpty()) text.append(i18n("<b>Email1:</b> %1<br>").arg(what->Email1));
        if (!what->Email2.isEmpty()) text.append(i18n("<b>Email2:</b> %1<br>").arg(what->Email2));
        if (!what->Address.isEmpty()) text.append(i18n("<b>Address:</b> %1<br>").arg(what->Address));
        if (!what->City.isEmpty()) text.append(i18n("<b>City:</b> %1<br>").arg(what->City));
        if (!what->State.isEmpty()) text.append(i18n("<b>State:</b> %1<br>").arg(what->State));
        if (!what->Zip.isEmpty()) text.append(i18n("<b>Zip:</b> %1<br>").arg(what->Zip));
        if (!what->Country.isEmpty()) text.append(i18n("<b>Country:</b> %1<br>").arg(what->Country));
        if (!what->Custom1.isEmpty()) text.append(i18n("<b>Custom1:</b> %1<br>").arg(what->Custom1));
        if (!what->Custom2.isEmpty()) text.append(i18n("<b>Custom2:</b> %1<br>").arg(what->Custom2));
        if (!what->Custom3.isEmpty()) text.append(i18n("<b>Custom3:</b> %1<br>").arg(what->Custom3));
        if (!what->Custom4.isEmpty()) text.append(i18n("<b>Custom4:</b> %1<br>").arg(what->Custom4));
        if (!what->Note.isEmpty()) text.append(i18n("<b>Note:</b> %1<br>").arg(what->Note));
        if (what->Private != -1) text.append(i18n("<b>Private:</b> %1<br>").arg(what->Private == 1?i18n("Yes"):i18n("No")));
        text.append(i18n("<b>Storage:</b> %1<br>").arg(i18n("Phone")));
    }
    text.append(i18n("<b>Position:</b> %1").arg(what->Id));
    where->setText(text);
    where->setMinimumHeight(where->contentsHeight()); /* resize to show all contents*/
}

/*
void KAlcatelView::print(QPrinter *pPrinter)
{
  QPainter printpainter;
  printpainter.begin(pPrinter);
	
  // TODO: add your printing code here

  printpainter.end();
}
*/
