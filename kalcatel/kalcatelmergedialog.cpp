/*
 * kalcatelmergedialog.cpp
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

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qframe.h>
#include <qgrid.h>
#include <qdatetime.h>

#include <kdialog.h>
#include <klocale.h>

#include "kalcatelmergedialog.h"

#include "kalcatel.h"
#include "kalcateldoc.h"

#include "alcatelclasses.h"
#include "alcatool/logging.h"

KAlcatelMergeDialog::KAlcatelMergeDialog(QWidget *parent, const char *name ) : KDialog(parent,name,true) {
    QLabel *label;
    setCaption( i18n( "Merging conflict" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    conflictGrid = new QGrid( 6, this );
    conflictGrid->setSpacing( 6 );
    conflictGrid->setMargin( 8 );

    new QLabel(i18n("<b>Field</b>"), conflictGrid);
    new QLabel(i18n("<b>In PC</b>"), conflictGrid);

    pcButton = new QPushButton(i18n("&PC"), conflictGrid);
    deleteButton = new QPushButton(i18n("&Delete"), conflictGrid);
    mobileButton = new QPushButton(i18n("&Mobile"), conflictGrid);
    connect( pcButton, SIGNAL( clicked() ), this, SLOT( slotPC() ) );
    connect( mobileButton, SIGNAL( clicked() ), this, SLOT( slotMobile() ) );
    connect( deleteButton, SIGNAL( clicked() ), this, SLOT( slotDelete() ) );

    new QLabel(i18n("<b>In mobile</b>"), conflictGrid);

    mainLayout->addWidget(conflictGrid, 3, 0);

    QFrame *line = new QFrame(this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addWidget( line, 2, 0 );

    label = new QLabel(i18n("<b>During merge occured following conflict(s)</b><br/>Select which solution of this conflict is the best for you."), this);
    mainLayout->addWidget(label, 0, 0);

    titleLabel = new QLabel( this);
    mainLayout->addWidget(titleLabel, 1, 0);

    QHBoxLayout *layout = new QHBoxLayout;

    layout->setSpacing( 6 );
    layout->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout->addItem( spacer );

    buttonOK = new QPushButton(i18n("&OK"), this);
    buttonOK->setDefault(true);
    layout->addWidget(buttonOK);

    buttonBoth = new QPushButton(i18n("Add &both"), this);
    layout->addWidget(buttonBoth);

    mainLayout->addLayout( layout, 5, 0 );

    line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addWidget( line, 4, 0 );

    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonBoth, SIGNAL( clicked() ), this, SLOT( slotBoth() ) );
}

KAlcatelMergeDialog::~KAlcatelMergeDialog() {
    slotClear();
}

void KAlcatelMergeDialog::slotClear() {
    MergeItemList::Iterator it;
    for( it = itemList.begin(); it != itemList.end(); ) {
        conflictGrid->removeChild((*it).mobileCheck);
        delete (*it).mobileCheck;
        (*it).mobileCheck = NULL;
        conflictGrid->removeChild((*it).deleteCheck);
        delete (*it).deleteCheck;
        (*it).deleteCheck = NULL;
        conflictGrid->removeChild((*it).pcCheck);
        delete (*it).pcCheck;
        (*it).pcCheck = NULL;
        conflictGrid->removeChild((*it).mobileLabel);
        delete (*it).mobileLabel;
        (*it).mobileLabel = NULL;
        conflictGrid->removeChild((*it).pcLabel);
        delete (*it).pcLabel;
        (*it).pcLabel = NULL;
        it = itemList.remove(it);
    }
    repaint();
}

AlcatelContact *KAlcatelMergeDialog::exec(AlcatelContact &c1, AlcatelContact &c2) {
    data1 = &c1;
    data2 = &c2;

    slotClear();

    titleLabel->setText(i18n("Field type: <b>%1</b><br/>Field name: <b>%2</b>").arg(i18n("Contact")).arg(c1.getName()));

    if (((c1.LastName.isEmpty() ^ c2.LastName.isEmpty())) || (c1.LastName != c2.LastName))  { itemList.append(MergeItem(conflictGrid, i18n("LastName"), c1.LastName, c2.LastName)); }
    if (((c1.FirstName.isEmpty() ^ c2.FirstName.isEmpty())) || (c1.FirstName != c2.FirstName))  { itemList.append(MergeItem(conflictGrid, i18n("FirstName"), c1.FirstName, c2.FirstName)); }
    if (((c1.Company.isEmpty() ^ c2.Company.isEmpty())) || (c1.Company != c2.Company))  { itemList.append(MergeItem(conflictGrid, i18n("Company"), c1.Company, c2.Company)); }
    if (((c1.JobTitle.isEmpty() ^ c2.JobTitle.isEmpty())) || (c1.JobTitle != c2.JobTitle))  { itemList.append(MergeItem(conflictGrid, i18n("JobTitle"), c1.JobTitle, c2.JobTitle)); }
    if (((c1.Note.isEmpty() ^ c2.Note.isEmpty())) || (c1.Note != c2.Note))  { itemList.append(MergeItem(conflictGrid, i18n("Note"), c1.Note, c2.Note)); }
    if (c1.Category != c2.Category)  {
        KAlcatelApp *theApp = (KAlcatelApp *) parentWidget();
        KAlcatelDoc *theDoc = theApp->getDocument();
        AlcatelCategory *cat1 = getCategoryById(theDoc->contact_cats, c1.Category, c1.Storage);
        AlcatelCategory *cat2 = getCategoryById(theDoc->contact_cats, c2.Category, c2.Storage);
        QString s1 = cat1 == NULL ? i18n("Unknown") : cat1->Name;
        QString s2 = cat2 == NULL ? i18n("Unknown") : cat2->Name;
        itemList.append(MergeItem(conflictGrid, i18n("Category"), s1, s2));
    }
    if (c1.Private != c2.Private)  { itemList.append(MergeItem(conflictGrid, i18n("Private"), c1.Private, c2.Private)); }
    if (((c1.WorkNumber.isEmpty() ^ c2.WorkNumber.isEmpty())) || (c1.WorkNumber != c2.WorkNumber))  { itemList.append(MergeItem(conflictGrid, i18n("WorkNumber"), c1.WorkNumber, c2.WorkNumber)); }
    if (((c1.MainNumber.isEmpty() ^ c2.MainNumber.isEmpty())) || (c1.MainNumber != c2.MainNumber))  { itemList.append(MergeItem(conflictGrid, i18n("MainNumber"), c1.MainNumber, c2.MainNumber)); }
    if (((c1.FaxNumber.isEmpty() ^ c2.FaxNumber.isEmpty())) || (c1.FaxNumber != c2.FaxNumber))  { itemList.append(MergeItem(conflictGrid, i18n("FaxNumber"), c1.FaxNumber, c2.FaxNumber)); }
    if (((c1.OtherNumber.isEmpty() ^ c2.OtherNumber.isEmpty())) || (c1.OtherNumber != c2.OtherNumber))  { itemList.append(MergeItem(conflictGrid, i18n("OtherNumber"), c1.OtherNumber, c2.OtherNumber)); }
    if (((c1.PagerNumber.isEmpty() ^ c2.PagerNumber.isEmpty())) || (c1.PagerNumber != c2.PagerNumber))  { itemList.append(MergeItem(conflictGrid, i18n("PagerNumber"), c1.PagerNumber, c2.PagerNumber)); }
    if (((c1.MobileNumber.isEmpty() ^ c2.MobileNumber.isEmpty())) || (c1.MobileNumber != c2.MobileNumber))  { itemList.append(MergeItem(conflictGrid, i18n("MobileNumber"), c1.MobileNumber, c2.MobileNumber)); }
    if (((c1.HomeNumber.isEmpty() ^ c2.HomeNumber.isEmpty())) || (c1.HomeNumber != c2.HomeNumber))  { itemList.append(MergeItem(conflictGrid, i18n("HomeNumber"), c1.HomeNumber, c2.HomeNumber)); }
    if (((c1.Email1.isEmpty() ^ c2.Email1.isEmpty())) || (c1.Email1 != c2.Email1))  { itemList.append(MergeItem(conflictGrid, i18n("Email1"), c1.Email1, c2.Email1)); }
    if (((c1.Email2.isEmpty() ^ c2.Email2.isEmpty())) || (c1.Email2 != c2.Email2))  { itemList.append(MergeItem(conflictGrid, i18n("Email2"), c1.Email2, c2.Email2)); }
    if (((c1.Address.isEmpty() ^ c2.Address.isEmpty())) || (c1.Address != c2.Address))  { itemList.append(MergeItem(conflictGrid, i18n("Address"), c1.Address, c2.Address)); }
    if (((c1.City.isEmpty() ^ c2.City.isEmpty())) || (c1.City != c2.City))  { itemList.append(MergeItem(conflictGrid, i18n("City"), c1.City, c2.City)); }
    if (((c1.State.isEmpty() ^ c2.State.isEmpty())) || (c1.State != c2.State))  { itemList.append(MergeItem(conflictGrid, i18n("State"), c1.State, c2.State)); }
    if (((c1.Zip.isEmpty() ^ c2.Zip.isEmpty())) || (c1.Zip != c2.Zip))  { itemList.append(MergeItem(conflictGrid, i18n("Zip"), c1.Zip, c2.Zip)); }
    if (((c1.Country.isEmpty() ^ c2.Country.isEmpty())) || (c1.Country != c2.Country))  { itemList.append(MergeItem(conflictGrid, i18n("Country"), c1.Country, c2.Country)); }
    if (((c1.Custom1.isEmpty() ^ c2.Custom1.isEmpty())) || (c1.Custom1 != c2.Custom1))  { itemList.append(MergeItem(conflictGrid, i18n("Custom1"), c1.Custom1, c2.Custom1)); }
    if (((c1.Custom2.isEmpty() ^ c2.Custom2.isEmpty())) || (c1.Custom2 != c2.Custom2))  { itemList.append(MergeItem(conflictGrid, i18n("Custom2"), c1.Custom2, c2.Custom2)); }
    if (((c1.Custom3.isEmpty() ^ c2.Custom3.isEmpty())) || (c1.Custom3 != c2.Custom3))  { itemList.append(MergeItem(conflictGrid, i18n("Custom3"), c1.Custom3, c2.Custom3)); }
    if (((c1.Custom4.isEmpty() ^ c2.Custom4.isEmpty())) || (c1.Custom4 != c2.Custom4))  { itemList.append(MergeItem(conflictGrid, i18n("Custom4"), c1.Custom4, c2.Custom4)); }
    exec();
    slotClear();
    return (AlcatelContact *)result;
}

AlcatelMessage *KAlcatelMergeDialog::exec(AlcatelMessage &c1, AlcatelMessage &c2) {
    data1 = &c1;
    data2 = &c2;
    slotClear();

    titleLabel->setText(i18n("Field type: <b>%1</b><br/>Field name: <b>%2</b>").arg(i18n("Message")).arg(c1.getName()));

    exec();
    slotClear();
    return (AlcatelMessage *)result;
}

AlcatelCalendar *KAlcatelMergeDialog::exec(AlcatelCalendar &c1, AlcatelCalendar &c2) {
    data1 = &c1;
    data2 = &c2;
    slotClear();

    titleLabel->setText(i18n("Field type: <b>%1</b><br/>Field name: <b>%2</b>").arg(i18n("Calendar")).arg(c1.getName()));

    if (c1.Date != c2.Date)  { itemList.append(MergeItem(conflictGrid, i18n("Date"), c1.Date, c2.Date)); }
    if (c1.StartTime != c2.StartTime) { itemList.append(MergeItem(conflictGrid, i18n("StartTime"), c1.StartTime, c2.StartTime)); }
    if (c1.EndTime != c2.EndTime)  { itemList.append(MergeItem(conflictGrid, i18n("EndTime"), c1.EndTime, c2.EndTime)); }
    if (c1.Alarm != c2.Alarm) { itemList.append(MergeItem(conflictGrid, i18n("Alarm"), c1.Alarm, c2.Alarm)); }
    if (((c1.Subject.isEmpty() ^ c2.Subject.isEmpty())) || (c1.Subject != c2.Subject))  { itemList.append(MergeItem(conflictGrid, i18n("Subject"), c1.Subject, c2.Subject)); }
    if (c1.Private != c2.Private)  { itemList.append(MergeItem(conflictGrid, i18n("Private"), c1.Private, c2.Private)); }
    if (c1.EventType != c2.EventType)  { itemList.append(MergeItem(conflictGrid, i18n("EventType"), CalendarTypes[c1.EventType], CalendarTypes[c2.EventType])); }
    if (c1.ContactID != c2.ContactID)  { itemList.append(MergeItem(conflictGrid, i18n("ContactID"), c1.ContactID, c2.ContactID)); }
    if (c1.DayOfWeek != c2.DayOfWeek)  { itemList.append(MergeItem(conflictGrid, i18n("DayOfWeek"), c1.DayOfWeek, c2.DayOfWeek)); }
    if (c1.Day != c2.Day)  { itemList.append(MergeItem(conflictGrid, i18n("Day"), c1.Day, c2.Day)); }
    if (c1.WeekOfMonth != c2.WeekOfMonth)  { itemList.append(MergeItem(conflictGrid, i18n("WeekOfMonth"), c1.WeekOfMonth, c2.WeekOfMonth)); }
    if (c1.Month != c2.Month)  { itemList.append(MergeItem(conflictGrid, i18n("Month"), c1.Month, c2.Month)); }
    if (c1.Frequency != c2.Frequency)  { itemList.append(MergeItem(conflictGrid, i18n("Frequency"), c1.Frequency, c2.Frequency)); }
    if (c1.StartDate != c2.StartDate)  { itemList.append(MergeItem(conflictGrid, i18n("StartDate"), c1.StartDate, c2.StartDate)); }
    if (c1.StopDate != c2.StopDate)  { itemList.append(MergeItem(conflictGrid, i18n("StopDate"), c1.StopDate, c2.StopDate)); }
    if (c1.Alarm2 != c2.Alarm2)  { itemList.append(MergeItem(conflictGrid, i18n("Alarm2"), c1.Alarm2, c2.Alarm2)); }

    exec();
    slotClear();
    return (AlcatelCalendar *)result;
}

AlcatelTodo *KAlcatelMergeDialog::exec(AlcatelTodo &c1, AlcatelTodo &c2) {
    data1 = &c1;
    data2 = &c2;
    slotClear();

    titleLabel->setText(i18n("Field type: <b>%1</b><br/>Field name: <b>%2</b>").arg(i18n("Todo")).arg(c1.getName()));

    exec();
    slotClear();
    return (AlcatelTodo *)result;
}

AlcatelCategory *KAlcatelMergeDialog::exec(AlcatelCategory &c1, AlcatelCategory &c2) {
    data1 = &c1;
    data2 = &c2;
    slotClear();

    titleLabel->setText(i18n("Field type: <b>%1</b><br/>Field name: <b>%2</b>").arg(i18n("Category")).arg(c1.getName()));

    if (((c1.Name.isEmpty() ^ c2.Name.isEmpty())) || (c1.Name != c2.Name))  { itemList.append(MergeItem(conflictGrid, i18n("Name"), c1.Name, c2.Name)); }

    exec();
    slotClear();
    return (AlcatelCategory *)result;
}

void KAlcatelMergeDialog::slotBoth() {
    accept();
}

void KAlcatelMergeDialog::slotOK() {
    MergeItemList::Iterator it;

    if (strcmp(data1->getClassName(), "AlcatelContact") == 0) {
        AlcatelContact *c = new AlcatelContact(*((AlcatelContact *)data1));
        if (((((AlcatelContact *)data1)->LastName.isEmpty() ^ ((AlcatelContact *)data2)->LastName.isEmpty())) || (((AlcatelContact *)data1)->LastName != ((AlcatelContact *)data2)->LastName)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->LastName = ((AlcatelContact *)data2)->LastName; break;
                case Delete: c->LastName = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->FirstName.isEmpty() ^ ((AlcatelContact *)data2)->FirstName.isEmpty())) || (((AlcatelContact *)data1)->FirstName != ((AlcatelContact *)data2)->FirstName)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->FirstName = ((AlcatelContact *)data2)->FirstName; break;
                case Delete: c->FirstName = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Company.isEmpty() ^ ((AlcatelContact *)data2)->Company.isEmpty())) || (((AlcatelContact *)data1)->Company != ((AlcatelContact *)data2)->Company)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Company = ((AlcatelContact *)data2)->Company; break;
                case Delete: c->Company = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->JobTitle.isEmpty() ^ ((AlcatelContact *)data2)->JobTitle.isEmpty())) || (((AlcatelContact *)data1)->JobTitle != ((AlcatelContact *)data2)->JobTitle)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->JobTitle = ((AlcatelContact *)data2)->JobTitle; break;
                case Delete: c->JobTitle = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Note.isEmpty() ^ ((AlcatelContact *)data2)->Note.isEmpty())) || (((AlcatelContact *)data1)->Note != ((AlcatelContact *)data2)->Note)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Note = ((AlcatelContact *)data2)->Note; break;
                case Delete: c->Note = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((AlcatelContact *)data1)->Category != ((AlcatelContact *)data2)->Category) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Category = ((AlcatelContact *)data2)->Category; break;
                case Delete: c->Category = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((AlcatelContact *)data1)->Private != ((AlcatelContact *)data2)->Private) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Private = ((AlcatelContact *)data2)->Private; break;
                case Delete: c->Private = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->WorkNumber.isEmpty() ^ ((AlcatelContact *)data2)->WorkNumber.isEmpty())) || (((AlcatelContact *)data1)->WorkNumber != ((AlcatelContact *)data2)->WorkNumber)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->WorkNumber = ((AlcatelContact *)data2)->WorkNumber; break;
                case Delete: c->WorkNumber = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->MainNumber.isEmpty() ^ ((AlcatelContact *)data2)->MainNumber.isEmpty())) || (((AlcatelContact *)data1)->MainNumber != ((AlcatelContact *)data2)->MainNumber)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->MainNumber = ((AlcatelContact *)data2)->MainNumber; break;
                case Delete: c->MainNumber = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->FaxNumber.isEmpty() ^ ((AlcatelContact *)data2)->FaxNumber.isEmpty())) || (((AlcatelContact *)data1)->FaxNumber != ((AlcatelContact *)data2)->FaxNumber)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->FaxNumber = ((AlcatelContact *)data2)->FaxNumber; break;
                case Delete: c->FaxNumber = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->OtherNumber.isEmpty() ^ ((AlcatelContact *)data2)->OtherNumber.isEmpty())) || (((AlcatelContact *)data1)->OtherNumber != ((AlcatelContact *)data2)->OtherNumber)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->OtherNumber = ((AlcatelContact *)data2)->OtherNumber; break;
                case Delete: c->OtherNumber = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->PagerNumber.isEmpty() ^ ((AlcatelContact *)data2)->PagerNumber.isEmpty())) || (((AlcatelContact *)data1)->PagerNumber != ((AlcatelContact *)data2)->PagerNumber)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->PagerNumber = ((AlcatelContact *)data2)->PagerNumber; break;
                case Delete: c->PagerNumber = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->MobileNumber.isEmpty() ^ ((AlcatelContact *)data2)->MobileNumber.isEmpty())) || (((AlcatelContact *)data1)->MobileNumber != ((AlcatelContact *)data2)->MobileNumber)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->MobileNumber = ((AlcatelContact *)data2)->MobileNumber; break;
                case Delete: c->MobileNumber = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->HomeNumber.isEmpty() ^ ((AlcatelContact *)data2)->HomeNumber.isEmpty())) || (((AlcatelContact *)data1)->HomeNumber != ((AlcatelContact *)data2)->HomeNumber)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->HomeNumber = ((AlcatelContact *)data2)->HomeNumber; break;
                case Delete: c->HomeNumber = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Email1.isEmpty() ^ ((AlcatelContact *)data2)->Email1.isEmpty())) || (((AlcatelContact *)data1)->Email1 != ((AlcatelContact *)data2)->Email1)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Email1 = ((AlcatelContact *)data2)->Email1; break;
                case Delete: c->Email1 = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Email2.isEmpty() ^ ((AlcatelContact *)data2)->Email2.isEmpty())) || (((AlcatelContact *)data1)->Email2 != ((AlcatelContact *)data2)->Email2)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Email2 = ((AlcatelContact *)data2)->Email2; break;
                case Delete: c->Email2 = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Address.isEmpty() ^ ((AlcatelContact *)data2)->Address.isEmpty())) || (((AlcatelContact *)data1)->Address != ((AlcatelContact *)data2)->Address)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Address = ((AlcatelContact *)data2)->Address; break;
                case Delete: c->Address = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->City.isEmpty() ^ ((AlcatelContact *)data2)->City.isEmpty())) || (((AlcatelContact *)data1)->City != ((AlcatelContact *)data2)->City)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->City = ((AlcatelContact *)data2)->City; break;
                case Delete: c->City = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->State.isEmpty() ^ ((AlcatelContact *)data2)->State.isEmpty())) || (((AlcatelContact *)data1)->State != ((AlcatelContact *)data2)->State)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->State = ((AlcatelContact *)data2)->State; break;
                case Delete: c->State = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Zip.isEmpty() ^ ((AlcatelContact *)data2)->Zip.isEmpty())) || (((AlcatelContact *)data1)->Zip != ((AlcatelContact *)data2)->Zip)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Zip = ((AlcatelContact *)data2)->Zip; break;
                case Delete: c->Zip = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Country.isEmpty() ^ ((AlcatelContact *)data2)->Country.isEmpty())) || (((AlcatelContact *)data1)->Country != ((AlcatelContact *)data2)->Country)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Country = ((AlcatelContact *)data2)->Country; break;
                case Delete: c->Country = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Custom1.isEmpty() ^ ((AlcatelContact *)data2)->Custom1.isEmpty())) || (((AlcatelContact *)data1)->Custom1 != ((AlcatelContact *)data2)->Custom1)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Custom1 = ((AlcatelContact *)data2)->Custom1; break;
                case Delete: c->Custom1 = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Custom2.isEmpty() ^ ((AlcatelContact *)data2)->Custom2.isEmpty())) || (((AlcatelContact *)data1)->Custom2 != ((AlcatelContact *)data2)->Custom2)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Custom2 = ((AlcatelContact *)data2)->Custom2; break;
                case Delete: c->Custom2 = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Custom3.isEmpty() ^ ((AlcatelContact *)data2)->Custom3.isEmpty())) || (((AlcatelContact *)data1)->Custom3 != ((AlcatelContact *)data2)->Custom3)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Custom3 = ((AlcatelContact *)data2)->Custom3; break;
                case Delete: c->Custom3 = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        if (((((AlcatelContact *)data1)->Custom4.isEmpty() ^ ((AlcatelContact *)data2)->Custom4.isEmpty())) || (((AlcatelContact *)data1)->Custom4 != ((AlcatelContact *)data2)->Custom4)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Custom4 = ((AlcatelContact *)data2)->Custom4; break;
                case Delete: c->Custom4 = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        result = c;
    } else if (strcmp(data1->getClassName(), "AlcatelCategory") == 0) {
        AlcatelCategory *c = new AlcatelCategory(*((AlcatelCategory *)data1));
        if (((((AlcatelCategory *)data1)->Name.isEmpty() ^ ((AlcatelCategory *)data2)->Name.isEmpty())) || (((AlcatelCategory *)data1)->Name != ((AlcatelCategory *)data2)->Name)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Name = ((AlcatelCategory *)data2)->Name; break;
                case Delete: c->Name = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        result = c;
    } else if (strcmp(data1->getClassName(), "AlcatelCalendar") == 0) {
        AlcatelCalendar *c = new AlcatelCalendar(*((AlcatelCalendar *)data1));
        if (((AlcatelCalendar *)data1)->Date != ((AlcatelCalendar *)data2)->Date) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Date = ((AlcatelCalendar *)data2)->Date; break;
                case Delete: c->Date = QDate(); break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->StartTime != ((AlcatelCalendar *)data2)->StartTime) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->StartTime = ((AlcatelCalendar *)data2)->StartTime; break;
                case Delete: c->StartTime = QTime(); break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->EndTime != ((AlcatelCalendar *)data2)->EndTime) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->EndTime = ((AlcatelCalendar *)data2)->EndTime; break;
                case Delete: c->EndTime = QTime(); break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->Alarm != ((AlcatelCalendar *)data2)->Alarm) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Alarm = ((AlcatelCalendar *)data2)->Alarm; break;
                case Delete: c->Alarm = QDateTime(); break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((((AlcatelCalendar *)data1)->Subject.isEmpty() ^ ((AlcatelCalendar *)data2)->Subject.isEmpty())) || (((AlcatelCalendar *)data1)->Subject != ((AlcatelCalendar *)data2)->Subject)) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Subject = ((AlcatelCalendar *)data2)->Subject; break;
                case Delete: c->Subject = (char *)NULL; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->Private != ((AlcatelCalendar *)data2)->Private) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Private = ((AlcatelCalendar *)data2)->Private; break;
                case Delete: c->Private = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->EventType != ((AlcatelCalendar *)data2)->EventType) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->EventType = ((AlcatelCalendar *)data2)->EventType; break;
                case Delete: c->EventType = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->ContactID != ((AlcatelCalendar *)data2)->ContactID) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->ContactID = ((AlcatelCalendar *)data2)->ContactID; break;
                case Delete: c->ContactID = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->DayOfWeek != ((AlcatelCalendar *)data2)->DayOfWeek) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->DayOfWeek = ((AlcatelCalendar *)data2)->DayOfWeek; break;
                case Delete: c->DayOfWeek = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->Day != ((AlcatelCalendar *)data2)->Day) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Day = ((AlcatelCalendar *)data2)->Day; break;
                case Delete: c->Day = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->WeekOfMonth != ((AlcatelCalendar *)data2)->WeekOfMonth) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->WeekOfMonth = ((AlcatelCalendar *)data2)->WeekOfMonth; break;
                case Delete: c->WeekOfMonth = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->Month != ((AlcatelCalendar *)data2)->Month) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Month = ((AlcatelCalendar *)data2)->Month; break;
                case Delete: c->Month = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->Frequency != ((AlcatelCalendar *)data2)->Frequency) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Frequency = ((AlcatelCalendar *)data2)->Frequency; break;
                case Delete: c->Frequency = -1; break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->StartDate != ((AlcatelCalendar *)data2)->StartDate) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->StartDate = ((AlcatelCalendar *)data2)->StartDate; break;
                case Delete: c->StartDate = QDate(); break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->StopDate != ((AlcatelCalendar *)data2)->StopDate) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->StopDate = ((AlcatelCalendar *)data2)->StopDate; break;
                case Delete: c->StopDate = QDate(); break;
                case Mobile: break;
            }
            itemList.remove(it);
        }

        if (((AlcatelCalendar *)data1)->Alarm2 != ((AlcatelCalendar *)data2)->Alarm2) {
            it = itemList.begin();
            switch ((*it).getStatus()) {
                case PC: c->Alarm2 = ((AlcatelCalendar *)data2)->Alarm2; break;
                case Delete: c->Alarm2 = QDateTime(); break;
                case Mobile: break;
            }
            itemList.remove(it);
        }
        result = c;
    } else if (strcmp(data1->getClassName(), "AlcatelTodo") == 0) {
        AlcatelTodo *c = new AlcatelTodo(*((AlcatelTodo *)data1));
        result = c;
    } else if (strcmp(data1->getClassName(), "AlcatelMessage") == 0) {
        AlcatelMessage *c = new AlcatelMessage(*((AlcatelMessage*)data1));
        result = c;
    } else {
        result = NULL;
    }
    accept();
}

void KAlcatelMergeDialog::slotPC(void) {
    MergeItemList::Iterator it;
    for( it = itemList.begin(); it != itemList.end(); ++it ) {
        (*it).pcCheck->setChecked(true);
    }
}

void KAlcatelMergeDialog::slotDelete(void) {
    MergeItemList::Iterator it;
    for( it = itemList.begin(); it != itemList.end(); ++it ) {
        (*it).deleteCheck->setChecked(true);
    }
}

void KAlcatelMergeDialog::slotMobile(void) {
    MergeItemList::Iterator it;
    for( it = itemList.begin(); it != itemList.end(); ++it ) {
        (*it).mobileCheck->setChecked(true);
    }
}

int KAlcatelMergeDialog::exec () {
    result = NULL;
    return KDialog::exec();
}

void MergeItem::MergeItem_init(QWidget *parent, QString name, QString mobileText, QString pcText) {
    nameLabel = new QLabel(name, parent);
    pcLabel = new QLabel(pcText, parent);

    pcCheck = new QRadioButton(parent);
    deleteCheck = new QRadioButton(parent);
    mobileCheck = new QRadioButton(parent);

    connect(pcCheck, SIGNAL (toggled(bool)), this, SLOT (pcToggle(bool)));
    connect(deleteCheck, SIGNAL (toggled(bool)), this, SLOT (deleteToggle(bool)));
    connect(mobileCheck, SIGNAL (toggled(bool)), this, SLOT (mobileToggle(bool)));

    mobileCheck->setChecked(true);

    mobileLabel = new QLabel(mobileText, parent);
}

MergeItem::MergeItem(QWidget *parent, QString name, QString mobileText, QString pcText) {
    MergeItem_init(parent, name, mobileText, pcText);
}

MergeItem::MergeItem(QWidget *parent, QString name, QDate mobileData, QDate pcData) {
    QString s1 = mobileData.toString();
    QString s2 = pcData.toString();
    MergeItem_init(parent, name, s1, s2);
}

MergeItem::MergeItem(QWidget *parent, QString name, QTime mobileData, QTime pcData) {
    QString s1 = mobileData.toString();
    QString s2 = pcData.toString();
    MergeItem_init(parent, name, s1, s2);
}

MergeItem::MergeItem(QWidget *parent, QString name, QDateTime mobileData, QDateTime pcData) {
    QString s1 = mobileData.toString();
    QString s2 = pcData.toString();
    MergeItem_init(parent, name, s1, s2);
}

MergeItem::MergeItem(QWidget *parent, QString name, int mobileData, int pcData) {
    QString s1 = mobileData==-1 ? i18n("Not set") : mobileData==0 ? i18n("No") : i18n("Yes");
    QString s2 = pcData==-1 ? i18n("Not set") : pcData==0 ? i18n("No") : i18n("Yes");
    MergeItem_init(parent, name, s1, s2);
}

MergeItem::MergeItem(const MergeItem  &item) {
    nameLabel = item.nameLabel;
    pcLabel = item.pcLabel;

    pcCheck = item.pcCheck;
    deleteCheck = item.deleteCheck;
    mobileCheck = item.mobileCheck;

    connect(pcCheck, SIGNAL (toggled(bool)), this, SLOT (pcToggle(bool)));
    connect(deleteCheck, SIGNAL (toggled(bool)), this, SLOT (deleteToggle(bool)));
    connect(mobileCheck, SIGNAL (toggled(bool)), this, SLOT (mobileToggle(bool)));

    mobileCheck->setChecked(true);

    mobileLabel = item.mobileLabel;
}

MergeItem::MergeItem() {
    nameLabel = new QLabel(0);
    pcLabel = new QLabel(0);

    pcCheck = new QRadioButton(0);
    deleteCheck = new QRadioButton(0);
    mobileCheck = new QRadioButton(0);

    connect(pcCheck, SIGNAL (toggled(bool)), this, SLOT (pcToggle(bool)));
    connect(deleteCheck, SIGNAL (toggled(bool)), this, SLOT (deleteToggle(bool)));
    connect(mobileCheck, SIGNAL (toggled(bool)), this, SLOT (mobileToggle(bool)));

    mobileCheck->setChecked(true);

    mobileLabel = new QLabel(0);
}

MergeItem::~MergeItem(void) {
}

void MergeItem::pcToggle(bool state) {
    if (state) {
        mobileCheck->setChecked(false);
        deleteCheck->setChecked(false);
    }
}

void MergeItem::deleteToggle(bool state) {
    if (state) {
        pcCheck->setChecked(false);
        mobileCheck->setChecked(false);
    }
}

void MergeItem::mobileToggle(bool state) {
    if (state) {
        pcCheck->setChecked(false);
        deleteCheck->setChecked(false);
    }
}

MergeStatus MergeItem::getStatus(void) {
    if (pcCheck->isChecked()) return PC;
    else if (mobileCheck->isChecked()) return Mobile;
    else return Delete;
}

void MergeItem::setStatus(MergeStatus status) {
    if (status == PC) pcCheck->setChecked(true);
    else if (status == Mobile) mobileCheck->setChecked(true);
    else deleteCheck->setChecked(true);
}
