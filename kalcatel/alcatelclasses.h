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

#include "alcasync/alcatel.h"

/**
  *@author Michal Cihar
  */

/** type of call
  */
enum CallType { CallMissed = 0, CallReceived = 1, CallDialed = 2};
/** type of storage
  */
enum AlcatelStorage {StorageNone = 0, StoragePC = 1, StorageSIM = 2, StorageMobile = 3, StorageAny = 0xff } ;

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
    bool Created;
    bool Deleted;
    virtual const char *getClassName() {static const char cn[]="AlcatelClass";return cn;};
    /** returns formatted printable name of class
      */
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
    /** gets number-th field
      */
    AlcatelFieldStruct *getField(int number);
    /** sets fields that are missing in current object from cmp as deleted
      */
    void diffDeleted(const AlcatelContact &cmp);

    bool operator==(const AlcatelContact &cmp) const;
    bool operator!=(const AlcatelContact &cmp) const;
    /** returns of contacts are same, same contacts can differ in storage,
      * position (both also previous) and modified/created/deleted flag
      */
    bool isSame(const AlcatelContact &cmp) const;
    /** returns formatted printable name of contact
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

    const char *getClassName() {static const char cn[]="AlcatelContact";return cn;}
    static const int max_field = 25;
    bool deleted_flags[max_field];
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
    /** gets number-th field
      */
    AlcatelFieldStruct *getField(int number);
    /** sets fields that are missing in current object from cmp as deleted
      */
    void diffDeleted(const AlcatelCalendar &cmp);

    bool operator==(const AlcatelCalendar &cmp) const;
    bool operator!=(const AlcatelCalendar &cmp) const;
    /** returns of events are same, same events can differ in storage,
      * position (both also previous) and modified/created/deleted flag
      */
    bool isSame(const AlcatelCalendar &cmp) const;
    /** returns string with short information about repeating events
      */
    QString Repeating(void) const;
    /** returns string with detailed information about repeating events
      */
    QString RepeatingDetail(void) const;

    /** returns formatted printable name of event
      */
    QString getName(void);

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

    const char *getClassName() {static const char cn[]="AlcatelCalendar";return cn;}
    static const int max_field = 22;
    bool deleted_flags[max_field];
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
    /** gets number-th field
      */
    AlcatelFieldStruct *getField(int number);
    /** sets fields that are missing in current object from cmp as deleted
      */
    void diffDeleted(const AlcatelTodo &cmp);

    bool operator==(const AlcatelTodo &cmp) const;
    bool operator!=(const AlcatelTodo &cmp) const;
    /** returns of todos are same, same todos can differ in storage,
      * position (both also previous) and modified/created/deleted flag
      */
    bool isSame(const AlcatelTodo &cmp) const;

    /** returns formatted printable name of todo
      */
    QString getName(void);

    QDate DueDate;
    int Completed;
    QDateTime Alarm;
    QString Subject;
    int Private;
    int Category;
    int Priority;
    int ContactID;

    const char *getClassName() {static const char cn[]="AlcatelTodo";return cn;}
    static const int max_field = 9;
    bool deleted_flags[max_field];
};

/** class for storing messages
  */
class AlcatelMessage : public AlcatelClass {
public:
    AlcatelMessage();
    ~AlcatelMessage();

    bool operator==(const AlcatelMessage &cmp) const;
    bool operator!=(const AlcatelMessage &cmp) const;
    /** returns of messages are same, same messages can differ in storage,
      * position (both also previous) and modified/created/deleted flag
      */
    bool isSame(const AlcatelMessage &cmp) const;
    /** returns formatted printable name of message
      */
    QString getName(void);

    int Status;
    int Length;
    QString Raw;
    QString Sender;
    QDateTime Date;
    QString Text;
    QString SMSC;

    const char *getClassName() {static const char cn[]="AlcatelMessage";return cn;}
};

/** class for storing calls
  */
class AlcatelCall : public AlcatelClass {
public:
    AlcatelCall();
    ~AlcatelCall();

    bool operator==(const AlcatelCall &cmp) const;
    bool operator!=(const AlcatelCall &cmp) const;
    /** returns of calls are same, same calls can differ in storage,
      * position (both also previous) and modified/created/deleted flag
      */
    bool isSame(const AlcatelCall &cmp) const;
    /** returns formatted printable name of call
      */
    QString getName(void);

    CallType Type;
    QString Number;
    QString Name;

    const char *getClassName() {static const char cn[]="AlcatelCall";return cn;}
};

/** class for storing categories
  */
class AlcatelCategory : public AlcatelClass {
public:
    /** creates class and sets name and id
      */
    AlcatelCategory(const char *name, int id, AlcatelStorage storage);
    AlcatelCategory();

    bool operator==(const AlcatelCategory &cmp) const;
    bool operator!=(const AlcatelCategory &cmp) const;
    /** returns of categories are same, same categories can differ in storage,
      * position (both also previous) and modified/created/deleted flag
      */
    bool isSame(const AlcatelCategory &cmp) const;
    /** returns formatted printable name of category
      */
    QString getName(void);

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

/** returns category by name
  */
AlcatelCategory *getCategoryByName(AlcatelCategoryList *list, const QString &name, AlcatelStorage type);


void getStatistics(AlcatelMessageList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created);
void getStatistics(AlcatelCalendarList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created);
void getStatistics(AlcatelCategoryList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created);
void getStatistics(AlcatelContactList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created);
void getStatistics(AlcatelTodoList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created);
void getStatistics(AlcatelCallList *list, int &total, int &pc, int &mobile, int &sim, int &deleted, int &modified, int &created);

#endif
