/*
 * kalcateltreeviewitem.h
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

#ifndef KALCATELTREEVIEWITEM_H
#define KALCATELTREEVIEWITEM_H

#include <qlistview.h>

/**List view item with added field for widget which to show when selected.
  *@author Michal Cihar
  */

class QPixmap;

class KAlcatelTreeViewItem : public QListViewItem  {
public:
    KAlcatelTreeViewItem( QListView *parent, const char *name );
    KAlcatelTreeViewItem ( QListView * parent, QString text, QPixmap icon, int widget );
    KAlcatelTreeViewItem ( QListViewItem * parent, QString text, QPixmap icon, int widget );

    ~KAlcatelTreeViewItem();
    QString key ( int column, bool ascending ) const;
    int showWidget;
};

#endif
