/*
 * editeventdialog.cpp
 *
 * Dialog for editing calendar event
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

#include "timevalidator.h"
#include "selectcontactdialog.h"

#include "editeventdialog.h"
#include "alcatelclasses.h"
#include "kalcatel.h"
#include "kalcateldoc.h"

EditEventDialog::EditEventDialog(AlcatelCalendarList *lst, AlcatelContactList *conts, const AlcatelCalendar *cont, QWidget *parent, const char *name) : KDialog(parent,name,true) {
    event = cont;
    list = lst;
    contacts = conts;

    QLabel *label;
    QFrame *line;

    resize(400, 530);
    if (cont == NULL) setCaption( i18n( "New event" ) );
    else setCaption( i18n( "Edit event" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    if (cont == NULL) label = new QLabel( i18n("<b>New event</b>"), this );
    else label = new QLabel( i18n("<b>Edit event</b>"), this );
    mainLayout->addMultiCellWidget(label,0,0,0,3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,1,1,0,3);

    mainLayout->setRowStretch(0, -1);
    mainLayout->setRowStretch(1, -1);

    mainLayout->addWidget(new QLabel(i18n("Type"), this), 5, 0);
    mainLayout->addWidget(editType = new KComboBox(this), 5, 1);

    for( int i = 0; i <= 9; i++ ) {
        editType->insertItem(CalendarTypes[i]);
    }
    editType->setEnabled(cont == NULL);
    connect( editType, SIGNAL( activated(int)), this, SLOT( slotType(int)));

    mainLayout->addWidget(new QLabel(i18n("Private"), this), 5, 2);
    mainLayout->addWidget(editPrivate = new QCheckBox(this), 5, 3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,7,7,0,3);

    mainLayout->addWidget(new QLabel(i18n("Subject"), this), 8, 0);
    mainLayout->addMultiCellWidget(editSubject = new QLineEdit(this), 8, 8, 1, 3);
    editSubject->setFocus ();
    connect( editSubject, SIGNAL( returnPressed () ), this, SLOT( slotOK () ) );

    mainLayout->addWidget(new QLabel(i18n("Date"), this), 9, 0);
    mainLayout->addMultiCellWidget(editDate = new KDateWidget(this), 9, 9, 1, 2);

    mainLayout->addWidget(editDateEnabled = new QCheckBox(i18n("Enabled"),this),9,3);
    connect ( editDateEnabled, SIGNAL( toggled ( bool ) ), this, SLOT(toggleDate ( bool )));
    toggleDate(false);

    mainLayout->addWidget(new QLabel(i18n("Contact"), this), 10, 0);
    mainLayout->addWidget(editContactID = new QLineEdit(this), 10, 1);
    editContactID->setDisabled(true);
    editContactID->setText(i18n("None"));

    buttonContacts = new QPushButton(i18n("Contacts..."), this);
    connect ( buttonContacts, SIGNAL( clicked() ), this, SLOT(selectContacts()));
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

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,14,14,0,3);

    mainLayout->addWidget(new QLabel(i18n("Start time"), this), 15, 0);
    mainLayout->addWidget(editStartTime = new KComboBox(true, this), 15, 1);
    editStartTime->setValidator(new TimeValidator(editStartTime));

    mainLayout->addWidget(new QLabel(i18n("End time"), this), 15, 2);
    mainLayout->addWidget(editEndTime = new KComboBox(true, this), 15, 3);
    editEndTime->setValidator(new TimeValidator(editEndTime));

    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 4; j++) {
            QString str = QString().sprintf("%d:%02d", i, j * 15);
            editAlarmTime->insertItem(str);
            editEndTime->insertItem(str);
            editStartTime->insertItem(str);
        }
    }
    toggleAlarm(false);


    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,16,16,0,3);


    mainLayout->addWidget(new QLabel(i18n("<b>Repeating:</b>"), this), 17, 0);

    mainLayout->addWidget(new QLabel(i18n("Frequency"), this), 17, 2);
    editFrequency = new KIntNumInput(this);
    editFrequency->setRange(-1, 255,1,false);
    editFrequency->setSpecialValueText(i18n("Not set"));
    mainLayout->addWidget(editFrequency, 17, 3);

    mainLayout->addWidget(new QLabel(i18n("Day of week"), this), 18, 0);
    editDayOfWeek = new KIntNumInput(this);
    editDayOfWeek->setRange(-1,6,1,false);
    editDayOfWeek->setSpecialValueText(i18n("Not set"));
    mainLayout->addWidget(editDayOfWeek, 18, 1);

    mainLayout->addWidget(new QLabel(i18n("Day"), this), 18, 2);
    editDay = new KIntNumInput(this);
    editDay->setRange(-1, 31,1,false);
    editDay->setSpecialValueText(i18n("Not set"));
    mainLayout->addWidget(editDay, 18, 3);

    mainLayout->addWidget(new QLabel(i18n("Week of month"), this), 19, 0);
    editWeekOfMonth = new KIntNumInput(this);
    editWeekOfMonth->setRange(-1, 4,1,false);
    editWeekOfMonth->setSpecialValueText(i18n("Not set"));
    mainLayout->addWidget(editWeekOfMonth, 19, 1);

    mainLayout->addWidget(new QLabel(i18n("Month"), this), 19, 2);
    editMonth = new KIntNumInput(this);
    editMonth->setRange(-1, 12,1,false);
    editMonth->setSpecialValueText(i18n("Not set"));
    mainLayout->addWidget(editMonth, 19, 3);

    mainLayout->addWidget(new QLabel(i18n("Start date"), this), 20, 0);
    mainLayout->addMultiCellWidget(editStartDate = new KDateWidget(this), 20, 20, 1, 2);
    editStartDate->setEnabled(false);

    mainLayout->addWidget(editStartEnabled = new QCheckBox(i18n("Enabled"),this),20,3);
    connect ( editStartEnabled, SIGNAL( toggled ( bool ) ), this, SLOT(toggleStart ( bool )));

    mainLayout->addWidget(new QLabel(i18n("End date"), this), 21, 0);
    mainLayout->addMultiCellWidget(editEndDate = new KDateWidget(this), 21, 21, 1, 2);
    editEndDate->setEnabled(false);

    mainLayout->addWidget(editEndEnabled = new QCheckBox(i18n("Enabled"),this),21,3);
    connect ( editEndEnabled, SIGNAL( toggled ( bool ) ), this, SLOT(toggleStart ( bool )));


    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,22,22,0,3);

    mainLayout->addWidget(new QLabel(i18n("Storage"), this), 23, 0);

    editStorage = new KComboBox(this);
    editStorage->insertItem(i18n("None"));
    editStorage->insertItem(i18n("PC"));
    editStorage->insertItem(i18n("SIM"));
    editStorage->insertItem(i18n("Mobile"));
    connect( editStorage, SIGNAL( activated(int)), this, SLOT( slotStorage(int)));
    editStorage->setCurrentItem(1);
    mainLayout->addWidget(editStorage, 23, 1);

    mainLayout->addWidget(new QLabel(i18n("Position"), this), 23, 2);

    editPosition = new KIntNumInput(this);
    editPosition->setRange(-1, 9999,1,false);
    editPosition->setSpecialValueText(i18n("Auto"));
    mainLayout->addWidget(editPosition, 23, 3);

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

    mainLayout->addMultiCellLayout( layout, 25,25, 0,3 );

    line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addMultiCellWidget( line, 24,24, 0,3 );
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );

    editPosition->setEnabled(false);
    loadEvent();
    if (cont == NULL) editPosition->setValue(-1);
    editStorage->setEnabled(cont == NULL);
}

EditEventDialog::~EditEventDialog() {
}

void EditEventDialog::loadEvent(const AlcatelCalendar *cont) {
    if (cont != NULL) event = cont;

    emptyFields();

    if (event == NULL) return;
    if (event->Storage == StorageAny || event->Storage == StorageNone || event->Storage == StorageSIM) return;

    editStorage->setCurrentItem((int)event->Storage);

    editPosition->setValue(event->Id);

    if (!event->Date.isNull()) {editDate->setDate(event->Date); editDateEnabled->setChecked(true); }
    else editDateEnabled->setChecked(false);

    if (event->Alarm.isNull()) {
        if (!event->Date.isNull()) editAlarmDate->setDate(event->Date);
        else editAlarmDate->setDate(QDate::currentDate());
        editAlarmEnabled->setChecked(false);
    } else {
        editAlarmDate->setDate(event->Alarm.date());
        editAlarmTime->setEditText(event->Alarm.time().toString());
        editAlarmEnabled->setChecked(true);
    }

    editSubject->setText(event->Subject);

    if (event->ContactID != -1 && event->ContactID != 0) {
        AlcatelContact *cont = getContactById(contacts, event->ContactID, event->Storage);
        if (cont == NULL) {
            editContactID->setText(i18n("Unknown (id=%1)").arg(event->ContactID));
        } else {
            editContactID->setText(cont->getName());
        }
    } else {
        editContactID->setText(i18n("None"));
    }
    ContactID = event->ContactID;

    editPrivate->setChecked(event->Private);

    editType->setCurrentItem(event->EventType);

    if (!event->StartTime.isNull()) editStartTime->setEditText(event->StartTime.toString());
    if (!event->EndTime.isNull()) editEndTime->setEditText(event->EndTime.toString());

    editFrequency->setValue(event->Frequency);
    editMonth->setValue(event->Month);
    editWeekOfMonth->setValue(event->WeekOfMonth);
    editDay->setValue(event->Day);
    editDayOfWeek->setValue(event->DayOfWeek);

    if (!event->StartDate.isNull()) {editStartDate->setDate(event->StartDate); editStartEnabled->setChecked(true); }
    else editStartEnabled->setChecked(false);
    if (!event->StopDate.isNull()) {editEndDate->setDate(event->StopDate); editEndEnabled->setChecked(true); }
    else editEndEnabled->setChecked(false);

    if (event->EventType == ALC_CALENDAR_ALARM || event->EventType == ALC_CALENDAR_DAILY_ALARM) {
        if (event->Alarm2.isNull() && event->Alarm.isNull()) {
            if (!event->Date.isNull()) editAlarmDate->setDate(event->Date);
            else editAlarmDate->setDate(QDate::currentDate());
            editAlarmEnabled->setChecked(false);
        } else if (!event->Alarm2.isNull()) {
            editAlarmDate->setDate(event->Alarm2.date());
            editAlarmTime->setEditText(event->Alarm2.time().toString());
            editAlarmEnabled->setChecked(true);
        }
    }

    slotType(editType->currentItem());
}

void EditEventDialog::emptyFields() {
    editFrequency->setValue(-1);
    editMonth->setValue(-1);
    editWeekOfMonth->setValue(-1);
    editDay->setValue(-1);
    editDayOfWeek->setValue(-1);

    editStartEnabled->setChecked(false);
    editStartDate->setDate(QDate::currentDate());
    editEndEnabled->setChecked(false);
    editEndDate->setDate(QDate::currentDate());
    editDateEnabled->setChecked(false);
    editDate->setDate(QDate::currentDate());
    editAlarmDate->setDate(QDate::currentDate());
    editAlarmEnabled->setChecked(false);

    editStorage->setCurrentItem(StoragePC);
    editPosition->setValue(-1);
    editAlarmTime->setCurrentItem ( 4 * 9 ); /* 9 hours *//* TODO: this could be in config */

    editStartTime->setCurrentItem ( 4 * 9 ); /* 9 hours *//* TODO: this could be in config */
    editEndTime->setCurrentItem ( 4 * 10 ); /* 10 hours *//* TODO: this could be in config */

    editSubject->setText("");
    editContactID->setText(i18n("None"));
    ContactID = -1;
    editPrivate->setChecked(false); /* TODO: add in config */

    slotType(editType->currentItem());
}

