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
/*
AlcatelClasses::AlcatelClasses() {
}
AlcatelClasses::~AlcatelClasses() {
}
*/

AlcatelContact::AlcatelContact() {
    Category = -1;
    Private = -1;
}

AlcatelContact::~AlcatelContact() {
}

void AlcatelContact::setField(int number, FIELD *data) {
    switch (number) {
        case 0: chk_type(_string) LastName = (char *)((*data).data); break;
        case 1: chk_type(_string) FirstName = (char *)((*data).data); break;
        case 2: chk_type(_string) Company = (char *)((*data).data); break;
        case 3: chk_type(_string) JobTitle = (char *)((*data).data); break;
        case 4: chk_type(_string) Note = (char *)((*data).data); break;
        case 5: chk_type(_byte) Category = *((int *)((*data).data)); break;
        case 6: chk_type(_bool) Private = *((int *)((*data).data)); break;
        case 7: chk_type(_phone) WorkNumber = (char *)((*data).data); break;
        case 8: chk_type(_phone) MainNumber = (char *)((*data).data); break;
        case 9: chk_type(_phone) FaxNumber = (char *)((*data).data); break;
        case 10: chk_type(_phone) OtherNumber = (char *)((*data).data); break;
        case 11: chk_type(_phone) PagerNumber = (char *)((*data).data); break;
        case 12: chk_type(_phone) MobileNumber = (char *)((*data).data); break;
        case 13: chk_type(_phone) HomeNumber = (char *)((*data).data); break;
        case 14: chk_type(_string) Email1 = (char *)((*data).data); break;
        case 15: chk_type(_string) Email2 = (char *)((*data).data); break;
        case 16: chk_type(_string) Address = (char *)((*data).data); break;
        case 17: chk_type(_string) City = (char *)((*data).data); break;
        case 18: chk_type(_string) State = (char *)((*data).data); break;
        case 19: chk_type(_string) Zip = (char *)((*data).data); break;
        case 20: chk_type(_string) Coutry = (char *)((*data).data); break;
        case 21: chk_type(_string) Custom1 = (char *)((*data).data); break;
        case 22: chk_type(_string) Custom2 = (char *)((*data).data); break;
        case 23: chk_type(_string) Custom3 = (char *)((*data).data); break;
        case 24: chk_type(_string) Custom4 = (char *)((*data).data); break;

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

void AlcatelCalendar::setField(int number, FIELD *data) {
    switch (number) {
        case 0: chk_type(_date) Date.setYMD(((DATE *)((*data).data))->year, ((DATE *)((*data).data))->month, ((DATE *)((*data).data))->day); break;
        case 1: chk_type(_time) StartTime.setHMS (((TIME *)((*data).data))->hour, ((TIME *)((*data).data))->minute, ((TIME *)((*data).data))->second); break;
        case 2: chk_type(_time) EndTime.setHMS (((TIME *)((*data).data))->hour, ((TIME *)((*data).data))->minute, ((TIME *)((*data).data))->second); break;
        case 3: chk_type(_date) Alarm.date().setYMD(((DATE *)((*data).data))->year, ((DATE *)((*data).data))->month, ((DATE *)((*data).data))->day); break;
        case 4: chk_type(_time) Alarm.time().setHMS (((TIME *)((*data).data))->hour, ((TIME *)((*data).data))->minute, ((TIME *)((*data).data))->second); break;
        case 5: chk_type(_string) Subject = (char *)((*data).data); break;
        case 6: chk_type(_bool) Private = *((int *)((*data).data)); break;
        case 7: chk_type(_enum) EventType = *((int *)((*data).data)); break;
        case 8: chk_type(_int) ContactID = *((int *)((*data).data)); break;

        case 10: chk_type(_byte) DayOfWeek = *((int *)((*data).data)); break;
        case 11: chk_type(_byte) Day = *((int *)((*data).data)); break;
        case 12: chk_type(_byte) WeekOfMonth = *((int *)((*data).data)); break;
        case 13: chk_type(_byte) Month = *((int *)((*data).data)); break;

        case 17: chk_type(_byte) Frequency = *((int *)((*data).data)); break;
        case 18: chk_type(_date) StartDate.setYMD(((DATE *)((*data).data))->year, ((DATE *)((*data).data))->month, ((DATE *)((*data).data))->day); break;
        case 19: chk_type(_date) StopDate.setYMD(((DATE *)((*data).data))->year, ((DATE *)((*data).data))->month, ((DATE *)((*data).data))->day); break;
        case 20: chk_type(_date) UnknownDate.setYMD(((DATE *)((*data).data))->year, ((DATE *)((*data).data))->month, ((DATE *)((*data).data))->day); break;
        case 21: chk_type(_time) UnknownTime.setHMS (((TIME *)((*data).data))->hour, ((TIME *)((*data).data))->minute, ((TIME *)((*data).data))->second); break;

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

void AlcatelTodo::setField(int number, FIELD *data) {
    switch (number) {
        case 0: chk_type(_date) DueDate.setYMD(((DATE *)((*data).data))->year, ((DATE *)((*data).data))->month, ((DATE *)((*data).data))->day); break;
        case 1: chk_type(_bool) Completed = *((int *)((*data).data)); break;
        case 2: chk_type(_date) Alarm.date().setYMD(((DATE *)((*data).data))->year, ((DATE *)((*data).data))->month, ((DATE *)((*data).data))->day); break;
        case 3: chk_type(_time) Alarm.time().setHMS (((TIME *)((*data).data))->hour, ((TIME *)((*data).data))->minute, ((TIME *)((*data).data))->second); break;
        case 4: chk_type(_string) Subject = (char *)((*data).data); break;
        case 5: chk_type(_bool) Private = *((int *)((*data).data)); break;
        case 6: chk_type(_enum) Category = *((int *)((*data).data)); break;
        case 7: chk_type(_enum) Priority = *((int *)((*data).data)); break;
        case 8: chk_type(_int) ContactID = *((int *)((*data).data)); break;

        default: message(MSG_WARNING, "Unknown field occured (%02d)!", number); break;
    }
}

AlcatelSMS::AlcatelSMS() {
    Id = -1;
    Status = -1;
    Length = -1;
    Raw = NULL;
}

AlcatelSMS::~AlcatelSMS() {
    if (Raw != NULL) {
        free( Raw );
        Raw = NULL;
    }
}

AlcatelCategory::AlcatelCategory(char* name, int id) {
    Name = QString(name);
    Id = id;
}

AlcatelCategory::AlcatelCategory() {
    Name = QString("");
    Id = -1;
}


QString SMSTypes[] = { i18n("Unread"), i18n("Read"), i18n("Unsent"), i18n("Sent")};

AlcatelSMS *findAlcatelSMSById(AlcatelSMSList *list, int id) {
    AlcatelSMSList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id)
            return &(*it);
    }
    return NULL;
}

QString *getCategoryName(AlcatelCategoryList *list, int id) {
    static QString *str=new QString("");
    AlcatelCategoryList::Iterator it;
    for( it = list->begin(); it != list->end(); ++it ) {
        if ((* it).Id == id)
            return &((*it).Name);
    }
    return str;
}

int phoneCmp(QString *number1, QString *number2, QString *prefix) {
    QString n1 = QString(*number1);
    QString n2 = QString(*number2);
    if (n1[0] != '+')
        n1.prepend(*prefix);
    if (n2[0] != '+')
        n2.prepend(*prefix);
    return n1 == n2;
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
