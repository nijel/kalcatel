/*
 * kalcateldoc.cpp
 *
 * "document" for KAlcatel - here are all data
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

#include <stdlib.h>
#include <unistd.h>

// include files for Qt
#include <qdir.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qdatetime.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/job.h>
#include <kio/netaccess.h>

// application specific includes
#include "kalcateldoc.h"
#include "kalcatel.h"
#include "kalcatelview.h"

#include "alcatelclasses.h"

// alcatool includes
#include "alcatool/modem.h"
#include "alcatool/mobile_info.h"
#include "alcatool/logging.h"
#include "alcatool/version.h"
#include "alcatool/charset.h"
#include "alcatool/sms.h"
#include "alcatool/common.h"
#include "alcatool/pdu.h"
#include "alcatool/alcatel.h"

QList<KAlcatelView> *KAlcatelDoc::pViewList = 0L;

KAlcatelDoc::KAlcatelDoc(QWidget *parent, const char *name) : QObject(parent, name)
{
  if(!pViewList)
  {
    pViewList = new QList<KAlcatelView>();
  }

  pViewList->setAutoDelete(true);

  todo_cats = new AlcatelCategoryList();
  contact_cats = new AlcatelCategoryList();

  contacts = new AlcatelContactList();
  calendar = new AlcatelCalendarList();
  todo = new AlcatelTodoList();
  sms = new AlcatelSMSList();
}

KAlcatelDoc::~KAlcatelDoc()
{
}

void KAlcatelDoc::addView(KAlcatelView *view)
{
  pViewList->append(view);
}

void KAlcatelDoc::removeView(KAlcatelView *view)
{
  pViewList->remove(view);
}
void KAlcatelDoc::setURL(const KURL &url)
{
  doc_url=url;
}

const KURL& KAlcatelDoc::URL() const
{
  return doc_url;
}

void KAlcatelDoc::slotUpdateAllViews(KAlcatelView *sender)
{
  KAlcatelView *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
      if(w!=sender)
        w->repaint();
    }
  }

}

bool KAlcatelDoc::saveModified()
{
  bool completed=true;

  if(modified)
  {
    KAlcatelApp *win=(KAlcatelApp *) parent();
    int want_save = KMessageBox::warningYesNoCancel(win,
                                         i18n("The current file has been modified.\n"
                                              "Do you want to save it?"),
                                         i18n("Warning"));
    switch(want_save)
    {
      case KMessageBox::Yes:
           if (doc_url.fileName() == i18n("Untitled"))
           {
             win->slotFileSaveAs();
           }
           else
           {
             saveDocument(URL());
       	   };

       	   deleteContents();
           completed=true;
           break;

      case KMessageBox::No:
           setModified(false);
           deleteContents();
           completed=true;
           break;

      case KMessageBox::Cancel:
           completed=false;
           break;

      default:
           completed=false;
           break;
    }
  }

  return completed;
}

void KAlcatelDoc::closeDocument()
{
  deleteContents();
}

bool KAlcatelDoc::newDocument()
{
  /////////////////////////////////////////////////
  // TODO: Add your document initialization code here
  /////////////////////////////////////////////////
  modified=false;
  doc_url.setFileName(i18n("Untitled"));

  return true;
}

bool KAlcatelDoc::openDocument(const KURL& url, const char *format /*=0*/)
{
  QString tmpfile;
  KIO::NetAccess::download( url, tmpfile );
  /////////////////////////////////////////////////
  // TODO: Add your document opening code here
  /////////////////////////////////////////////////

  KIO::NetAccess::removeTempFile( tmpfile );

  modified=false;
  return true;
}

