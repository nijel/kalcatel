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
 * if and only if programmer/distributor of that code receives written
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
QString StorageTypes[]= { i18n("None"), i18n("PC"), i18n("SIM"), i18n("Mobile") };
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
    Deleted = false;
    Created = false;
}

AlcatelContact::AlcatelContact() {
    Category = -1;
    Private = -1;
}

AlcatelContact::~AlcatelContact() {
}

QString AlcatelContact::getName(void) {
    return (FirstName.isEmpty())?
                            ((LastName.isEmpty())?
                                QString(""):
                                QString(LastName)):
                            ((LastName.isEmpty())?
                                QString(FirstName):
                                QString("%1, %2").arg(LastName).arg(FirstName)); /* TODO: configure how will be name shown ... */
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

QString AlcatelCalendar::Repeating(void) const {
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

QString AlcatelCalendar::RepeatingDetail(void) const {
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
QString AlcatelCalendar::getName(void) {
    return QString("%1 %2-%3 (%4)").arg(Date.toString()).arg(StartTime.toString()).arg(EndTime.toString()).arg(Subject);
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

QString AlcatelTodo::getName(void) {
    return Subject;
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
}

QString AlcatelMessage::getName(void) {
    return Text;
}

AlcatelMessage::~AlcatelMessage() {
}

AlcatelCall::AlcatelCall() {
}

AlcatelCall::~AlcatelCall() {
}

QString AlcatelCall::getName(void) {
    return Name;
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

QString AlcatelCategory::getName(void) {
    return Name;
}

AlcatelMessage *getMessageById(AlcatelMessageList *list, int id, AlcatelStorage type) {
    AlcatelMessageList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id && (type == StorageAny || (* it).Storage == type))
            return &(*it);
    }
    return NULL;
}

AlcatelCategory *getCategoryById(AlcatelCategoryList *list, int id, AlcatelStorage type) {
    AlcatelCategoryList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id && (type == StorageAny || (* it).Storage == type))
            return &(*it);
    }
    return NULL;
}

int phoneCmp(QString *number1, QString *number2, QString *prefix) {
    QString n1(*number1);
    QString n2(*number2);
    if (n1[0] != '+')
        n1.prepend(*prefix);
    if (n2[0] != '+')
        n2.prepend(*prefix);
    if (!(n1 == n2)) {
        n1 = *number1;
        n2 = *number2;
        if (n1[0] != '+')
            n1.prepend("+");
        if (n2[0] != '+')
            n2.prepend("+");
        return (n1 == n2);
    } else {
        return true;
    }
}

AlcatelContact *getContactByPhone(AlcatelContactList *list, QString *number, QString *prefix) {
    AlcatelContactList::Iterator it;
    if (number->isEmpty()) return NULL;
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
        if ((* it).Id == id && (type == StorageAny || (* it).Storage == type))
            return &(*it);
    }
    return NULL;
}

AlcatelTodo *getTodoById(AlcatelTodoList *list, int id, AlcatelStorage type) {
    AlcatelTodoList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id && (type == StorageAny || (* it).Storage == type))
            return &(*it);
    }
    return NULL;
}
	
AlcatelCalendar *getCalendarById(AlcatelCalendarList *list, int id, AlcatelStorage type) {
    AlcatelCalendarList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id && (type == StorageAny || (* it).Storage == type))
            return &(*it);
    }
    return NULL;
}

AlcatelCall *getCallById(AlcatelCallList *list, int id, AlcatelStorage stype, CallType type) {
    AlcatelCallList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id && (* it).Type == type && (stype == StorageAny || stype == (*it).Storage))
            return &(*it);
    }
    return NULL;
}

