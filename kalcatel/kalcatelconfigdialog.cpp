/*
 * kalcatelconfigdialog.cpp
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
 * if and only if programmer/distributor of that code recieves written
 * permission from author of this code.
 *
 */
/* $Id$ */


#include <qpushbutton.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <kdialog.h>
#include <klocale.h>
#include <kjanuswidget.h>
#include <kiconloader.h>

#include "kalcatelconfigdialog.h"

KAlcatelConfigDialog::KAlcatelConfigDialog(QWidget *parent, const char *name ) : KDialog(parent,name,true) {
    resize(500, 300 );
    setCaption( i18n( "KAlcatel configuration" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    janus = new KJanusWidget(this, "JanusWidget", KJanusWidget::IconList);

    mobilePage = janus->addVBoxPage (i18n("Mobile"), i18n("Mobile configuration"), DesktopIcon("kalcatel-mobile.png"));
    QGridLayout *mobileLayout = new QGridLayout( mobilePage);

    QLabel *label = new QLabel(mobilePage);
    label->setText("Device filename:");

    editDevice = new QLineEdit(mobilePage);

    label = new QLabel(mobilePage);
    label->setText("Lock filename:");

    editLock = new QLineEdit(mobilePage);

    label = new QLabel(mobilePage);
    label->setText("Init string:");

    editInit = new QLineEdit(mobilePage);


    otherPage = janus->addVBoxPage (i18n("Other"), i18n("Other configuration"), DesktopIcon("kalcatel-configure.png"));

    mainLayout->addWidget( janus, 0, 0);

    QHBoxLayout *layout = new QHBoxLayout;

    layout->setSpacing( 6 );
    layout->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout->addItem( spacer );

    QPushButton *buttonOK = new QPushButton(this);
    buttonOK->setText(i18n("&OK"));
    buttonOK->setDefault(true);
    layout->addWidget(buttonOK);

    QPushButton *buttonCancel = new QPushButton(this);
    buttonCancel->setText(i18n("&Cancel"));
    layout->addWidget(buttonCancel);

    mainLayout->addLayout( layout, 2, 0 );

    QFrame *line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addWidget( line, 1, 0 );
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
}

KAlcatelConfigDialog::~KAlcatelConfigDialog(){
}

void KAlcatelConfigDialog::slotCancel() {
    reject();
}

void KAlcatelConfigDialog::slotOK() {
    accept();
}
