/*
 * edittododialog.cpp
 *
 * TODO: description
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

#include <qpushbutton.h>
#include <qvbox.h>
#include <qframe.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>

#include <kdialog.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knuminput.h>

#include <kdatewidget.h>

#include "alcatelclasses.h"
#include "kalcatel.h"
#include "kalcateldoc.h"

#include "timevalidator.h"

#include "edittododialog.h"

EditTodoDialog::EditTodoDialog(AlcatelCategoryList *cat, AlcatelTodoList *lst, AlcatelContactList *conts, const AlcatelTodo *cont, QWidget *parent, const char *name ) : KDialog(parent,name,true) {
    todo = cont;
    list = lst;
    categories = cat;
    contacts = conts;

    QLabel *label;
    QFrame *line;

    resize(400, 350 );
    if (cont == NULL) setCaption( i18n( "New todo" ) );
    else setCaption( i18n( "Edit todo" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    if (cont == NULL) label = new QLabel( i18n("<b>New todo</b>"), this );
    else label = new QLabel( i18n("<b>Edit todo</b>"), this );
    mainLayout->addMultiCellWidget(label,0,0,0,3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,1,1,0,3);

    mainLayout->setRowStretch(0, -1);
    mainLayout->setRowStretch(1, -1);

    mainLayout->addWidget(new QLabel(i18n("Category"), this), 5, 0);
    mainLayout->addWidget(editCategory = new KComboBox(this), 5, 1);

    editCategory->insertItem(i18n("Not set")); /* -1 */
    editCategory->insertItem(i18n("None")); /* 255 */
    for( AlcatelCategoryList::Iterator c_it = categories->begin(); c_it != categories->end(); ++c_it ) {
        editCategory->insertItem((*c_it).Name);
    }

    mainLayout->addWidget(new QLabel(i18n("Private"), this), 5, 2);
    mainLayout->addWidget(editPrivate = new QCheckBox(this), 5, 3);

    mainLayout->addWidget(new QLabel(i18n("Priority"), this), 6, 0);
    mainLayout->addWidget(editPriority = new KComboBox(this), 6, 1);

    editPriority->insertItem(Priorities[0]);
    editPriority->insertItem(Priorities[1]);
    editPriority->insertItem(Priorities[2]);

    mainLayout->addWidget(new QLabel(i18n("Completed"), this), 6, 2);
    mainLayout->addWidget(editCompleted = new QCheckBox(this), 6, 3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,7,7,0,3);

    mainLayout->addWidget(new QLabel(i18n("Subject"), this), 8, 0);
    mainLayout->addMultiCellWidget(editSubject = new QLineEdit(this), 8, 8, 1, 3);
    editSubject->setFocus ();
    connect( editSubject, SIGNAL( returnPressed () ), this, SLOT( slotOK () ) );

    mainLayout->addWidget(new QLabel(i18n("Due date"), this), 9, 0);
    mainLayout->addMultiCellWidget(editDueDate = new KDateWidget(this), 9, 9, 1, 2);

    mainLayout->addWidget(editDueDateEnabled = new QCheckBox(i18n("Enabled"),this),9,3);
    connect ( editDueDateEnabled, SIGNAL( toggled ( bool ) ), this, SLOT(toggleDueDate ( bool )));
    toggleDueDate(false);

    mainLayout->addWidget(new QLabel(i18n("Contact"), this), 10, 0);
    mainLayout->addWidget(editContactID = new QLineEdit(this), 10, 1);
    editContactID->setDisabled(true);
    editContactID->setText(i18n("None"));

    QPushButton *buttonContacts = new QPushButton(i18n("Contacts..."), this);
    buttonContacts->setDisabled(true);
    mainLayout->addWidget(buttonContacts,10,2);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,11,11,0,3);


    mainLayout->addWidget(new QLabel(i18n("Alarm date"), this), 12, 0);
    mainLayout->addMultiCellWidget(editAlarmDate = new KDateWidget(this), 12, 12, 1, 2);

    mainLayout->addWidget(editAlarmEnabled = new QCheckBox(i18n("Enabled"),this),12,3);
    connect ( editAlarmEnabled, SIGNAL( toggled ( bool ) ), this, SLOT(toggleAlarm ( bool )));

    mainLayout->addWidget(new QLabel(i18n("Alarm time"), this), 13, 0);
    mainLayout->addWidget(editAlarmTime = new KComboBox(true, this), 13, 1);
    editAlarmTime->setValidator(new TimeValidator(editAlarmTime));

    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 4; j++) {
            editAlarmTime->insertItem(QString().sprintf("%d:%02d", i, j * 15));
        }
    }
    toggleAlarm(false);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,14,14,0,3);

    mainLayout->addWidget(new QLabel(i18n("Storage"), this), 15, 0);

    editStorage = new KComboBox(this);
    editStorage->insertItem(i18n("None"));
    editStorage->insertItem(i18n("PC"));
    editStorage->insertItem(i18n("SIM"));
    editStorage->insertItem(i18n("Mobile"));
    connect( editStorage, SIGNAL( activated(int)), this, SLOT( slotStorage(int)));
    editStorage->setCurrentItem(1);
    mainLayout->addWidget(editStorage, 15, 1);

    mainLayout->addWidget(new QLabel(i18n("Position"), this), 15, 2);

    editPosition = new KIntNumInput(this);
    editPosition->setRange(-1, 9999,1,false);
    editPosition->setSpecialValueText(i18n("Auto"));
    mainLayout->addWidget(editPosition, 15, 3);

    QHBoxLayout *layout = new QHBoxLayout;

    layout->setSpacing( 6 );
    layout->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout->addItem( spacer );

    QPushButton *buttonOK = new QPushButton(i18n("&OK"), this);
    buttonOK->setDefault(true);
    layout->addWidget(buttonOK);

    QPushButton *buttonCancel = new QPushButton(i18n("&Cancel"), this);
    layout->addWidget(buttonCancel);

    mainLayout->addMultiCellLayout( layout, 17,17, 0,3 );

    line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addMultiCellWidget( line, 16,16, 0,3 );
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );

    editPosition->setEnabled(false);
    loadTodo();
    if (todo == NULL) editPosition->setValue(-1);
    editStorage->setEnabled(todo == NULL);
}

