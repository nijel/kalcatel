/*
 * kalcatelmergedialog.h
 *
 * dialog for merging when conflict occurs
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

#ifndef KALCATELMERGEDIALOG_H
#define KALCATELMERGEDIALOG_H

#include <qwidget.h>
#include <kdialog.h>
#include <qvaluelist.h> 

class AlcatelMessage;
class AlcatelContact;
class AlcatelCalendar;
class AlcatelTodo;
class AlcatelCategory;
class AlcatelCall;
class AlcatelClass;

class QPushButton;
class QRadioButton;
class QGrid;
class QLabel;

class QDate;
class QDateTime;
class QTime;

class KAlcatelMergeDialog;

enum MergeStatus {
    Mobile,
    Delete,
    PC
    };

class MergeItem : public QObject {
   Q_OBJECT
   friend class KAlcatelMergeDialog;
public:
    MergeItem(QWidget *parent, QString name, QString mobileText, QString pcText, bool disableDelete = false);
    MergeItem(QWidget *parent, QString name, QDate mobileData, QDate pcData, bool disableDelete = false);
    MergeItem(QWidget *parent, QString name, QTime mobileData, QTime pcData, bool disableDelete = false);
    MergeItem(QWidget *parent, QString name, QDateTime mobileData, QDateTime pcData, bool disableDelete = false);
    MergeItem(QWidget *parent, QString name, int mobileData, int pcData, bool disableDelete = false);
    MergeItem(QWidget *parent, QString name, int mobileData, int pcData, int, bool disableDelete = false);
    MergeItem(const MergeItem  &item);
    MergeItem();
	~MergeItem(void);
	MergeStatus getStatus(void);
	void setStatus(MergeStatus status);
public slots:
    void pcToggle(bool state);
    void deleteToggle(bool state);
    void mobileToggle(bool state);
private:
    void MergeItem_init(QWidget *parent, QString name, QString mobileText, QString pcText, bool disableDelete = false);
    QRadioButton *mobileCheck, *deleteCheck, *pcCheck;
    QLabel *mobileLabel, *pcLabel, *nameLabel;
};


typedef QValueList<MergeItem> MergeItemList;


/**dialog to show conflict during merge and allow user to select which fields takes precendence
  *@author Michal Cihar
  */

class KAlcatelMergeDialog : public KDialog  {
   Q_OBJECT
public:
	KAlcatelMergeDialog(QWidget *parent=0, const char *name=0);
	~KAlcatelMergeDialog();

    AlcatelContact *exec(AlcatelContact &c1, AlcatelContact &c2);
    AlcatelMessage *exec(AlcatelMessage &c1, AlcatelMessage &c2);
    AlcatelCalendar *exec(AlcatelCalendar &c1, AlcatelCalendar &c2);
    AlcatelTodo *exec(AlcatelTodo &c1, AlcatelTodo &c2);
    AlcatelCall *exec(AlcatelCall &c1, AlcatelCall &c2);
    AlcatelCategory *exec(AlcatelCategory &c1, AlcatelCategory &c2);
public slots:
    /** called when cancel pressed
      */
    void slotBoth();
    /** called when ok pressed
      */
    void slotOK();

    void slotClear();
    void slotPC(void);
    void slotDelete(void);
    void slotMobile(void);
private:
    int exec ();

    QGrid *conflictGrid;

    QLabel *titleLabel;

    QPushButton *buttonOK;
    QPushButton *buttonBoth;

    QPushButton *mobileButton;
    QPushButton *deleteButton;
    QPushButton *pcButton;

    AlcatelClass *result;
    AlcatelClass *data1;
    AlcatelClass *data2;

    MergeItemList itemList;
};

#endif