void clearContacts(AlcatelContactList *list, AlcatelStorage type) {
    AlcatelContactList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if (type == StorageAny || (* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearCalendar(AlcatelCalendarList *list, AlcatelStorage type) {
    AlcatelCalendarList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if (type == StorageAny || (* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearTodos(AlcatelTodoList *list, AlcatelStorage type) {
    AlcatelTodoList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if (type == StorageAny || (* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearMessages(AlcatelMessageList *list, AlcatelStorage type) {
    AlcatelMessageList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if (type == StorageAny || (* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearCalls(AlcatelCallList *list, AlcatelStorage type) {
    AlcatelCallList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if (type == StorageAny || (* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

void clearCategories(AlcatelCategoryList *list, AlcatelStorage type) {
    AlcatelCategoryList::Iterator it;
    it = list->begin();
    while (it != list->end()) {
        if (type == StorageAny || (* it).Storage == type )
            it = list->remove(it);
        else
            it++;
    }
}

AlcatelMessage *getMessageByPrevId(AlcatelMessageList *list, int id, AlcatelStorage type) {
    AlcatelMessageList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).PrevId == id && (type == StorageAny || (* it).PrevStorage == type))
            return &(*it);
    }
    return NULL;
}

AlcatelCategory *getCategoryByPrevId(AlcatelCategoryList *list, int id, AlcatelStorage type) {
    AlcatelCategoryList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).PrevId == id && (type == StorageAny || (* it).PrevStorage == type))
            return &(*it);
    }
    return NULL;
}

AlcatelContact *getContactByPrevId(AlcatelContactList *list, int id, AlcatelStorage type) {
    AlcatelContactList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).PrevId == id && (type == StorageAny || (* it).PrevStorage == type))
            return &(*it);
    }
    return NULL;
}

AlcatelTodo *getTodoByPrevId(AlcatelTodoList *list, int id, AlcatelStorage type) {
    AlcatelTodoList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).PrevId == id && (type == StorageAny || (* it).PrevStorage == type))
            return &(*it);
    }
    return NULL;
}
	
AlcatelCalendar *getCalendarByPrevId(AlcatelCalendarList *list, int id, AlcatelStorage type) {
    AlcatelCalendarList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).PrevId == id && (type == StorageAny || (* it).PrevStorage == type))
            return &(*it);
    }
    return NULL;
}

AlcatelCall *getCallByPrevId(AlcatelCallList *list, int id, AlcatelStorage stype, CallType type) {
    AlcatelCallList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).PrevId == id &&  (* it).Type == type && (stype == StorageAny || (* it).PrevStorage == stype))
            return &(*it);
    }
    return NULL;
}

/* TODO: add isNull to all date/time comparsion */
bool AlcatelCalendar::operator==(const AlcatelCalendar &cmp) const {
    return
        (Deleted == cmp.Deleted) &&
        (Created == cmp.Created) &&
        (Modified == cmp.Modified) &&
        (Id == cmp.Id) &&
        (Storage == cmp.Storage) &&
        (PrevId == cmp.PrevId) &&
        (PrevStorage == cmp.PrevStorage) &&
        (Date == cmp.Date) &&
        (StartTime == cmp.StartTime) &&
        (EndTime == cmp.EndTime) &&
        (Alarm == cmp.Alarm) &&
        ((Subject.isEmpty() && cmp.Subject.isEmpty()) || (Subject == cmp.Subject)) &&
        (Private == cmp.Private) &&
        (EventType == cmp.EventType) &&
        (ContactID == cmp.ContactID) &&
        (DayOfWeek == cmp.DayOfWeek) &&
        (Day == cmp.Day) &&
        (WeekOfMonth == cmp.WeekOfMonth) &&
        (Month == cmp.Month) &&
        (Frequency == cmp.Frequency) &&
        (StartDate == cmp.StartDate) &&
        (StopDate == cmp.StopDate) &&
        (Alarm2 == cmp.Alarm2);
}
bool AlcatelCalendar::operator!=(const AlcatelCalendar &cmp) const {
    return !(*this == cmp);
}

bool AlcatelTodo::operator==(const AlcatelTodo &cmp) const {
    return
        (Deleted == cmp.Deleted) &&
        (Created == cmp.Created) &&
        (Modified == cmp.Modified) &&
        (Id == cmp.Id) &&
        (Storage == cmp.Storage) &&
        (PrevId == cmp.PrevId) &&
        (PrevStorage == cmp.PrevStorage) &&
        (DueDate == cmp.DueDate) &&
        (Completed == cmp.Completed) &&
        (Alarm == cmp.Alarm) &&
        ((Subject.isEmpty() && cmp.Subject.isEmpty()) || (Subject == cmp.Subject)) &&
        (Private == cmp.Private) &&
        (Category == cmp.Category) &&
        (Priority == cmp.Priority) &&
        (ContactID == cmp.ContactID);
}
bool AlcatelTodo::operator!=(const AlcatelTodo &cmp) const {
    return !(*this == cmp);
}