int KAlcatelDoc::readMobileItems(alc_type sync, alc_type type) {
    alc_type *result;
    int i, j;
    int *ids, *items;
    FIELD *field;

    int count;

    KAlcatelApp *win=(KAlcatelApp *) parent();

    switch (sync) {
        case ALC_SYNC_CALENDAR:
            count = ALC_CALENDAR_FIELDS;
            calendar->clear();
            break;
        case ALC_SYNC_TODO:
            count = ALC_TODO_FIELDS;
            todo->clear();
            break;
        case ALC_SYNC_CONTACTS:
            count = ALC_CONTACTS_FIELDS;
            contacts->clear();
            break;
    }

    alcatel_attach();

    sync_start_session();
    if (sync_select_type(type) == 0) {
        sync_begin_read(sync);

        ids = sync_get_ids(type);
        if (ids == NULL) {
            sync_close_session(type);
            alcatel_detach();
            return false;
        }

        message(MSG_INFO, "Received %d ids", ids[0]);

        for (i = 1; i <= ids[0]; i++) {
            win->slotStatusMsg(i18n("Reading item %1 of %2").arg(i).arg(ids[0]),ID_DETAIL_MSG);
            message(MSG_DEBUG, "Reading id[%d] = %d", i-1, ids[i]);
            items = sync_get_fields(type, ids[i]);
            if (!items) return false;
            message(MSG_INFO, "Receiving data for item %d (%d fields)", ids[i], items[0]);
            printf ("Item %d (fields: %d):\n", ids[i], items[0]);
            AlcatelContact *Contact;
            AlcatelTodo *Todo;
            AlcatelCalendar *Calendar;
            switch (sync) {
                case ALC_SYNC_CALENDAR:
                    Calendar = new AlcatelCalendar();
                    Calendar->Id = ids[i];
                    break;
                case ALC_SYNC_TODO:
                    Todo = new AlcatelTodo();
                    Todo->Id = ids[i];
                    break;
                case ALC_SYNC_CONTACTS:
                    Contact = new AlcatelContact();
                    Contact->Storage = AlcatelContact::Mobile;
                    Contact->Id = ids[i];
                    break;
            }
            for (j = 1; j <= items[0]; j++) {
                message(MSG_DEBUG, "items[%d] = %d", j-1, items[j]);
                result = sync_get_field_value(type, ids[i], items[j]);
                if (!result) return false;
                field = decode_field_value(result);
                if (items[j] >= count) {
                    message(MSG_WARNING, "Unknown field %02d, ignoring!",items[j]);
                }
                if (field == NULL) {
                    message(MSG_WARNING, "Unknown field type(%02X %02X), ingnoring", result[1], result[2]);
                } else {
                    switch (sync) {
                        case ALC_SYNC_CALENDAR:
                            Calendar->setField(items[j], field);
                            break;
                        case ALC_SYNC_TODO:
                            Todo->setField(items[j], field);
                            break;
                        case ALC_SYNC_CONTACTS:
                            Contact->setField(items[j], field);
                            break;
                    }
                }
                free(result);
            }
            free(items);
            switch (sync) {
                case ALC_SYNC_CALENDAR:
                    calendar->append(*Calendar);
                    break;
                case ALC_SYNC_TODO:
                    todo->append(*Todo);
                    break;
                case ALC_SYNC_CONTACTS:
                    contacts->append(*Contact);
                    break;
            }
        }
        free(ids);
    } else {
        message(MSG_ERROR, "Can not open sync session!");
        sync_close_session(type);
        alcatel_detach();
        return false;
    }

    sync_close_session(type);
    alcatel_detach();
    return true;
}

int KAlcatelDoc::readMobileCategories(AlcatelCategoryList *strList, alc_type sync, alc_type type, alc_type cat) {
    int *list;
    int i;
    char *result;

    alcatel_attach();

    sync_start_session();

    if (sync_select_type(type) == 0) {
        sync_begin_read(sync);

        list = sync_get_obj_list(type, cat);

        message(MSG_INFO, "Received %d categories:", list[0]);

        strList->clear();

        for (i = 1; i <= list[0]; i++) {
            result = sync_get_obj_list_item(type, cat, list[i]);
            strList->append(AlcatelCategory(result, list[i]));
            message (MSG_DEBUG, "Read category name: %02d: %s", list[i],  result);
            free(result);
        }

        free(list);
        i = true;
    } else {
        message(MSG_ERROR, "Can not open sync session!");
        i = false;
    }

    sync_close_session(type);
    alcatel_detach();
    return i;
}