void EditEventDialog::slotOK() {
    AlcatelCalendar cont;
    cont.Storage =  (AlcatelStorage)editStorage->currentItem();

    cont.Id = editPosition->value();

    cont.EventType = editType->currentItem();

    cont.Subject = editSubject->text();

    cont.Private = editPrivate->isChecked();

    QStringList slist;
    switch (cont.EventType) {
        case ALC_CALENDAR_CALL:
            cont.ContactID = ContactID;
            // otherwise it has same items as appointment
        case ALC_CALENDAR_APPOINTMENT:
            slist = QStringList::split(QRegExp("[:.]"), editStartTime->currentText(), false);
            switch (slist.count()) {
                case 3:
                    cont.StartTime.setHMS(slist[0].toInt(), slist[1].toInt(), slist[2].toInt());
                    break;
                case 2:
                    cont.StartTime.setHMS(slist[0].toInt(), slist[1].toInt(), 0);
                    break;
                case 1:
                    cont.StartTime.setHMS(slist[0].toInt(), 0, 0);
                    break;
                case 0:
                default:
                    cont.Alarm.setTime(QTime(0, 0, 0));
            }
            slist = QStringList::split(QRegExp("[:.]"), editEndTime->currentText(), false);
            switch (slist.count()) {
                case 3:
                    cont.EndTime.setHMS(slist[0].toInt(), slist[1].toInt(), slist[2].toInt());
                    break;
                case 2:
                    cont.EndTime.setHMS(slist[0].toInt(), slist[1].toInt(), 0);
                    break;
                case 1:
                    cont.EndTime.setHMS(slist[0].toInt(), 0, 0);
                    break;
                case 0:
                default:
                    cont.Alarm.setTime(QTime(0, 0, 0));
            }
        case ALC_CALENDAR_BIRTHDAY:
            if (editDateEnabled->isChecked()) cont.Date = editDate->date();
            if (editAlarmEnabled->isChecked()) {
                cont.Alarm.setDate(editAlarmDate->date());
                slist = QStringList::split(QRegExp("[:.]"), editAlarmTime->currentText(), false);
                switch (slist.count()) {
                    case 3:
                        cont.Alarm.setTime(QTime(slist[0].toInt(), slist[1].toInt(), slist[2].toInt()));
                        break;
                    case 2:
                        cont.Alarm.setTime(QTime(slist[0].toInt(), slist[1].toInt(), 0));
                        break;
                    case 1:
                        cont.Alarm.setTime(QTime(slist[0].toInt(), 0, 0));
                        break;
                    case 0:
                    default:
                        cont.Alarm.setTime(QTime(0, 0, 0));
                }
            } else {
                cont.Alarm.setDate(QDate());
                cont.Alarm.setTime(QTime());
            }
            break;
        case ALC_CALENDAR_ALARM:
        case ALC_CALENDAR_DAILY_ALARM:
            if (editAlarmEnabled->isChecked()) {
                cont.Alarm2.setDate(editAlarmDate->date());
                slist = QStringList::split(QRegExp("[:.]"), editAlarmTime->currentText(), false);
                switch (slist.count()) {
                    case 3:
                        cont.Alarm2.setTime(QTime(slist[0].toInt(), slist[1].toInt(), slist[2].toInt()));
                        break;
                    case 2:
                        cont.Alarm2.setTime(QTime(slist[0].toInt(), slist[1].toInt(), 0));
                        break;
                    case 1:
                        cont.Alarm2.setTime(QTime(slist[0].toInt(), 0, 0));
                        break;
                    case 0:
                    default:
                        cont.Alarm2.setTime(QTime(0, 0, 0));
                }
            } else {
                cont.Alarm2.setDate(QDate());
                cont.Alarm2.setTime(QTime());
            }
            cont.Alarm = cont.Alarm2;
            break;
        case ALC_CALENDAR_REPEATING:
            slist = QStringList::split(QRegExp("[:.]"), editStartTime->currentText(), false);
            switch (slist.count()) {
                case 3:
                    cont.StartTime.setHMS(slist[0].toInt(), slist[1].toInt(), slist[2].toInt());
                    break;
                case 2:
                    cont.StartTime.setHMS(slist[0].toInt(), slist[1].toInt(), 0);
                    break;
                case 1:
                    cont.StartTime.setHMS(slist[0].toInt(), 0, 0);
                    break;
                case 0:
                default:
                    cont.Alarm.setTime(QTime(0, 0, 0));
            }
            slist = QStringList::split(QRegExp("[:.]"), editEndTime->currentText(), false);
            switch (slist.count()) {
                case 3:
                    cont.EndTime.setHMS(slist[0].toInt(), slist[1].toInt(), slist[2].toInt());
                    break;
                case 2:
                    cont.EndTime.setHMS(slist[0].toInt(), slist[1].toInt(), 0);
                    break;
                case 1:
                    cont.EndTime.setHMS(slist[0].toInt(), 0, 0);
                    break;
                case 0:
                default:
                    cont.Alarm.setTime(QTime(0, 0, 0));
            }
            if (editDateEnabled->isChecked()) cont.Date = editDate->date();
            if (editAlarmEnabled->isChecked()) {
                cont.Alarm.setDate(editAlarmDate->date());
                slist = QStringList::split(QRegExp("[:.]"), editAlarmTime->currentText(), false);
                switch (slist.count()) {
                    case 3:
                        cont.Alarm.setTime(QTime(slist[0].toInt(), slist[1].toInt(), slist[2].toInt()));
                        break;
                    case 2:
                        cont.Alarm.setTime(QTime(slist[0].toInt(), slist[1].toInt(), 0));
                        break;
                    case 1:
                        cont.Alarm.setTime(QTime(slist[0].toInt(), 0, 0));
                        break;
                    case 0:
                    default:
                        cont.Alarm.setTime(QTime(0, 0, 0));
                }
            } else {
                cont.Alarm.setDate(QDate());
                cont.Alarm.setTime(QTime());
            }

            cont.DayOfWeek = editDayOfWeek->value();
            cont.Day = editDay->value();
            cont.WeekOfMonth = editWeekOfMonth->value();
            cont.Month = editMonth->value();
            cont.Frequency = editFrequency->value();

            if (editStartEnabled->isChecked()) cont.StartDate = editStartDate->date();
            if (editEndEnabled->isChecked()) cont.StopDate = editEndDate->date();
            break;
        default:
            break;
    }


    KAlcatelDoc *theDoc=((KAlcatelApp *) parentWidget())->getDocument();
    if (event == NULL) {
        // insert new item
        cont.Modified = true;
        if (cont.Storage == StoragePC) {
            cont.Id = theDoc->getPCStorageId();
        } else {
            // as created are flaged only these which will be commited into mobile
            cont.Created = true;
        }
        list->append(cont);
        theDoc->updateDocument(alcatel_calendar);
    } else {
        cont.Created = event->Created;
        cont.Deleted = event->Deleted;
        cont.PrevId = event->PrevId;
        cont.PrevStorage = event->PrevStorage;
        if (*event != cont) {
            // item was changed
            cont.diffDeleted(*event);
            list->remove(*event);
            cont.Modified = true;
            list->append(cont);
            theDoc->updateDocument(alcatel_calendar);
        }
    }

    accept();
}

