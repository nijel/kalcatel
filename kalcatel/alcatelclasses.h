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

/** type of call
  */
enum CallType { CallMissed, CallReceived, CallDialled };
/** type of storage
  */
enum AlcatelStorage {StoragePC, StorageSIM, StorageMobile } ;

/** Generic class used only as base for other storage classes
  */
class AlcatelClass {
public:
    /** Position of record
      */
    int Id;
    /** Storage of record
      */
	AlcatelStorage Storage;
    /** Call type (used only for calls)
      */
    CallType Type;
};

extern QString StorageTypes[];
extern QString SMSTypes[];
extern QString CallTypes[];
extern QString Priorities[];
extern QString CalendarTypes[];

/** class for storing contacts
  */
class AlcatelContact : public AlcatelClass {
public:
	AlcatelContact();
	~AlcatelContact();
	
    /** sets field read from mobile in this class
      */
	void setField(int number, struct AlcatelFieldStruct *data);
	
    /** returns formatted name of contact
      */
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

/** class for storing calendar
  */
class AlcatelCalendar : public AlcatelClass {
public:
	AlcatelCalendar();
	~AlcatelCalendar();
	
    /** sets field read from mobile in this class
      */
	void setField(int number, struct AlcatelFieldStruct *data);
	
    /** returns string with short information about repeating events
      */
	QString Repeating(void);
    /** returns string with detailed information about repeating events
      */
    QString RepeatingDetail(void);
	
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

    /* used when EventType is alarm */
    QDateTime Alarm2;
};

/** class for storing todos
  */
class AlcatelTodo : public AlcatelClass {
public:
	AlcatelTodo();
	~AlcatelTodo();
	
    /** sets field read from mobile in this class
      */
	void setField(int number, struct AlcatelFieldStruct *data);
	
    QDate DueDate;
    int Completed;
    QDateTime Alarm;
    QString Subject;
    int Private;
    int Category;
    int Priority;
    int ContactID;
};

/** class for storing messages
  */
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

/** class for storing calls
  */
class AlcatelCall : public AlcatelClass {
public:
	AlcatelCall();
	~AlcatelCall();
	
    QString Number;
    QString Name;
};

/** class for storing categories
  */
class AlcatelCategory : public AlcatelClass {
public:
    /** creates class and sets name and id
      */
	AlcatelCategory(char* name, int id);
	AlcatelCategory();
	
    QString Name;
};

/** list of contacts
  */
typedef QValueList<AlcatelContact> AlcatelContactList;
/** list of calendar items
  */
typedef QValueList<AlcatelCalendar> AlcatelCalendarList;
/** list of todos
  */
typedef QValueList<AlcatelTodo> AlcatelTodoList;
/** list of messages
  */
typedef QValueList<AlcatelSMS> AlcatelSMSList;
/** list of calls
  */
typedef QValueList<AlcatelCall> AlcatelCallList;
/** list of categories
  */
typedef QValueList<AlcatelCategory> AlcatelCategoryList;

/** find message by id
  */
AlcatelSMS *getSMSById(AlcatelSMSList *list, int id);
/** returns category name
  */
QString *getCategoryName(AlcatelCategoryList *list, int id);
/** compares two phone numbers, prefix should be international prefix
  */
int phoneCmp(QString *number1, QString *number2, QString *prefix);
/** find contact by phone
  */
AlcatelContact *getContactByPhone(AlcatelContactList *list, QString *number, QString *prefix);
/** find contact by id
  */
AlcatelContact *getContactById(AlcatelContactList *list, int id, AlcatelStorage type);
/** find todo by id
  */
AlcatelTodo *getTodoById(AlcatelTodoList *list, int id);
/** find calendar item by id
  */
AlcatelCalendar *getCalendarById(AlcatelCalendarList *list, int id);

/** find call by id
  */
AlcatelCall *getCallById(AlcatelCallList *list, int id, CallType type);

/** clears contacts of selected type
  */
void clearContacts(AlcatelContactList *list, AlcatelStorage type);

#endif
