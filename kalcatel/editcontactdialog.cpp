/*
 * editcontactdialog.cpp
 *
 * contact editing dialog
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

#include "editcontactdialog.h"
#include "phonenumbervalidator.h"
#include "alcatelclasses.h"

EditContactDialog::EditContactDialog(AlcatelContactList *lst, const AlcatelContact *cont, QWidget *parent, const char *name ) : KDialog(parent,name,true) {
    contact = cont;
    list= lst;

    QLabel *label;
    QFrame *line;

    resize(400, 580 );
    if (cont == NULL) setCaption( i18n( "New contact" ) );
    else setCaption( i18n( "Edit contact" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    if (cont == NULL) label = new QLabel( i18n("<b>New contact</b>"), this );
    else label = new QLabel( i18n("<b>Edit contact</b>"), this );
    mainLayout->addMultiCellWidget(label,0,0,0,3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,1,1,0,3);

    mainLayout->setRowStretch(0, -1);
    mainLayout->setRowStretch(1, -1);

    mainLayout->addWidget(new QLabel(i18n("First name"), this), 2, 0);
    mainLayout->addWidget(editLastName = new QLineEdit(this), 2, 1);

    mainLayout->addWidget(new QLabel(i18n("Last name"), this), 2, 2);
    mainLayout->addWidget(editFirstName = new QLineEdit(this), 2, 3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,3,3,0,3);

    mainLayout->addWidget(new QLabel(i18n("Company"), this), 4, 0);
    mainLayout->addWidget(editCompany = new QLineEdit(this), 4, 1);

    mainLayout->addWidget(new QLabel(i18n("Job title"), this), 4, 2);
    mainLayout->addWidget(editJobTitle = new QLineEdit(this), 4, 3);

    mainLayout->addWidget(new QLabel(i18n("Category"), this), 5, 0);
    mainLayout->addWidget(editCategory = new KComboBox(this), 5, 1);

    mainLayout->addWidget(new QLabel(i18n("Private"), this), 5, 2);
    mainLayout->addWidget(editPrivate = new QCheckBox(this), 5, 3);

    mainLayout->addWidget(new QLabel(i18n("Note"), this), 6, 0); /* this will be longer */
    mainLayout->addMultiCellWidget(editNote = new QLineEdit(this), 6, 6, 1, 3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,7,7,0,3);

    mainLayout->addWidget(new QLabel(i18n("Work nuber"), this),8 , 0);
    mainLayout->addWidget(editWorkNumber = new QLineEdit(this),8 , 1);
    editWorkNumber->setValidator(new PhoneNumberValidator(true, true, false, editWorkNumber));

    mainLayout->addWidget(new QLabel(i18n("Main number"), this),8 , 2);
    mainLayout->addWidget(editMainNumber = new QLineEdit(this),8 , 3);
    editMainNumber->setValidator(new PhoneNumberValidator(true, true, false, editMainNumber));

    mainLayout->addWidget(new QLabel(i18n("Fax number"), this),9 , 0);
    mainLayout->addWidget(editFaxNumber = new QLineEdit(this),9 , 1);
    editFaxNumber->setValidator(new PhoneNumberValidator(true, true, false, editFaxNumber));

    mainLayout->addWidget(new QLabel(i18n("Other number"), this),9 , 2);
    mainLayout->addWidget(editOtherNumber = new QLineEdit(this),9 , 3);
    editOtherNumber->setValidator(new PhoneNumberValidator(true, true, false, editOtherNumber));

    mainLayout->addWidget(new QLabel(i18n("Pager number"), this),10 , 0);
    mainLayout->addWidget(editPagerNumber = new QLineEdit(this),10 , 1);
    editPagerNumber->setValidator(new PhoneNumberValidator(true, true, false, editPagerNumber));

    mainLayout->addWidget(new QLabel(i18n("Mobile number"), this),10 , 2);
    mainLayout->addWidget(editMobileNumber = new QLineEdit(this),10 , 3);
    editMobileNumber->setValidator(new PhoneNumberValidator(true, true, false, editMobileNumber));

    mainLayout->addWidget(new QLabel(i18n("Home number"), this),11 , 0);
    mainLayout->addWidget(editHomeNumber = new QLineEdit(this),11 , 1);
    editHomeNumber->setValidator(new PhoneNumberValidator(true, true, false, editHomeNumber));