bool AlcatelContact::operator==(const AlcatelContact &cmp) const {
    return
        (Deleted == cmp.Deleted) &&
        (Created == cmp.Created) &&
        (Modified == cmp.Modified) &&
        (Id == cmp.Id) &&
        (Storage == cmp.Storage) &&
        (PrevId == cmp.PrevId) &&
        (PrevStorage == cmp.PrevStorage) &&
        ((LastName.isEmpty() && cmp.LastName.isEmpty()) || (LastName == cmp.LastName)) &&
        ((FirstName.isEmpty() && cmp.FirstName.isEmpty()) || (FirstName == cmp.FirstName)) &&
        ((Company.isEmpty() && cmp.Company.isEmpty()) || (Company == cmp.Company)) &&
        ((JobTitle.isEmpty() && cmp.JobTitle.isEmpty()) || (JobTitle == cmp.JobTitle)) &&
        ((Note.isEmpty() && cmp.Note.isEmpty()) || (Note == cmp.Note)) &&
        (Category == cmp.Category) &&
        (Private == cmp.Private) &&
        ((WorkNumber.isEmpty() && cmp.WorkNumber.isEmpty()) || (WorkNumber == cmp.WorkNumber)) &&
        ((MainNumber.isEmpty() && cmp.MainNumber.isEmpty()) || (MainNumber == cmp.MainNumber)) &&
        ((FaxNumber.isEmpty() && cmp.FaxNumber.isEmpty()) || (FaxNumber == cmp.FaxNumber)) &&
        ((OtherNumber.isEmpty() && cmp.OtherNumber.isEmpty()) || (OtherNumber == cmp.OtherNumber)) &&
        ((PagerNumber.isEmpty() && cmp.PagerNumber.isEmpty()) || (PagerNumber == cmp.PagerNumber)) &&
        ((MobileNumber.isEmpty() && cmp.MobileNumber.isEmpty()) || (MobileNumber == cmp.MobileNumber)) &&
        ((HomeNumber.isEmpty() && cmp.HomeNumber.isEmpty()) || (HomeNumber == cmp.HomeNumber)) &&
        ((Email1.isEmpty() && cmp.Email1.isEmpty()) || (Email1 == cmp.Email1)) &&
        ((Email2.isEmpty() && cmp.Email2.isEmpty()) || (Email2 == cmp.Email2)) &&
        ((Address.isEmpty() && cmp.Address.isEmpty()) || (Address == cmp.Address)) &&
        ((City.isEmpty() && cmp.City.isEmpty()) || (City == cmp.City)) &&
        ((State.isEmpty() && cmp.State.isEmpty()) || (State == cmp.State)) &&
        ((Zip.isEmpty() && cmp.Zip.isEmpty()) || (Zip == cmp.Zip)) &&
        ((Country.isEmpty() && cmp.Country.isEmpty()) || (Country == cmp.Country)) &&
        ((Custom1.isEmpty() && cmp.Custom1.isEmpty()) || (Custom1 == cmp.Custom1)) &&
        ((Custom2.isEmpty() && cmp.Custom2.isEmpty()) || (Custom2 == cmp.Custom2)) &&
        ((Custom3.isEmpty() && cmp.Custom3.isEmpty()) || (Custom3 == cmp.Custom3)) &&
        ((Custom4.isEmpty() && cmp.Custom4.isEmpty()) || (Custom4 == cmp.Custom4));
}
bool AlcatelContact::operator!=(const AlcatelContact &cmp) const {
    return !(*this == cmp);
}