bool KAlcatelDoc::readMobile(AlcReadType what = alcatel_read_all, int category = -1)
{
    /* TODO: this should be in configuration */
    char default_device[] = "/dev/ttyS1";
    char default_lock[] = "/var/lock/LCK..%s";
    char default_init[] = "AT S7=45 S0=0 L1 V1 X4 &c1 E1 Q0";
    int default_rate = 19200;
    char *devname;
    int i;

    KAlcatelApp *win=(KAlcatelApp *) parent();

    msg_level = MSG_DEBUG;

    strcpy(initstring, default_init);
    strcpy(device, default_device);
    devname = strrchr(device, '/');
    devname++;
    sprintf(lockname, default_lock, devname);
    rate=default_rate;

    switch (rate) {
        case 2400:   baudrate=B2400; break;
        case 4800:   baudrate=B4800; break;
        case 9600:   baudrate=B9600; break;
        case 19200:  baudrate=B19200; break;
        case 38400:  baudrate=B38400; break;
        default:
            message(MSG_ERROR,"Ivalid baud rate (%d), setting to default (19200)!", rate);
            baudrate=B19200;
    }


     win->slotStatusMsg(i18n("Opening modem"),ID_DETAIL_MSG);
     if (!modem_open()) {
         switch (modem_errno) {
             case ERR_MDM_OPEN:
                 KMessageBox::error(win, i18n("Failed opening modem for read/write."), i18n("Error"));
                 modem_close();
                 return false;
             case ERR_MDM_LOCK:
                 KMessageBox::error(win, i18n("Modem locked."), i18n("Error"));
                 modem_close();
                 return false;
             case ERR_MDM_LOCK_OPEN:
                 KMessageBox::error(win, i18n("Can not open modem lock."), i18n("Error"));
                 modem_close();
                 return false;
             default:
                 KMessageBox::error(win, i18n("Failed opening modem.\nUnknown error (%1).").arg(modem_errno), i18n("Error"));
                 modem_close();
                 return false;
         }
         return false;
     }
     win->slotStatusMsg(i18n("Setting serial port"),ID_DETAIL_MSG);
     modem_setup();
     win->slotStatusMsg(i18n("Initializing modem"),ID_DETAIL_MSG);
     if (!modem_init()) {
         switch (modem_errno) {
             case ERR_MDM_AT:
                 KMessageBox::error(win, i18n("Modem doesn't react on AT command."), i18n("Error"));
                 modem_close();
                 return false;
             case ERR_MDM_PDU:
                 KMessageBox::error(win, i18n("Failed selecting PDU mode."), i18n("Error"));
                 modem_close();
                 return false;
             default:
                 KMessageBox::error(win, i18n("Failed initializing modem.\nUnknown error (%1).").arg(modem_errno), i18n("Error"));
                 modem_close();
                 return false;
         }
         return false;
     }

    if (what == alcatel_read_sms || what == alcatel_read_all) {
        SMS *msg;
        win->slotStatusMsg(i18n("Reading messages"),ID_DETAIL_MSG);
        sms->clear();
        msg = get_smss();

        i = 0;
        while (msg[i].pos != -1) {
            AlcatelSMS *Msg = new AlcatelSMS();
            Msg->Date = QDateTime();
            Msg->Date.setTime_t(msg[i].date);
            Msg->Length = msg[i].len;
            Msg->Id = msg[i].pos;
            Msg->Raw = strdup(msg[i].raw);
            free(msg[i].raw);
            Msg->SMSC = QString(msg[i].smsc);
            free(msg[i].smsc);
            Msg->Sender = QString(msg[i].sendr);
            free(msg[i].sendr);
            Msg->Status = msg[i].stat;
            Msg->Text = QString(msg[i].ascii);
            free(msg[i].ascii);
            sms->append(*Msg);
            i++;
        }
        free(msg);

        win->slotStatusMsg(i18n("SMS messages read"),ID_DETAIL_MSG);
        smsVersion++;
    }

    if (what == alcatel_read_calls || what == alcatel_read_all) {
        KMessageBox::sorry(win, i18n("Reading calls is not implemented yet..."), i18n("Sorry"));
    }

    if (what == alcatel_read_contacts_sim || what == alcatel_read_all) {
        KMessageBox::sorry(win, i18n("Reading contacts from SIM is not implemented yet..."), i18n("Sorry"));
    }

    if (what != alcatel_read_sms || what == alcatel_read_all) {
        win->slotStatusMsg(i18n("Opening binary mode"),ID_DETAIL_MSG);
        alcatel_init();

        if (what == alcatel_read_contacts_mobile || what == alcatel_read_all) {
            /* at first read categories */
            win->slotStatusMsg(i18n("Reading contact categories"),ID_DETAIL_MSG);
            if (!readMobileCategories(contact_cats, ALC_SYNC_CONTACTS, ALC_SYNC_TYPE_CONTACTS, ALC_LIST_CONTACTS_CAT))
                KMessageBox::error(win, i18n("Reading contacts categories failed!"), i18n("Error"));

            win->slotStatusMsg(i18n("Reading contact items"),ID_DETAIL_MSG);
            if (!readMobileItems(ALC_SYNC_CONTACTS, ALC_SYNC_TYPE_CONTACTS))
                KMessageBox::error(win, i18n("Reading contacts failed!"), i18n("Error"));

            contactVersion++;
        }

        if (what == alcatel_read_calendar || what == alcatel_read_all) {
            win->slotStatusMsg(i18n("Reading calendar items"),ID_DETAIL_MSG);
            if (!readMobileItems(ALC_SYNC_CALENDAR, ALC_SYNC_TYPE_CALENDAR))
                KMessageBox::error(win, i18n("Reading calendar items failed!"), i18n("Error"));
            calendarVersion++;
        }

        if (what == alcatel_read_todo || what == alcatel_read_all) {
            /* at first read categories */
            win->slotStatusMsg(i18n("Reading todo categories"),ID_DETAIL_MSG);
            if (!readMobileCategories(todo_cats, ALC_SYNC_TODO, ALC_SYNC_TYPE_TODO, ALC_LIST_TODO_CAT))
                KMessageBox::error(win, i18n("Reading todo categories failed!"), i18n("Error"));

            win->slotStatusMsg(i18n("Reading todo items"),ID_DETAIL_MSG);
            if (!readMobileItems(ALC_SYNC_TODO, ALC_SYNC_TYPE_TODO))
                KMessageBox::error(win, i18n("Reading todos failed!"), i18n("Error"));

            todoVersion++;
        }

        win->slotStatusMsg(i18n("Closing binary mode"),ID_DETAIL_MSG);
        alcatel_done();
    }
    modem_close();
    win->slotStatusMsg(i18n("Items read"),ID_DETAIL_MSG);

    version++;
    modified=true;
    slotUpdateAllViews(NULL);

    return true;
}

bool KAlcatelDoc::saveDocument(const KURL& url, const char *format /*=0*/) {
    /////////////////////////////////////////////////
    // TODO: Add your document saving code here
    /////////////////////////////////////////////////

    KMessageBox::sorry((KAlcatelApp *) parent(), i18n("Saving not implemented yet..."), i18n("Sorry"));

    modified=false;
    return true;
}

void KAlcatelDoc::deleteContents() {
    todo_cats->clear();
    contact_cats->clear();

    contacts->clear();
    calendar->clear();
    todo->clear();
    sms->clear();

    modified=true;
    version++;
    todoVersion++;
    callVersion++;
    calendarVersion++;
    contactVersion++;
    smsVersion++;
    slotUpdateAllViews(NULL);
    /* TODO here is maybe something missing.. */
}

int KAlcatelDoc::getVersion() {
    return version;
}

int KAlcatelDoc::getSMSVersion() {
    return smsVersion;
}

int KAlcatelDoc::getCallVersion() {
    return callVersion;
}

int KAlcatelDoc::getContactVersion() {
    return contactVersion;
}

int KAlcatelDoc::getCalendarVersion() {
    return calendarVersion;
}

int KAlcatelDoc::getTodoVersion() {
    return todoVersion;
}
