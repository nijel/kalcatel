/*
 * timevalidator.cpp
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

#include "timevalidator.h"

TimeValidator::TimeValidator(QWidget *parent, const char *name ) : QValidator(parent,name) {
}

TimeValidator::~TimeValidator(){
}

TimeValidator::State TimeValidator::validate ( QString &what, int &pos ) const {
    int len = what.length();
    if (what.isEmpty()) return Intermediate;
    if (len>8) return Invalid;
    if (!what[0].isNumber()) return Invalid;
    if (len == 1) return Acceptable; /* 2 */
    if (what[0].isNumber() && what[1].isNumber()) {
        if (what[0].digitValue() * 10 + what[1].digitValue() >= 24) return Invalid;
        if (len == 2) return Acceptable; /* 22 */
        if (what[2] == ':' || what[2] == '.') {
            if (len == 3) return Acceptable; /* 22[:.] */
            if (what[3].isNumber() && len == 4) return Acceptable; /* 22[:.]2 */
            if (what[3].isNumber() && what[4].isNumber()) {
                if (what[3].digitValue() * 10 + what[4].digitValue() >= 60) return Invalid;
                if (len == 5) return Acceptable; /* 22[:.]22 */
                if (what[5] == ':' || what[5] == '.') {
                    if (len == 6) return Acceptable; /* 22[:.]22[:.] */
                    if (what[6].isNumber() && len == 7) return Acceptable; /* 22[:.]22[:.]2 */
                    if (what[6].isNumber() && what[7].isNumber() && (what[6].digitValue() * 10 + what[7].digitValue() < 60) && len == 8) return Acceptable; /* 22[:.]22[:.]22 */
                }
            }
            if (what[3].isNumber() && (what[4] == ':' || what[4] == '.')) {
                if (len == 5) return Acceptable; /* 22[:.]2[:.] */
                if (what[5].isNumber() && len == 6) return Acceptable; /* 22[:.]2[:.]2 */
                if (what[5].isNumber() && what[6].isNumber() && (what[5].digitValue() * 10 + what[6].digitValue() < 60) && len == 7) return Acceptable; /* 22[:.]2[:.]22 */
            }
        }
    } else if (what[0].isNumber() && ( what[1] == ':' || what[1] == '.' )) {
        if (len == 2) return Acceptable; /* 2[:.] */
        if (!what[2].isNumber()) return Invalid;
        if (len == 3) return Acceptable; /* 2[:.]2 */
        if (what[2].isNumber() && what[3].isNumber()) {
            if (what[3].digitValue() * 10 + what[4].digitValue() >= 60) return Invalid;
            if (len == 4) return Acceptable; /* 2[:.]22 */
            if (what[4] == ':' || what[4] == '.') {
                if (len == 5) return Acceptable; /* 2[:.]22[:.] */
                if (what[5].isNumber() && len == 6) return Acceptable; /* 2[:.]22[:.]2 */
                if (what[5].isNumber() && what[6].isNumber() && (what[5].digitValue() * 10 + what[6].digitValue() < 60)  && len == 7) return Acceptable; /* 2[:.]22[:.]22 */
            }
        }
        if (what[2].isNumber() && (what[3] == ':' || what[3] == '.')) {
            if (len == 4) return Acceptable; /* 2[:.]2[:.] */
            if (what[4].isNumber() && len == 5) return Acceptable; /* 2[:.]2[:.]2 */
            if (what[4].isNumber() && what[5].isNumber() && (what[4].digitValue() * 10 + what[5].digitValue() < 60) && len == 6) return Acceptable; /* 2[:.]2[:.]22 */
        }
    }
    return Invalid;
}

void TimeValidator::fixup ( QString &what) const {
    int pos;
    if (validate(what,pos) != Acceptable) what = "";
}

