/*
 * kalcatelviewitem.h
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

#ifndef KALCATELVIEWITEM_H
#define KALCATELVIEWITEM_H

#include <qlistview.h>

class AlcatelClass;
class AlcatelMessage;
class AlcatelCall;

/**KAlcatel generic view item
  *@author Michal Cihar
  */

class KAlcatelDataItem : public QListViewItem  {
public:
    KAlcatelDataItem ( QListView * parent, AlcatelClass *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
    AlcatelClass *alcatelData;
};

class KAlcatelMessageViewItem : public KAlcatelDataItem  {
public:
    KAlcatelMessageViewItem ( QListView * parent, AlcatelMessage *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
    QString key ( int column, bool ascending ) const;
};

class KAlcatelMessageCatViewItem : public KAlcatelDataItem  {
public:
    KAlcatelMessageCatViewItem ( QListView * parent, AlcatelMessage *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
    QString key ( int column, bool ascending ) const;
};

class KAlcatelCallViewItem : public KAlcatelDataItem  {
public:
    KAlcatelCallViewItem ( QListView * parent, AlcatelCall *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
    QString key ( int column, bool ascending ) const;
};

class KAlcatelCallCatViewItem : public KAlcatelDataItem  {
public:
    KAlcatelCallCatViewItem ( QListView * parent, AlcatelCall *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
    QString key ( int column, bool ascending ) const;
};

#endif