EditTodoDialog::~EditTodoDialog() {
}

void EditTodoDialog::emptyFields() {
    /* TODO: add action here */
    editDueDate->setDate(QDate::currentDate());
    editAlarmDate->setDate(QDate::currentDate());
    editAlarmEnabled->setChecked(false);
    editCompleted->setChecked(false);
    editCategory->setCurrentItem(0);
    editDueDateEnabled->setChecked(false);
    editStorage->setCurrentItem(StoragePC);
    editPosition->setValue(-1);
    editAlarmTime->setCurrentItem ( 4 * 9 ); /* 9 hours *//* TODO: this could be in config */
    editSubject->setText("");
    editPriority->setCurrentItem(1);
    editContactID->setText(i18n("None"));
    ContactID = -1;
    editPrivate->setChecked(false); /* TODO: add in config */
}

void EditTodoDialog::loadTodo(const AlcatelTodo *cont) {
    if (cont != NULL) todo = cont;

    emptyFields();

    if (todo == NULL) return;
    if (todo->Storage == StorageAny || todo->Storage == StorageNone) return;

    editStorage->setCurrentItem((int)todo->Storage);

    editPosition->setValue(todo->Id);

    editDueDateEnabled->setChecked(! todo->DueDate.isNull());

    if (todo->DueDate.isNull()) editDueDate->setDate(QDate::currentDate());
    else editDueDate->setDate(todo->DueDate);

    editCompleted->setChecked(todo->Completed);

    editAlarmEnabled->setChecked(! (todo->Alarm.date().isNull() || todo->Alarm.time().isNull()));

    if (todo->Alarm.date().isNull()) editAlarmDate->setDate(QDate::currentDate());
    else editAlarmDate->setDate(todo->Alarm.date());

    editAlarmTime->setEditText(todo->Alarm.time().toString());

    editSubject->setText(todo->Subject);

    editPriority->setCurrentItem(todo->Priority);

    if (todo->Category == -1) editCategory->setCurrentItem(0);
    else if (todo->Category == 255) editCategory->setCurrentItem(1);
    else {
        AlcatelCategory *cat = getCategoryById(categories, todo->Category, StorageAny);
        if (cat == NULL) editCategory->setCurrentItem(0);
        else {
            for (int i=0; i<editCategory->count(); i++) {
                if (editCategory->text(i) == cat->Name) {
                    editCategory->setCurrentItem(i);
                    break;
                }
            }
        }
    }

    if (todo->ContactID != -1 && todo->ContactID != 0) {
        AlcatelContact *cont = getContactById(contacts, todo->ContactID, todo->Storage);
        if (cont == NULL) {
            editContactID->setText(i18n("Unknown (id=%1)").arg(todo->ContactID));
        } else {
            editContactID->setText(cont->getName());
        }
    } else {
        editContactID->setText(i18n("None"));
    }
    ContactID = todo->ContactID;

    editPrivate->setChecked(todo->Private);
}

void EditTodoDialog::toggleDueDate ( bool t ) {
    editDueDate->setEnabled(t);
}

void EditTodoDialog::toggleAlarm ( bool t ) {
    editAlarmDate->setEnabled(t);
    editAlarmTime->setEnabled(t);
}

void EditTodoDialog::slotOK() {
    accept();
}

void EditTodoDialog::slotCancel() {
    reject();
}

void EditTodoDialog::slotStorage(int st) {
    if ((AlcatelStorage)st == StorageSIM ||
        (AlcatelStorage)st == StorageAny ||
        (AlcatelStorage)st == StorageNone)
            editStorage->setCurrentItem((int)StoragePC);
}