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
#include <klocale.h>
#include <kmessagebox.h>

#include "editcontactdialog.h"
#include "phonenumbervalidator.h"
#include "alcatelclasses.h"

EditContactDialog::EditContactDialog(const AlcatelContact *cont, QWidget *parent, const char *name ) : KDialog(parent,name,true) {
    contact = cont;
    QLabel *label;
    QFrame *line;

    resize(450, 280 );
    if (cont == NULL) setCaption( i18n( "New contact" ) );
    else setCaption( i18n( "Edit contact" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    if (cont == NULL) label = new QLabel( i18n("<b>New contact</b>"), this );
    else label = new QLabel( i18n("<b>Edit contact</b>"), this );
    mainLayout->addMultiCellWidget(label,0,0,0,4);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,1,1,0,4);

    mainLayout->setRowStretch(0, -1);
    mainLayout->setRowStretch(1, -1);


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

    mainLayout->addMultiCellLayout( layout, 7,7, 0,4 );

    line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addMultiCellWidget( line, 6,6, 0,4 );
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
}

EditContactDialog::~EditContactDialog(){
}

void EditContactDialog::slotCancel() {
    reject();
}

void EditContactDialog::slotOK() {
    accept();
}
