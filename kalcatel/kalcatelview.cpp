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

// application specific includes
#include "kalcatelview.h"
#include "kalcateldoc.h"
#include "kalcatel.h"
#include "alcatelclasses.h"
#include "alcatool/sms.h"

KAlcatelView::KAlcatelView(QWidget *parent, const char *name) : KJanusWidget(parent, name, TreeList/*IconList or Tabbed*/)
{
  QStringList list = QStringList();
  setShowIconsInTreeList (true);
//  setTreeListAutoResize (true);

//  setShowAll (true);

  todo = addVBoxPage (i18n("Todos"), i18n("All todos"), SmallIcon("kalcatel-todo.png"));
  todo_list = createListView(todo, alc_todos);

  calendar = addVBoxPage (i18n("Calendar"), i18n("Whole calendar"), SmallIcon("kalcatel-calendar.png"));
  calendar_list = createListView(calendar, alc_calendar);

  contacts = addVBoxPage (i18n("Contacts"), i18n("All contacts"), SmallIcon("kalcatel-contact.png"));
  contacts_list = createListView(contacts, alc_contacts);

  list.append(i18n("Contacts"));
  list.append(i18n("SIM"));
  contacts_sim = addVBoxPage (list, i18n("Contacts on SIM card"), SmallIcon("kalcatel-contact-sim.png"));
  contacts_sim_list = createListView(contacts_sim, alc_contacts_sim);
  list.clear();

  list.append(i18n("Contacts"));
  list.append(i18n("Mobile"));
  contacts_mobile = addVBoxPage (list, i18n("Contacts stored in mobile"), SmallIcon("kalcatel-contact-mobile.png"));
  contacts_mobile_list = createListView(contacts_mobile, alc_contacts_mobile);
  list.clear();

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
  QSplitter *vsplitter = new QSplitter( Qt::Vertical, messages );
  messages_list = createListView(vsplitter, alc_messages);
  connect( messages_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotMessageChanged() ) );
  message = new QTextView( vsplitter );
  message->setBackgroundMode( PaletteBase );
  vsplitter->setResizeMode(message, QSplitter::FollowSizeHint);


  list.append(i18n("Messages"));
  list.append(i18n("Unread"));
  msg_unread = addVBoxPage (list, i18n("Unread messages"), SmallIcon("kalcatel-message-unread.png"));
  vsplitter = new QSplitter( Qt::Vertical, msg_unread );
  msg_unread_list = createListView(vsplitter, alc_messages_in);
  list.clear();
  connect( msg_unread_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotUnreadMessageChanged() ) );
  message_unread = new QTextView( vsplitter );
  message_unread->setBackgroundMode( PaletteBase );
  vsplitter->setResizeMode(message_unread, QSplitter::FollowSizeHint);

  list.append(i18n("Messages"));
  list.append(i18n("Read"));
  msg_read = addVBoxPage (list, i18n("Read messages"), SmallIcon("kalcatel-message-read.png"));
  vsplitter = new QSplitter( Qt::Vertical, msg_read );
  msg_read_list = createListView(vsplitter, alc_messages_in);
  list.clear();
  connect( msg_read_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotReadMessageChanged() ) );
  message_read = new QTextView( vsplitter );
  message_read->setBackgroundMode( PaletteBase );
  vsplitter->setResizeMode(message_read, QSplitter::FollowSizeHint);

  list.append(i18n("Messages"));
  list.append(i18n("Sent"));
  msg_sent = addVBoxPage (list, i18n("Sent messages"), SmallIcon("kalcatel-message-sent.png"));
  vsplitter = new QSplitter( Qt::Vertical, msg_sent );
  msg_sent_list = createListView(vsplitter, alc_messages_out);
  list.clear();
  connect( msg_sent_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotSentMessageChanged() ) );
  message_sent = new QTextView( vsplitter );
  message_sent->setBackgroundMode( PaletteBase );
  vsplitter->setResizeMode(message_sent, QSplitter::FollowSizeHint);

  list.append(i18n("Messages"));
  list.append(i18n("Unsent"));
  msg_unsent = addVBoxPage (list, i18n("Unsent messages"), SmallIcon("kalcatel-message-unsent.png"));
  vsplitter = new QSplitter( Qt::Vertical, msg_unsent );
  msg_unsent_list = createListView(vsplitter, alc_messages_out);
  list.clear();
  connect( msg_unsent_list, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( slotUnsentMessageChanged() ) );
  message_unsent = new QTextView( vsplitter );
  message_unsent->setBackgroundMode( PaletteBase );
  vsplitter->setResizeMode(message_unsent, QSplitter::FollowSizeHint);
}

KAlcatelView::~KAlcatelView()
{
}

