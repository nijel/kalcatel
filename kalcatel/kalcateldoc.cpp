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
 * if and only if programmer/distributor of that code receives written
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
#include <qtextstream.h>
#include <qregexp.h>

#include <qdom.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <ktempfile.h>
#include <kio/job.h>
#include <kio/netaccess.h>

// application specific includes
#include "kalcateldoc.h"
#include "kalcatel.h"
#include "kalcatelview.h"

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
#include "alcatool/contacts.h"

QList<KAlcatelView> *KAlcatelDoc::pViewList = 0L;

KAlcatelDoc::KAlcatelDoc(QWidget *parent, const char *name) : QObject(parent, name) {
    if(!pViewList) {
        pViewList = new QList<KAlcatelView>();
    }

    pViewList->setAutoDelete(true);

    todo_cats = new AlcatelCategoryList();
    contact_cats = new AlcatelCategoryList();

    contacts = new AlcatelContactList();
    calendar = new AlcatelCalendarList();
    todos = new AlcatelTodoList();
    messages = new AlcatelMessageList();

    calls = new AlcatelCallList();

/* I hope that phone doesn't support more than 1000 contact IDs, which should allways have original ID */
    pcStorageCounter = 1000;
}

KAlcatelDoc::~KAlcatelDoc() {
    delete todo_cats;
    delete contact_cats;
    delete contacts;
    delete calendar;
    delete todos;
    delete messages;
    delete calls;
}

void KAlcatelDoc::addView(KAlcatelView *view) {
    pViewList->append(view);
}

void KAlcatelDoc::removeView(KAlcatelView *view) {
    pViewList->remove(view);
}

void KAlcatelDoc::setURL(const KURL &url) {
    doc_url=url;
}

const KURL& KAlcatelDoc::URL() const {
    return doc_url;
}

void KAlcatelDoc::slotUpdateAllViews(KAlcatelView *sender) {
    KAlcatelView *w;
    if(pViewList) {
        for(w=pViewList->first(); w!=0; w=pViewList->next()) {
            if(w!=sender)
                w->repaint();
        }
    }
}

