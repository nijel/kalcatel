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

/* SIM phonebook */
#define PB_SIM "SM"
/* Own number phonebook */
#define PB_OWN "ON"
/* Last dialling phonebook */
#define PB_LAST_DIAL "LD"
/* Received calls phonebook */
#define PB_RECEIVED "RC"
/* Missed calls phonebook */
#define PB_MISSED "MC"

/** Structure used to save contacts
 */
struct ContactData {
    /** position
      */
    int pos;
    /** phone number
      */
    char *number;
    /** contact name
      */
    QString *name;
};

/** Reads contacts from current phonebook.
 */
struct ContactData *get_contacts(int from=1, int to=200);

/** Selects currect phonebook.
 */
int select_phonebook(char *pbtype);
#endif