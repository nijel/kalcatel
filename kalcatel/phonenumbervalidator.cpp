/*
 * phonenumbervalidator.cpp
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

#include "phonenumbervalidator.h"
#include <qregexp.h>
#include <qstringlist.h>

/* TODO: shouldn't phone number have more numbers than one? */
QRegExp PhoneNumberValidator::phoneNumberPauseRegExp = QRegExp("^\\+?[pP0-9]+$");
QRegExp PhoneNumberValidator::phoneNumberRegExp = QRegExp("^\\+?[0-9]+$");

PhoneNumberValidator::PhoneNumberValidator(QWidget *parent, const char *name ) : QValidator(parent,name), multiple(false), pause(true), empty(true) {
}
PhoneNumberValidator::PhoneNumberValidator(const bool mul, QWidget *parent, const char *name ) : QValidator(parent,name), multiple(mul), pause(true), empty(true) {
}
PhoneNumberValidator::PhoneNumberValidator(const bool paus, const bool mul, QWidget *parent, const char *name ) : QValidator(parent,name), multiple(mul), pause(paus), empty(true) {
}
PhoneNumberValidator::PhoneNumberValidator(const bool empt, const bool paus, const bool mul, QWidget *parent, const char *name ) : QValidator(parent,name), multiple(mul), pause(paus), empty(empt) {
}
PhoneNumberValidator::~PhoneNumberValidator(){
}

bool PhoneNumberValidator::allowMultiple() const { return multiple; }
void PhoneNumberValidator::setMultiple( const bool val) { multiple = val; }

bool PhoneNumberValidator::allowPause() const { return pause; }
void PhoneNumberValidator::setPause( const bool val) { pause = val; }

bool PhoneNumberValidator::allowEmpty() const { return empty; }
void PhoneNumberValidator::setEmpty( const bool val) { empty = val; }

PhoneNumberValidator::State PhoneNumberValidator::validate ( QString &what, int &pos ) const {
    if (what.isEmpty()) {
        if (empty) return Acceptable;
        else return Intermediate;
    }
    if (multiple) {
        bool wasEmpty = true;
        QStringList list = QStringList::split(QRegExp("[; ,]"), what, true);
        if (list.count() == 0) return Acceptable;
        if (pause) {
            for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
                if ((*it).isEmpty()) {
                    if (wasEmpty) return Invalid;
                    wasEmpty = true;
                } else {
                    wasEmpty = false;
                }
                if (*it == "+") return Intermediate;
                if (phoneNumberPauseRegExp.find(*it, 0) == -1) return Invalid;
            }
        } else {
            for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
                if ((*it).isEmpty()) {
                    if (wasEmpty) return Invalid;
                    wasEmpty = true;
                } else {
                    wasEmpty = false;
                }
                if (*it == "+") return Intermediate;
                if (phoneNumberRegExp.find(*it, 0) == -1) return Invalid;
            }
        }
        return Acceptable;
    } else {
        if (pause) {
            if (what == "+") return Intermediate;
            if (phoneNumberPauseRegExp.find(what, 0) == -1) return Invalid;
            return Acceptable;
        } else {
            if (what == "+") return Intermediate;
            if (phoneNumberRegExp.find(what, 0) == -1) return Invalid;
            return Acceptable;
        }
    }
}

void PhoneNumberValidator::fixup ( QString &what) const {
    int pos;
    if (validate(what, pos) != Acceptable ) what = ""; /* TODO: this is just simple, should be more comfort for multiple numbers */
}
