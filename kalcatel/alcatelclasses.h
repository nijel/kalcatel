/*
 * 
 * alcatelclasses.h
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

#ifndef ALCATELCLASSES_H
#define ALCATELCLASSES_H

#include <time.h>

#include <qstring.h>
#include <qvaluelist.h>
#include <qdatetime.h>

#include "alcatool/alcatel.h"

/**
  *@author Michal Cihar
  */

/*
class AlcatelClasses {
public: 
	AlcatelClasses();
	~AlcatelClasses();
};
*/

typedef enum {SIM, Phone} ContactStorage;

class AlcatelContact {
public:
	AlcatelContact();
	~AlcatelContact();
	
	ContactStorage Storage;
	
    QString LastName;
    QString FirstName;
    QString Company;
    QString JobTitle;
    QString Note;
    int Category;
    int Private;
    QString WorkNumber;
    QString MainNumber;
    QString FaxNumber;
    QString OtherNumber;
    QString PagerNumber;
    QString MobileNumber;
    QString HomeNumber;
    QString Email1;
    QString Email2;
    QString Address;
    QString City;
    QString State;
    QString Zip;
    QString Coutry;
    QString Custom1;
    QString Custom2;
    QString Custom3;
    QString Custom4;
};

class AlcatelCalendar {
public:
	AlcatelCalendar();
	~AlcatelCalendar();
	
    QDate Date;
    QTime StartTime;
    QTime EndTime;
    QDateTime Alarm;
    QString Subject;
    int Private;
    int EventType;
    int ContactID;

    /* repeating events: */
    int DayOfWeek;
    int Day;
    int WeekOfMonth;
    int Month;
    int Frequency;
    QDate StartDate;
    QDate StopDate;

    /* Following two were created by IntelliSync, but it couldn't read it back... */
    QDate UnknownDate;        /* this contains date, probably same as AlarmDate */
    QTime UnknownTime;        /* this contains time, probably same as AlarmTime */
};

class AlcatelTodo {
public:
	AlcatelTodo();
	~AlcatelTodo();
	
    QDate DueDate;
    int Completed;
    QDateTime Alarm;
    QString Subject;
    int Private;
    int Category;
    int Priority;
    int ContactID;
};

class AlcatelSMS{
public:
	AlcatelSMS();
	~AlcatelSMS();
	
    int Position;
    int Status;
    int Length;
    char *Raw;
    QString Sender;
    time_t Date;
    QString Text;
    QString SMSC;
};

typedef QValueList<AlcatelContact> AlcatelContactList;
typedef QValueList<AlcatelCalendar> AlcatelCalendarList;
typedef QValueList<AlcatelTodo> AlcatelTodoList;
typedef QValueList<AlcatelSMS> AlcatelSMSList;
#endif