/* what should be here?
    mainLayout->addWidget(new QLabel(i18n(""), this), , 2);*/

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,12,12,0,3);

    mainLayout->addWidget(new QLabel(i18n("Email 1"), this), 13, 0);
    mainLayout->addWidget(editEmail1 = new QLineEdit(this), 13, 1);

    mainLayout->addWidget(new QLabel(i18n("Email 2"), this), 13, 2);
    mainLayout->addWidget(editEmail2 = new QLineEdit(this), 13, 3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,14,14,0,3);

    mainLayout->addWidget(new QLabel(i18n("Address"), this), 15, 0); /* this will be longer */
    mainLayout->addMultiCellWidget(editAddress = new QLineEdit(this), 15, 15, 1, 3);

    mainLayout->addWidget(new QLabel(i18n("City"), this), 16, 0);
    mainLayout->addWidget(editCity = new QLineEdit(this), 16, 1);

    mainLayout->addWidget(new QLabel(i18n("Zip"), this), 16, 2);
    mainLayout->addWidget(editZip = new QLineEdit(this), 16, 3);

    mainLayout->addWidget(new QLabel(i18n("State"), this), 17, 0);
    mainLayout->addWidget(editState = new QLineEdit(this), 17, 1);

    mainLayout->addWidget(new QLabel(i18n("Country"), this), 17, 2);
    mainLayout->addWidget(editCountry = new QLineEdit(this), 17, 3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,18,18,0,3);

    mainLayout->addWidget(new QLabel(i18n("Custom 1"), this), 19, 0);
    mainLayout->addWidget(editCustom1 = new QLineEdit(this), 19, 1);

    mainLayout->addWidget(new QLabel(i18n("Custom 2"), this), 19, 2);
    mainLayout->addWidget(editCustom2 = new QLineEdit(this), 19, 3);

    mainLayout->addWidget(new QLabel(i18n("Custom 3"), this), 20, 0);
    mainLayout->addWidget(editCustom3 = new QLineEdit(this), 20, 1);

    mainLayout->addWidget(new QLabel(i18n("Custom 4"), this), 20, 2);
    mainLayout->addWidget(editCustom4 = new QLineEdit(this), 20, 3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,21,21,0,3);

    mainLayout->addWidget(new QLabel(i18n("Storage"), this), 22, 0);

    editStorage = new KComboBox(this);
    editStorage->insertItem(i18n("None"));
    editStorage->insertItem(i18n("PC"));
    editStorage->insertItem(i18n("SIM"));
    editStorage->insertItem(i18n("Mobile"));
    connect( editStorage, SIGNAL( activated(int)), this, SLOT( slotStorage(AlcatelStorage)));
    mainLayout->addWidget(editStorage, 22, 1);

    mainLayout->addWidget(new QLabel(i18n("Position"), this), 22, 2);

    editPosition = new KIntNumInput(this);
    editPosition->setRange(-1, 9999,1,false);
    editPosition->setSpecialValueText(i18n("Auto"));
    mainLayout->addWidget(editPosition, 22, 3);

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

    mainLayout->addMultiCellLayout( layout, 24,24, 0,3 );

    line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addMultiCellWidget( line, 23,23, 0,3 );
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );

    editPosition->setEnabled(false);
}

EditContactDialog::~EditContactDialog(){
}

void EditContactDialog::slotCancel() {
    reject();
}

void EditContactDialog::slotOK() {
    accept();
}

void EditContactDialog::slotStorage(AlcatelStorage st) {
    switch (st) {
        case StorageAny:
        case StorageNone:
            editLastName->setEnabled();
            editFirstName->setEnabled();
            editCompany->setEnabled();
            editJobTitle->setEnabled();
            editNote->setEnabled();
            editCategory->setEnabled();
            editPrivate->setEnabled();
            editWorkNumber->setEnabled();
            editMainNumber->setEnabled();
            editFaxNumber->setEnabled();
            editOtherNumber->setEnabled();
            editPagerNumber->setEnabled();
            editMobileNumber->setEnabled();
            editHomeNumber->setEnabled();
            editEmail1->setEnabled();
            editEmail2->setEnabled();
            editAddress->setEnabled();
            editCity->setEnabled();
            editState->setEnabled();
            editZip->setEnabled();
            editCountry->setEnabled();
            editCustom1->setEnabled();
            editCustom2->setEnabled();
            editCustom3->setEnabled();
            editCustom4->setEnabled();
            break;
        case StorageSIM:
            editLastName->setEnabled();
            editFirstName->setEnabled();
            editCompany->setEnabled();
            editJobTitle->setEnabled();
            editNote->setEnabled();
            editCategory->setEnabled();
            editPrivate->setEnabled();
            editWorkNumber->setEnabled();
            editMainNumber->setEnabled();
            editFaxNumber->setEnabled();
            editOtherNumber->setEnabled();
            editPagerNumber->setEnabled();
            editMobileNumber->setEnabled();
            editHomeNumber->setEnabled();
            editEmail1->setEnabled();
            editEmail2->setEnabled();
            editAddress->setEnabled();
            editCity->setEnabled();
            editState->setEnabled();
            editZip->setEnabled();
            editCountry->setEnabled();
            editCustom1->setEnabled();
            editCustom2->setEnabled();
            editCustom3->setEnabled();
            editCustom4->setEnabled();
            break;
        case StoragePC:
        case StorageMobile:
            editLastName->setEnabled();
            editFirstName->setEnabled();
            editCompany->setEnabled();
            editJobTitle->setEnabled();
            editNote->setEnabled();
            editCategory->setEnabled();
            editPrivate->setEnabled();
            editWorkNumber->setEnabled();
            editMainNumber->setEnabled();
            editFaxNumber->setEnabled();
            editOtherNumber->setEnabled();
            editPagerNumber->setEnabled();
            editMobileNumber->setEnabled();
            editHomeNumber->setEnabled();
            editEmail1->setEnabled();
            editEmail2->setEnabled();
            editAddress->setEnabled();
            editCity->setEnabled();
            editState->setEnabled();
            editZip->setEnabled();
            editCountry->setEnabled();
            editCustom1->setEnabled();
            editCustom2->setEnabled();
            editCustom3->setEnabled();
            editCustom4->setEnabled();
            break;
    }
}

