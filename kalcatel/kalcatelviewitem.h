/*
 * kalcatelviewitem.h
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

#ifndef KALCATELVIEWITEM_H
#define KALCATELVIEWITEM_H

#include <qlistview.h>

class AlcatelClass;
class AlcatelMessage;
class AlcatelCall;
class AlcatelTodo;
class AlcatelContact;
class AlcatelCalendar;

/**KAlcatel generic view item
  *@author Michal Cihar
  */

class KAlcatelDataItem : public QListViewItem  {
public:
    KAlcatelDataItem ( QListView * parent, AlcatelClass *data );
    AlcatelClass *alcatelData;
    virtual void update() = 0;
    virtual void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment );
};

class KAlcatelMessageViewItem : public KAlcatelDataItem  {
public:
    KAlcatelMessageViewItem ( QListView * parent, AlcatelMessage *data, AlcatelContact *cont);
    QString key ( int column, bool ascending ) const;
    void update();
    AlcatelContact *contact;
};

class KAlcatelMessageCatViewItem : public KAlcatelDataItem  {
public:
    KAlcatelMessageCatViewItem ( QListView * parent, AlcatelMessage *data, AlcatelContact *cont);
    QString key ( int column, bool ascending ) const;
    void update();
    AlcatelContact *contact;
};

class KAlcatelCallViewItem : public KAlcatelDataItem  {
public:
    KAlcatelCallViewItem ( QListView * parent, AlcatelCall *data, AlcatelContact *cont);
    QString key ( int column, bool ascending ) const;
    void update();
    AlcatelContact *contact;
};

class KAlcatelCallCatViewItem : public KAlcatelDataItem  {
public:
    KAlcatelCallCatViewItem ( QListView * parent, AlcatelCall *data, AlcatelContact *cont);
    QString key ( int column, bool ascending ) const;
    void update();
    AlcatelContact *contact;
};

class KAlcatelTodoViewItem : public KAlcatelDataItem  {
public:
    KAlcatelTodoViewItem ( QListView * parent, AlcatelTodo *data, AlcatelContact *cont, QString catname);
    QString key ( int column, bool ascending ) const;
    void update();
    AlcatelContact *contact;
    QString categoryname;
};

class KAlcatelTodoCatViewItem : public KAlcatelDataItem  {
public:
    KAlcatelTodoCatViewItem ( QListView * parent, AlcatelTodo *data, AlcatelContact *cont);
    QString key ( int column, bool ascending ) const;
    void update();
    AlcatelContact *contact;
};

class KAlcatelContactViewItem : public KAlcatelDataItem  {
public:
    KAlcatelContactViewItem ( QListView * parent, AlcatelContact *data);
    QString key ( int column, bool ascending ) const;
    void update();
};

class KAlcatelContactMobileViewItem : public KAlcatelDataItem  {
public:
    KAlcatelContactMobileViewItem ( QListView * parent, AlcatelContact *data, QString catname);
    QString key ( int column, bool ascending ) const;
    void update();
    QString categoryname;
};

class KAlcatelContactMobileCatViewItem : public KAlcatelDataItem  {
public:
    KAlcatelContactMobileCatViewItem ( QListView * parent, AlcatelContact *data);
    QString key ( int column, bool ascending ) const;
    void update();
};

class KAlcatelContactSIMViewItem : public KAlcatelDataItem  {
public:
    KAlcatelContactSIMViewItem ( QListView * parent, AlcatelContact *data);
    QString key ( int column, bool ascending ) const;
    void update();
};

class KAlcatelCalendarViewItem : public KAlcatelDataItem  {
public:
    KAlcatelCalendarViewItem ( QListView * parent, AlcatelCalendar *data, AlcatelContact *cont);
    QString key ( int column, bool ascending ) const;
    void update();
    AlcatelContact *contact;
};

#endif
