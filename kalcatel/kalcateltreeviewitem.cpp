/*
 * kalcateltreeviewitem.cpp
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

#include <qpixmap.h>

#include "kalcateltreeviewitem.h"

KAlcatelTreeViewItem::KAlcatelTreeViewItem( QListView *parent, const char *name ) : QListViewItem(parent,name) {
}

KAlcatelTreeViewItem::KAlcatelTreeViewItem ( QListView * parent, QString text, QPixmap icon, int widget  ) : QListViewItem(parent,text) {
    showWidget = widget;
    setPixmap(0,icon);
    setOpen(true);
}

KAlcatelTreeViewItem::KAlcatelTreeViewItem ( QListViewItem * parent, QString text, QPixmap icon, int widget  ) : QListViewItem(parent,text) {
    showWidget = widget;
    setPixmap(0,icon);
    setOpen(true);
}

KAlcatelTreeViewItem::~KAlcatelTreeViewItem(){
}
