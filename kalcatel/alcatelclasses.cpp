/*
 * 
 * alcatelclasses.cpp
 *
 * classes to store data from mobile
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

#include <klocale.h>

#include "alcatelclasses.h"
#include "alcatool/logging.h"
#include "alcatool/alcatel.h"

/**
 * used for type checking
 */
#define chk_type(which) if ((*data).type != which) message( MSG_ERROR, "Type check failed for item %d!", number); else


QString MessageTypes[] = { i18n("Unread"), i18n("Read"), i18n("Unsent"), i18n("Sent") };
QString StorageTypes[]= { i18n("PC"), i18n("SIM"), i18n("Mobile") };
QString CallTypes[] = { i18n("Missed"), i18n("Received"), i18n("Dialled") };
QString Priorities[] = { i18n("High"), i18n("Normal"), i18n("Low") };
QString CalendarTypes[] = {
    i18n("Appointment"),
    i18n("Unknown(1)"),
    i18n("Birthday"),
    i18n("Call"),
    i18n("Alarm"),
    i18n("Unknown(5)"),
    i18n("Unknown(6)"),
    i18n("Unknown(7)"),
    i18n("Unknown(8)"),
    i18n("Repeating") };

QString DayNames[] = {
    i18n("Sunday"),
    i18n("Monday"),
    i18n("Tuesday"),
    i18n("Wednesday"),
    i18n("Thursday"),
    i18n("Friday"),
    i18n("Saturday"),
    i18n("Sunday") };

QString MonthNames[] = {
    i18n("None"),
    i18n("January"),
    i18n("February"),
    i18n("March"),
    i18n("April"),
    i18n("May"),
    i18n("June"),
    i18n("July"),
    i18n("August"),
    i18n("September"),
    i18n("October"),
    i18n("November"),
    i18n("December") };

AlcatelClass::AlcatelClass() {
    Id = -1;
    Storage = StorageNone;
    PrevId = -1;
    PrevStorage = StorageNone;
    Modified = false;
}

AlcatelContact::AlcatelContact() {
    Category = -1;
    Private = -1;
}

AlcatelContact::~AlcatelContact() {
}

QString AlcatelContact::Name(void) {
    return (FirstName.isEmpty())?
                            ((LastName.isEmpty())?
                                QString("?"):
                                QString(LastName)):
                            ((LastName.isEmpty())?
                                QString(FirstName):
                                QString("%1, %2").arg(LastName).arg(FirstName));
}


void AlcatelContact::setField(int number, AlcatelFieldStruct *data) {
    switch (number) {
        case 0: chk_type(_string) LastName.setLatin1((char *)((*data).data)); break;
        case 1: chk_type(_string) FirstName.setLatin1((char *)((*data).data)); break;
        case 2: chk_type(_string) Company.setLatin1((char *)((*data).data)); break;
        case 3: chk_type(_string) JobTitle.setLatin1((char *)((*data).data)); break;
        case 4: chk_type(_string) Note.setLatin1((char *)((*data).data)); break;
        case 5: chk_type(_byte) Category = *((int *)((*data).data)); break;
        case 6: chk_type(_bool) Private = *((int *)((*data).data)); break;
        case 7: chk_type(_phone) WorkNumber.setLatin1((char *)((*data).data)); break;
        case 8: chk_type(_phone) MainNumber.setLatin1((char *)((*data).data)); break;
        case 9: chk_type(_phone) FaxNumber.setLatin1((char *)((*data).data)); break;
        case 10: chk_type(_phone) OtherNumber.setLatin1((char *)((*data).data)); break;
        case 11: chk_type(_phone) PagerNumber.setLatin1((char *)((*data).data)); break;
        case 12: chk_type(_phone) MobileNumber.setLatin1((char *)((*data).data)); break;
        case 13: chk_type(_phone) HomeNumber.setLatin1((char *)((*data).data)); break;
        case 14: chk_type(_string) Email1.setLatin1((char *)((*data).data)); break;
        case 15: chk_type(_string) Email2.setLatin1((char *)((*data).data)); break;
        case 16: chk_type(_string) Address.setLatin1((char *)((*data).data)); break;
        case 17: chk_type(_string) City.setLatin1((char *)((*data).data)); break;
        case 18: chk_type(_string) State.setLatin1((char *)((*data).data)); break;
        case 19: chk_type(_string) Zip.setLatin1((char *)((*data).data)); break;
        case 20: chk_type(_string) Country.setLatin1((char *)((*data).data)); break;
        case 21: chk_type(_string) Custom1.setLatin1((char *)((*data).data)); break;
        case 22: chk_type(_string) Custom2.setLatin1((char *)((*data).data)); break;
        case 23: chk_type(_string) Custom3.setLatin1((char *)((*data).data)); break;
        case 24: chk_type(_string) Custom4.setLatin1((char *)((*data).data)); break;

        default: message(MSG_WARNING, "Unknown field occured (%02d)!", number); break;
    }
}