KListView *KAlcatelView::createListView(QWidget *parent, AlcListType type)
{
    KListView *list;
    list = new KListView(parent);

    list->setAllColumnsShowFocus(true);
    list->setShowSortIndicator(true);

    switch (type) {
        case alc_contacts:
        case alc_contacts_sim:
            list->addColumn(i18n("Name"));
            list->addColumn(i18n("Phone"));
            break;
        case alc_contacts_mobile:
            list->addColumn(i18n("Last name"));
            list->addColumn(i18n("First name"));
            list->addColumn(i18n("Category"));
            list->addColumn(i18n("Mobile"));
            list->addColumn(i18n("Work"));
            list->addColumn(i18n("Main"));
            list->addColumn(i18n("Email"));
/*            list->addColumn(i18n(""));
            list->addColumn(i18n(""));
            list->addColumn(i18n(""));
            list->addColumn(i18n(""));
    "LastName",
    "FirstName",
    "Company",
    "JobTitle",
    "Note",
    "Category",
    "Private",
    "WorkNumber",
    "MainNumber",
    "FaxNumber",
    "OtherNumber",
    "PagerNumber",
    "MobileNumber",
    "HomeNumber",
    "Email1",
    "Email2",
    "Address",
    "City",
    "State",
    "Zip",
    "Coutry",
    "Custom1",
    "Custom2",
    "Custom3",
    "Custom4"*/
            break;
        case alc_contacts_mobile_cat:
            list->addColumn(i18n("Last name"));
            list->addColumn(i18n("First name"));
            list->addColumn(i18n("Mobile"));
            list->addColumn(i18n("Work"));
            list->addColumn(i18n("Main"));
            list->addColumn(i18n("Email"));
            break;
        case alc_todos:
            list->addColumn(i18n("Completed"));
            list->addColumn(i18n("Priority"));
            list->addColumn(i18n("Due date"));
            list->addColumn(i18n("Subject"));
            list->addColumn(i18n("Category"));
/*
            list->addColumn(i18n(""));
    "DueDate",
    "Completed",
    "AlarmDate",
    "AlarmTime",
    "Subject",
    "Private",
    "Category",
    "Priority",
    "ContactID"
*/
            break;
        case alc_calendar:
            list->addColumn(i18n("Date"));
            list->addColumn(i18n("Start"));
            list->addColumn(i18n("End"));
            list->addColumn(i18n("Subject"));
            list->addColumn(i18n("Alarm"));
            list->addColumn(i18n("Repeat"));
            /*
            list->addColumn(i18n(""));
            list->addColumn(i18n(""));
    "Date",
    "StartTime",
    "EndTime",
    "AlarmDate",
    "AlarmTime",
    "Subject",
    "Private",
    "EventType",
    "ContactID",
    "KNOWN UNKNOWN (9)",      I haven't seen this yet
    "DayOfWeek",
    "Day",
    "WeekOfMonth",
    "Month",
    "KNOWN UNKNOWN (14)",     I haven't seen this yet
    "KNOWN UNKNOWN (15)",     I haven't seen this yet
    "KNOWN UNKNOWN (16)",     I haven't seen this yet
    "Frequency",
    "StartDate",
    "StopDate",
     Following two were created by IntelliSync, but it couldn't read it back...
    "KNOWN UNKNOWN (21)",     this contains date, probably same as AlarmDate
    "KNOWN UNKNOWN (22)"      this contains time, probably same as AlarmTime
*/
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

KAlcatelDoc *KAlcatelView::getDocument() const
{
  KAlcatelApp *theApp=(KAlcatelApp *) parentWidget();

  return theApp->getDocument();
}

void KAlcatelView::repaint() {
    KAlcatelDoc *doc = getDocument();
    int unread_sms = 0;
    if (doc->getVersion() != docVersion) {
        docVersion = doc->getVersion();

        if (doc->getSMSVersion() != smsVersion) {
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
                QListViewItem *newItem = new QListViewItem (messages_list,
                        QString((* it).Sender),
                        QString("")/*TODO: get ii from contacts... (* it).Sender*/,
                        type,
                        QString((* it).Date.date().toString()),
                        QString((* it).Date.time().toString()),
                        QString((* it).Text),
                        QString().sprintf("%d", (* it).Id));

                newItem = new QListViewItem (list,
                        QString((* it).Sender),
                        QString("")/*TODO: get ii from contacts... (* it).Sender*/,
                        QString((* it).Date.date().toString()),
                        QString((* it).Date.time().toString()),
                        QString((* it).Text),
                        QString().sprintf("%d", (* it).Id));
            } /* for cycle over sms */
            if (unread_sms)
                showPage(10);
            else
                showPage(9);
        } /* change in sms */
    } /* any change */
    KJanusWidget::repaint();
}

void KAlcatelView::slotMessageChanged() {
    QListViewItem *i = messages_list->currentItem();
    if ( !i ) return;
    slotShowMessage(message, findAlcatelSMSById(getDocument()->sms, i->text(6).toInt()));
}

void KAlcatelView::slotReadMessageChanged() {
    QListViewItem *i = msg_read_list->currentItem();
    if ( !i ) return;
    slotShowMessage(message_read, findAlcatelSMSById(getDocument()->sms, i->text(5).toInt()));
}

void KAlcatelView::slotUnreadMessageChanged() {
    QListViewItem *i = msg_unread_list->currentItem();
    if ( !i ) return;
    slotShowMessage(message_unread, findAlcatelSMSById(getDocument()->sms, i->text(5).toInt()));
}

void KAlcatelView::slotSentMessageChanged() {
    QListViewItem *i = msg_sent_list->currentItem();
    if ( !i ) return;
    slotShowMessage(message_sent, findAlcatelSMSById(getDocument()->sms, i->text(5).toInt()));
}

void KAlcatelView::slotUnsentMessageChanged() {
    QListViewItem *i = msg_unsent_list->currentItem();
    if ( !i ) return;
    slotShowMessage(message_sent, findAlcatelSMSById(getDocument()->sms, i->text(5).toInt()));
}

void KAlcatelView::slotShowMessage(QTextView *where, AlcatelSMS *what) {
    where->setText( i18n(
        "<b>From:</b> %1 (%2)<br>"
        "<b>Date:</b> %3<br>"
        "<b>Time:</b> %4<br>"
        "<b>SMSC:</b> %5<br>"
        "<b>Position:</b> %6<br>"
        "<b>Type:</b> %7<br><br>"
        "%8").
        arg(""). /*TODO: decode using contacts ...*/
        arg(what->Sender).
        arg(what->Date.date().toString()).
        arg(what->Date.time().toString()).
        arg(what->SMSC).
        arg(what->Id).
        arg(SMSTypes[what->Status]).
        arg(what->Text));
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