bool AlcatelCategory::operator==(const AlcatelCategory &cmp) const {
    return
        (Deleted == cmp.Deleted) &&
        (Created == cmp.Created) &&
        (Modified == cmp.Modified) &&
        (Id == cmp.Id) &&
        (Storage == cmp.Storage) &&
        (PrevId == cmp.PrevId) &&
        (PrevStorage == cmp.PrevStorage) &&
        ((Name.isEmpty() && cmp.Name.isEmpty()) || (Name == cmp.Name));
}
bool AlcatelCategory::operator!=(const AlcatelCategory &cmp) const {
    return !(*this == cmp);
}

bool AlcatelMessage::operator==(const AlcatelMessage &cmp) const {
    return
        (Deleted == cmp.Deleted) &&
        (Created == cmp.Created) &&
        (Modified == cmp.Modified) &&
        (Id == cmp.Id) &&
        (Storage == cmp.Storage) &&
        (PrevId == cmp.PrevId) &&
        (PrevStorage == cmp.PrevStorage) &&
        (Status == cmp.Status) &&
        (Length == cmp.Length) &&
        (Date == cmp.Date) &&
        ((Raw.isEmpty() && cmp.Raw.isEmpty()) || (Raw == cmp.Raw)) &&
        ((Sender.isEmpty() && cmp.Sender.isEmpty()) || (Sender == cmp.Sender)) &&
        ((Text.isEmpty() && cmp.Text.isEmpty()) || (Text == cmp.Text)) &&
        ((SMSC.isEmpty() && cmp.SMSC.isEmpty()) || (SMSC == cmp.SMSC));
}
bool AlcatelMessage::operator!=(const AlcatelMessage &cmp) const {
    return !(*this == cmp);
}

bool AlcatelCall::operator==(const AlcatelCall &cmp) const {
    return
        (Deleted == cmp.Deleted) &&
        (Created == cmp.Created) &&
        (Modified == cmp.Modified) &&
        (Id == cmp.Id) &&
        (Storage == cmp.Storage) &&
        (PrevId == cmp.PrevId) &&
        (PrevStorage == cmp.PrevStorage) &&
        (Type == cmp.Type) &&
        (Number == cmp.Number) &&
        ((Name.isEmpty() && cmp.Name.isEmpty()) || (Name == cmp.Name));
}
bool AlcatelCall::operator!=(const AlcatelCall &cmp) const {
    return !(*this == cmp);
}

bool AlcatelCalendar::isSame(const AlcatelCalendar &cmp) const {
    return
        (Date == cmp.Date) &&
        (StartTime == cmp.StartTime) &&
        (EndTime == cmp.EndTime) &&
        (Alarm == cmp.Alarm) &&
        ((Subject.isEmpty() && cmp.Subject.isEmpty()) || (Subject == cmp.Subject)) &&
        (Private == cmp.Private) &&
        (EventType == cmp.EventType) &&
        (ContactID == cmp.ContactID) &&
        (DayOfWeek == cmp.DayOfWeek) &&
        (Day == cmp.Day) &&
        (WeekOfMonth == cmp.WeekOfMonth) &&
        (Month == cmp.Month) &&
        (Frequency == cmp.Frequency) &&
        (StartDate == cmp.StartDate) &&
        (StopDate == cmp.StopDate) &&
        (Alarm2 == cmp.Alarm2);
}

bool AlcatelTodo::isSame(const AlcatelTodo &cmp) const {
    return
        (DueDate == cmp.DueDate) &&
        (Completed == cmp.Completed) &&
        (Alarm == cmp.Alarm) &&
        ((Subject.isEmpty() && cmp.Subject.isEmpty()) || (Subject == cmp.Subject)) &&
        (Private == cmp.Private) &&
        (Category == cmp.Category) &&
        (Priority == cmp.Priority) &&
        (ContactID == cmp.ContactID);
}

