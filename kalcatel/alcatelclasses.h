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

#include <qstring.h>
#include <qvaluelist.h>
#include <qdatetime.h>

#include "alcatool/alcatel.h"

/**
  *@author Michal Cihar
  */

class AlcatelClass {
public:
    int Id;
};

extern QString SMSTypes[];
extern QString Priorities[];


class AlcatelContact : public AlcatelClass {
public:
	AlcatelContact();
	~AlcatelContact();
	
	void setField(int number, FIELD *data);
	
    enum ContactStorage {SIM, Mobile} ;

	enum ContactStorage Storage;
	
	QString Name(void);
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
    QString Country;
    QString Custom1;
    QString Custom2;
    QString Custom3;
    QString Custom4;
};

class AlcatelCalendar : public AlcatelClass {
public:
	AlcatelCalendar();
	~AlcatelCalendar();
	
	void setField(int number, FIELD *data);
	
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

class AlcatelTodo : public AlcatelClass {
public:
	AlcatelTodo();
	~AlcatelTodo();
	
	void setField(int number, FIELD *data);
	
    QDate DueDate;
    int Completed;
    QDateTime Alarm;
    QString Subject;
    int Private;
    int Category;
    int Priority;
    int ContactID;
};

class AlcatelSMS : public AlcatelClass {
public:
	AlcatelSMS();
	~AlcatelSMS();
	
    int Status;
    int Length;
    char *Raw;
    QString Sender;
    QDateTime Date;
    QString Text;
    QString SMSC;
};

class AlcatelCategory : public AlcatelClass {
public:
	AlcatelCategory(char* name, int id);
	AlcatelCategory();
    QString Name;
};

typedef QValueList<AlcatelContact> AlcatelContactList;
typedef QValueList<AlcatelCalendar> AlcatelCalendarList;
typedef QValueList<AlcatelTodo> AlcatelTodoList;
typedef QValueList<AlcatelSMS> AlcatelSMSList;
typedef QValueList<AlcatelCategory> AlcatelCategoryList;

AlcatelSMS *getSMSById(AlcatelSMSList *list, int id);
QString *getCategoryName(AlcatelCategoryList *list, int id);
int phoneCmp(QString *number1, QString *number2, QString *prefix);
AlcatelContact *getContactByPhone(AlcatelContactList *list, QString *number, QString *prefix);
AlcatelContact *getContactById(AlcatelContactList *list, int id);
AlcatelTodo *getTodoById(AlcatelTodoList *list, int id);

#endif
