/*
 * editcontactdialog.h
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

#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include <qwidget.h>
#include <kdialog.h>
#include "alcatelclasses.h"

/**dialog for editing contact
  *@author Michal Cihar
  */

class EditContactDialog : public KDialog  {
    Q_OBJECT
public: 
    EditContactDialog(AlcatelContactList *lst, const AlcatelContact *cont, QWidget *parent=0, const char *name=0);
    ~EditContactDialog();
public slots:
    void slotOK();
    void slotCancel();
private:
    const AlcatelContact *contact;
    AlcatelContactList *list;
};

#endif