bool KAlcatelDoc::saveModified() {
    bool completed=true;

    if(modified) {
        KAlcatelApp *win=(KAlcatelApp *) parent();
        int want_save = KMessageBox::warningYesNoCancel(win,
                                             i18n("The current file has been modified.\n"
                                                  "Do you want to save it?"),
                                             i18n("Warning"));
        switch(want_save){
            case KMessageBox::Yes:
                if (doc_url.fileName() == i18n("Untitled")) {
                    win->slotFileSaveAs();
                } else {
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

void KAlcatelDoc::closeDocument() {
    deleteContents();
}

bool KAlcatelDoc::newDocument() {
    /////////////////////////////////////////////////
    // TODO: Add your document initialization code here
    /////////////////////////////////////////////////
    deleteContents();
    modified=false;
    doc_url.setFileName(i18n("Untitled"));

    return true;
}


QDateTime KAlcatelDoc::readDomDateTime(QDomElement el) {
    QDateTime date;
    QDomNode n = el.firstChild();
    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "date") {
                date.setDate(readDomDate(l));
            } else if (l.tagName() == "time") {
                date.setTime(readDomTime(l));
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    return date;
}

QDate KAlcatelDoc::readDomDate(QDomElement el) {
    QDate date;
    date.setYMD(2002,1,31); // need to set date, month set here MUST have 31 days...
    QDomNode n = el.firstChild();
    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "day") {
                date.setYMD(date.year(), date.month(), readDomInt(l));
            } else if (l.tagName() == "month") {
                date.setYMD(date.year(), readDomInt(l), date.day());
            } else if (l.tagName() == "year") {
                date.setYMD(readDomInt(l), date.month(), date.day());
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    return date;
}

QTime KAlcatelDoc::readDomTime(QDomElement el) {
    QTime time;
    QDomNode n = el.firstChild();
    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "second") {
                time.setHMS(time.hour(), time.minute(), readDomInt(l));
            } else if (l.tagName() == "minute") {
                time.setHMS(time.hour(), readDomInt(l), time.second());
            } else if (l.tagName() == "hour") {
                time.setHMS(readDomInt(l), time.minute(), time.second());
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    return time;
}

QString KAlcatelDoc::readDomString(QDomElement el) {
    QDomNode i = el.firstChild();
    QDomText t = i.toText();
    if (!t.isNull()) {
        return t.data();
    } else {
        return "";
    }
}

int KAlcatelDoc::readDomInt(QDomElement el) {
    QDomNode i = el.firstChild();
    QDomText t = i.toText();
    if (!t.isNull()) {
        return t.data().toInt();
    } else {
        return 0;
    }
}

void KAlcatelDoc::readDomMessage(QDomElement el) {
    QDomNode n = el.firstChild();
    AlcatelMessage Msg;
    Msg.Id = pcStorageCounter++;
    Msg.Storage = StoragePC;

    QString del = el.attribute("deleted");
    if (del == "yes") {
        Msg.Deleted = true;
    }
    QString mod = el.attribute("modified");
    if (mod == "yes") {
        Msg.Modified = true;
    }
    QString cre = el.attribute("created");
    if (cre == "yes") {
        Msg.Created = true;
    }

    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "id") {
                Msg.PrevId = readDomInt(l);
            } else if (l.tagName() == "storage") {
                Msg.PrevStorage = (AlcatelStorage)readDomInt(l);
            } else if (l.tagName() == "status") {
                Msg.Status = readDomInt(l);
            } else if (l.tagName() == "length") {
                Msg.Length = readDomInt(l);
            } else if (l.tagName() == "raw") {
                Msg.Raw = readDomString(l).latin1();
            } else if (l.tagName() == "sender") {
                Msg.Sender = readDomString(l);
            } else if (l.tagName() == "text") {
                Msg.Text = readDomString(l);
            } else if (l.tagName() == "smsc") {
                Msg.SMSC = readDomString(l);
            } else if (l.tagName() == "date") {
                Msg.Date = readDomDateTime(l);
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    ::message(MSG_DEBUG2, "Read message %d", Msg.Id);
    messages->append(Msg);
}

void KAlcatelDoc::readDomContact(QDomElement el) {
    QDomNode n = el.firstChild();
    AlcatelContact Cont;
    Cont.Storage = StoragePC;

    QString del = el.attribute("deleted");
    if (del == "yes") {
        Cont.Deleted = true;
    }
    QString mod = el.attribute("modified");
    if (mod == "yes") {
        Cont.Modified = true;
    }
    QString cre = el.attribute("created");
    if (cre == "yes") {
        Cont.Created = true;
    }

    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "id") {
                Cont.PrevId = readDomInt(l);
            } else if (l.tagName() == "storage") {
                Cont.PrevStorage = (AlcatelStorage)readDomInt(l);
            } else if (l.tagName() == "lastname") {
                Cont.LastName = readDomString(l);
            } else if (l.tagName() == "firstname") {
                Cont.FirstName = readDomString(l);
            } else if (l.tagName() == "company") {
                Cont.Company = readDomString(l);
            } else if (l.tagName() == "jobtitle") {
                Cont.JobTitle = readDomString(l);
            } else if (l.tagName() == "note") {
                Cont.Note = readDomString(l);
            } else if (l.tagName() == "category") {
                Cont.Category = readDomInt(l);
            } else if (l.tagName() == "private") {
                Cont.Private = readDomInt(l);
            } else if (l.tagName() == "worknumber") {
                Cont.WorkNumber = readDomString(l);
            } else if (l.tagName() == "mainnumber") {
                Cont.MainNumber = readDomString(l);
            } else if (l.tagName() == "faxnumber") {
                Cont.FaxNumber = readDomString(l);
            } else if (l.tagName() == "othernumber") {
                Cont.OtherNumber = readDomString(l);
            } else if (l.tagName() == "pagernumber") {
                Cont.PagerNumber = readDomString(l);
            } else if (l.tagName() == "mobilenumber") {
                Cont.MobileNumber = readDomString(l);
            } else if (l.tagName() == "homenumber") {
                Cont.HomeNumber = readDomString(l);
            } else if (l.tagName() == "email1") {
                Cont.Email1 = readDomString(l);
            } else if (l.tagName() == "email2") {
                Cont.Email2= readDomString(l);
            } else if (l.tagName() == "address") {
                Cont.Address = readDomString(l);
            } else if (l.tagName() == "city") {
                Cont.City = readDomString(l);
            } else if (l.tagName() == "state") {
                Cont.State = readDomString(l);
            } else if (l.tagName() == "zip") {
                Cont.Zip = readDomString(l);
            } else if (l.tagName() == "country") {
                Cont.Country = readDomString(l);
            } else if (l.tagName() == "custom1") {
                Cont.Custom1 = readDomString(l);
            } else if (l.tagName() == "custom2") {
                Cont.Custom2 = readDomString(l);
            } else if (l.tagName() == "custom3") {
                Cont.Custom3 = readDomString(l);
            } else if (l.tagName() == "custom4") {
                Cont.Custom4 = readDomString(l);
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    if (Cont.PrevStorage == StorageMobile) {
        Cont.Id = Cont.PrevId;
    } else {
        Cont.Id = pcStorageCounter++;
    }
    ::message(MSG_DEBUG2, "Read contact %d", Cont.Id);
    contacts->append(Cont);
}

void KAlcatelDoc::readDomEvent(QDomElement el) {
    QDomNode n = el.firstChild();
    AlcatelCalendar Cal;
    Cal.Id = pcStorageCounter++;
    Cal.Storage = StoragePC;

    QString del = el.attribute("deleted");
    if (del == "yes") {
        Cal.Deleted = true;
    }
    QString mod = el.attribute("modified");
    if (mod == "yes") {
        Cal.Modified = true;
    }
    QString cre = el.attribute("created");
    if (cre == "yes") {
        Cal.Created = true;
    }

    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "id") {
                Cal.PrevId = readDomInt(l);
            } else if (l.tagName() == "storage") {
                Cal.PrevStorage = (AlcatelStorage)readDomInt(l);
            } else if (l.tagName() == "date") {
                Cal.Date = readDomDate(l);
            } else if (l.tagName() == "starttime") {
                Cal.StartTime = readDomTime(l);
            } else if (l.tagName() == "endtime") {
                Cal.EndTime = readDomTime(l);
            } else if (l.tagName() == "alarm") {
                Cal.Alarm = readDomDateTime(l);
            } else if (l.tagName() == "alarm2") {
                Cal.Alarm2 = readDomDateTime(l);
            } else if (l.tagName() == "subject") {
                Cal.Subject = readDomString(l);
            } else if (l.tagName() == "private") {
                Cal.Private = readDomInt(l);
            } else if (l.tagName() == "eventtype") {
                Cal.EventType = readDomInt(l);
            } else if (l.tagName() == "contactid") {
                Cal.ContactID = readDomInt(l);
            } else if (l.tagName() == "dayofweek") {
                Cal.DayOfWeek = readDomInt(l);
            } else if (l.tagName() == "day") {
                Cal.Day = readDomInt(l);
            } else if (l.tagName() == "weekofmonth") {
                Cal.WeekOfMonth = readDomInt(l);
            } else if (l.tagName() == "month") {
                Cal.Month = readDomInt(l);
            } else if (l.tagName() == "frequency") {
                Cal.Frequency = readDomInt(l);
            } else if (l.tagName() == "stopdate") {
                Cal.StopDate = readDomDate(l);
            } else if (l.tagName() == "startdate") {
                Cal.StartDate = readDomDate(l);
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    ::message(MSG_DEBUG2, "Read event %d", Cal.Id);
    calendar->append(Cal);
}

void KAlcatelDoc::readDomTodo(QDomElement el) {
    QDomNode n = el.firstChild();
    AlcatelTodo Cal;
    Cal.Id = pcStorageCounter++;
    Cal.Storage = StoragePC;

    QString del = el.attribute("deleted");
    if (del == "yes") {
        Cal.Deleted = true;
    }
    QString mod = el.attribute("modified");
    if (mod == "yes") {
        Cal.Modified = true;
    }
    QString cre = el.attribute("created");
    if (cre == "yes") {
        Cal.Created = true;
    }

    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "id") {
                Cal.PrevId = readDomInt(l);
            } else if (l.tagName() == "storage") {
                Cal.PrevStorage = (AlcatelStorage)readDomInt(l);
            } else if (l.tagName() == "alarm") {
                Cal.Alarm = readDomDateTime(l);
            } else if (l.tagName() == "subject") {
                Cal.Subject = readDomString(l);
            } else if (l.tagName() == "duedate") {
                Cal.DueDate = readDomDate(l);
            } else if (l.tagName() == "private") {
                Cal.Private = readDomInt(l);
            } else if (l.tagName() == "contactid") {
                Cal.ContactID = readDomInt(l);
            } else if (l.tagName() == "completed") {
                Cal.Completed = readDomInt(l);
            } else if (l.tagName() == "category") {
                Cal.Category = readDomInt(l);
            } else if (l.tagName() == "priority") {
                Cal.Priority = readDomInt(l);
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    ::message(MSG_DEBUG2, "Read todo %d", Cal.Id);
    todos->append(Cal);
}

void KAlcatelDoc::readDomCall(QDomElement el) {
    QDomNode n = el.firstChild();
    AlcatelCall Call;
    Call.Storage = StoragePC;
    Call.Id = pcStorageCounter++;

    QString del = el.attribute("deleted");
    if (del == "yes") {
        Call.Deleted = true;
    }
    QString mod = el.attribute("modified");
    if (mod == "yes") {
        Call.Modified = true;
    }
    QString cre = el.attribute("created");
    if (cre == "yes") {
        Call.Created = true;
    }

    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "id") {
                Call.PrevId = readDomInt(l);
            } else if (l.tagName() == "storage") {
                Call.PrevStorage = (AlcatelStorage)readDomInt(l);
            } else if (l.tagName() == "name") {
                Call.Name = readDomString(l);
            } else if (l.tagName() == "number") {
                Call.Number = readDomString(l);
            } else if (l.tagName() == "type") {
                Call.Type = (CallType)readDomInt(l);
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    ::message(MSG_DEBUG2, "Read call %d", Call.Id);
    calls->append(Call);
}

void KAlcatelDoc::readDomCategory(QDomElement el, AlcatelCategoryList *list) {
    QDomNode n = el.firstChild();
    AlcatelCategory Cat;
    Cat.Storage = StoragePC;

    QString del = el.attribute("deleted");
    if (del == "yes") {
        Cat.Deleted = true;
    }
    QString mod = el.attribute("modified");
    if (mod == "yes") {
        Cat.Modified = true;
    }
    QString cre = el.attribute("created");
    if (cre == "yes") {
        Cat.Created = true;
    }

    while( !n.isNull() ) {
        QDomElement l = n.toElement();
        if( !l.isNull() ) {
            if (l.tagName() == "id") {
                Cat.PrevId = readDomInt(l);
                Cat.Id = Cat.PrevId;
            } else if (l.tagName() == "storage") {
                Cat.PrevStorage = (AlcatelStorage)readDomInt(l);
            } else if (l.tagName() == "name") {
                Cat.Name = readDomString(l);
            } else {
                ::message(MSG_WARNING, "Unknown tag in document: %s", l.tagName().latin1());
            }
        }
        n = n.nextSibling();
    }
    ::message(MSG_DEBUG2, "Read category %d", Cat.Id);
    list->append(Cat);
}

bool KAlcatelDoc::openDocument(const KURL& url, const char *format /*=0*/) {
    KAlcatelApp *theApp=(KAlcatelApp *) parent();
    QString tmpfile;
    deleteContents();
    if (KIO::NetAccess::download( url, tmpfile )) {
        QDomDocument doc;
        QFile f( tmpfile );
        if ( !f.open( IO_ReadOnly ) ) {
            KMessageBox::error((KAlcatelApp *) parent(), i18n("Opening of selected document (%1) failed.").arg(url.path()).append('\n').append(i18n("File can not be opened for reading.")), i18n("Error"));
            return false;
        }
        if ( !doc.setContent( &f ) ) {
            KMessageBox::error((KAlcatelApp *) parent(), i18n("Opening of selected document (%1) failed.").arg(url.path()).append('\n').append(i18n("File can not be parsed as XML.")), i18n("Error"));
            f.close();
            return false;
        }
        f.close();

        QDomElement docElem = doc.documentElement();

        QTextStream cerr( stderr, IO_WriteOnly );

#define chk_doc_ver if (document_version == 0) ::message(MSG_WARNING, "Unknown document version!"); else if (document_version > CURRENT_DOC_VERSION) ::message(MSG_WARNING, "Document version higher than supported!");

        QDomNode n = docElem.firstChild();
        int document_version = 0;
        while( !n.isNull() ) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if( !e.isNull() ) { // the node was really an element.
                if (e.tagName() == "info") {
                    ::message(MSG_DEBUG, "Parsing info");
                    QDomNode m = e.firstChild();
                    while( !m.isNull() ) {
                        QDomElement l = m.toElement(); // try to convert the node to an element.
                        if( !l.isNull() ) { // the node was really an element.
                            if (l.tagName() == "version") {
                                document_version = readDomInt(l);
                                ::message(MSG_DEBUG2, "Document version %d", document_version);
                            } else {
                                ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                            }
                        }
                        m = m.nextSibling();
                    }
                } else if (e.tagName() == "messages") {
                    if (theApp->loadMessages){
                        ::message(MSG_DEBUG, "Parsing messages");
                        chk_doc_ver
                        QDomNode m = e.firstChild();
                        while( !m.isNull() ) {
                            QDomElement l = m.toElement(); // try to convert the node to an element.
                            if( !l.isNull() ) { // the node was really an element.
                                if (l.tagName() == "message") {
                                    readDomMessage(l);
                                } else {
                                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                                }
                            }
                            m = m.nextSibling();
                        }
                        messagesVersion++;
                        version++;
                    }
                } else if (e.tagName() == "contacts") {
                    if (theApp->loadContacts){
                        ::message(MSG_DEBUG, "Parsing contacts");
                        chk_doc_ver
                        QDomNode m = e.firstChild();
                        while( !m.isNull() ) {
                            QDomElement l = m.toElement(); // try to convert the node to an element.
                            if( !l.isNull() ) { // the node was really an element.
                                if (l.tagName() == "contact") {
                                    readDomContact(l);
                                } else {
                                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                                }
                            }
                            m = m.nextSibling();
                        }
                        contactsVersion++;
                        version++;
                    }
                } else if (e.tagName() == "calendar") {
                    if (theApp->loadEvents){
                        ::message(MSG_DEBUG, "Parsing calendar");
                        chk_doc_ver
                        QDomNode m = e.firstChild();
                        while( !m.isNull() ) {
                            QDomElement l = m.toElement(); // try to convert the node to an element.
                            if( !l.isNull() ) { // the node was really an element.
                                if (l.tagName() == "event") {
                                    readDomEvent(l);
                                } else {
                                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                                }
                            }
                            m = m.nextSibling();
                        }
                        calendarVersion++;
                        version++;
                    }
                } else if (e.tagName() == "todos") {
                    if (theApp->loadTodos){
                        ::message(MSG_DEBUG, "Parsing todos");
                        chk_doc_ver
                        QDomNode m = e.firstChild();
                        while( !m.isNull() ) {
                            QDomElement l = m.toElement(); // try to convert the node to an element.
                            if( !l.isNull() ) { // the node was really an element.
                                if (l.tagName() == "todo") {
                                    readDomTodo(l);
                                } else {
                                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                                }
                            }
                            m = m.nextSibling();
                        }
                        todosVersion++;
                        version++;
                    }
                } else if (e.tagName() == "calls") {
                    if (theApp->loadCalls){
                        ::message(MSG_DEBUG, "Parsing calls");
                        chk_doc_ver
                        QDomNode m = e.firstChild();
                        while( !m.isNull() ) {
                            QDomElement l = m.toElement(); // try to convert the node to an element.
                            if( !l.isNull() ) { // the node was really an element.
                                if (l.tagName() == "call") {
                                    readDomCall(l);
                                } else {
                                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                                }
                            }
                            m = m.nextSibling();
                        }
                        callsVersion++;
                        version++;
                    }
                } else if (e.tagName() == "todocategories") {
                    if (theApp->loadTodos){
                        ::message(MSG_DEBUG, "Parsing todocategories");
                        chk_doc_ver
                        QDomNode m = e.firstChild();
                        while( !m.isNull() ) {
                            QDomElement l = m.toElement(); // try to convert the node to an element.
                            if( !l.isNull() ) { // the node was really an element.
                                if (l.tagName() == "category") {
                                    readDomCategory(l, todo_cats);
                                } else {
                                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                                }
                            }
                            m = m.nextSibling();
                        }
                        todosVersion++;
                        version++;
                    }
                } else if (e.tagName() == "contactcategories") {
                    if (theApp->loadContacts){
                        ::message(MSG_DEBUG, "Parsing contactcategories");
                        chk_doc_ver
                        QDomNode m = e.firstChild();
                        while( !m.isNull() ) {
                            QDomElement l = m.toElement(); // try to convert the node to an element.
                            if( !l.isNull() ) { // the node was really an element.
                                if (l.tagName() == "category") {
                                    readDomCategory(l, contact_cats);
                                } else {
                                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                                }
                            }
                            m = m.nextSibling();
                        }
                        contactsVersion++;
                        version++;
                    }
                } else {
                    ::message(MSG_WARNING, "Unknown tag in document: %s", e.tagName().latin1());
                }
            }
            n = n.nextSibling();
        }

#undef chk_doc_ver

        doc_url = url;

        KIO::NetAccess::removeTempFile( tmpfile );

        version++;
        slotUpdateAllViews(NULL);
    } else {
        KMessageBox::error((KAlcatelApp *) parent(), i18n("Opening of selected document (%1) failed.").arg(url.path()).append('\n').append(i18n("Document not found.")), i18n("Error"));
        return false;
    }

    modified=false;
    return true;
}

bool KAlcatelDoc::saveDocument(const KURL& url, const char *format /*=0*/) {
    KAlcatelApp *theApp=(KAlcatelApp *) parent();
    KTempFile tmp;
    QRegExp rlt("<");
    QRegExp rgt(">");

    QTextStream *strm = tmp.textStream();
    strm->setEncoding(QTextStream::UnicodeUTF8);

    *strm << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    *strm << "<!-- KAlcatel saved data -->" << endl;
    *strm << "<document>" << endl;
    *strm << " <info>" << endl;
    *strm << "  <version>" << CURRENT_DOC_VERSION << "</version>" << endl;
    *strm << " </info>" << endl;
    if (theApp->saveMessages) {
        *strm << " <messages>" << endl;
        AlcatelMessageList::Iterator messagesit;
        for( messagesit = messages->begin(); messagesit != messages->end(); ++messagesit ) {
            *strm << "  <message";
            if ((*messagesit).Modified)
                *strm << " modified=\"yes\"";
            if ((*messagesit).Deleted)
                *strm << " deleted=\"yes\"";
            if ((*messagesit).Created)
                *strm << " created=\"yes\"";
            *strm << ">" << endl;
            if ((*messagesit).Storage != StoragePC) {
                *strm << "   <id>" << (*messagesit).Id << "</id>" << endl;
                *strm << "   <storage>" << (*messagesit).Storage << "</storage>" << endl;
                (*messagesit).PrevId = (*messagesit).Id;
                (*messagesit).Id = pcStorageCounter++;
                (*messagesit).PrevStorage = (*messagesit).Storage;
                (*messagesit).Storage = StoragePC;
            } else {
                *strm << "   <id>" << (*messagesit).PrevId << "</id>" << endl;
                *strm << "   <storage>" << (*messagesit).PrevStorage << "</storage>" << endl;
            }
            *strm << "   <status>" << (*messagesit).Status << "</status>" << endl;
            *strm << "   <length>" << (*messagesit).Length << "</length>" << endl;
            *strm << "   <raw>" << (*messagesit).Raw << "</raw>" << endl;
            *strm << "   <sender>" << (*messagesit).Sender.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</sender>" << endl;
            *strm << "   <date>" << endl;
            *strm << "    <date>" << endl;
            *strm << "     <day>" << (*messagesit).Date.date().day() << "</day>" << endl;
            *strm << "     <month>" << (*messagesit).Date.date().month() << "</month>" << endl;
            *strm << "     <year>" << (*messagesit).Date.date().year() << "</year>" << endl;
            *strm << "    </date>" << endl;
            *strm << "    <time>" << endl;
            *strm << "     <hour>" << (*messagesit).Date.time().hour() << "</hour>" << endl;
            *strm << "     <minute>" << (*messagesit).Date.time().minute() << "</minute>" << endl;
            *strm << "     <second>" << (*messagesit).Date.time().second() << "</second>" << endl;
            *strm << "    </time>" << endl;
            *strm << "   </date>" << endl;
            *strm << "   <text>" << (*messagesit).Text.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</text>" << endl;
            *strm << "   <smsc>" << (*messagesit).SMSC.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</smsc>" << endl;
            *strm << "  </message>" << endl;
        }
        *strm << " </messages>" << endl;
    }

    if (theApp->saveContacts) {
        *strm << " <contacts>" << endl;
        AlcatelContactList::Iterator cit;
        for( cit = contacts->begin(); cit != contacts->end(); ++cit ) {
            *strm << "  <contact";
            if ((*cit).Modified)
                *strm << " modified=\"yes\"";
            if ((*cit).Deleted)
                *strm << " deleted=\"yes\"";
            if ((*cit).Created)
                *strm << " created=\"yes\"";
            *strm << ">" << endl;
            if ((*cit).Storage != StoragePC) {
                *strm << "   <id>" << (*cit).Id << "</id>" << endl;
                *strm << "   <storage>" << (*cit).Storage << "</storage>" << endl;
                (*cit).PrevId = (*cit).Id;
                (*cit).PrevStorage = (*cit).Storage;
                (*cit).Storage = StoragePC;
            } else {
                *strm << "   <id>" << (*cit).PrevId << "</id>" << endl;
                *strm << "   <storage>" << (*cit).PrevStorage << "</storage>" << endl;
            }
            if (!(*cit).LastName.isEmpty()) {
                *strm << "   <lastname>" << (*cit).LastName.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</lastname>" << endl;
            }
            if (!(*cit).FirstName.isEmpty()) {
                *strm << "   <firstname>" << (*cit).FirstName.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</firstname>" << endl;
            }
            if (!(*cit).Company.isEmpty()) {
                *strm << "   <company>" << (*cit).Company.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</company>" << endl;
            }
            if (!(*cit).JobTitle.isEmpty()) {
                *strm << "   <jobtitle>" << (*cit).JobTitle.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</jobtitle>" << endl;
            }
            if (!(*cit).Note.isEmpty()) {
                *strm << "   <note>" << (*cit).Note.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</note>" << endl;
            }
            if ((*cit).Category != -1) {
                *strm << "   <category>" << (*cit).Category << "</category>" << endl;
            }
            if ((*cit).Private != -1) {
                *strm << "   <private>" << (*cit).Private << "</private>" << endl;
            }
            if (!(*cit).WorkNumber.isEmpty()) {
                *strm << "   <worknumber>" << (*cit).WorkNumber.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</worknumber>" << endl;
            }
            if (!(*cit).MainNumber.isEmpty()) {
                *strm << "   <mainnumber>" << (*cit).MainNumber.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</mainnumber>" << endl;
            }
            if (!(*cit).FaxNumber.isEmpty()) {
                *strm << "   <faxnumber>" << (*cit).FaxNumber.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</faxnumber>" << endl;
            }
            if (!(*cit).OtherNumber.isEmpty()) {
                *strm << "   <othernumber>" << (*cit).OtherNumber.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</othernumber>" << endl;
            }
            if (!(*cit).PagerNumber.isEmpty()) {
                *strm << "   <pagernumber>" << (*cit).PagerNumber.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</pagernumber>" << endl;
            }
            if (!(*cit).MobileNumber.isEmpty()) {
                *strm << "   <mobilenumber>" << (*cit).MobileNumber.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</mobilenumber>" << endl;
            }
            if (!(*cit).HomeNumber.isEmpty()) {
                *strm << "   <homenumber>" << (*cit).HomeNumber.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</homenumber>" << endl;
            }
            if (!(*cit).Email1.isEmpty()) {
                *strm << "   <email1>" << (*cit).Email1.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</email1>" << endl;
            }
            if (!(*cit).Email2.isEmpty()) {
                *strm << "   <email2>" << (*cit).Email2.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</email2>" << endl;
            }
            if (!(*cit).Address.isEmpty()) {
                *strm << "   <address>" << (*cit).Address.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</address>" << endl;
            }
            if (!(*cit).City.isEmpty()) {
                *strm << "   <city>" << (*cit).City.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</city>" << endl;
            }
            if (!(*cit).State.isEmpty()) {
                *strm << "   <state>" << (*cit).State.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</state>" << endl;
            }
            if (!(*cit).Zip.isEmpty()) {
                *strm << "   <zip>" << (*cit).Zip.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</zip>" << endl;
            }
            if (!(*cit).Country.isEmpty()) {
                *strm << "   <country>" << (*cit).Country.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</country>" << endl;
            }
            if (!(*cit).Custom1.isEmpty()) {
                *strm << "   <custom1>" << (*cit).Custom1.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</custom1>" << endl;
            }
            if (!(*cit).Custom2.isEmpty()) {
                *strm << "   <custom2>" << (*cit).Custom2.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</custom2>" << endl;
            }
            if (!(*cit).Custom3.isEmpty()) {
                *strm << "   <custom3>" << (*cit).Custom3.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</custom3>" << endl;
            }
            if (!(*cit).Custom4.isEmpty()) {
                *strm << "   <custom4>" << (*cit).Custom4.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</custom4>" << endl;
            }
            *strm << "  </contact>" << endl;
        }
        *strm << " </contacts>" << endl;
    }

    if (theApp->saveEvents) {
        *strm << " <calendar>" << endl;
        AlcatelCalendarList::Iterator calit;
        for( calit = calendar->begin(); calit != calendar->end(); ++calit ) {
            *strm << "  <event";
            if ((*calit).Modified)
                *strm << " modified=\"yes\"";
            if ((*calit).Deleted)
                *strm << " deleted=\"yes\"";
            if ((*calit).Created)
                *strm << " created=\"yes\"";
            *strm << ">" << endl;
            if ((*calit).Storage != StoragePC) {
                *strm << "   <id>" << (*calit).Id << "</id>" << endl;
                *strm << "   <storage>" << (*calit).Storage << "</storage>" << endl;
                (*calit).PrevId = (*calit).Id;
                (*calit).Id = pcStorageCounter++;
                (*calit).PrevStorage = (*calit).Storage;
                (*calit).Storage = StoragePC;
            } else {
                *strm << "   <id>" << (*calit).PrevId << "</id>" << endl;
                *strm << "   <storage>" << (*calit).PrevStorage << "</storage>" << endl;
            }
            if (!(*calit).Date.isNull()){
                *strm << "   <date>" << endl;
                *strm << "    <day>" << (*calit).Date.day() << "</day>" << endl;
                *strm << "    <month>" << (*calit).Date.month() << "</month>" << endl;
                *strm << "    <year>" << (*calit).Date.year() << "</year>" << endl;
                *strm << "   </date>" << endl;
            }
            if (!(*calit).StartTime.isNull()) {
                *strm << "   <starttime>" << endl;
                *strm << "    <hour>" << (*calit).StartTime.hour() << "</hour>" << endl;
                *strm << "    <minute>" << (*calit).StartTime.minute() << "</minute>" << endl;
                *strm << "    <second>" << (*calit).StartTime.second() << "</second>" << endl;
                *strm << "   </starttime>" << endl;
            }
            if (!(*calit).EndTime.isNull()) {
                *strm << "   <endtime>" << endl;
                *strm << "    <hour>" << (*calit).EndTime.hour() << "</hour>" << endl;
                *strm << "    <minute>" << (*calit).EndTime.minute() << "</minute>" << endl;
                *strm << "    <second>" << (*calit).EndTime.second() << "</second>" << endl;
                *strm << "   </endtime>" << endl;
            }
            if (!(*calit).Alarm.isNull()) {
                *strm << "   <alarm>" << endl;
                *strm << "    <date>" << endl;
                *strm << "     <day>" << (*calit).Alarm.date().day() << "</day>" << endl;
                *strm << "     <month>" << (*calit).Alarm.date().month() << "</month>" << endl;
                *strm << "     <year>" << (*calit).Alarm.date().year() << "</year>" << endl;
                *strm << "    </date>" << endl;
                *strm << "    <time>" << endl;
                *strm << "     <hour>" << (*calit).Alarm.time().hour() << "</hour>" << endl;
                *strm << "     <minute>" << (*calit).Alarm.time().minute() << "</minute>" << endl;
                *strm << "     <second>" << (*calit).Alarm.time().second() << "</second>" << endl;
                *strm << "    </time>" << endl;
                *strm << "   </alarm>" << endl;
            }
            if (!(*calit).Alarm2.isNull()) {
                *strm << "   <alarm2>" << endl;
                *strm << "    <date>" << endl;
                *strm << "     <day>" << (*calit).Alarm2.date().day() << "</day>" << endl;
                *strm << "     <month>" << (*calit).Alarm2.date().month() << "</month>" << endl;
                *strm << "     <year>" << (*calit).Alarm2.date().year() << "</year>" << endl;
                *strm << "    </date>" << endl;
                *strm << "    <time>" << endl;
                *strm << "     <hour>" << (*calit).Alarm2.time().hour() << "</hour>" << endl;
                *strm << "     <minute>" << (*calit).Alarm2.time().minute() << "</minute>" << endl;
                *strm << "     <second>" << (*calit).Alarm2.time().second() << "</second>" << endl;
                *strm << "    </time>" << endl;
                *strm << "   </alarm2>" << endl;
            }
            if (!(*calit).Subject.isEmpty()) {
                *strm << "   <subject>" << (*calit).Subject.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</subject>" << endl;
            }
            if ((*calit).Private != -1) {
                *strm << "   <private>" << (*calit).Private << "</private>" << endl;
            }
            if ((*calit).EventType != -1) {
                *strm << "   <eventtype>" << (*calit).EventType << "</eventtype>" << endl;
            }
            if ((*calit).ContactID != -1) {
                *strm << "   <contactid>" << (*calit).ContactID << "</contactid>" << endl;
            }
            if ((*calit).DayOfWeek != -1) {
                *strm << "   <dayofweek>" << (*calit).DayOfWeek << "</dayofweek>" << endl;
            }
             if ((*calit).Day != -1) {
                *strm << "   <day>" << (*calit).Day << "</day>" << endl;
            }
            if ((*calit).WeekOfMonth != -1) {
                *strm << "   <weekofmonth>" << (*calit).WeekOfMonth << "</weekofmonth>" << endl;
            }
            if ((*calit).Month != -1) {
                *strm << "   <month>" << (*calit).Month << "</month>" << endl;
            }
            if ((*calit).Frequency != -1) {
                *strm << "   <frequency>" << (*calit).Frequency << "</frequency>" << endl;
            }
            if (!((*calit).StopDate.isNull())) {
                *strm << "   <stopdate>" << endl;
                *strm << "    <day>" << (*calit).StopDate.day() << "</day>" << endl;
                *strm << "    <month>" << (*calit).StopDate.month() << "</month>" << endl;
                *strm << "    <year>" << (*calit).StopDate.year() << "</year>" << endl;
                *strm << "   </stopdate>" << endl;
            }
            if (!((*calit).StartDate.isNull())) {
                *strm << "   <startdate>" << endl;
                *strm << "    <day>" << (*calit).StartDate.day() << "</day>" << endl;
                *strm << "    <month>" << (*calit).StartDate.month() << "</month>" << endl;
                *strm << "    <year>" << (*calit).StartDate.year() << "</year>" << endl;
                *strm << "   </startdate>" << endl;
            }
            *strm << "  </event>" << endl;
        }
        *strm << " </calendar>" << endl;
    }

    if (theApp->saveTodos) {
        *strm << " <todos>" << endl;
        AlcatelTodoList::Iterator tit;
        for( tit = todos->begin(); tit != todos->end(); ++tit ) {
            *strm << "  <todo";
            if ((*tit).Modified)
                *strm << " modified=\"yes\"";
            if ((*tit).Deleted)
                *strm << " deleted=\"yes\"";
            if ((*tit).Created)
                *strm << " created=\"yes\"";
            *strm << ">" << endl;
            if ((*tit).Storage != StoragePC) {
                *strm << "   <id>" << (*tit).Id << "</id>" << endl;
                *strm << "   <storage>" << (*tit).Storage << "</storage>" << endl;
                (*tit).PrevId = (*tit).Id;
                (*tit).Id = pcStorageCounter++;
                (*tit).PrevStorage = (*tit).Storage;
                (*tit).Storage = StoragePC;
            } else {
                *strm << "   <id>" << (*tit).PrevId << "</id>" << endl;
                *strm << "   <storage>" << (*tit).PrevStorage << "</storage>" << endl;
            }

            if (!(*tit).Subject.isEmpty()) {
                *strm << "   <subject>" << (*tit).Subject.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</subject>" << endl;
            }
            if ((*tit).Completed != -1) {
                *strm << "   <completed>" << (*tit).Completed << "</completed>" << endl;
            }
            if ((*tit).Private != -1) {
                *strm << "   <private>" << (*tit).Private << "</private>" << endl;
            }
            if ((*tit).Category != -1) {
                *strm << "   <category>" << (*tit).Category << "</category>" << endl;
            }
            if ((*tit).Priority != -1) {
                *strm << "   <priority>" << (*tit).Priority << "</priority>" << endl;
            }
            if ((*tit).ContactID != -1) {
                *strm << "   <contactid>" << (*tit).ContactID << "</contactid>" << endl;
            }
            if (!(*tit).DueDate.isNull()) {
                *strm << "   <duedate>" << endl;
                *strm << "    <day>" << (*tit).DueDate.day() << "</day>" << endl;
                *strm << "    <month>" << (*tit).DueDate.month() << "</month>" << endl;
                *strm << "    <year>" << (*tit).DueDate.year() << "</year>" << endl;
                *strm << "   </duedate>" << endl;
            }

            if (!(*tit).Alarm.isNull()) {
                *strm << "   <alarm>" << endl;
                *strm << "    <date>" << endl;
                *strm << "     <day>" << (*tit).Alarm.date().day() << "</day>" << endl;
                *strm << "     <month>" << (*tit).Alarm.date().month() << "</month>" << endl;
                *strm << "     <year>" << (*tit).Alarm.date().year() << "</year>" << endl;
                *strm << "    </date>" << endl;
                *strm << "    <time>" << endl;
                *strm << "     <hour>" << (*tit).Alarm.time().hour() << "</hour>" << endl;
                *strm << "     <minute>" << (*tit).Alarm.time().minute() << "</minute>" << endl;
                *strm << "     <second>" << (*tit).Alarm.time().second() << "</second>" << endl;
                *strm << "    </time>" << endl;
                *strm << "   </alarm>" << endl;
            }
            *strm << "  </todo>" << endl;
        }
        *strm << " </todos>" << endl;
    }

    if (theApp->saveCalls) {
        AlcatelCallList::Iterator callit;
        *strm << " <calls>" << endl;
        for( callit = calls->begin(); callit != calls->end(); ++callit ) {
            *strm << "  <call";
            if ((*callit).Modified)
                *strm << " modified=\"yes\"";
            if ((*callit).Deleted)
                *strm << " deleted=\"yes\"";
            if ((*callit).Created)
                *strm << " created=\"yes\"";
            *strm << ">" << endl;
            if ((*callit).Storage != StoragePC) {
                (*callit).PrevId = (*callit).Id;
                (*callit).Id = pcStorageCounter++;
                (*callit).PrevStorage = (*callit).Storage;
                (*callit).Storage = StoragePC;
                *strm << "   <id>" << (*callit).Id << "</id>" << endl;
                *strm << "   <storage>" << (*callit).Storage << "</storage>" << endl;
            } else {
                *strm << "   <id>" << (*callit).PrevId << "</id>" << endl;
                *strm << "   <storage>" << (*callit).PrevStorage << "</storage>" << endl;
            }
            if (!(*callit).Name.isEmpty()) {
                *strm << "   <name>" << (*callit).Name.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</name>" << endl;
            }
            *strm << "   <number>" << (*callit).Number.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</number>" << endl;
            *strm << "   <type>" << (*callit).Type << "</type>" << endl;
            *strm << "  </call>" << endl;
        }
        *strm << " </calls>" << endl;
    }

    if (theApp->saveTodos) {
        AlcatelCategoryList::Iterator catit;
        *strm << " <todocategories>" << endl;
        for( catit = todo_cats->begin(); catit != todo_cats->end(); ++catit ) {
            *strm << "  <category";
            if ((*catit).Modified)
                *strm << " modified=\"yes\"";
            if ((*catit).Deleted)
                *strm << " deleted=\"yes\"";
            if ((*catit).Created)
                *strm << " created=\"yes\"";
            *strm << ">" << endl;
            if ((*catit).Storage != StoragePC) {
                *strm << "   <id>" << (*catit).Id << "</id>" << endl;
                *strm << "   <storage>" << (*catit).Storage << "</storage>" << endl;
            } else {
                *strm << "   <id>" << (*catit).PrevId << "</id>" << endl;
                *strm << "   <storage>" << (*catit).PrevStorage << "</storage>" << endl;
            }
            *strm << "   <name>" << (*catit).Name.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</name>" << endl;
            *strm << "  </category>" << endl;
        }
        *strm << " </todocategories>" << endl;
    }

    if (theApp->saveContacts) {
        AlcatelCategoryList::Iterator catit;
        *strm << " <contactcategories>" << endl;
        for( catit = contact_cats->begin(); catit != contact_cats->end(); ++catit ) {
            *strm << "  <category";
            if ((*catit).Modified)
                *strm << " modified=\"yes\"";
            if ((*catit).Deleted)
                *strm << " deleted=\"yes\"";
            if ((*catit).Created)
                *strm << " created=\"yes\"";
            *strm << ">" << endl;
            if ((*catit).Storage != StoragePC) {
                *strm << "   <id>" << (*catit).Id << "</id>" << endl;
                *strm << "   <storage>" << (*catit).Storage << "</storage>" << endl;
            } else {
                *strm << "   <id>" << (*catit).PrevId << "</id>" << endl;
                *strm << "   <storage>" << (*catit).PrevStorage << "</storage>" << endl;
            }
            *strm << "   <name>" << (*catit).Name.replace(rlt, "&lt;").replace(rgt, "&gt;") << "</name>" << endl;
            *strm << "  </category>" << endl;
        }
        *strm << " </contactcategories>" << endl;
    }

    *strm << "</document>" << endl;

    tmp.close();
    KIO::NetAccess::upload( tmp.name(), url );
    tmp.unlink();

    doc_url = url;
    modified=false;
    version++;
    callsVersion++;
    todosVersion++;
    calendarVersion++;
    contactsVersion++;
    messagesVersion++;
    slotUpdateAllViews(NULL);
    return true;
}

bool KAlcatelDoc::readMobileItems(alc_type sync, alc_type type) {
    alc_type *result;
    int i, j;
    int *ids, *items;
    AlcatelFieldStruct *field;

    AlcatelContactList local_contacts;
    AlcatelCalendarList local_calendar;
    AlcatelTodoList local_todos;

    int count = 0;

    KAlcatelApp *win=(KAlcatelApp *) parent();

    switch (sync) {
        case ALC_SYNC_CALENDAR:
            count = ALC_CALENDAR_FIELDS;
            clearCalendar(calendar, StorageMobile);
            calendarVersion++;
            version++;
            slotUpdateAllViews(NULL);
            if (win->mergeData == 1)
                clearCalendar(calendar, StoragePC);
            break;
        case ALC_SYNC_TODO:
            count = ALC_TODO_FIELDS;
            clearTodos(todos, StorageMobile);
            todosVersion++;
            version++;
            slotUpdateAllViews(NULL);
            if (win->mergeData == 1)
                clearTodos(todos, StoragePC);
            break;
        case ALC_SYNC_CONTACTS:
            count = ALC_CONTACTS_FIELDS;
            clearContacts(contacts, StorageMobile);
            contactsVersion++;
            version++;
            slotUpdateAllViews(NULL);
            if (win->mergeData == 1)
                clearContacts(contacts, StoragePC);
            break;
    }

    if (!alcatel_attach()) return false;

    if (!alcatel_start_session()) return false;

    if (alcatel_select_type(type)) {
        if (!alcatel_begin_transfer(sync)) {
            alcatel_close_session(type);
            alcatel_detach();
            return false;
        }

        ids = alcatel_get_ids(type);
        if (ids == NULL) {
            alcatel_close_session(type);
            alcatel_detach();
            return false;
        }

        message(MSG_INFO, "Received %d ids", ids[0]);

        for (i = 1; i <= ids[0]; i++) {
            win->slotStatusMsg(i18n("Reading item %1 of %2").arg(i).arg(ids[0]),ID_DETAIL_MSG);
            message(MSG_DEBUG, "Reading id[%d] = %d", i-1, ids[i]);
            items = alcatel_get_fields(type, ids[i]);
            if (items == NULL) {
                alcatel_close_session(type);
                alcatel_detach();
                return false;
            }
            message(MSG_INFO, "Receiving data for item %d (%d fields)", ids[i], items[0]);
            printf ("Item %d (fields: %d):\n", ids[i], items[0]);
            AlcatelContact Contact;
            AlcatelTodo Todo;
            AlcatelCalendar Calendar;
            switch (sync) {
                case ALC_SYNC_CALENDAR:
                    Calendar.Id = ids[i];
                    Calendar.Storage = StorageMobile;
                    break;
                case ALC_SYNC_TODO:
                    Todo.Id = ids[i];
                    Todo.Storage = StorageMobile;
                    break;
                case ALC_SYNC_CONTACTS:
                    Contact.Storage = StorageMobile;
                    Contact.Id = ids[i];
                    break;
            }
            for (j = 1; j <= items[0]; j++) {
                message(MSG_DEBUG, "items[%d] = %d", j-1, items[j]);
                result = alcatel_get_field_value(type, ids[i], items[j]);
                if (result == NULL) {
                    alcatel_close_session(type);
                    alcatel_detach();
                    return false;
                }
                field = decode_field_value(result);
                if (items[j] >= count) {
                    message(MSG_WARNING, "Unknown field %02d, ignoring!",items[j]);
                }
                if (field == NULL) {
                    message(MSG_WARNING, "Unknown field type(%02X %02X), ingnoring", result[1], result[2]);
                } else {
                    switch (sync) {
                        case ALC_SYNC_CALENDAR:
                            Calendar.setField(items[j], field);
                            break;
                        case ALC_SYNC_TODO:
                            Todo.setField(items[j], field);
                            break;
                        case ALC_SYNC_CONTACTS:
                            Contact.setField(items[j], field);
                            break;
                    }
                }
                free(result);
            }
            free(items);
            switch (sync) {
                case ALC_SYNC_CALENDAR:
                    if (win->mergeData == 0)
                        local_calendar.append(Calendar);
                    else
                        calendar->append(Calendar);
                    break;
                case ALC_SYNC_TODO:
                    if (win->mergeData == 0)
                        local_todos.append(Todo);
                    else
                        todos->append(Todo);
                    break;
                case ALC_SYNC_CONTACTS:
                    if (win->mergeData == 0)
                        local_contacts.append(Contact);
                    else
                        contacts->append(Contact);
                    break;
            }
        }
        free(ids);
    } else {
        message(MSG_ERROR, "Can not open sync session!");
        alcatel_close_session(type);
        alcatel_detach();
        return false;
    }

    alcatel_close_session(type);
    alcatel_detach();

    if (win->mergeData == 0) {
        switch (sync) {
            case ALC_SYNC_CALENDAR: {
                AlcatelCalendarList::Iterator it;
                AlcatelCalendar *item;
                for( it = local_calendar.begin(); it != local_calendar.end(); /*++it*/ ) {
                    if ((item = getCalendarByPrevId(calendar, (*it).Id, (*it).Storage))!=NULL) {
                        if (!(item->isSame(*it))) {
                            ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                            if (win->conflictAction == 0) {
                                calendar->append((*it));
                                it = local_calendar.remove(it);
                                calendar->remove(*item);
                            } else if (win->conflictAction == 1) {
                                it = local_calendar.remove(it);
                            } else if (win->conflictAction == 2) {
                                AlcatelCalendar *cont = win->solveConflict(*it, *item);
                                if (cont == NULL) {
                                    AlcatelCalendar old(*item);
                                    calendar->remove(*item);
                                    old.PrevStorage = StorageNone;
                                    old.PrevId = -1;
                                    old.Modified = true;
                                    calendar->append(old);
                                    calendar->append((*it));
                                    it = local_calendar.remove(it);
                                } else {
                                    calendar->remove(*item);
                                    calendar->append(*cont);
                                    it = local_calendar.remove(it);
                                }
                            }
                        } else {
                            ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                            calendar->remove(*item);
                            calendar->append((*it));
                            it = local_calendar.remove(it);
                        }
                    } else {
                        ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", (*it).getName().latin1());
                        calendar->append((*it));
                        it = local_calendar.remove(it);
                    }
                    calendarVersion++;
                    version++;
                    slotUpdateAllViews(NULL);
                }
                break; }
            case ALC_SYNC_TODO: {
                AlcatelTodoList::Iterator it;
                AlcatelTodo *item;
                for( it = local_todos.begin(); it != local_todos.end(); /*++it*/ ) {
                    if ((item = getTodoByPrevId(todos, (*it).Id, (*it).Storage))!=NULL) {
                        if (!(item->isSame(*it))) {
                            ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                            if (win->conflictAction == 0) {
                                todos->append((*it));
                                it = local_todos.remove(it);
                                todos->remove(*item);
                            } else if (win->conflictAction == 1) {
                                it = local_todos.remove(it);
                            } else if (win->conflictAction == 2) {
                                AlcatelTodo *cont = win->solveConflict(*it, *item);
                                if (cont == NULL) {
                                    AlcatelTodo old(*item);
                                    todos->remove(*item);
                                    old.PrevStorage = StorageNone;
                                    old.PrevId = -1;
                                    old.Modified = true;
                                    todos->append(old);
                                    todos->append((*it));
                                    it = local_todos.remove(it);
                                } else {
                                    todos->remove(*item);
                                    todos->append(*cont);
                                    it = local_todos.remove(it);
                                }
                            }
                        } else {
                            ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                            todos->remove(*item);
                            todos->append((*it));
                            it = local_todos.remove(it);
                        }
                    } else {
                        ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", (*it).getName().latin1());
                        todos->append((*it));
                        it = local_todos.remove(it);
                    }
                    todosVersion++;
                    version++;
                    slotUpdateAllViews(NULL);
                }
                break; }
            case ALC_SYNC_CONTACTS: {
                AlcatelContactList::Iterator it;
                AlcatelContact *item;
                for( it = local_contacts.begin(); it != local_contacts.end(); /*++it*/ ) {
                    if ((item = getContactByPrevId(contacts, (*it).Id, (*it).Storage))!=NULL) {
                        if (!(item->isSame(*it))) {
                            ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                            if (win->conflictAction == 0) {
                                contacts->append((*it));
                                it = local_contacts.remove(it);
                                contacts->remove(*item);
                            } else if (win->conflictAction == 1) {
                                it = local_contacts.remove(it);
                            } else if (win->conflictAction == 2) {
                                AlcatelContact *cont = win->solveConflict(*it, *item);
                                if (cont == NULL) {
                                    AlcatelContact old(*item);
                                    contacts->remove(*item);
                                    old.PrevStorage = StorageNone;
                                    old.PrevId = -1;
                                    old.Modified = true;
                                    contacts->append(old);
                                    contacts->append((*it));
                                    it = local_contacts.remove(it);
                                } else {
                                    contacts->remove(*item);
                                    contacts->append(*cont);
                                    it = local_contacts.remove(it);
                                    delete cont;
                                }
                            }
                        } else {
                            ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                            contacts->remove(*item);
                            contacts->append((*it));
                            it = local_contacts.remove(it);
                        }
                    } else {
                        ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", (*it).getName().latin1());
                        contacts->append((*it));
                        it = local_contacts.remove(it);
                    }
                    contactsVersion++;
                    version++;
                    slotUpdateAllViews(NULL);
                }
                break; }
        }
    }

    return true;
}

bool KAlcatelDoc::readMobileCategories(AlcatelCategoryList *strList, alc_type sync, alc_type type, alc_type cat) {
    int *list;
    int i;
    char *result;

    KAlcatelApp *win=(KAlcatelApp *) parent();

    alcatel_attach();

    if (!alcatel_start_session()) return false;

    if (alcatel_select_type(type)) {
        if (!alcatel_begin_transfer(sync)) {
            alcatel_close_session(type);
            alcatel_detach();
            return false;
        }

        list = alcatel_get_obj_list(type, cat);
        if (list == NULL) {
            alcatel_close_session(type);
            alcatel_detach();
            return false;
        }

        message(MSG_INFO, "Received %d categories:", list[0]);

        clearCategories(strList, StorageMobile);

        for (i = 1; i <= list[0]; i++) {
            result = alcatel_get_obj_list_item(type, cat, list[i]);
            if (result == NULL) {
                alcatel_close_session(type);
                alcatel_detach();
                return false;
            }

            AlcatelCategory *item;
            AlcatelCategory cat(result, list[i], StorageMobile);
            if ((item = getCategoryByPrevId(strList, list[i], StorageAny))!=NULL) {
                if (!(item->isSame(cat))) {
                    ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                    if (win->conflictAction == 0) {
                        strList->remove(*item);
                        strList->append(cat);
                    } else if (win->conflictAction == 1) {
                        /* nothing to do here ... */
                    } else if (win->conflictAction == 2) {
                        AlcatelCategory *cont = win->solveConflict(cat, *item);
                        strList->remove(*item);
                        strList->append(*cont);
                        delete cont;
                    }
                } else {
                    ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                    strList->remove(*item);
                    strList->append(cat);
                }
            } else {
                ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", cat.getName().latin1());
                strList->append(cat);
            }

            message (MSG_DEBUG, "Read category name: %02d: %s", list[i],  result);
            contactsVersion++;
            todosVersion++;
            version++;
            slotUpdateAllViews(NULL);
            free(result);
        }

        free(list);
        i = true;
    } else {
        message(MSG_ERROR, "Can not open sync session!");
        i = false;
    }

    alcatel_close_session(type);
    alcatel_detach();
    return i;
}

bool KAlcatelDoc::readMobile(AlcDataType what) {
    int i;

    KAlcatelApp *win=(KAlcatelApp *) parent();

    if (!win->modemConnect()) return false;

    if (what == alcatel_messages || what == alcatel_all) {
        MessageData *msg;
        win->slotStatusMsg(i18n("Reading messages"),ID_DETAIL_MSG);
        clearMessages(messages, StorageSIM);
        messagesVersion++;
        version++;
        slotUpdateAllViews(NULL);
        msg = get_messages();

        i = 0;
        while (msg[i].pos != -1) {
            AlcatelMessage Msg;
            Msg.Date = QDateTime();
            Msg.Storage = StorageSIM;
            if (msg[i].date != -1) Msg.Date.setTime_t(msg[i].date);
            Msg.Length = msg[i].len;
            Msg.Id = msg[i].pos;
            Msg.Raw = msg[i].raw;
            free(msg[i].raw);
            Msg.SMSC = QString(msg[i].smsc);
            free(msg[i].smsc);
            Msg.Sender = QString(msg[i].sendr);
            free(msg[i].sendr);
            Msg.Status = msg[i].stat;
            Msg.Text = QString(msg[i].ascii);
            free(msg[i].ascii);

            AlcatelMessage *item;
            if ((item = getMessageByPrevId(messages, Msg.Id, StorageSIM))!=NULL) {
                if (!(item->isSame(Msg))) {
                    ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                    if (win->conflictAction == 0) {
                        messages->remove(*item);
                        messages->append(Msg);
                    } else if (win->conflictAction == 1) {
                        /* nothing to do here ... */
                    } else if (win->conflictAction == 2) {
                        AlcatelMessage *cont = win->solveConflict(Msg, *item);
                        if (cont == NULL) {
                            AlcatelMessage msg_old(*item);
                            messages->remove(*item);
                            msg_old.PrevStorage = StorageNone;
                            msg_old.PrevId = -1;
                            msg_old.Modified = true;
                            messages->append(msg_old);
                            messages->append(Msg);
                        } else {
                            messages->remove(*item);
                            messages->append(*cont);
                            delete cont;
                        }
                    }
                } else {
                    ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                    messages->remove(*item);
                    messages->append(Msg);
                }
            } else {
                ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", Msg.getName().latin1());
                messages->append(Msg);
            }

            messagesVersion++;
            version++;
            slotUpdateAllViews(NULL);
            i++;
        }
        free(msg);
        win->slotStatusMsg(i18n("SMS messages read"),ID_DETAIL_MSG);

    }

    if (what == alcatel_calls || what == alcatel_all) {
        ContactData *cont;
        win->slotStatusMsg(i18n("Reading calls"),ID_DETAIL_MSG);
        clearCalls(calls, StorageMobile);
        callsVersion++;
        version++;
        slotUpdateAllViews(NULL);

        select_phonebook(PB_LAST_DIAL);
        cont = get_contacts(1, 10);
        i = 0;
        while (cont[i].pos != -1) {
            AlcatelCall Call;
            Call.Storage = StorageMobile;
            Call.Type = CallDialled;
            Call.Id = cont[i].pos;
            Call.Name = *(cont[i].name);
            delete cont[i].name;
            Call.Number = cont[i].number;
            free(cont[i].number);
            AlcatelCall *item;
            if ((item = getCallByPrevId(calls, Call.Id, StorageMobile, Call.Type))!=NULL) {
                if (!(item->isSame(Call))) {
                    ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                    if (win->conflictAction == 0) {
                        calls->remove(*item);
                        calls->append(Call);
                    } else if (win->conflictAction == 1) {
                        /* nothing to do here ... */
                    } else if (win->conflictAction == 2) {
                        AlcatelCall *cont = win->solveConflict(Call, *item);
                        if (cont == NULL) {
                            AlcatelCall old(*item);
                            calls->remove(*item);
                            old.PrevStorage = StorageNone;
                            old.PrevId = -1;
                            old.Modified = true;
                            calls->append(old);
                            calls->append(Call);
                        } else {
                            calls->remove(*item);
                            calls->append(*cont);
                            delete cont;
                        }
                    }
                } else {
                    ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                    calls->remove(*item);
                    calls->append(Call);
                }
            } else {
                ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", Call.getName().latin1());
                calls->append(Call);
            }
            callsVersion++;
            version++;
            slotUpdateAllViews(NULL);
            i++;
        }
        free(cont);

        select_phonebook(PB_RECEIVED);
        cont = get_contacts(1, 10);
        i = 0;
        while (cont[i].pos != -1) {
            AlcatelCall Call;
            Call.Storage = StorageMobile;
            Call.Type = CallReceived;
            Call.Id = cont[i].pos;
            Call.Name = *(cont[i].name);
            delete cont[i].name;
            Call.Number = cont[i].number;
            free(cont[i].number);
            AlcatelCall *item;
            if ((item = getCallByPrevId(calls, Call.Id, StorageMobile, Call.Type))!=NULL) {
                if (!(item->isSame(Call))) {
                    ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                    if (win->conflictAction == 0) {
                        calls->remove(*item);
                        calls->append(Call);
                    } else if (win->conflictAction == 1) {
                        /* nothing to do here ... */
                    } else if (win->conflictAction == 2) {
                        AlcatelCall *cont = win->solveConflict(Call, *item);
                        if (cont == NULL) {
                            AlcatelCall old(*item);
                            calls->remove(*item);
                            old.PrevStorage = StorageNone;
                            old.PrevId = -1;
                            old.Modified = true;
                            calls->append(old);
                            calls->append(Call);
                        } else {
                            calls->remove(*item);
                            calls->append(*cont);
                            delete cont;
                        }
                    }
                } else {
                    ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                    calls->remove(*item);
                    calls->append(Call);
                }
            } else {
                ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", Call.getName().latin1());
                calls->append(Call);
            }
            callsVersion++;
            version++;
            slotUpdateAllViews(NULL);
            i++;
        }
        free(cont);

        select_phonebook(PB_MISSED);
        cont = get_contacts(1, 10);
        i = 0;
        while (cont[i].pos != -1) {
            AlcatelCall Call;
            Call.Storage = StorageMobile;
            Call.Type = CallMissed;
            Call.Id = cont[i].pos;
            Call.Name = *(cont[i].name);
            delete cont[i].name;
            Call.Number = cont[i].number;
            free(cont[i].number);

            AlcatelCall *item;
            if ((item = getCallByPrevId(calls, Call.Id, StorageMobile, Call.Type))!=NULL) {
                if (!(item->isSame(Call))) {
                    ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                    if (win->conflictAction == 0) {
                        calls->remove(*item);
                        calls->append(Call);
                    } else if (win->conflictAction == 1) {
                        /* nothing to do here ... */
                    } else if (win->conflictAction == 2) {
                        AlcatelCall *cont = win->solveConflict(Call, *item);
                        if (cont == NULL) {
                            AlcatelCall old(*item);
                            calls->remove(*item);
                            old.PrevStorage = StorageNone;
                            old.PrevId = -1;
                            old.Modified = true;
                            calls->append(old);
                            calls->append(Call);
                        } else {
                            calls->remove(*item);
                            calls->append(*cont);
                            delete cont;
                        }
                    }
                } else {
                    ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                    calls->remove(*item);
                    calls->append(Call);
                }
            } else {
                ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", Call.getName().latin1());
                calls->append(Call);
            }

            callsVersion++;
            version++;
            slotUpdateAllViews(NULL);
            i++;
        }
        free(cont);

        win->slotStatusMsg(i18n("Calls read"),ID_DETAIL_MSG);
    }

    if (what == alcatel_contacts || what == alcatel_contacts_sim || what == alcatel_all) {
        ContactData *cont;
        win->slotStatusMsg(i18n("Reading SIM contacts"),ID_DETAIL_MSG);
        clearContacts(contacts, StorageSIM);
        contactsVersion++;
        version++;
        slotUpdateAllViews(NULL);
        select_phonebook(PB_SIM);
        cont = get_contacts(1, 200);

        i = 0;
        while (cont[i].pos != -1) {
            AlcatelContact Cont;
            Cont.Storage = StorageSIM;
            Cont.Id = cont[i].pos;
            Cont.LastName = *(cont[i].name);
            delete cont[i].name;
            Cont.MainNumber = cont[i].number;
            free(cont[i].number);

            AlcatelContact *item;
            if ((item = getContactByPrevId(contacts, Cont.Id, StorageSIM))!=NULL) {
                if (!(item->isSame(Cont))) {
                    ::message(MSG_DEBUG, "MERGE:Something different -> merging (%s)", item->getName().latin1());
                    if (win->conflictAction == 0) {
                        contacts->remove(*item);
                        contacts->append(Cont);
                    } else if (win->conflictAction == 1) {
                        /* nothing to do here ... */
                    } else if (win->conflictAction == 2) {
                        AlcatelContact *cont = win->solveConflict(Cont, *item);
                        if (cont == NULL) {
                            AlcatelContact old(*item);
                            contacts->remove(*item);
                            old.PrevStorage = StorageNone;
                            old.PrevId = -1;
                            old.Modified = true;
                            contacts->append(old);
                            contacts->append(Cont);
                        } else {
                            contacts->remove(*item);
                            contacts->append(*cont);
                            delete cont;
                        }
                    }
                } else {
                    ::message(MSG_DEBUG, "MERGE:Same records (%s)", item->getName().latin1());
                    contacts->remove(*item);
                    contacts->append(Cont);
                }
            } else {
                ::message(MSG_DEBUG, "MERGE:None correspondent record found (%s)", Cont.getName().latin1());
                contacts->append(Cont);
            }

            contactsVersion++;
            version++;
            slotUpdateAllViews(NULL);
            i++;
        }
        free(cont);

        win->slotStatusMsg(i18n("SIM contacts read"),ID_DETAIL_MSG);
    }

    if (what == alcatel_contacts || what == alcatel_todos || what == alcatel_calendar || what == alcatel_contacts_mobile || what == alcatel_all) {
        win->slotStatusMsg(i18n("Opening binary mode"),ID_DETAIL_MSG);
        alcatel_init();

        if (what == alcatel_contacts || what == alcatel_contacts_mobile || what == alcatel_all) {
            /* at first read categories */
            win->slotStatusMsg(i18n("Reading contact categories"),ID_DETAIL_MSG);
            if (!readMobileCategories(contact_cats, ALC_SYNC_CONTACTS, ALC_SYNC_TYPE_CONTACTS, ALC_LIST_CONTACTS_CAT)) {
                KMessageBox::error(win, i18n("Reading contacts categories failed!"), i18n("Error"));
                alcatel_done();
                version++;
                modified=true;
                slotUpdateAllViews(NULL);
                return false;
            }

            win->slotStatusMsg(i18n("Reading contact items"),ID_DETAIL_MSG);
            if (!readMobileItems(ALC_SYNC_CONTACTS, ALC_SYNC_TYPE_CONTACTS)) {
                KMessageBox::error(win, i18n("Reading contacts failed!"), i18n("Error"));
                alcatel_done();
                version++;
                modified=true;
                slotUpdateAllViews(NULL);
                return false;
            }

            contactsVersion++;
            version++;
        }

        if (what == alcatel_calendar || what == alcatel_all) {
            win->slotStatusMsg(i18n("Reading calendar items"),ID_DETAIL_MSG);
            if (!readMobileItems(ALC_SYNC_CALENDAR, ALC_SYNC_TYPE_CALENDAR)) {
                KMessageBox::error(win, i18n("Reading calendar items failed!"), i18n("Error"));
                alcatel_done();
                version++;
                modified=true;
                slotUpdateAllViews(NULL);
                return false;
            }
            calendarVersion++;
            version++;
        }

        if (what == alcatel_todos || what == alcatel_all) {
            /* at first read categories */
            win->slotStatusMsg(i18n("Reading todo categories"),ID_DETAIL_MSG);
            if (!readMobileCategories(todo_cats, ALC_SYNC_TODO, ALC_SYNC_TYPE_TODO, ALC_LIST_TODO_CAT)) {
                KMessageBox::error(win, i18n("Reading todo categories failed!"), i18n("Error"));
                alcatel_done();
                version++;
                modified=true;
                slotUpdateAllViews(NULL);
                return false;
            }

            win->slotStatusMsg(i18n("Reading todo items"),ID_DETAIL_MSG);
            if (!readMobileItems(ALC_SYNC_TODO, ALC_SYNC_TYPE_TODO)) {
                KMessageBox::error(win, i18n("Reading todos failed!"), i18n("Error"));
                alcatel_done();
                version++;
                modified=true;
                slotUpdateAllViews(NULL);
                return false;
            }

            todosVersion++;
            version++;
        }

        win->slotStatusMsg(i18n("Closing binary mode"),ID_DETAIL_MSG);
        alcatel_done();
    }

    win->modemDisconnect();

    win->slotStatusMsg(i18n("Items read"),ID_DETAIL_MSG);

    version++;
    modified=true;
    slotUpdateAllViews(NULL);

    return true;
}

void KAlcatelDoc::deleteContents() {
    todo_cats->clear();
    contact_cats->clear();

    contacts->clear();
    calendar->clear();
    calls->clear();
    todos->clear();
    messages->clear();

    modified=true;
    version++;
    todosVersion++;
    callsVersion++;
    calendarVersion++;
    contactsVersion++;
    messagesVersion++;
    slotUpdateAllViews(NULL);
    /* TODO: here is maybe something missing.. */
}

int KAlcatelDoc::getVersion() {
    return version;
}

int KAlcatelDoc::getMessagesVersion() {
    return messagesVersion;
}

int KAlcatelDoc::getCallsVersion() {
    return callsVersion;
}

int KAlcatelDoc::getContactsVersion() {
    return contactsVersion;
}

int KAlcatelDoc::getCalendarVersion() {
    return calendarVersion;
}

int KAlcatelDoc::getTodosVersion() {
    return todosVersion;
}

int KAlcatelDoc::getPCStorageId() {
    return pcStorageCounter++;
}

void KAlcatelDoc::updateDocument(AlcDataType which) {
    modified=true;

    version++;

    switch (which) {
        case alcatel_all:
            todosVersion++;
            callsVersion++;
            calendarVersion++;
            contactsVersion++;
            messagesVersion++;
            break;
        case alcatel_todos:
            todosVersion++;
            break;
        case alcatel_messages:
            messagesVersion++;
            break;
        case alcatel_calendar:
            calendarVersion++;
            break;
        case alcatel_calls:
            callsVersion++;
            break;
        case alcatel_contacts:
        case alcatel_contacts_sim:
        case alcatel_contacts_mobile:
            contactsVersion++;
            break;
    }

    slotUpdateAllViews(NULL);
}

bool KAlcatelDoc::commitChanges() {
    KAlcatelApp *win=(KAlcatelApp *) parent();

    if (!win->modemConnect()) return false;

    {
        ::message(MSG_INFO, "Commiting calls... N/A really yet");
        AlcatelCallList::Iterator it;
        for( it = calls->begin(); it != calls->end(); ++it ) {
            if ((*it).Deleted) {
                ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Deleted call: %s", (*it).getName().latin1());
            } else if ((*it).Created) {
                ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Created call: %s", (*it).getName().latin1());
            } else if ((*it).Modified) {
                ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Modified call: %s", (*it).getName().latin1());
            }
        }
    }

    {
        ::message(MSG_INFO, "Commiting SIM contacts... N/A really yet");
        AlcatelContactList::Iterator it;
        for( it = contacts->begin(); it != contacts->end(); ++it ) {
            if ((*it).Storage == StorageSIM || ((*it).Storage == StoragePC && (*it).PrevStorage == StorageSIM)) {
                if ((*it).Deleted) {
                    ::message(MSG_INFO, "NOT IMPLEMENTED ACTION! Deleted contact: %s", (*it).getName().latin1());
                } else if ((*it).Created) {
                    ::message(MSG_INFO, "NOT IMPLEMENTED ACTION! Created contact: %s", (*it).getName().latin1());
                } else if ((*it).Modified) {
                    ::message(MSG_INFO, "NOT IMPLEMENTED ACTION! Modified contact: %s", (*it).getName().latin1());
                }
            }
        }
    }

    alcatel_init();

/*
this shouldn't be here because it should be created on the fly
    {
        ::message(MSG_INFO, "Commiting todo categories... N/A really yet");
        AlcatelCategoryList::Iterator it;
        for( it = todo_cats->begin(); it != todo_cats->end(); ++it ) {
            if ((*it).Deleted) ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Deleted category: %s", (*it).getName().latin1());
            else if ((*it).Created) ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Created category: %s", (*it).getName().latin1());
            else if ((*it).Modified) ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Modified category: %s", (*it).getName().latin1());
        }
    }

    {
        ::message(MSG_INFO, "Commiting contact categories... N/A really yet");
        AlcatelCategoryList::Iterator it;
        for( it = contact_cats->begin(); it != contact_cats->end(); ++it ) {
            if ((*it).Deleted) ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Deleted category: %s", (*it).getName().latin1());
            else if ((*it).Created) ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Created category: %s", (*it).getName().latin1());
            else if ((*it).Modified) ::message(MSG_WARNING, "NOT IMPLEMENTED ACTION! Modified category: %s", (*it).getName().latin1());
        }
    }*/

    {
        ::message(MSG_INFO, "Commiting todos...");
        AlcatelTodoList::Iterator it;
        for( it = todos->begin(); it != todos->end(); ++it ) {
            if ((*it).Storage == StorageMobile || ((*it).Storage == StoragePC && (*it).PrevStorage == StorageMobile)) {
                if ((*it).Deleted) {
                    ::message(MSG_INFO, "NOT IMPLEMENTED ACTION! Deleted todo: %s", (*it).getName().latin1());
                    KMessageBox::sorry(win, i18n("Deleting of todos is not yet implemented."), i18n("Sorry"));
                } else if ((*it).Created) {
                    if (!alcatel_attach() ||
                        !alcatel_start_session() ||
                        !alcatel_select_type(ALC_SYNC_TYPE_TODO)) {
                        KMessageBox::error(win, i18n("Creating of todo failed!"), i18n("Error"));
                    } else {
                        alcatel_begin_transfer(ALC_SYNC_TODO);

                        for (int fieldno = 0; fieldno < (*it).max_field; fieldno++) {
                            AlcatelFieldStruct *field = (*it).getField(fieldno);
                            if (field != NULL) {
                                if (field->data != NULL) {
                                    alcatel_create_field(ALC_SYNC_TYPE_TODO, fieldno, field);
                                    free(field->data);
                                }
                                free(field);
                            }
                        }

                        int num = alcatel_commit(ALC_SYNC_TYPE_TODO);
                        if (num == -1) {
                            KMessageBox::error(win, i18n("Creating of todo failed!"), i18n("Error"));
                        } else {
                            ::message(MSG_INFO, "Created: %s (id = %d)", (*it).getName().latin1(), num);
                            (*it).Created = false;
                            (*it).Modified = false;
                            (*it).Storage = StorageMobile;
                            (*it).PrevStorage = StorageNone;
                            (*it).Id = num;
                        }
                    }
                    alcatel_close_session(ALC_SYNC_TYPE_TODO);
                } else if ((*it).Modified) {
                    if (!alcatel_attach() ||
                        !alcatel_start_session() ||
                        !alcatel_select_type(ALC_SYNC_TYPE_TODO)) {
                        KMessageBox::error(win, i18n("Updating of todo failed!"), i18n("Error"));
                    } else {
                        alcatel_begin_transfer(ALC_SYNC_TODO);

                        int id = (*it).Storage == StorageMobile ? (*it).Id : (*it).PrevId;

                        for (int fieldno = 0; fieldno < (*it).max_field; fieldno++) {
                            AlcatelFieldStruct *field = (*it).getField(fieldno);
                            if (field != NULL) {
                                alcatel_update_field(ALC_SYNC_TYPE_TODO, id, fieldno, field);
                                if (field->data!=NULL) free(field->data);
                                free(field);
                            }
                        }

                        alcatel_commit(ALC_SYNC_TYPE_TODO);
                        ::message(MSG_INFO, "Modified: %s", (*it).getName().latin1());
                        (*it).Modified = false;
                        (*it).Storage = StorageMobile;
                        (*it).PrevStorage = StorageNone;
                        (*it).Id = id;
                    }
                    alcatel_close_session(ALC_SYNC_TYPE_TODO);
                }
            }
        }
    }

    {
        ::message(MSG_INFO, "Commiting events...");
        AlcatelCalendarList::Iterator it;
        for( it = calendar->begin(); it != calendar->end(); ++it ) {
            if ((*it).Storage == StorageMobile || ((*it).Storage == StoragePC && (*it).PrevStorage == StorageMobile)) {
                if ((*it).Deleted) {
                    ::message(MSG_INFO, "NOT IMPLEMENTED ACTION! Deleted event: %s", (*it).getName().latin1());
                    KMessageBox::sorry(win, i18n("Deleting of events is not yet implemented."), i18n("Sorry"));
                } else if ((*it).Created) {
                    if (!alcatel_attach() ||
                        !alcatel_start_session() ||
                        !alcatel_select_type(ALC_SYNC_TYPE_CALENDAR)) {
                        KMessageBox::error(win, i18n("Creating of event failed!"), i18n("Error"));
                    } else {
                        alcatel_begin_transfer(ALC_SYNC_CALENDAR);

                        for (int fieldno = 0; fieldno < (*it).max_field; fieldno++) {
                            AlcatelFieldStruct *field = (*it).getField(fieldno);
                            if (field != NULL) {
                                if (field->data != NULL) {
                                    alcatel_create_field(ALC_SYNC_TYPE_CALENDAR, fieldno, field);
                                    free(field->data);
                                }
                                free(field);
                            }
                        }

                        int num = alcatel_commit(ALC_SYNC_TYPE_CALENDAR);
                        if (num == -1) {
                            KMessageBox::error(win, i18n("Creating of event failed!"), i18n("Error"));
                        } else {
                            ::message(MSG_INFO, "Created: %s (id = %d)", (*it).getName().latin1(), num);
                            (*it).Created = false;
                            (*it).Modified = false;
                            (*it).Storage = StorageMobile;
                            (*it).PrevStorage = StorageNone;
                            (*it).Id = num;
                        }
                    }
                    alcatel_close_session(ALC_SYNC_TYPE_CALENDAR);
                } else if ((*it).Modified) {
                    if (!alcatel_attach() ||
                        !alcatel_start_session() ||
                        !alcatel_select_type(ALC_SYNC_TYPE_CALENDAR)) {
                        KMessageBox::error(win, i18n("Updating of event failed!"), i18n("Error"));
                    } else {
                        alcatel_begin_transfer(ALC_SYNC_CALENDAR);

                        int id = (*it).Storage == StorageMobile ? (*it).Id : (*it).PrevId;

                        for (int fieldno = 0; fieldno < (*it).max_field; fieldno++) {
                            AlcatelFieldStruct *field = (*it).getField(fieldno);
                            if (field != NULL) {
                                alcatel_update_field(ALC_SYNC_TYPE_CALENDAR, id, fieldno, field);
                                if (field->data!=NULL) free(field->data);
                                free(field);
                            }
                        }

                        alcatel_commit(ALC_SYNC_TYPE_CALENDAR);
                        ::message(MSG_INFO, "Modified: %s", (*it).getName().latin1());
                        (*it).Modified = false;
                        (*it).Storage = StorageMobile;
                        (*it).PrevStorage = StorageNone;
                        (*it).Id = id;
                    }
                    alcatel_close_session(ALC_SYNC_TYPE_CALENDAR);
                }
            }
        }
    }

    {
        ::message(MSG_INFO, "Commiting contacts...");
        AlcatelContactList::Iterator it;


        for( it = contacts->begin(); it != contacts->end(); ++it ) {
            if ((*it).Storage == StorageMobile || ((*it).Storage == StoragePC && (*it).PrevStorage == StorageMobile)) {
                if ((*it).Deleted) {
                    ::message(MSG_INFO, "NOT IMPLEMENTED ACTION! Deleted contact: %s", (*it).getName().latin1());
                    KMessageBox::sorry(win, i18n("Deleting of contacts is not yet implemented."), i18n("Sorry"));
                } else if ((*it).Created) {
                    if (!alcatel_attach() ||
                        !alcatel_start_session() ||
                        !alcatel_select_type(ALC_SYNC_TYPE_CONTACTS)) {
                        KMessageBox::error(win, i18n("Creating of contact failed!"), i18n("Error"));
                    } else {
                        alcatel_begin_transfer(ALC_SYNC_CONTACTS);

                        for (int fieldno = 0; fieldno < (*it).max_field; fieldno++) {
                            AlcatelFieldStruct *field = (*it).getField(fieldno);
                            if (field != NULL) {
                                if (field->data != NULL) {
                                    alcatel_create_field(ALC_SYNC_TYPE_CONTACTS, fieldno, field);
                                    free(field->data);
                                }
                                free(field);
                            }
                        }

                        int num = alcatel_commit(ALC_SYNC_TYPE_CONTACTS);
                        if (num == -1) {
                            KMessageBox::error(win, i18n("Creating of contact failed!"), i18n("Error"));
                        } else {
                            ::message(MSG_INFO, "Created: %s (id = %d)", (*it).getName().latin1(), num);
                            (*it).Created = false;
                            (*it).Modified = false;
                            (*it).Storage = StorageMobile;
                            (*it).PrevStorage = StorageNone;
                            (*it).Id = num;
                        }
                    }
                    alcatel_close_session(ALC_SYNC_TYPE_CONTACTS);
                } else if ((*it).Modified) {
                    if (!alcatel_attach() ||
                        !alcatel_start_session() ||
                        !alcatel_select_type(ALC_SYNC_TYPE_CONTACTS)) {
                        KMessageBox::error(win, i18n("Updating of contact failed!"), i18n("Error"));
                    } else {
                        alcatel_begin_transfer(ALC_SYNC_CONTACTS);

                        int id = (*it).Storage == StorageMobile ? (*it).Id : (*it).PrevId;

                        for (int fieldno = 0; fieldno < (*it).max_field; fieldno++) {
                            AlcatelFieldStruct *field = (*it).getField(fieldno);
                            if (field != NULL) {
                                alcatel_update_field(ALC_SYNC_TYPE_CONTACTS, id, fieldno, field);
                                if (field->data!=NULL) free(field->data);
                                free(field);
                            }
                        }

                        alcatel_commit(ALC_SYNC_TYPE_CONTACTS);
                        ::message(MSG_INFO, "Modified: %s", (*it).getName().latin1());
                        (*it).Modified = false;
                        (*it).Storage = StorageMobile;
                        (*it).PrevStorage = StorageNone;
                        (*it).Id = id;
                    }
                    alcatel_close_session(ALC_SYNC_TYPE_CONTACTS);
                }
            }
        }
    }

    alcatel_done();

    win->modemDisconnect();

    contactsVersion++;
    version++;
    modified=true;
    slotUpdateAllViews(NULL);

    win->slotStatusMsg(i18n("Items commited"),ID_DETAIL_MSG);

    return true;
}
