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

void *getAlcatelField(const QDate &data, AlcatelFieldType type) {
    if (type == _date) {
        AlcatelDateStruct *date = (AlcatelDateStruct *)malloc(sizeof(AlcatelDateStruct));
        date->day = data.day();
        date->month = data.month();
        date->year = data.year();
        return (void *)date;
    } else {
        message(MSG_WARNING, "Bad type conversion occured (QDate -> %02d)!", type);
        return NULL;
    }
}

void *getAlcatelField(const QTime &data, AlcatelFieldType type) {
    if (type == _date) {
        AlcatelTimeStruct *time = (AlcatelTimeStruct *)malloc(sizeof(AlcatelTimeStruct));
        time->hour = data.hour();
        time->minute = data.minute();
        time->second = data.second();
        return (void *)time;
    } else {
        message(MSG_WARNING, "Bad type conversion occured (QTime -> %02d)!", type);
        return NULL;
    }
}

void *getAlcatelField(const QDateTime &data, AlcatelFieldType type) {
    if (type == _date) {
        AlcatelDateStruct *date = (AlcatelDateStruct *)malloc(sizeof(AlcatelDateStruct));
        date->day = data.date().day();
        date->month = data.date().month();
        date->year = data.date().year();
        return (void *)date;
    } else if (type == _time) {
        AlcatelTimeStruct *time = (AlcatelTimeStruct *)malloc(sizeof(AlcatelTimeStruct));
        time->hour = data.time().hour();
        time->minute = data.time().minute();
        time->second = data.time().second();
        return (void *)time;
    } else {
        message(MSG_WARNING, "Bad type conversion occured (QDateTime -> %02d)!", type);
        return NULL;
    }
}

void *getAlcatelField(const QString &data, AlcatelFieldType type) {
    if (type == _string || type == _phone) {
        return (void *)strdup(data.latin1());
    } else {
        message(MSG_WARNING, "Bad type conversion occured (QString -> %02d)!", type);
        return NULL;
    }
}

void *getAlcatelField(const int &data, AlcatelFieldType type) {
    if (type == _enum || type == _bool || type == _int || type == _byte) {
        int *i = (int *)malloc(sizeof(int));
        *i = data;
        return (void *)i;
    } else {
        message(MSG_WARNING, "Bad type conversion occured (int -> %02d)!", type);
        return NULL;
    }
}


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
    for (int i = 0; i < max_field; i++) deleted_flags[i] = false;
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

