/*
 * editeventdialog.h
 *
 * Dialog for editing calendar event
 *
 * Copyright (c) 2002-2003 by Michal Cihar <cihar@email.cz>
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

#ifndef EDITEVENTDIALOG_H
#define EDITEVENTDIALOG_H

#include <qwidget.h>
#include <kdialog.h>
#include "alcatelclasses.h"

class KComboBox;
class QCheckBox;
class KIntNumInput;
class KDateWidget;
class QLineEdit;
class QPushButton;

/**Dialog for editing calendar event
  *@author Michal Cihar
  */

class EditEventDialog : public KDialog  {
   Q_OBJECT
public:
    EditEventDialog(AlcatelCalendarList *lst, AlcatelContactList *conts, const AlcatelCalendar *cont, QWidget *parent=0, const char *name=0);
    ~EditEventDialog();
    void loadEvent(const AlcatelCalendar *cont = NULL);
    void emptyFields();
private:
    const AlcatelCalendar *event;
    AlcatelCalendarList *list;
    AlcatelContactList *contacts;

    KComboBox *editStorage;
    KIntNumInput *editPosition;

    QCheckBox *editDateEnabled;
    KDateWidget *editDate;

    QCheckBox *editAlarmEnabled;
    KDateWidget *editAlarmDate;
    KComboBox *editAlarmTime;

    KComboBox *editStartTime;
    KComboBox *editEndTime;

    QLineEdit *editSubject;

    KComboBox *editType;
    QLineEdit *editContactID;

    QCheckBox *editPrivate;

    KIntNumInput *editDayOfWeek;
    KIntNumInput *editDay;
    KIntNumInput *editWeekOfMonth;
    KIntNumInput *editMonth;
    KIntNumInput *editFrequency;

    KDateWidget *editStartDate;
    QCheckBox *editStartEnabled;
    KDateWidget *editEndDate;
    QCheckBox *editEndEnabled;

    QPushButton *buttonContacts;

    int ContactID;
private slots:
    void toggleAlarm ( bool t );
    void toggleDate ( bool t );
    void toggleEnd ( bool t );
    void toggleStart ( bool t );
    void slotOK();
    void slotCancel();
    void slotStorage(int st);
    void slotType(int typ);
    void selectContacts();
};

#endif
