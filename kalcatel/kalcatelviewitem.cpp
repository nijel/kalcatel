/*
 * kalcatelviewitem.cpp
 *
 * TODO: description
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

#include <qdatetime.h>
#include <qpalette.h>
#include <qpainter.h>
#include <qfont.h>

#include <klocale.h>
#include <kiconloader.h>

#include "kalcatelviewitem.h"
#include "alcatelclasses.h"
#include "alcasync/logging.h"

#define epoch_time      "%010d"
#define epoch_empty     "          "
#define secs_time       "%04d"
#define position_type   "%d-%04d"

KAlcatelDataItem::KAlcatelDataItem ( QListView * parent, AlcatelClass *data ) : QListViewItem ( parent ) {
    alcatelData = data;
}

void KAlcatelDataItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment ) {
    QColorGroup _cg( cg );
    QColor c = _cg.text();
    QFont f = p->font();
    bool it = f.italic();
    bool bld = f.bold();

    if ( alcatelData->Deleted )
        _cg.setColor( QColorGroup::Text, Qt::red );
    if ( alcatelData->Created || alcatelData->Modified)
        f.setItalic(true);
    if ( (strcmp(alcatelData->getClassName(), "AlcatelTodo") == 0) && (((AlcatelTodo *)alcatelData)->Completed == 0))
        f.setBold(true);
    p->setFont(f);

    QListViewItem::paintCell( p, _cg, column, width, alignment );

    f.setItalic(it);
    f.setBold(bld);
    p->setFont(f);
    _cg.setColor( QColorGroup::Text, c );
}

KAlcatelMessageViewItem::KAlcatelMessageViewItem ( QListView * parent, AlcatelMessage *data, AlcatelContact *cont) : KAlcatelDataItem ( parent, data ) {
    contact = cont;
    update();
}

void KAlcatelMessageViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, QString(((AlcatelMessage *)alcatelData)->Sender));
    setText( 2, contact == NULL? i18n("Unknown") : contact->getName());
    setText( 3, MessageTypes[((AlcatelMessage *)alcatelData)->Status]);
    setText( 4, ((AlcatelMessage *)alcatelData)->Date.date().toString());
    setText( 5, ((AlcatelMessage *)alcatelData)->Date.time().toString());
    setText( 6, ((AlcatelMessage *)alcatelData)->Text);
    setText( 7, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelMessageViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 4) {
        QString tmpString;
        QDateTime epoch( QDate(1970,1,1), QTime(0,0) );
        tmpString.sprintf( epoch_time, epoch.secsTo(((AlcatelMessage *)alcatelData)->Date) );
        return tmpString;
    } else if (column == 5) {
        QString tmpString;
        QTime epoch( 0, 0 );
        tmpString.sprintf( secs_time, epoch.secsTo(((AlcatelMessage *)alcatelData)->Date.time()) );
        return tmpString;
    } else if (column == 7) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelMessageCatViewItem::KAlcatelMessageCatViewItem ( QListView * parent, AlcatelMessage *data, AlcatelContact *cont) : KAlcatelDataItem ( parent, data ) {
    contact = cont;
    update();
}

void KAlcatelMessageCatViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, QString(((AlcatelMessage *)alcatelData)->Sender));
    setText( 2, contact == NULL? i18n("Unknown") : contact->getName());
    setText( 3, ((AlcatelMessage *)alcatelData)->Date.date().toString());
    setText( 4, ((AlcatelMessage *)alcatelData)->Date.time().toString());
    setText( 5, ((AlcatelMessage *)alcatelData)->Text);
    setText( 6, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelMessageCatViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 3) {
        QString tmpString;
        QDateTime epoch( QDate(1970,1,1), QTime(0,0) );
        tmpString.sprintf( epoch_time, epoch.secsTo(((AlcatelMessage *)alcatelData)->Date) );
        return tmpString;
    } else if (column == 4) {
        QString tmpString;
        QTime epoch( 0, 0 );
        tmpString.sprintf( secs_time, epoch.secsTo(((AlcatelMessage *)alcatelData)->Date.time()) );
        return tmpString;
    } else if (column == 6) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelCallViewItem::KAlcatelCallViewItem ( QListView * parent, AlcatelCall *data, AlcatelContact *cont) : KAlcatelDataItem ( parent, data ) {
    contact = cont;
    update();
}

void KAlcatelCallViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelCall *)alcatelData)->Number);
    setText( 2, ((AlcatelCall *)alcatelData)->Name);
    setText( 3, contact == NULL? i18n("Unknown") : contact->getName());
    setText( 4, CallTypes[((AlcatelCall *)alcatelData)->Type]);
    setText( 5, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelCallViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 4) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        tmpString.prepend(text(column));
        return tmpString;
    } else if (column == 5) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelCallCatViewItem::KAlcatelCallCatViewItem ( QListView * parent, AlcatelCall *data, AlcatelContact *cont) : KAlcatelDataItem ( parent, data ) {
    contact = cont;
    update();
}

void KAlcatelCallCatViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelCall *)alcatelData)->Number);
    setText( 2, ((AlcatelCall *)alcatelData)->Name);
    setText( 3, contact == NULL? i18n("Unknown") : contact->getName());
    setText( 4, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelCallCatViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 4) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelTodoViewItem::KAlcatelTodoViewItem ( QListView *parent, AlcatelTodo *data, AlcatelContact *cont, QString catname) : KAlcatelDataItem ( parent, data ) {
    contact = cont;
    categoryname = catname;
    update();
}

void KAlcatelTodoViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelTodo *)alcatelData)->Completed == -1 ? QString(""): ((AlcatelTodo *)alcatelData)->Completed ? i18n("Yes") : i18n("No"));
    setText( 2, ((AlcatelTodo *)alcatelData)->Priority == -1 ? QString("") : Priorities[((AlcatelTodo *)alcatelData)->Priority]);
    setText( 3, ((AlcatelTodo *)alcatelData)->DueDate.isNull()?i18n("none_date", "None"):((AlcatelTodo *)alcatelData)->DueDate.toString());
    setText( 4, (((AlcatelTodo *)alcatelData)->Subject.isEmpty() && ((AlcatelTodo *)alcatelData)->ContactID != -1 && contact != NULL) ? i18n("Call to %1").arg(contact->getName()) : ((AlcatelTodo *)alcatelData)->Subject);
    setText( 5, categoryname);
    setText( 6, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelTodoViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 2) {
        QString tmpString;
        tmpString.sprintf( "%d", 3 - ((AlcatelTodo *)alcatelData)->Priority );
        return tmpString;
    } else if (column == 3) {
        QString tmpString;
        QDateTime epoch( QDate(1970,1,1), QTime(0,0) );
        tmpString.sprintf( epoch_time, epoch.secsTo(((AlcatelTodo *)alcatelData)->DueDate) );
        return tmpString;
    } else if (column == 6) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelTodoCatViewItem::KAlcatelTodoCatViewItem ( QListView *parent, AlcatelTodo *data, AlcatelContact *cont) : KAlcatelDataItem ( parent, data ) {
    contact = cont;
    update();
}

void KAlcatelTodoCatViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelTodo *)alcatelData)->Completed == -1 ? QString(""): ((AlcatelTodo *)alcatelData)->Completed ? i18n("Yes") : i18n("No"));
    setText( 2, ((AlcatelTodo *)alcatelData)->Priority == -1 ? QString("") : Priorities[((AlcatelTodo *)alcatelData)->Priority]);
    setText( 3, ((AlcatelTodo *)alcatelData)->DueDate.isNull()?i18n("none_date", "None"):((AlcatelTodo *)alcatelData)->DueDate.toString());
    setText( 4, (((AlcatelTodo *)alcatelData)->Subject.isEmpty() && ((AlcatelTodo *)alcatelData)->ContactID != -1 && contact != NULL) ? i18n("Call to %1").arg(contact->getName()) : ((AlcatelTodo *)alcatelData)->Subject);
    setText( 5, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelTodoCatViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 2) {
        QString tmpString;
        tmpString.sprintf( "%d", 3 - ((AlcatelTodo *)alcatelData)->Priority );
        return tmpString;
    } else if (column == 3) {
        QString tmpString;
        QDateTime epoch( QDate(1970,1,1), QTime(0,0) );
        tmpString.sprintf( epoch_time, epoch.secsTo(((AlcatelTodo *)alcatelData)->DueDate) );
        return tmpString;
    } else if (column == 5) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelContactViewItem::KAlcatelContactViewItem ( QListView * parent, AlcatelContact *data) : KAlcatelDataItem ( parent, data ) {
    update();
}

void KAlcatelContactViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelContact *)alcatelData)->getName());
    /* try to find non-empty phone */
    setText( 2, QString(!((AlcatelContact *)alcatelData)->MainNumber.isEmpty()?((AlcatelContact *)alcatelData)->MainNumber:
                            !((AlcatelContact *)alcatelData)->MobileNumber.isEmpty()?((AlcatelContact *)alcatelData)->MobileNumber:
                                !((AlcatelContact *)alcatelData)->WorkNumber.isEmpty()?((AlcatelContact *)alcatelData)->WorkNumber:
                                    !((AlcatelContact *)alcatelData)->HomeNumber.isEmpty()?((AlcatelContact *)alcatelData)->HomeNumber:
                                        !((AlcatelContact *)alcatelData)->OtherNumber.isEmpty()?((AlcatelContact *)alcatelData)->OtherNumber:
                                            !((AlcatelContact *)alcatelData)->FaxNumber.isEmpty()?((AlcatelContact *)alcatelData)->FaxNumber:
                                                ((AlcatelContact *)alcatelData)->PagerNumber));
    setText( 3, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelContactViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 3) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelContactMobileViewItem::KAlcatelContactMobileViewItem ( QListView * parent, AlcatelContact *data, QString catname) : KAlcatelDataItem ( parent, data ) {
    categoryname = catname;
    update();
}

void KAlcatelContactMobileViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelContact *)alcatelData)->LastName);
    setText( 2, ((AlcatelContact *)alcatelData)->FirstName);
    setText( 3, categoryname);
    setText( 4, ((AlcatelContact *)alcatelData)->MobileNumber);
    setText( 5, ((AlcatelContact *)alcatelData)->WorkNumber);
    setText( 6, ((AlcatelContact *)alcatelData)->MainNumber);
    setText( 7, ((AlcatelContact *)alcatelData)->Email1);
    setText( 8, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelContactMobileViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 8) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelContactMobileCatViewItem::KAlcatelContactMobileCatViewItem ( QListView * parent, AlcatelContact *data) : KAlcatelDataItem ( parent, data ) {
    update();
}

void KAlcatelContactMobileCatViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelContact *)alcatelData)->LastName);
    setText( 2, ((AlcatelContact *)alcatelData)->FirstName);
    setText( 3, ((AlcatelContact *)alcatelData)->MobileNumber);
    setText( 4, ((AlcatelContact *)alcatelData)->WorkNumber);
    setText( 5, ((AlcatelContact *)alcatelData)->MainNumber);
    setText( 6, ((AlcatelContact *)alcatelData)->Email1);
    setText( 7, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelContactMobileCatViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 7) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelContactSIMViewItem::KAlcatelContactSIMViewItem ( QListView * parent, AlcatelContact *data) : KAlcatelDataItem ( parent, data ) {
    update();
}

void KAlcatelContactSIMViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelContact *)alcatelData)->LastName);
    setText( 2, ((AlcatelContact *)alcatelData)->MainNumber);
    setText( 3, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelContactSIMViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 3) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelCalendarViewItem::KAlcatelCalendarViewItem ( QListView * parent, AlcatelCalendar *data, AlcatelContact *cont) : KAlcatelDataItem ( parent, data ) {
    contact = cont;
    update();
}

void KAlcatelCalendarViewItem::update() {
    if (alcatelData->Deleted) {
        setPixmap(0, SmallIcon("kalcatel-deleted.png"));
    } else if (alcatelData->Created) {
        setPixmap(0, SmallIcon("kalcatel-created.png"));
    } else if (alcatelData->Modified) {
        setPixmap(0, SmallIcon("kalcatel-modified.png"));
    } else if (alcatelData->Storage == StoragePC){
        setPixmap(0, SmallIcon("kalcatel-computer.png"));
    } else {
        setPixmap(0, SmallIcon("kalcatel-mobile.png"));
    }

    setText( 1, ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_DAILY_ALARM || ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : ((AlcatelCalendar *)alcatelData)->Date.isNull()?i18n("none_date", "None"):((AlcatelCalendar *)alcatelData)->Date.toString());
    setText( 2, ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_BIRTHDAY || ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_DAILY_ALARM || ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : ((AlcatelCalendar *)alcatelData)->StartTime.toString());
    setText( 3, ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_BIRTHDAY || ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_DAILY_ALARM || ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_ALARM ? i18n("N/A") : ((AlcatelCalendar *)alcatelData)->EndTime.toString());
    setText( 4, ((AlcatelCalendar *)alcatelData)->EventType!=-1?CalendarTypes[((AlcatelCalendar *)alcatelData)->EventType]:i18n("Unknown"));
    setText( 5, (((AlcatelCalendar *)alcatelData)->Subject.isEmpty() && ((AlcatelCalendar *)alcatelData)->EventType == ALC_CALENDAR_CALL && contact != NULL) ? i18n("Call to %1").arg(contact->getName()) : ((AlcatelCalendar *)alcatelData)->Subject);
    setText( 6, ((AlcatelCalendar *)alcatelData)->Alarm.isNull()?i18n("none_alarm", "None"):((AlcatelCalendar *)alcatelData)->Alarm.toString());
    setText( 7, ((AlcatelCalendar *)alcatelData)->Repeating());
    setText( 8, QString("%1 %2").arg(StorageTypes[alcatelData->Storage]).arg(alcatelData->Id));
}

