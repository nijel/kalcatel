/*
 * selectcontactdialog.cpp
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
#include <qpushbutton.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>

#include <klocale.h>
#include <klistbox.h>

#include "selectcontactdialog.h"

SelectContactDialog::SelectContactDialog(SelectContactDialog::SelectType type, AlcatelStorage st, AlcatelContactList *conts, QWidget *parent, const char *name ) : KDialog(parent, name, true), type(type) {
    list = conts;
    storage = st;

    QLabel *label;
    QFrame *line;

    if (type == Numbers) setCaption( i18n( "Select phone number(s)" ) );
    else setCaption( i18n( "Select contact" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    if (type == Numbers) label = new QLabel( i18n("<b>Select phone number(s)</b>"), this );
    else label = new QLabel( i18n("<b>Select contact</b>"), this );
    mainLayout->addMultiCellWidget(label,0,0,0,3);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,1,1,0,3);


    listbox = new KListBox(this);
    if (type == Contact) {
        listbox-> setSelectionMode (KListBox::Single);
        AlcatelContactList::Iterator it;
        for( it = list->begin(); it != list->end(); ++it ) {
            new ContactBoxItem( "", (*it).Id, listbox, (*it).getName());
        }
    } else {
        listbox-> setSelectionMode (KListBox::Extended);
        AlcatelContactList::Iterator it;
        for( it = list->begin(); it != list->end(); ++it ) {
            if (!(*it).MainNumber.isEmpty()) new ContactBoxItem( (*it).MainNumber, (*it).Id, listbox, (*it).getName().append(" (").append(i18n("Main")).append(')'));
            if (!(*it).WorkNumber.isEmpty()) new ContactBoxItem( (*it).WorkNumber, (*it).Id, listbox, (*it).getName().append(" (").append(i18n("Work")).append(')'));
            if (!(*it).FaxNumber.isEmpty()) new ContactBoxItem( (*it).FaxNumber, (*it).Id, listbox, (*it).getName().append(" (").append(i18n("Fax")).append(')'));
            if (!(*it).MobileNumber.isEmpty()) new ContactBoxItem( (*it).MobileNumber, (*it).Id, listbox, (*it).getName().append(" (").append(i18n("Mobile")).append(')'));
            if (!(*it).OtherNumber.isEmpty()) new ContactBoxItem( (*it).OtherNumber, (*it).Id, listbox, (*it).getName().append(" (").append(i18n("Other")).append(')'));
            if (!(*it).HomeNumber.isEmpty()) new ContactBoxItem( (*it).HomeNumber, (*it).Id, listbox, (*it).getName().append(" (").append(i18n("Home")).append(')'));
            if (!(*it).PagerNumber.isEmpty()) new ContactBoxItem( (*it).PagerNumber, (*it).Id, listbox, (*it).getName().append(" (").append(i18n("Pager")).append(')'));
        }
    }

    connect(listbox, SIGNAL(executed(QListBoxItem *)), this, SLOT(slotExecuted(QListBoxItem *)));
    mainLayout->addMultiCellWidget( listbox, 2,2, 0,3 );


    QHBoxLayout *layout = new QHBoxLayout();

    layout->setSpacing( 6 );
    layout->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout->addItem( spacer );

    QPushButton *buttonOK = new QPushButton(i18n("&OK"), this);
    buttonOK->setDefault(true);
    layout->addWidget(buttonOK);

    QPushButton *buttonCancel = new QPushButton(i18n("&Cancel"), this);
    layout->addWidget(buttonCancel);

    mainLayout->addMultiCellLayout( layout, 4,4, 0,3 );

    line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addMultiCellWidget( line, 3,3, 0,3 );
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
}

SelectContactDialog::~SelectContactDialog(){
}

void SelectContactDialog::slotOK() {
    if (type == Contact) {
        ContactID = ((ContactBoxItem *)listbox->item(listbox->currentItem()))->Contact;
    } else {
        numbers = "";
        for (unsigned int i = 0; i < listbox->count(); i++) {
            if (listbox->isSelected(i)){
                numbers.append(((ContactBoxItem *)listbox->item(i))->Number).append(' ');;
            }
        }
    }
    accept();
}

void SelectContactDialog::slotCancel() {
    reject();
}

void SelectContactDialog::slotExecuted(QListBoxItem *item) {
    if (type == Contact) {
        ContactID = ((ContactBoxItem *)item)->Contact;
    } else {
        numbers = ((ContactBoxItem *)item)->Number;
        numbers.append(" ");
    }
    accept();
}

ContactBoxItem::ContactBoxItem( QString num, int cont, QListBox * listbox, const QString & text=QString::null ) : QListBoxText (listbox, text){
    Number = num;
    Contact = cont;
}

ContactBoxItem::~ContactBoxItem() {
}
