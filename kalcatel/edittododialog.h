/*
 * edittododialog.h
 *
 * TODO: description
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

#ifndef EDITTODODIALOG_H
#define EDITTODODIALOG_H

#include <qwidget.h>
#include <kdialog.h>

#include "alcatelclasses.h"

class KComboBox;
class QCheckBox;
class KIntNumInput;
class KDateWidget;
class QLineEdit;
class QPushButton;

/**dialog for editing todo
  *@author Michal Cihar
  */

class EditTodoDialog : public KDialog  {
   Q_OBJECT
public: 
    EditTodoDialog(AlcatelCategoryList *cat, AlcatelTodoList *lst, AlcatelContactList *conts, const AlcatelTodo *cont, QWidget *parent=0, const char *name=0);
    ~EditTodoDialog();
    void loadTodo(const AlcatelTodo *cont = NULL);
    void emptyFields();
private:
    const AlcatelTodo *todo;
    AlcatelTodoList *list;
    AlcatelCategoryList *categories;
    AlcatelContactList *contacts;

    KComboBox *editStorage;
    KIntNumInput *editPosition;

    QCheckBox *editDueDateEnabled;
    KDateWidget *editDueDate;

    QCheckBox *editCompleted;

    QCheckBox *editAlarmEnabled;
    KDateWidget *editAlarmDate;
    KComboBox *editAlarmTime;

    QLineEdit *editSubject;

    KComboBox *editPriority;
    QLineEdit *editContactID;

    KComboBox *editCategory;
    QCheckBox *editPrivate;

    QPushButton *buttonContacts;

    int ContactID;
private slots:
    void toggleDueDate ( bool t );
    void toggleAlarm ( bool t );
    void slotOK();
    void slotCancel();
    void slotStorage(int st);
    void selectContacts();
};

#endif