QString KAlcatelCalendarViewItem::key ( int column, bool ascending ) const {
    if (column == 0) {
        if (alcatelData->Deleted) return "1";
        if (alcatelData->Modified) return "2";
        if (alcatelData->Storage == StoragePC) return "3";
        return "4";
    } else if (column == 1) {
        QString tmpString;
        if (((AlcatelCalendar *)alcatelData)->Date.isNull())
            tmpString.sprintf( epoch_empty );
        else
            tmpString.sprintf( epoch_time, QDate(1970,1,1).daysTo(((AlcatelCalendar *)alcatelData)->Date) );
        return tmpString;
    } else if (column == 2) {
        QString tmpString;
        tmpString.sprintf( epoch_time, QDateTime(QDate(1970,1,1),QTime(0,0)).secsTo(QDateTime(((AlcatelCalendar *)alcatelData)->Date,((AlcatelCalendar *)alcatelData)->StartTime)));
        return tmpString;
    } else if (column == 3) {
        QString tmpString;
        tmpString.sprintf( epoch_time, QDateTime(QDate(1970,1,1),QTime(0,0)).secsTo(QDateTime(((AlcatelCalendar *)alcatelData)->Date,((AlcatelCalendar *)alcatelData)->EndTime)));
        return tmpString;
    } else if (column == 6) {
        QString tmpString;
        tmpString.sprintf( epoch_time, QDateTime(QDate(1970,1,1),QTime(0,0)).secsTo(((AlcatelCalendar *)alcatelData)->Alarm));
        return tmpString;
    } else if (column == 8) {
        QString tmpString;
        tmpString.sprintf( position_type, alcatelData->Storage, alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

