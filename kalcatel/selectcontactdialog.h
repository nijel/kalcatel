/*
 * selectcontactdialog.h
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

#ifndef SELECTCONTACTDIALOG_H
#define SELECTCONTACTDIALOG_H

#include <qwidget.h>
#include <qlistbox.h>

#include <kdialog.h>

#include "alcatelclasses.h"

class KListBox;

/**Dialog for selecting contact or phone number from contacts
  *@author Michal Cihar
  */

class SelectContactDialog : public KDialog  {
   Q_OBJECT
public:
    enum SelectType { Contact, Numbers };
    SelectContactDialog(SelectType type, AlcatelStorage st, AlcatelStorage pst, AlcatelContactList *conts, QWidget *parent=0, const char *name=0);
    ~SelectContactDialog();
    int getContactID() {return ContactID;}
    QString getNumbers() {return numbers;}
protected:
    AlcatelContactList *list;
    KListBox *listbox;
    SelectType type;
    int ContactID;
    AlcatelStorage storage;
    AlcatelStorage prev_storage;
    QString numbers;
private slots:
    void slotOK();
    void slotCancel();
    void slotDoubleClicked(QListBoxItem *item, const QPoint &pos);
};

/**Class for ListBoxItem with additional data
  */
class ContactBoxItem : public QListBoxText {
public:
    ContactBoxItem( QString num, int cont, QListBox *listbox, const QString & text=QString::null );
    ~ContactBoxItem();
    int Contact;
    QString Number;
};

#endif
