/*
 * kalcatelconfigdialog.cpp
 *
 * KAlcatel configuation dialog
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
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qtooltip.h>

#include <kdialog.h>
#include <klocale.h>
#include <kjanuswidget.h>
#include <kiconloader.h>
#include <kurlrequester.h>
#include <kcombobox.h>

#include "kalcatelconfigdialog.h"
#include "kalcatel.h"

KAlcatelConfigDialog::KAlcatelConfigDialog(QWidget *parent, const char *name ) : KDialog(parent,name,true) {
    resize(450, 200 );
    setCaption( i18n( "KAlcatel configuration" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    janus = new KJanusWidget(this, "JanusWidget", KJanusWidget::IconList);

    mobilePage = janus->addPage (i18n("Mobile"), i18n("Mobile configuration"), DesktopIcon("kalcatel-mobile.png"));
    QGridLayout *mobileLayout = new QGridLayout( mobilePage, 2, 5);

    mobileLayout->setSpacing( 6 );
    mobileLayout->setMargin( 8 );

    QLabel *label = new QLabel(mobilePage);
    label->setText(i18n("Device filename:"));
    mobileLayout->addWidget(label, 0, 0);

    KComboBox *devCombo = new KComboBox(true, mobilePage);

    devCombo->insertItem("/dev/ttyS0");
    devCombo->insertItem("/dev/ttyS1");
    devCombo->insertItem("/dev/mobile");
    devCombo->insertItem("/dev/modem");

    editDevice = new KURLRequester(devCombo, mobilePage);

    QWhatsThis::add(editDevice ,i18n("<b>Device filename</b><br>Give here name of device that will be used to communicate with mobile."));

    mobileLayout->addWidget(editDevice, 0, 1);

    label = new QLabel(mobilePage);
    label->setText(i18n("Lock filename:"));
    mobileLayout->addWidget(label, 1, 0);

    editLock = new QLineEdit(mobilePage);

    QWhatsThis::add(editLock ,i18n("<b>Lock filename</b><br>Filename of lock that will be used for locking device, %s will be replaced by device basename (for example: when device is /dev/ttyS1 and lock filename is /var/lock/LCK..%s, then lock used will be /var/lock/LCK..ttyS1)."));
    QToolTip::add(editLock ,i18n("%s = device basename"));

    mobileLayout->addWidget(editLock, 1, 1);

    label = new QLabel(mobilePage);
    label->setText(i18n("Init string:"));
    mobileLayout->addWidget(label, 2, 0);

    editInit = new QLineEdit(mobilePage);

    QWhatsThis::add(editInit ,i18n("<b>Init string</b><br>Init string that is sent to modem just after opening it."));

    mobileLayout->addWidget(editInit, 2, 1);

    label = new QLabel(mobilePage);
    label->setText(i18n("Baud rate:"));
    mobileLayout->addWidget(label, 3, 0);

    rateEdit = new QComboBox(mobilePage);
    rateEdit->insertItem("2400");
    rateEdit->insertItem("4800");
    rateEdit->insertItem("9600");
    rateEdit->insertItem("19200");
    rateEdit->insertItem("38400");

    QWhatsThis::add(rateEdit ,i18n("<b>Baud rate</b><br>Baud rate that is used when communicating with mobile, 19200 is is most cases best."));

    mobileLayout->addWidget(rateEdit, 3, 1);

    label = new QLabel(mobilePage);
    label->setText(i18n("Stderr messages:"));
    mobileLayout->addWidget(label, 4, 0);

    debugEdit = new QComboBox(mobilePage);
    debugEdit->insertItem(i18n("Debug 2"));
    debugEdit->insertItem(i18n("Debug"));
    debugEdit->insertItem(i18n("Detail"));
    debugEdit->insertItem(i18n("Information"));
    debugEdit->insertItem(i18n("Warning"));
    debugEdit->insertItem(i18n("Error"));
    debugEdit->insertItem(i18n("None"));

    QWhatsThis::add(debugEdit ,i18n("<b>Stderr messages</b><br>How many messages will be written to standard error output. Messages with lower priority than selected won't be shown. Use debug if there are probles using this program."));
    QToolTip::add(debugEdit ,i18n("Do not display messages with lower priority than"));

    mobileLayout->addWidget(debugEdit, 4, 1);

    otherPage = janus->addPage (i18n("Other"), i18n("Other configuration"), DesktopIcon("kalcatel-configure.png"));
    QGridLayout *otherLayout = new QGridLayout( otherPage, 2, 1);

    otherLayout->setSpacing( 6 );
    otherLayout->setMargin( 8 );

    label = new QLabel(otherPage);
    label->setText(i18n("Phone prefix:"));
    otherLayout->addWidget(label, 0, 0);

    editPrefix = new QLineEdit(otherPage);

    QWhatsThis::add(editPrefix ,i18n("<b>Phone prefix</b><br>Write here prefix that is prepended to national number to get international number. This is used just for resolving contacts by phone (in messages). For example for Czech Republic use +420."));

    otherLayout->addWidget(editPrefix, 0, 1);

    label = new QLabel(otherPage);
    label->setText(i18n("Open last document on start:"));
    otherLayout->addWidget(label, 1, 0);

    editAutoOpen = new QCheckBox(otherPage);

    QWhatsThis::add(editAutoOpen ,i18n("<b>Open last document on start</b><br>If checked, application will try to open last working file on start."));

    otherLayout->addWidget(editAutoOpen, 1, 1);

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
    KAlcatelApp *theApp=(KAlcatelApp *) parentWidget();

    theApp->mobile_device = editDevice->url();
    theApp->mobile_lock = editLock->text();
    theApp->mobile_init = editInit->text();
    theApp->phone_prefix = editPrefix->text();
    theApp->auto_open_last = editAutoOpen->isChecked();

    switch (rateEdit->currentItem()) {
        case 0: theApp->mobile_rate = 2400; break;
        case 1: theApp->mobile_rate = 4800; break;
        case 2: theApp->mobile_rate = 9600; break;
        case 3: theApp->mobile_rate = 19200; break;
        case 4: theApp->mobile_rate = 38400; break;
    }

    theApp->mobile_debug = debugEdit->currentItem();

    accept();
}



int KAlcatelConfigDialog::exec () {
    KAlcatelApp *theApp=(KAlcatelApp *) parentWidget();

    editDevice->setURL(theApp->mobile_device);
    editLock->setText(theApp->mobile_lock);
    editInit->setText(theApp->mobile_init);
    editPrefix->setText(theApp->phone_prefix);
    editAutoOpen->setChecked(theApp->auto_open_last);

    switch (theApp->mobile_rate) {
        case 2400:   rateEdit->setCurrentItem(0); break;
        case 4800:   rateEdit->setCurrentItem(1); break;
        case 9600:   rateEdit->setCurrentItem(2); break;
        case 19200:  rateEdit->setCurrentItem(3); break;
        case 38400:  rateEdit->setCurrentItem(4); break;
    }

    debugEdit->setCurrentItem(theApp->mobile_debug);

    return KDialog::exec();
}