AlcatelCalendar::AlcatelCalendar() {
    Private = -1;
    EventType = -1;
    ContactID = -1;

    DayOfWeek = -1;
    Day = -1;
    WeekOfMonth = -1;
    Month = -1;
    Frequency = -1;
}

AlcatelCalendar::~AlcatelCalendar() {
}

QString AlcatelCalendar::Repeating(void) {
    static QString birthday = i18n("Each year (birthday)");
    static QString none = i18n("None");
    static QString unknown = i18n("Unknown");
    static QString forever = i18n("Forever");
    switch (EventType) {
        case ALC_CALENDAR_BIRTHDAY:
            return birthday;
        case ALC_CALENDAR_APPOINTMENT:
        case ALC_CALENDAR_ALARM:
        case ALC_CALENDAR_CALL:
            return none;
        case ALC_CALENDAR_REPEATING:
            if (StartDate.isNull() && StopDate.isNull())
                return forever;
            else if (StartDate.isNull())
                return i18n("Till %2").arg(StopDate.toString());
            else if (StopDate.isNull())
                return i18n("From %1").arg(StartDate.toString());
            else
                return i18n("From %1 till %2").arg(StartDate.toString()).arg(StopDate.toString());
        default:
            return unknown;
    }
}

QString AlcatelCalendar::RepeatingDetail(void) {
    static QString birthday = i18n("Each year (birthday)");
    static QString none = i18n("None");
    static QString unknown = i18n("Unknown");
    static QString forever = i18n("Forever");
    QString text;
    switch (EventType) {
        case ALC_CALENDAR_BIRTHDAY:
            return birthday;
        case ALC_CALENDAR_APPOINTMENT:
        case ALC_CALENDAR_ALARM:
        case ALC_CALENDAR_CALL:
            return none;
        case ALC_CALENDAR_REPEATING:
            if (StartDate.isNull() && StopDate.isNull())
                text.append(forever);
            else if (StartDate.isNull())
                text.append(i18n("Till %2").arg(StopDate.toString()));
            else if (StopDate.isNull())
                text.append(i18n("From %1").arg(StartDate.toString()));
            else
                text.append(i18n("From %1 till %2").arg(StartDate.toString()).arg(StopDate.toString()));

            if (Frequency==1)
                text.append(i18n(" on each"));
            else if (Frequency>1)
                text.append(i18n(" on each %1.").arg(Frequency));
            else
                return text;

            if (DayOfWeek!=0) {
                text.append(i18n(" %1").arg(DayNames[DayOfWeek]));
                if (WeekOfMonth!=0)
                    text.append(i18n(" in %1. week of").arg(WeekOfMonth));
                if (Month!=0)
                    text.append(i18n(" %1").arg(MonthNames[Month]));
                else
                    text.append(i18n(" month"));
            } else if (Day!=0) {
                text.append(i18n(" %1. day of").arg(Day));
                if (Month!=0)
                    text.append(i18n(" %1").arg(MonthNames[Month]));
                else
                    text.append(i18n(" each moth"));
            }
            return text;

        default:
            return unknown;
    }
}

