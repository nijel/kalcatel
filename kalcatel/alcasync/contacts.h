/*
 * contacts.h
 *
 * reading of contacts and calls using AT commands
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
#ifndef CONTACTS_H
#define CONTACTS_H

class QString;

#define PB_SIM "SM"
#define PB_OWN "ON"

#define PB_LAST_DIAL "LD"
#define PB_RECEIVED "RC"
#define PB_MISSED "MC"

typedef struct {
    int pos;
    char *number;
    QString *name;
} CONTACT;

CONTACT *get_contacts(int from=1, int to=200);
int select_phonebook(char *pbtype);
#endif