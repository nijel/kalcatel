/*
 * 
 * alcatelclasses.cpp
 *
 * classes to store data from mobile
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

#include <stdlib.h>

#include "alcatelclasses.h"

/*
AlcatelClasses::AlcatelClasses(){
}
AlcatelClasses::~AlcatelClasses(){
}
*/

AlcatelContact::AlcatelContact(){
    Category = -1;
    Private = -1;
}

AlcatelContact::~AlcatelContact(){
}

AlcatelCalendar::AlcatelCalendar(){
    Private = -1;
    EventType = -1;
    ContactID = -1;

    DayOfWeek = -1;
    Day = -1;
    WeekOfMonth = -1;
    Month = -1;
    Frequency = -1;
}

AlcatelCalendar::~AlcatelCalendar(){
}

AlcatelTodo::AlcatelTodo(){
    Completed = -1;
    Private = -1;
    Category = -1;
    Priority = -1;
    ContactID = -1;
}

AlcatelTodo::~AlcatelTodo(){
}

AlcatelSMS::AlcatelSMS(){
    Position = -1;
    Status = -1;
    Length = -1;
    Raw = NULL;
}

AlcatelSMS::~AlcatelSMS(){
    free( Raw );
}

