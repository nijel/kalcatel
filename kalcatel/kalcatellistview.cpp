/*
 * kalcatellistview.cpp
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

#include "kalcatellistview.h"

KAlcatelListView::KAlcatelListView(QWidget *parent, const char *name ) : KListView(parent,name) {
}

KAlcatelListView::~KAlcatelListView(){
}

void  KAlcatelListView::keyPressEvent (QKeyEvent *event) {
    ButtonState stat = event->state();
    if (event->key() == Key_Delete && ( stat == NoButton )) {
        deletePressed(currentItem());
        event->accept ();
    } else if (event->key() == Key_Delete && ( stat == ShiftButton )) {
        shiftDeletePressed(currentItem());
        event->accept ();
    } else {
        KListView::keyPressEvent(event);
    }
}