AlcatelFieldStruct *AlcatelContact::getField(int number) {
    AlcatelFieldStruct *field = (AlcatelFieldStruct *)malloc(sizeof(AlcatelFieldStruct));
    switch (number) {
        case 0: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (LastName.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(LastName, _string); break;
        case 1: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (FirstName.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(FirstName, _string); break;
        case 2: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Company.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Company, _string); break;
        case 3: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (JobTitle.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(JobTitle, _string); break;
        case 4: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Note.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Note, _string); break;
        case 5: field->type = _byte; if (deleted_flags[number]) field->data = NULL; else if (Category == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Category, _byte); break;
        case 6: field->type = _bool; if (deleted_flags[number]) field->data = NULL; else if (Private == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Private, _bool); break;
        case 7: field->type = _phone; if (deleted_flags[number]) field->data = NULL; else if (WorkNumber.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(WorkNumber, _phone); break;
        case 8: field->type = _phone; if (deleted_flags[number]) field->data = NULL; else if (MainNumber.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(MainNumber, _phone); break;
        case 9: field->type = _phone; if (deleted_flags[number]) field->data = NULL; else if (FaxNumber.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(FaxNumber, _phone); break;
        case 10: field->type = _phone; if (deleted_flags[number]) field->data = NULL; else if (OtherNumber.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(OtherNumber, _phone); break;
        case 11: field->type = _phone; if (deleted_flags[number]) field->data = NULL; else if (PagerNumber.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(PagerNumber, _phone); break;
        case 12: field->type = _phone; if (deleted_flags[number]) field->data = NULL; else if (MobileNumber.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(MobileNumber, _phone); break;
        case 13: field->type = _phone; if (deleted_flags[number]) field->data = NULL; else if (HomeNumber.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(HomeNumber, _phone); break;
        case 14: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Email1.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Email1, _string); break;
        case 15: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Email2.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Email2, _string); break;
        case 16: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Address.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Address, _string); break;
        case 17: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (City.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(City, _string); break;
        case 18: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (State.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(State, _string); break;
        case 19: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Zip.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Zip, _string); break;
        case 20: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Country.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Country, _string); break;
        case 21: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Custom1.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Custom1, _string); break;
        case 22: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Custom2.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Custom2, _string); break;
        case 23: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Custom3.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Custom3, _string); break;
        case 24: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Custom4.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Custom4, _string); break;

        default: message(MSG_WARNING, "Unknown field occured (%02d)!", number); free (field); field = NULL; break;
    }
    return field;
}

void AlcatelContact::diffDeleted(const AlcatelContact &cmp) {
    deleted_flags[0] = (this->LastName.isEmpty()) && (!cmp.LastName.isEmpty());
    deleted_flags[1] = (this->FirstName.isEmpty()) && (!cmp.FirstName.isEmpty());
    deleted_flags[2] = (this->Company.isEmpty()) && (!cmp.Company.isEmpty());
    deleted_flags[3] = (this->JobTitle.isEmpty()) && (!cmp.JobTitle.isEmpty());
    deleted_flags[4] = (this->Note.isEmpty()) && (!cmp.Note.isEmpty());
    deleted_flags[5] = (this->Category == -1) && (cmp.Category != -1);
    deleted_flags[6] = (this->Private == -1) && (cmp.Private != -1);
    deleted_flags[7] = (this->WorkNumber.isEmpty()) && (!cmp.WorkNumber.isEmpty());
    deleted_flags[8] = (this->MainNumber.isEmpty()) && (!cmp.MainNumber.isEmpty());
    deleted_flags[9] = (this->FaxNumber.isEmpty()) && (!cmp.FaxNumber.isEmpty());
    deleted_flags[10] = (this->OtherNumber.isEmpty()) && (!cmp.OtherNumber.isEmpty());
    deleted_flags[11] = (this->PagerNumber.isEmpty()) && (!cmp.PagerNumber.isEmpty());
    deleted_flags[12] = (this->MobileNumber.isEmpty()) && (!cmp.MobileNumber.isEmpty());
    deleted_flags[13] = (this->HomeNumber.isEmpty()) && (!cmp.HomeNumber.isEmpty());
    deleted_flags[14] = (this->Email1.isEmpty()) && (!cmp.Email2.isEmpty());
    deleted_flags[15] = (this->Email2.isEmpty()) && (!cmp.Email2.isEmpty());
    deleted_flags[16] = (this->Address.isEmpty()) && (!cmp.Address.isEmpty());
    deleted_flags[17] = (this->City.isEmpty()) && (!cmp.City.isEmpty());
    deleted_flags[18] = (this->State.isEmpty()) && (!cmp.State.isEmpty());
    deleted_flags[19] = (this->Zip.isEmpty()) && (!cmp.Zip.isEmpty());
    deleted_flags[20] = (this->Country.isEmpty()) && (!cmp.Country.isEmpty());
    deleted_flags[21] = (this->Custom1.isEmpty()) && (!cmp.Custom1.isEmpty());
    deleted_flags[22] = (this->Custom2.isEmpty()) && (!cmp.Custom2.isEmpty());
    deleted_flags[23] = (this->Custom3.isEmpty()) && (!cmp.Custom3.isEmpty());
    deleted_flags[24] = (this->Custom4.isEmpty()) && (!cmp.Custom4.isEmpty());
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

    for (int i = 0; i < max_field; i++) deleted_flags[i] = false;
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

AlcatelFieldStruct *AlcatelCalendar::getField(int number) {
    AlcatelFieldStruct *field = (AlcatelFieldStruct *)malloc(sizeof(AlcatelFieldStruct));
    switch (number) {
        case 0: field->type = _date; if (deleted_flags[number]) field->data = NULL; else if (Date.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(Date, _date); break;
        case 1: field->type = _time; if (deleted_flags[number]) field->data = NULL; else if (StartTime.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(StartTime, _time); break;
        case 2: field->type = _time; if (deleted_flags[number]) field->data = NULL; else if (EndTime.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(EndTime, _time); break;
        case 3: field->type = _date; if (deleted_flags[number]) field->data = NULL; else if (Alarm.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(Alarm, _date); break;
        case 4: field->type = _time; if (deleted_flags[number]) field->data = NULL; else if (Alarm.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(Alarm, _time); break;
        case 5: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Subject.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Subject, _string); break;
        case 6: field->type = _bool; if (deleted_flags[number]) field->data = NULL; else if (Private == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Private, _bool); break;
        case 7: field->type = _enum; if (deleted_flags[number]) field->data = NULL; else if (EventType == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(EventType, _enum); break;
        case 8: field->type = _int; if (deleted_flags[number]) field->data = NULL; else if (ContactID == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(ContactID, _int); break;

        case 10: field->type = _byte; if (deleted_flags[number]) field->data = NULL; else if (DayOfWeek == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(DayOfWeek, _byte); break;
        case 11: field->type = _byte; if (deleted_flags[number]) field->data = NULL; else if (Day == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Day, _byte); break;
        case 12: field->type = _byte; if (deleted_flags[number]) field->data = NULL; else if (WeekOfMonth == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(WeekOfMonth, _byte); break;
        case 13: field->type = _byte; if (deleted_flags[number]) field->data = NULL; else if (Month == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Month, _byte); break;

        case 17: field->type = _byte; if (deleted_flags[number]) field->data = NULL; else if (Frequency == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Frequency, _byte); break;
        case 18: field->type = _date; if (deleted_flags[number]) field->data = NULL; else if (StartDate.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(StartDate, _date); break;
        case 19: field->type = _date; if (deleted_flags[number]) field->data = NULL; else if (StopDate.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(StopDate, _date); break;
        case 20: field->type = _date; if (deleted_flags[number]) field->data = NULL; else if (Alarm2.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(Alarm2, _date); break;
        case 21: field->type = _time; if (deleted_flags[number]) field->data = NULL; else if (Alarm2.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(Alarm2, _time); break;

        case 9:
        case 14:
        case 15:
        case 16: free (field); field = NULL; break; /* silently ignore this, because we loop over all fields */

        default: message(MSG_WARNING, "Unknown field occured (%02d)!", number); free (field); field = NULL; break;
    }
    return field;
}

void AlcatelCalendar::diffDeleted(const AlcatelCalendar &cmp) {
    deleted_flags[0] = (this->Date.isNull()) && (!cmp.Date.isNull());
    deleted_flags[1] = (this->StartTime.isNull()) && (!cmp.StartTime.isNull());
    deleted_flags[2] = (this->EndTime.isNull()) && (!cmp.EndTime.isNull());
    deleted_flags[3] = (this->Alarm.isNull()) && (!cmp.Alarm.isNull());
    deleted_flags[4] = (this->Alarm.isNull()) && (!cmp.Alarm.isNull());
    deleted_flags[5] = (this->Subject.isEmpty()) && (!cmp.Subject.isEmpty());
    deleted_flags[6] = (this->Private == -1) && (cmp.Private != -1);
    deleted_flags[7] = (this->EventType == -1) && (cmp.EventType != -1);
    deleted_flags[8] = (this->ContactID == -1) && (cmp.ContactID != -1);

    deleted_flags[10] = (this->DayOfWeek == -1) && (cmp.DayOfWeek != -1);
    deleted_flags[11] = (this->Day == -1) && (cmp.Day != -1);
    deleted_flags[12] = (this->WeekOfMonth == -1) && (cmp.WeekOfMonth != -1);
    deleted_flags[13] = (this->Month == -1) && (cmp.Month != -1);

    deleted_flags[17] = (this->Frequency == -1) && (cmp.Frequency != -1);
    deleted_flags[18] = (this->StartDate.isNull()) && (!cmp.StartDate.isNull());
    deleted_flags[19] = (this->StopDate.isNull()) && (!cmp.StopDate.isNull());
    deleted_flags[20] = (this->Alarm2.isNull()) && (!cmp.Alarm2.isNull());
    deleted_flags[21] = (this->Alarm2.isNull()) && (!cmp.Alarm2.isNull());
}

AlcatelTodo::AlcatelTodo() {
    Completed = -1;
    Private = -1;
    Category = -1;
    Priority = -1;
    ContactID = -1;

    for (int i = 0; i < max_field; i++) deleted_flags[i] = false;
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

AlcatelFieldStruct *AlcatelTodo::getField(int number) {
    AlcatelFieldStruct *field = (AlcatelFieldStruct *)malloc(sizeof(AlcatelFieldStruct));
    switch (number) {
        case 0: field->type = _date; if (deleted_flags[number]) field->data = NULL; else if (DueDate.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(DueDate, field->type); break;
        case 1: field->type = _bool; if (deleted_flags[number]) field->data = NULL; else if (Completed == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Completed, field->type);  break;
        case 2: field->type = _date; if (deleted_flags[number]) field->data = NULL; else if (Alarm.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(Alarm, field->type);  break;
        case 3: field->type = _time; if (deleted_flags[number]) field->data = NULL; else if (Alarm.isNull()) {free (field); field = NULL;} else  field->data = getAlcatelField(Alarm, field->type); break;
        case 4: field->type = _string; if (deleted_flags[number]) field->data = NULL; else if (Subject.isEmpty()) {free (field); field = NULL;} else  field->data = getAlcatelField(Subject, field->type); break;
        case 5: field->type = _bool; if (deleted_flags[number]) field->data = NULL; else if (Private == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Private, field->type); break;
        case 6: field->type = _byte; if (deleted_flags[number]) field->data = NULL; else if (Category == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Category, field->type); break;
        case 7: field->type = _enum; if (deleted_flags[number]) field->data = NULL; else if (Priority == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(Priority, field->type); break;
        case 8: field->type = _int; if (deleted_flags[number]) field->data = NULL; else if (ContactID == -1) {free (field); field = NULL;} else  field->data = getAlcatelField(ContactID, field->type); break;

        default: message(MSG_WARNING, "Unknown field occured (%02d)!", number); free (field); field = NULL; break;
    }
    return field;
}

void AlcatelTodo::diffDeleted(const AlcatelTodo &cmp) {
    deleted_flags[0] = (this->DueDate.isNull()) && (!cmp.DueDate.isNull());
    deleted_flags[1] = (this->Completed == -1) && (cmp.Completed != -1);
    deleted_flags[2] = (this->Alarm.isNull()) && (!cmp.Alarm.isNull());
    deleted_flags[3] = (this->Alarm.isNull()) && (!cmp.Alarm.isNull());
    deleted_flags[4] = (this->Subject.isEmpty()) && (!cmp.Subject.isEmpty());
    deleted_flags[5] = (this->Private == -1) && (cmp.Private != -1);
    deleted_flags[6] = (this->Category == -1) && (cmp.Category != -1);
    deleted_flags[7] = (this->Priority == -1) && (cmp.Priority != -1);
    deleted_flags[8] = (this->ContactID == -1) && (cmp.ContactID == -1);
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

void getStatistics(AlcatelMessageList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created) {
    AlcatelMessageList::Iterator it;
    total = 0;
    pc = 0;
    mobile = 0;
    sim = 0;
    deleted = 0;
    modified = 0;
    created = 0;
    for( it = list->begin(); it != list->end(); ++it ) {
        total++;
        if ((*it).Modified) modified++;
        if ((*it).Created) created++;
        if ((*it).Deleted) deleted++;
        switch ((*it).Storage) {
            case(StoragePC):
                pc++;
                break;
            case(StorageSIM):
                sim++;
                break;
            case(StorageMobile):
                mobile++;
                break;
            case(StorageNone):
            case(StorageAny):
                break;
        }
    }
}

void getStatistics(AlcatelCalendarList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created) {
    AlcatelCalendarList::Iterator it;
    total = 0;
    pc = 0;
    mobile = 0;
    sim = 0;
    deleted = 0;
    modified = 0;
    created = 0;
    for( it = list->begin(); it != list->end(); ++it ) {
        total++;
        if ((*it).Modified) modified++;
        if ((*it).Created) created++;
        if ((*it).Deleted) deleted++;
        switch ((*it).Storage) {
            case(StoragePC):
                pc++;
                break;
            case(StorageSIM):
                sim++;
                break;
            case(StorageMobile):
                mobile++;
                break;
            case(StorageNone):
            case(StorageAny):
                break;
        }
    }
}

void getStatistics(AlcatelCategoryList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created) {
    AlcatelCategoryList::Iterator it;
    total = 0;
    pc = 0;
    mobile = 0;
    sim = 0;
    deleted = 0;
    modified = 0;
    created = 0;
    for( it = list->begin(); it != list->end(); ++it ) {
        total++;
        if ((*it).Modified) modified++;
        if ((*it).Created) created++;
        if ((*it).Deleted) deleted++;
        switch ((*it).Storage) {
            case(StoragePC):
                pc++;
                break;
            case(StorageSIM):
                sim++;
                break;
            case(StorageMobile):
                mobile++;
                break;
            case(StorageNone):
            case(StorageAny):
                break;
        }
    }
}

void getStatistics(AlcatelContactList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created) {
    AlcatelContactList::Iterator it;
    total = 0;
    pc = 0;
    mobile = 0;
    sim = 0;
    deleted = 0;
    modified = 0;
    created = 0;
    for( it = list->begin(); it != list->end(); ++it ) {
        total++;
        if ((*it).Modified) modified++;
        if ((*it).Created) created++;
        if ((*it).Deleted) deleted++;
        switch ((*it).Storage) {
            case(StoragePC):
                pc++;
                break;
            case(StorageSIM):
                sim++;
                break;
            case(StorageMobile):
                mobile++;
                break;
            case(StorageNone):
            case(StorageAny):
                break;
        }
    }
}

void getStatistics(AlcatelTodoList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created) {
    AlcatelTodoList::Iterator it;
    total = 0;
    pc = 0;
    mobile = 0;
    sim = 0;
    deleted = 0;
    modified = 0;
    created = 0;
    for( it = list->begin(); it != list->end(); ++it ) {
        total++;
        if ((*it).Modified) modified++;
        if ((*it).Created) created++;
        if ((*it).Deleted) deleted++;
        switch ((*it).Storage) {
            case(StoragePC):
                pc++;
                break;
            case(StorageSIM):
                sim++;
                break;
            case(StorageMobile):
                mobile++;
                break;
            case(StorageNone):
            case(StorageAny):
                break;
        }
    }
}

void getStatistics(AlcatelCallList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created) {
    AlcatelCallList::Iterator it;
    total = 0;
    pc = 0;
    mobile = 0;
    sim = 0;
    deleted = 0;
    modified = 0;
    created = 0;
    for( it = list->begin(); it != list->end(); ++it ) {
        total++;
        if ((*it).Modified) modified++;
        if ((*it).Created) created++;
        if ((*it).Deleted) deleted++;
        switch ((*it).Storage) {
            case(StoragePC):
                pc++;
                break;
            case(StorageSIM):
                sim++;
                break;
            case(StorageMobile):
                mobile++;
                break;
            case(StorageNone):
            case(StorageAny):
                break;
        }
    }
}

