/*
 * contacts.cpp
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/*
#include <ctype.h>
#include <time.h>
*/
#include <qstring.h>

#include "contacts.h"
#include "modem.h"
#include "mobile.h"
#include "common.h"
#include "logging.h"

ushort text2int(char *text) {
    int result=0;
    if (text[2]>57)
        result+=text[2]-55;
    else
        result+=text[2]-48;
    result=result<<4;
    if (text[3]>57)
        result+=text[3]-55;
    else
        result+=text[3]-48;
    result=result<<4;
    if (text[0]>57)
        result+=text[0]-55;
    else
        result+=text[0]-48;
    result=result<<4;
    if (text[1]>57)
        result+=text[1]-55;
    else
        result+=text[1]-48;
    return result;
}

struct ContactData *get_contacts(int from=1, int to=200) {
    char answer[20000], raw[200], number[100], *data;
    ushort *binary;
    int count=0, i, len;
    modem_cmd("AT+CSCS=\"UCS2\"\r\n", answer, sizeof(answer), 50, NULL);
    struct ContactData *cont;

    sprintf(raw, "AT+CPBR=%d,%d\r\n", from, to);
    modem_cmd(raw, answer, sizeof(answer), 500, NULL);

    data = answer;
    while( (data = strchr(data,'"')) != NULL) data[0] = ' ';

    data = answer;
    /* how many contacts are listed? */
    while( (data = strstr(data,"+CPBR:")) != NULL) {
        count++;
        data++;
    }

    message(MSG_INFO,"Read %d contacts", count);

    /* allocate array for storing messages */
    cont = (struct ContactData *)malloc((count + 1) * sizeof(struct ContactData));
    chk(cont);
    cont[count].pos = -1; /* end symbol */

    /* fill array */
    data = answer;
    count = 0;
    while( (data = strstr(data,"+CPBR:")) != NULL) {
        sscanf(data, "+CPBR: %d, %s ,%d, %s ", &(cont[count].pos), number, &i, raw);
        data++;

        len = strlen(raw);
        len >>= 2;

        chk(cont[count].number = strdup(number));
        chk(binary = (ushort *)malloc(len * sizeof(ushort)));

        for(i=0; i<len; i++) {
            binary[i] = text2int (raw + (4*i));
        }
        cont[count].name = new QString();
        cont[count].name->setUnicodeCodes(binary, len);
        count++;
    }

    modem_cmd("AT+CSCS=\"GSM\"\r\n", answer, sizeof(answer), 50, NULL);

    return cont;
}

int select_phonebook(char *pbtype) {
    char answer[100], cmd[100];

    sprintf(cmd, "AT+CPBS=\"%s\"\r\n", pbtype);
    modem_cmd(cmd, answer, sizeof(answer), 50, NULL);
	
	if (strstr(answer, "ERROR") != NULL) return 0;
	return 1;
}