bool AlcatelContact::isSame(const AlcatelContact &cmp) const {
    return
        ((LastName.isEmpty() && cmp.LastName.isEmpty()) || (LastName == cmp.LastName)) &&
        ((FirstName.isEmpty() && cmp.FirstName.isEmpty()) || (FirstName == cmp.FirstName)) &&
        ((Company.isEmpty() && cmp.Company.isEmpty()) || (Company == cmp.Company)) &&
        ((JobTitle.isEmpty() && cmp.JobTitle.isEmpty()) || (JobTitle == cmp.JobTitle)) &&
        ((Note.isEmpty() && cmp.Note.isEmpty()) || (Note == cmp.Note)) &&
        (Category == cmp.Category) &&
        (Private == cmp.Private) &&
        ((WorkNumber.isEmpty() && cmp.WorkNumber.isEmpty()) || (WorkNumber == cmp.WorkNumber)) &&
        ((MainNumber.isEmpty() && cmp.MainNumber.isEmpty()) || (MainNumber == cmp.MainNumber)) &&
        ((FaxNumber.isEmpty() && cmp.FaxNumber.isEmpty()) || (FaxNumber == cmp.FaxNumber)) &&
        ((OtherNumber.isEmpty() && cmp.OtherNumber.isEmpty()) || (OtherNumber == cmp.OtherNumber)) &&
        ((PagerNumber.isEmpty() && cmp.PagerNumber.isEmpty()) || (PagerNumber == cmp.PagerNumber)) &&
        ((MobileNumber.isEmpty() && cmp.MobileNumber.isEmpty()) || (MobileNumber == cmp.MobileNumber)) &&
        ((HomeNumber.isEmpty() && cmp.HomeNumber.isEmpty()) || (HomeNumber == cmp.HomeNumber)) &&
        ((Email1.isEmpty() && cmp.Email1.isEmpty()) || (Email1 == cmp.Email1)) &&
        ((Email2.isEmpty() && cmp.Email2.isEmpty()) || (Email2 == cmp.Email2)) &&
        ((Address.isEmpty() && cmp.Address.isEmpty()) || (Address == cmp.Address)) &&
        ((City.isEmpty() && cmp.City.isEmpty()) || (City == cmp.City)) &&
        ((State.isEmpty() && cmp.State.isEmpty()) || (State == cmp.State)) &&
        ((Zip.isEmpty() && cmp.Zip.isEmpty()) || (Zip == cmp.Zip)) &&
        ((Country.isEmpty() && cmp.Country.isEmpty()) || (Country == cmp.Country)) &&
        ((Custom1.isEmpty() && cmp.Custom1.isEmpty()) || (Custom1 == cmp.Custom1)) &&
        ((Custom2.isEmpty() && cmp.Custom2.isEmpty()) || (Custom2 == cmp.Custom2)) &&
        ((Custom3.isEmpty() && cmp.Custom3.isEmpty()) || (Custom3 == cmp.Custom3)) &&
        ((Custom4.isEmpty() && cmp.Custom4.isEmpty()) || (Custom4 == cmp.Custom4));
}

bool AlcatelCategory::isSame(const AlcatelCategory &cmp) const {
    return ((Name.isEmpty() && cmp.Name.isEmpty()) || (Name == cmp.Name));
}

bool AlcatelMessage::isSame(const AlcatelMessage &cmp) const {
    return
        (Status == cmp.Status) &&
        (Length == cmp.Length) &&
        (Date == cmp.Date) &&
        ((Raw.isEmpty() && cmp.Raw.isEmpty()) || (Raw == cmp.Raw)) &&
        ((Sender.isEmpty() && cmp.Sender.isEmpty()) || (Sender == cmp.Sender)) &&
        ((Text.isEmpty() && cmp.Text.isEmpty()) || (Text == cmp.Text)) &&
        ((SMSC.isEmpty() && cmp.SMSC.isEmpty()) || (SMSC == cmp.SMSC));
}

bool AlcatelCall::isSame(const AlcatelCall &cmp) const {
    return
        (Type == cmp.Type) &&
        (Number == cmp.Number) &&
        ((Name.isEmpty() && cmp.Name.isEmpty()) || (Name == cmp.Name));
}

AlcatelCategory *getCategoryByName(AlcatelCategoryList *list, const QString &name, AlcatelStorage type) {
    AlcatelCategoryList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Name == name && (type == StorageAny || (* it).Storage == type))
            return &(*it);
    }
    return NULL;
}
