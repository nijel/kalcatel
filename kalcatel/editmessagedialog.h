/*
 * editmessagedialog.h
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

#ifndef EDITMESSAGEDIALOG_H
#define EDITMESSAGEDIALOG_H

#include <kdialog.h>
#include "alcatelclasses.h"


class QCheckBox;
class QComboBox;
class KComboBox;
class QMultiLineEdit;
class QLabel;

/**Dialog for editing message
  *@author Michal Cihar
  */

class EditMessageDialog : public KDialog  {
   Q_OBJECT
public: 
    EditMessageDialog(AlcatelContactList *cont, QWidget *parent=0, const char *name=0);
    ~EditMessageDialog();
    bool reread;
private slots:
    /** called when cancel pressed
      */
    void slotCancel();
    /** called when ok pressed, writes message
      */
    void slotOK();

    void slotWriteChanged(bool on);
    void slotSendChanged(bool on);
    void slotTextChanged();
    void selectContacts();
private:
    QCheckBox *sendCheck;
    QCheckBox *writeCheck;
    QComboBox *typeCombo;
    KComboBox *sendToCombo;
    QMultiLineEdit *messageEdit;
    QLabel *msgInfoLabel;
    QComboBox *classCombo;
    AlcatelContactList *contacts;
};

#endif
