/*
 * signalled.h
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

#ifndef SIGNALLED_H
#define SIGNALLED_H

#include <qwidget.h>
#include <kled.h>

/**Same as @ref KLed, just emmits some signal
  *@author Michal Cihar
  */

class SignalLed : public KLed  {
   Q_OBJECT
public:
/** Creates led
 */
    SignalLed (const QColor& col, KLed::State st, KLed::Look look, KLed::Shape shape, QWidget *parent=0, const char *name=0);
signals:
/** Emmited when label is clicked
 */
    void clicked();
protected:
/** Reimplemented to allow signal emmiting
 */
    void mousePressEvent(QMouseEvent *ev);
};

#endif