void EditEventDialog::slotCancel() {
    reject();
}

void EditEventDialog::slotStorage(int st) {
    if ((AlcatelStorage)st == StorageSIM ||
        (AlcatelStorage)st == StorageAny ||
        (AlcatelStorage)st == StorageNone)
            editStorage->setCurrentItem((int)StoragePC);
}

void EditEventDialog::slotType(int typ) {
    // at first enable items that are used everywhere except unknown
    editSubject->setEnabled(true);
    editPrivate->setEnabled(true);
    editStorage->setEnabled(true);

    if (typ == ALC_CALENDAR_APPOINTMENT) { /* Appointment */
        // disable repeating
        editDayOfWeek->setEnabled(false);
        editDay->setEnabled(false);
        editWeekOfMonth->setEnabled(false);
        editMonth->setEnabled(false);
        editFrequency->setEnabled(false);

        editStartDate->setEnabled(false);
        editStartEnabled->setEnabled(false);
        editEndDate->setEnabled(false);
        editEndEnabled->setEnabled(false);
        // disable contact selection
        buttonContacts->setEnabled(false);
        // enable start & end time
        editStartTime->setEnabled(true);
        editEndTime->setEnabled(true);
        // enable date
        editDateEnabled->setEnabled(true);
        editDate->setEnabled(editDateEnabled->isChecked());
        // enable alarm
        editAlarmEnabled->setEnabled(true);
        editAlarmDate->setEnabled(editAlarmEnabled->isChecked());
        editAlarmTime->setEnabled(editAlarmEnabled->isChecked());
   } else if (typ == ALC_CALENDAR_CALL) { /* Call */
        // disable repeating
        editDayOfWeek->setEnabled(false);
        editDay->setEnabled(false);
        editWeekOfMonth->setEnabled(false);
        editMonth->setEnabled(false);
        editFrequency->setEnabled(false);

        editStartDate->setEnabled(false);
        editStartEnabled->setEnabled(false);
        editEndDate->setEnabled(false);
        editEndEnabled->setEnabled(false);
        // enable contact selection
        buttonContacts->setEnabled(true);
        // enable start & end time
        editStartTime->setEnabled(true);
        editEndTime->setEnabled(true);
        // enable date
        editDateEnabled->setEnabled(true);
        editDate->setEnabled(editDateEnabled->isChecked());
        // enable alarm
        editAlarmEnabled->setEnabled(true);
        editAlarmDate->setEnabled(editAlarmEnabled->isChecked());
        editAlarmTime->setEnabled(editAlarmEnabled->isChecked());
    } else if (typ == ALC_CALENDAR_BIRTHDAY) { /* Bithday */
        // disable start & end time
        editStartTime->setEnabled(false);
        editEndTime->setEnabled(false);
        // disable contact selection
        buttonContacts->setEnabled(false);
        // disable repeating
        editDayOfWeek->setEnabled(false);
        editDay->setEnabled(false);
        editWeekOfMonth->setEnabled(false);
        editMonth->setEnabled(false);
        editFrequency->setEnabled(false);

        editStartDate->setEnabled(false);
        editStartEnabled->setEnabled(false);
        editEndDate->setEnabled(false);
        editEndEnabled->setEnabled(false);
        // enable date
        editDateEnabled->setEnabled(true);
        editDate->setEnabled(editDateEnabled->isChecked());
        // enable alarm
        editAlarmEnabled->setEnabled(true);
        editAlarmDate->setEnabled(editAlarmEnabled->isChecked());
        editAlarmTime->setEnabled(editAlarmEnabled->isChecked());
    } else if (typ == ALC_CALENDAR_ALARM) { /* Alarm */
        // disable start & end time
        editStartTime->setEnabled(false);
        editEndTime->setEnabled(false);
        // disable contact selection
        buttonContacts->setEnabled(false);
        // disable repeating
        editDayOfWeek->setEnabled(false);
        editDay->setEnabled(false);
        editWeekOfMonth->setEnabled(false);
        editMonth->setEnabled(false);
        editFrequency->setEnabled(false);

        editStartDate->setEnabled(false);
        editStartEnabled->setEnabled(false);
        editEndDate->setEnabled(false);
        editEndEnabled->setEnabled(false);
        // disable date
        editDateEnabled->setEnabled(false);
        editDate->setEnabled(false);
        // force alarm
        editAlarmEnabled->setEnabled(false);
        editAlarmEnabled->setChecked(true);
        editAlarmDate->setEnabled(true);
        editAlarmTime->setEnabled(true);
    } else if (typ == ALC_CALENDAR_DAILY_ALARM) { /* Daily alarm */
        // disable start & end time
        editStartTime->setEnabled(false);
        editEndTime->setEnabled(false);
        // disable contact selection
        buttonContacts->setEnabled(false);
        // disable repeating
        editDayOfWeek->setEnabled(false);
        editDay->setEnabled(false);
        editWeekOfMonth->setEnabled(false);
        editMonth->setEnabled(false);
        editFrequency->setEnabled(false);

        editStartDate->setEnabled(false);
        editStartEnabled->setEnabled(false);
        editEndDate->setEnabled(false);
        editEndEnabled->setEnabled(false);
        // disable date
        editDateEnabled->setEnabled(false);
        editDate->setEnabled(false);
    } else if (typ == ALC_CALENDAR_REPEATING) { /* Repeating */
        // enable start & end time
        editStartTime->setEnabled(true);
        editEndTime->setEnabled(true);
        // enable repeating
        editDayOfWeek->setEnabled(true);
        editDay->setEnabled(true);
        editWeekOfMonth->setEnabled(true);
        editMonth->setEnabled(true);
        editFrequency->setEnabled(true);

        editStartDate->setEnabled(editStartEnabled->isChecked());
        editStartEnabled->setEnabled(true);
        editEndDate->setEnabled(editEndEnabled->isChecked());
        editEndEnabled->setEnabled(true);
        // disable contact selection
        buttonContacts->setEnabled(false);
        // enable date
        editDateEnabled->setEnabled(true);
        editDate->setEnabled(editDateEnabled->isChecked());
        // enable alarm
        editAlarmEnabled->setEnabled(true);
        editAlarmDate->setEnabled(editAlarmEnabled->isChecked());
        editAlarmTime->setEnabled(editAlarmEnabled->isChecked());
    } else { /* Unknown */
        // disable everything
        editDayOfWeek->setEnabled(false);
        editDay->setEnabled(false);
        editWeekOfMonth->setEnabled(false);
        editMonth->setEnabled(false);
        editFrequency->setEnabled(false);

        editStartDate->setEnabled(false);
        editStartEnabled->setEnabled(false);
        editEndDate->setEnabled(false);
        editEndEnabled->setEnabled(false);

        buttonContacts->setEnabled(false);

        editStartTime->setEnabled(false);
        editEndTime->setEnabled(false);

        editSubject->setEnabled(false);
        editPrivate->setEnabled(false);
        editStorage->setEnabled(false);

        editDateEnabled->setEnabled(false);
        editDate->setEnabled(false);

        editAlarmEnabled->setEnabled(false);
        editAlarmDate->setEnabled(false);
        editAlarmTime->setEnabled(false);
    }
}

void EditEventDialog::selectContacts() {
    SelectContactDialog dlg(SelectContactDialog::Contact,(AlcatelStorage)editStorage->currentItem(), StorageMobile, contacts, this);
    if (dlg.exec()) {
        ContactID = dlg.getContactID();
        if (ContactID != -1 && ContactID != 0) {
            AlcatelContact *cont = getContactById(contacts, ContactID, (AlcatelStorage)editStorage->currentItem());
            if (cont == NULL) {
                editContactID->setText(i18n("Unknown (id=%1)").arg(ContactID));
            } else {
                editContactID->setText(cont->getName());
            }
        } else {
            editContactID->setText(i18n("None"));
        }
    }
}

void EditEventDialog::toggleDate ( bool t ) {
    editDate->setEnabled(t);
}

void EditEventDialog::toggleAlarm ( bool t ) {
    editAlarmDate->setEnabled(t);
    editAlarmTime->setEnabled(t);
}

void EditEventDialog::toggleEnd ( bool t ) {
    editEndDate->setEnabled(t);
}

void EditEventDialog::toggleStart ( bool t ){
    editStartDate->setEnabled(t);
}