void AlcatelCalendar::setField(int number, AlcatelFieldStruct *data) {
    switch (number) {
        case 0: chk_type(_date) Date.setYMD(((AlcatelDateStruct *)((*data).data))->year, ((AlcatelDateStruct *)((*data).data))->month, ((AlcatelDateStruct *)((*data).data))->day); break;
        case 1: chk_type(_time) StartTime.setHMS (((AlcatelTimeStruct *)((*data).data))->hour, ((AlcatelTimeStruct *)((*data).data))->minute, ((AlcatelTimeStruct *)((*data).data))->second); break;
        case 2: chk_type(_time) EndTime.setHMS (((AlcatelTimeStruct *)((*data).data))->hour, ((AlcatelTimeStruct *)((*data).data))->minute, ((AlcatelTimeStruct *)((*data).data))->second); break;
        case 3: chk_type(_date) Alarm.setDate(QDate ( ((AlcatelDateStruct *)((*data).data))->year, ((AlcatelDateStruct *)((*data).data))->month, ((AlcatelDateStruct *)((*data).data))->day)); break;
        case 4: chk_type(_time) Alarm.setTime(QTime ( ((AlcatelTimeStruct *)((*data).data))->hour, ((AlcatelTimeStruct *)((*data).data))->minute, ((AlcatelTimeStruct *)((*data).data))->second)); break;
        case 5: chk_type(_string) Subject.setLatin1((char *)((*data).data)); break;
        case 6: chk_type(_bool) Private = *((int *)((*data).data)); break;
        case 7: chk_type(_enum) EventType = *((int *)((*data).data)); break;
        case 8: chk_type(_int) ContactID = *((int *)((*data).data)); break;

        case 10: chk_type(_byte) DayOfWeek = *((int *)((*data).data)); break;
        case 11: chk_type(_byte) Day = *((int *)((*data).data)); break;
        case 12: chk_type(_byte) WeekOfMonth = *((int *)((*data).data)); break;
        case 13: chk_type(_byte) Month = *((int *)((*data).data)); break;

        case 17: chk_type(_byte) Frequency = *((int *)((*data).data)); break;
        case 18: chk_type(_date) StartDate.setYMD(((AlcatelDateStruct *)((*data).data))->year, ((AlcatelDateStruct *)((*data).data))->month, ((AlcatelDateStruct *)((*data).data))->day); break;
        case 19: chk_type(_date) StopDate.setYMD(((AlcatelDateStruct *)((*data).data))->year, ((AlcatelDateStruct *)((*data).data))->month, ((AlcatelDateStruct *)((*data).data))->day); break;
        case 20: chk_type(_date) Alarm2.setDate(QDate ( ((AlcatelDateStruct *)((*data).data))->year, ((AlcatelDateStruct *)((*data).data))->month, ((AlcatelDateStruct *)((*data).data))->day)); break;
        case 21: chk_type(_time) Alarm2.setTime(QTime ( ((AlcatelTimeStruct *)((*data).data))->hour, ((AlcatelTimeStruct *)((*data).data))->minute, ((AlcatelTimeStruct *)((*data).data))->second)); break;

        default: message(MSG_WARNING, "Unknown field occured (%02d)!", number); break;
    }
}

AlcatelTodo::AlcatelTodo() {
    Completed = -1;
    Private = -1;
    Category = -1;
    Priority = -1;
    ContactID = -1;
}

AlcatelTodo::~AlcatelTodo() {
}

void AlcatelTodo::setField(int number, AlcatelFieldStruct *data) {
    switch (number) {
        case 0: chk_type(_date) DueDate.setYMD(((AlcatelDateStruct *)((*data).data))->year, ((AlcatelDateStruct *)((*data).data))->month, ((AlcatelDateStruct *)((*data).data))->day); break;
        case 1: chk_type(_bool) Completed = *((int *)((*data).data)); break;
        case 2: chk_type(_date) Alarm.setDate(QDate ( ((AlcatelDateStruct *)((*data).data))->year, ((AlcatelDateStruct *)((*data).data))->month, ((AlcatelDateStruct *)((*data).data))->day)); break;
        case 3: chk_type(_time) Alarm.setTime(QTime ( ((AlcatelTimeStruct *)((*data).data))->hour, ((AlcatelTimeStruct *)((*data).data))->minute, ((AlcatelTimeStruct *)((*data).data))->second)); break;
        case 4: chk_type(_string) Subject.setLatin1((char *)((*data).data)); break;
        case 5: chk_type(_bool) Private = *((int *)((*data).data)); break;
        case 6: chk_type(_byte) Category = *((int *)((*data).data)); break;
        case 7: chk_type(_enum) Priority = *((int *)((*data).data)); break;
        case 8: chk_type(_int) ContactID = *((int *)((*data).data)); break;

        default: message(MSG_WARNING, "Unknown field occured (%02d)!", number); break;
    }
}

