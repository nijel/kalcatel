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
 * if and only if programmer/distributor of that code receives written
 * permission from author of this code.
 *
 */
/* $Id$ */

#ifndef KALCATELCONFIGDIALOG_H
#define KALCATELCONFIGDIALOG_H

#include <kdialog.h>

class KJanusWidget;
class QWidget;
class QFrame;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QButtonGroup;
class KURLRequester;
class KIntNumInput;

/**Configuration dialog for KAlcatel application
  *@author Michal Cihar
  */
class KAlcatelConfigDialog : public KDialog  {
   Q_OBJECT
public:
    /** Creates dialog
      */
	KAlcatelConfigDialog(QWidget *parent=0, const char *name=0);
    /** Destroys dialog
      */
	~KAlcatelConfigDialog();
    /** Executes dialog and loads configuration from application
      */
    int exec ();
public slots:
    /** called when cancel pressed
      */
    void slotCancel();
    /** called when ok pressed, sets configuration in application
      */
    void slotOK();
private:
    /** widget with configuration tabs
      */
    KJanusWidget *janus;
    /** mobile configuration frame
      */
    QFrame *mobilePage;
    /** merging configuration frame
      */
    QFrame *mergePage;
    /** other configuration frame
      */
    QFrame *otherPage;
    QFrame *savePage;
    /** device edit
      */
    KURLRequester *editDevice;
    /** lock edit
      */
    QLineEdit *editLock;
    /** init string edit
      */
    QLineEdit *editInit;
    /** phone prefix edit
      */
    QLineEdit *editPrefix;
    /** baud rate edit
      */
    QComboBox *rateEdit;
    QCheckBox *editRTSCTS;
    /** debug level edit
      */
    QComboBox *debugEdit;
    /** auto open last file edit
      */
    QCheckBox *editAutoOpen;
    /** which field show as url edit
      */
    QComboBox *contactUrlEdit;
    /**
      */
    QButtonGroup *mergeBox;
    QButtonGroup *conflictBox;

    QCheckBox *editConnect;
    QCheckBox *editPersistent;
    QCheckBox *editReread;

    QCheckBox *editSaveTodos;
    QCheckBox *editLoadTodos;
    QCheckBox *editSaveEvents;
    QCheckBox *editLoadEvents;
    QCheckBox *editSaveContacts;
    QCheckBox *editLoadContacts;
    QCheckBox *editSaveMessages;
    QCheckBox *editLoadMessages;
    QCheckBox *editSaveCalls;
    QCheckBox *editLoadCalls;

    KIntNumInput *editMonitor;
};

#endif
