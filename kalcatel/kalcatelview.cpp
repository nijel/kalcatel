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

// include files for KDE
#include <kjanuswidget.h>
#include <klocale.h>
#include <klistview.h>
#include <kiconloader.h>

// application specific includes
#include "kalcatelview.h"
#include "kalcateldoc.h"
#include "kalcatel.h"

KAlcatelView::KAlcatelView(QWidget *parent, const char *name) : KJanusWidget(parent, name, TreeList/*IconList or Tabbed*/)
{
  QStringList list = QStringList();
  setShowIconsInTreeList (true);
  setTreeListAutoResize (true);

  todo = addVBoxPage (i18n("Todos"), i18n("All todos"), UserIcon("todo.png"));
  todo_list = createListView(todo, alc_todos);

  calendar = addVBoxPage (i18n("Calendar"), i18n("Whole calendar"), UserIcon("calendar.png"));
  calendar_list = createListView(calendar, alc_calendar);

  contacts = addVBoxPage (i18n("Contacts"), i18n("All contacts"), UserIcon("contact.png"));
  contacts_list = createListView(contacts, alc_contacts);

  list.append(i18n("Contacts"));
  list.append(i18n("SIM"));
  contacts_sim = addVBoxPage (list, i18n("Contacts on SIM card"), UserIcon("contact-sim.png"));
  contacts_sim_list = createListView(contacts_sim, alc_contacts_sim);
  list.clear();

  list.append(i18n("Contacts"));
  list.append(i18n("Mobile"));
  contacts_mobile = addVBoxPage (list, i18n("Contacts stored in mobile"), UserIcon("contact-mobile.png"));
  contacts_mobile_list = createListView(contacts_mobile, alc_contacts_mobile);
  list.clear();

  calls = addVBoxPage (i18n("Calls"), i18n("All calls"), UserIcon("call.png"));
  calls_list = createListView(calls, alc_calls);

  list.append(i18n("Calls"));
  list.append(i18n("Missed"));
  calls_mc = addVBoxPage (list, i18n("Missed calls"), UserIcon("call-missed.png"));
  calls_mc_list = createListView(calls_mc, alc_calls_type);
  list.clear();

  list.append(i18n("Calls"));
  list.append(i18n("Received"));
  calls_rc = addVBoxPage (list, i18n("Received calls"), UserIcon("call-received.png"));
  calls_rc_list = createListView(calls_rc, alc_calls_type);
  list.clear();

  list.append(i18n("Calls"));
  list.append(i18n("Outgoing"));
  calls_ld = addVBoxPage (list, i18n("Outgoing calls"), UserIcon("call-outgoing.png"));
  calls_ld_list = createListView(calls_ld, alc_calls_type);
  list.clear();

  messages = addVBoxPage (i18n("Messages"), i18n("All messages"), UserIcon("message.png"));
  messages_list = createListView(messages, alc_messages);

  list.append(i18n("Messages"));
  list.append(i18n("Unread"));
  msg_unread = addVBoxPage (list, i18n("Unread messages"), UserIcon("message-unread.png"));
  msg_unread_list = createListView(msg_unread, alc_messages_in);
  list.clear();

  list.append(i18n("Messages"));
  list.append(i18n("Read"));
  msg_read = addVBoxPage (list, i18n("Read messages"), UserIcon("message-read.png"));
  msg_read_list = createListView(msg_read, alc_messages_in);
  list.clear();

  list.append(i18n("Messages"));
  list.append(i18n("Sent"));
  msg_sent = addVBoxPage (list, i18n("Sent messages"), UserIcon("message-sent.png"));
  msg_sent_list = createListView(msg_sent, alc_messages_out);
  list.clear();

  list.append(i18n("Messages"));
  list.append(i18n("Unsent"));
  msg_unsent = addVBoxPage (list, i18n("Unsent messages"), UserIcon("message-unsent.png"));
  msg_unsent_list = createListView(msg_unsent, alc_messages_out);
  list.clear();
}

KAlcatelView::~KAlcatelView()
{
}

KListView *KAlcatelView::createListView(QVBox *parent, AlcListType type)
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
            list->addColumn(i18n("Text"));
            list->addColumn(i18n("Position"));
            break;
        case alc_messages_in:
            list->addColumn(i18n("From"));
            list->addColumn(i18n("From (name)"));
            list->addColumn(i18n("Date"));
            list->addColumn(i18n("Text"));
            list->addColumn(i18n("Position"));
            break;
        case alc_messages_out:
            list->addColumn(i18n("To"));
            list->addColumn(i18n("To (name)"));
            list->addColumn(i18n("Date"));
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
/*
void KAlcatelView::print(QPrinter *pPrinter)
{
  QPainter printpainter;
  printpainter.begin(pPrinter);
	
  // TODO: add your printing code here

  printpainter.end();
}
*/