/*
 * kalcatelviewitem.cpp
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

#include <qdatetime.h>

#include "kalcatelviewitem.h"
#include "alcatelclasses.h"
#include "alcatool/logging.h"

KAlcatelDataItem::KAlcatelDataItem ( QListView * parent, AlcatelClass *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null ) : QListViewItem ( parent, label1, label2 , label3 , label4 , label5 , label6 , label7 , label8 ) {
    alcatelData = data;
}

KAlcatelMessageViewItem::KAlcatelMessageViewItem ( QListView * parent, AlcatelMessage *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null ) : KAlcatelDataItem ( parent, data, label1, label2 , label3 , label4 , label5 , label6 , label7 , label8 ) {
}

QString KAlcatelMessageViewItem::key ( int column, bool ascending ) const {
    if (column == 3) {
        QString tmpString;
        QDateTime epoch( QDate(1970,1,1), QTime(0,0) );
        tmpString.sprintf( "%08d", epoch.secsTo(((AlcatelMessage *)alcatelData)->Date) );
        return tmpString;
    } else if (column == 4) {
        QString tmpString;
        QTime epoch( 0, 0 );
        tmpString.sprintf( "%04d", epoch.secsTo(((AlcatelMessage *)alcatelData)->Date.time()) );
        return tmpString;
    } else if (column == 6) {
        QString tmpString;
        tmpString.sprintf( "%04d", alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelMessageCatViewItem::KAlcatelMessageCatViewItem ( QListView * parent, AlcatelMessage *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null ) : KAlcatelDataItem ( parent, data, label1, label2 , label3 , label4 , label5 , label6 , label7 , label8 ) {
}

QString KAlcatelMessageCatViewItem::key ( int column, bool ascending ) const {
    if (column == 2) {
        QString tmpString;
        QDateTime epoch( QDate(1970,1,1), QTime(0,0) );
        tmpString.sprintf( "%08d", epoch.secsTo(((AlcatelMessage *)alcatelData)->Date) );
        return tmpString;
    } else if (column == 3) {
        QString tmpString;
        QTime epoch( 0, 0 );
        tmpString.sprintf( "%04d", epoch.secsTo(((AlcatelMessage *)alcatelData)->Date.time()) );
        return tmpString;
    } else if (column == 5) {
        QString tmpString;
        tmpString.sprintf( "%04d", alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelCallViewItem::KAlcatelCallViewItem ( QListView * parent, AlcatelCall *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null ) : KAlcatelDataItem ( parent, data, label1, label2 , label3 , label4 , label5 , label6 , label7 , label8 ) {
}

QString KAlcatelCallViewItem::key ( int column, bool ascending ) const {
    if (column == 3) {
        QString tmpString;
        tmpString.sprintf( "%04d", alcatelData->Id );
        tmpString.prepend(text(column));
        return tmpString;
    } else if (column == 4) {
        QString tmpString;
        tmpString.sprintf( "%04d", alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

KAlcatelCallCatViewItem::KAlcatelCallCatViewItem ( QListView * parent, AlcatelCall *data, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null ) : KAlcatelDataItem ( parent, data, label1, label2 , label3 , label4 , label5 , label6 , label7 , label8 ) {
}

QString KAlcatelCallCatViewItem::key ( int column, bool ascending ) const {
    if (column == 3) {
        QString tmpString;
        tmpString.sprintf( "%04d", alcatelData->Id );
        return tmpString;
    } else {
        return text(column);
    }
}

