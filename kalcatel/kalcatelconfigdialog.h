/*
 * kalcatelconfigdialog.h
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

#ifndef KALCATELCONFIGDIALOG_H
#define KALCATELCONFIGDIALOG_H

#include <kdialog.h>

/**Configuration dialog for KAlcatel application
  *@author Michal Cihar
  */

class KJanusWidget;
class QWidget;
class QFrame;
class QLineEdit;
class QComboBox;
class KURLRequester;

class KAlcatelConfigDialog : public KDialog  {
   Q_OBJECT
public: 
	KAlcatelConfigDialog(QWidget *parent=0, const char *name=0);
	~KAlcatelConfigDialog();
    int exec ();
public slots:
    void slotCancel();
    void slotOK();
private:
    KJanusWidget *janus;
    QFrame *mobilePage;
    QFrame *otherPage;
    KURLRequester *editDevice;
    QLineEdit *editLock, *editInit, *editPrefix;
    QComboBox *rateEdit, *debugEdit;
};

#endif