AlcatelMessage::AlcatelMessage() {
    Id = -1;
    Status = -1;
    Length = -1;
    Raw = NULL;
}

AlcatelMessage::~AlcatelMessage() {
    if (Raw != NULL) {
        free( Raw );
        Raw = NULL;
    }
}

AlcatelCall::AlcatelCall() {
}

AlcatelCall::~AlcatelCall() {
}

AlcatelCategory::AlcatelCategory(char* name, int id, AlcatelStorage storage) {
    Name = QString().setLatin1(name);
    Id = id;
    Storage = storage;
}

AlcatelCategory::AlcatelCategory() {
    Name = QString("");
    Id = -1;
}

AlcatelMessage *getMessageById(AlcatelMessageList *list, int id) {
    AlcatelMessageList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id)
            return &(*it);
    }
    return NULL;
}

QString *getCategoryName(AlcatelCategoryList *list, int id) {
    static QString unk = i18n("Unknown");
    static QString non = i18n("None");
    if (id == -1) return &non;
    AlcatelCategoryList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id)
            return &((*it).Name);
    }
    return &unk;
}

int phoneCmp(QString *number1, QString *number2, QString *prefix) {
    QString n1 = QString(*number1);
    QString n2 = QString(*number2);
    if (n1[0] != '+')
        n1.prepend(*prefix);
    if (n2[0] != '+')
        n2.prepend(*prefix);
    if (!(n1 == n2)) {
        n1 = *number1;
        n2 = *number2;
        n1.prepend("+");
        n2.prepend("+");
        return n1 == n2;
    } else {
        return true;
    }
}

AlcatelContact *getContactByPhone(AlcatelContactList *list, QString *number, QString *prefix) {
    AlcatelContactList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if (phoneCmp(&((* it).MainNumber), number, prefix) ||
            phoneCmp(&((* it).WorkNumber), number, prefix) ||
            phoneCmp(&((* it).FaxNumber), number, prefix) ||
            phoneCmp(&((* it).OtherNumber), number, prefix) ||
            phoneCmp(&((* it).PagerNumber), number, prefix) ||
            phoneCmp(&((* it).MobileNumber), number, prefix) ||
            phoneCmp(&((* it).HomeNumber), number, prefix)
            )
            return &(*it);
    }
    return NULL;
}

AlcatelContact *getContactById(AlcatelContactList *list, int id, AlcatelStorage type) {
    AlcatelContactList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id && (* it).Storage == type)
            return &(*it);
    }
    return NULL;
}

AlcatelTodo *getTodoById(AlcatelTodoList *list, int id) {
    AlcatelTodoList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id)
            return &(*it);
    }
    return NULL;
}
	
AlcatelCalendar *getCalendarById(AlcatelCalendarList *list, int id) {
    AlcatelCalendarList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id)
            return &(*it);
    }
    return NULL;
}

AlcatelCall *getCallById(AlcatelCallList *list, int id, CallType type) {
    AlcatelCallList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id && (* it).Type == type)
            return &(*it);
    }
    return NULL;
}

void clearContacts(AlcatelContactList *list, AlcatelStorage type) {
    AlcatelContactList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if ((* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearCalendar(AlcatelCalendarList *list, AlcatelStorage type) {
    AlcatelCalendarList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if ((* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearTodos(AlcatelTodoList *list, AlcatelStorage type) {
    AlcatelTodoList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if ((* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearMessages(AlcatelMessageList *list, AlcatelStorage type) {
    AlcatelMessageList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if ((* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearCalls(AlcatelCallList *list, AlcatelStorage type) {
    AlcatelCallList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if ((* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearCategories(AlcatelCategoryList *list, AlcatelStorage type) {
    AlcatelCategoryList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if ((* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}
