/*
 * timevalidator.h
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

#ifndef TIMEVALIDATOR_H
#define TIMEVALIDATOR_H

#include <qwidget.h>
#include <qvalidator.h>

/**Validates time input
  *@author Michal Cihar
  */

class TimeValidator : public QValidator  {
   Q_OBJECT
public: 
    TimeValidator(QWidget *parent=0, const char *name=0);
    ~TimeValidator();
    State validate ( QString &what, int &pos ) const;
    void fixup ( QString &what) const;
};

#endif
