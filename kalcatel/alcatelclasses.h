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
 * if and only if programmer/distributor of that code receives written
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
enum AlcatelStorage {StorageNone, StoragePC, StorageSIM, StorageMobile, StorageAny } ;

/** Generic class used only as base for other storage classes
  */
class AlcatelClass {
public:
    AlcatelClass();
    virtual ~AlcatelClass() {}
    /** Position of record
      */
    int Id;
    /** Storage of record
      */
	AlcatelStorage Storage;
    /** Previous position of record (used only for StoragePC)
      */
    int PrevId;
    /** Previous storage of record (used only for StoragePC)
      */
	AlcatelStorage PrevStorage;
    /** Whether record has been modified, used for saving into mobile.
      * Takes affect only fo StorageMobile and StorageSIM.
      */
    bool Modified;
    bool Deleted;
    virtual const char *getClassName() = 0;
    virtual QString getName(void) = 0;
};

extern QString StorageTypes[];
extern QString MessageTypes[];
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
	void setField(int number, AlcatelFieldStruct *data);
	
    bool operator==(const AlcatelContact &cmp);
    bool isSame(const AlcatelContact &cmp);
    /** returns formatted name of contact
      */
	QString getName(void);
	
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

    const char *getClassName() {static char cn[]="AlcatelContact";return cn;}
};

/** class for storing calendar
  */
class AlcatelCalendar : public AlcatelClass {
public:
	AlcatelCalendar();
	~AlcatelCalendar();
	
    /** sets field read from mobile in this class
      */
	void setField(int number, AlcatelFieldStruct *data);
	
    bool operator==(const AlcatelCalendar &cmp);
    bool isSame(const AlcatelCalendar &cmp);
    /** returns string with short information about repeating events
      */
	QString Repeating(void);
    /** returns string with detailed information about repeating events
      */
    QString RepeatingDetail(void);
	
    QString getName(void) {return QString("%1 %2-%3 (%4)").arg(Date.toString()).arg(StartTime.toString()).arg(EndTime.toString()).arg(Subject);}

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

    const char *getClassName() {static char cn[]="AlcatelCalendar";return cn;}
};

/** class for storing todos
  */
class AlcatelTodo : public AlcatelClass {
public:
	AlcatelTodo();
	~AlcatelTodo();
	
    /** sets field read from mobile in this class
      */
	void setField(int number, AlcatelFieldStruct *data);
	
    bool operator==(const AlcatelTodo &cmp);
    bool isSame(const AlcatelTodo &cmp);

    QString getName(void) {return Subject;}

    QDate DueDate;
    int Completed;
    QDateTime Alarm;
    QString Subject;
    int Private;
    int Category;
    int Priority;
    int ContactID;

    const char *getClassName() {static char cn[]="AlcatelTodo";return cn;}
};

/** class for storing messages
  */
class AlcatelMessage : public AlcatelClass {
public:
	AlcatelMessage();
	~AlcatelMessage();
	
    bool operator==(const AlcatelMessage &cmp);
    bool isSame(const AlcatelMessage &cmp);
    QString getName(void) {return Text;}

    int Status;
    int Length;
    QString Raw;
    QString Sender;
    QDateTime Date;
    QString Text;
    QString SMSC;

    const char *getClassName() {static char cn[]="AlcatelMessage";return cn;}
};

/** class for storing calls
  */
class AlcatelCall : public AlcatelClass {
public:
	AlcatelCall();
	~AlcatelCall();
	
    bool operator==(const AlcatelCall &cmp);
    bool isSame(const AlcatelCall &cmp);
    QString getName(void) {return Name;}

    CallType Type;
    QString Number;
    QString Name;

    const char *getClassName() {static char cn[]="AlcatelCall";return cn;}
};

/** class for storing categories
  */
class AlcatelCategory : public AlcatelClass {
public:
    /** creates class and sets name and id
      */
	AlcatelCategory(char* name, int id, AlcatelStorage storage);
	AlcatelCategory();
	
    bool operator==(const AlcatelCategory &cmp);
    bool isSame(const AlcatelCategory &cmp);
    QString getName(void) {return Name;}

    QString Name;

    const char *getClassName() {static char cn[]="AlcatelCategory";return cn;}
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
typedef QValueList<AlcatelMessage> AlcatelMessageList;
/** list of calls
  */
typedef QValueList<AlcatelCall> AlcatelCallList;
/** list of categories
  */
typedef QValueList<AlcatelCategory> AlcatelCategoryList;

/** find message by id
  */
AlcatelMessage *getMessageById(AlcatelMessageList *list, int id, AlcatelStorage type);
/** returns category name
  */
AlcatelCategory *getCategoryById(AlcatelCategoryList *list, int id, AlcatelStorage type);
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
AlcatelTodo *getTodoById(AlcatelTodoList *list, int id, AlcatelStorage type);
/** find calendar item by id
  */
AlcatelCalendar *getCalendarById(AlcatelCalendarList *list, int id, AlcatelStorage type);

/** find call by id
  */
AlcatelCall *getCallById(AlcatelCallList *list, int id, AlcatelStorage stype, CallType type);

/** clears contacts of selected type
  */
void clearContacts(AlcatelContactList *list, AlcatelStorage type);
/** clears calendar events of selected type
  */
void clearCalendar(AlcatelCalendarList *list, AlcatelStorage type);
/** clears todos of selected type
  */
void clearTodos(AlcatelTodoList *list, AlcatelStorage type);
/** clears messages of selected type
  */
void clearMessages(AlcatelMessageList *list, AlcatelStorage type);
/** clears calls of selected type
  */
void clearCalls(AlcatelCallList *list, AlcatelStorage type);
/** clears categories of selected type
  */
void clearCategories(AlcatelCategoryList *list, AlcatelStorage type);

/** find message by previous id
  */
AlcatelMessage *getMessageByPrevId(AlcatelMessageList *list, int id, AlcatelStorage type);
/** returns category name
  */
AlcatelCategory *getCategoryByPrevId(AlcatelCategoryList *list, int id, AlcatelStorage type);
/** find contact by previous id
  */
AlcatelContact *getContactByPrevId(AlcatelContactList *list, int id, AlcatelStorage type);
/** find todo by previous id
  */
AlcatelTodo *getTodoByPrevId(AlcatelTodoList *list, int id, AlcatelStorage type);
/** find calendar item by previous id
  */
AlcatelCalendar *getCalendarByPrevId(AlcatelCalendarList *list, int id, AlcatelStorage type);
/** find call by previous id
  */
AlcatelCall *getCallByPrevId(AlcatelCallList *list, int id, AlcatelStorage stype, CallType type);
#endif
