/*
 * phonenumbervalidator.h
 *
 * phone number validation class
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

#ifndef PHONENUMBERVALIDATOR_H
#define PHONENUMBERVALIDATOR_H

#include <qwidget.h>
#include <qvalidator.h>

class QRegExp;

/**@ref QValidator descendant to validate phone number
  *@author Michal Cihar
  */

class PhoneNumberValidator : public QValidator  {
    Q_OBJECT
public: 
    PhoneNumberValidator(QWidget *parent=0, const char *name=0);
    PhoneNumberValidator(const bool mul, QWidget *parent=0, const char *name=0);
    PhoneNumberValidator(const bool paus, const bool mul, QWidget *parent=0, const char *name=0 );
    PhoneNumberValidator(const bool empt, const bool paus, const bool mul, QWidget *parent, const char *name );
    ~PhoneNumberValidator();

    bool allowMultiple() const;
    void setMultiple( const bool val);

    bool allowPause() const;
    void setPause( const bool val);

    bool allowEmpty() const;
    void setEmpty( const bool val);

    State validate ( QString &what, int &pos ) const;
    void fixup ( QString &what) const;
    static QRegExp phoneNumberPauseRegExp;
    static QRegExp phoneNumberRegExp;
private:
    bool multiple;
    bool pause;
    bool empty;
};

#endif
