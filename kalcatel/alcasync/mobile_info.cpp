/*
 * alcasync/mobile_info.cpp
 *
 * acquire information about mobile
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
#include <stdio.h>
#include <string.h>

#include "mobile_info.h"
#include "modem.h"
#include "logging.h"
#include "version.h"

/* TODO:
 * clock:
 *<at+cclk?
 *>+CCLK: "02/01/30,13:58:33"
 *
 *<at+cclk="02/01/30,14:02:40+00"
 *>OK
 *
 * alarm:
 *  - text = max 32 chars
 *  - at once can be active only one
 *
 *  - this is probably broken in current version, use binary protocol instead
 *
 *<at+cala="02/01/30,14:10:00+00",1,0,"hokus pokus"
 *>OK
 *
 *<at+cala?
 *>+CALA: "210/01/30,14:06:00",2,0,"test2"
 *>+CALA: "210/01/30,14:06:00",3,0,"test3"
 *>+CALA: "210/01/30,14:06:00",5,0,"test5"
 *>+CALA: "210/01/30,14:10:00",1,0,"hokus pokus"
 *>+CALA: "210/01/30,14:06:00",4,0,"test4"
 *>
 *>
 *>OK
 *
 *<at+cala=?
 *>+CALA: (1-5), (0), 241
 *
 * 
 */

char mobil_signal_info[][10] = {
    "<-113 dBm", 
    "-111 dBm", 
    "-109 dBm",
    "-107 dBm",
    "-105 dBm",
    "-103 dBm",
    "-101 dBm",
    "-99 dBm",
    "-97 dBm",
    "-95 dBm",
    "-93 dBm",
    "-91 dBm",
    "-89 dBm",
    "-87 dBm",
    "-85 dBm",
    "-83 dBm",
    "-81 dBm",
    "-79 dBm",
    "-77 dBm",
    "-75 dBm",
    "-73 dBm",
    "-71 dBm",
    "-69 dBm",
    "-67 dBm",
    "-65 dBm",
    "-63 dBm",
    "-61 dBm",
    "-59 dBm",
    "-57 dBm",
    "-55 dBm",
    "-53 dBm",
    ">-51 dBm"
};


int get_battery(int *mode, int *charge){
    char buffer[1000], *pos;
    modem_cmd("AT+CBC\r\n",buffer,sizeof(buffer)-1,50,0);
    
    pos = strstr(buffer, "+CBC:");
    if (pos == NULL) {
        message(MSG_WARNING,"Could not read battery status! (received: \"%s\")",reform(buffer,1));
        *mode = -1;
        *charge = -1;
    } else {
        sscanf(pos,"+CBC: %d, %d",mode,charge);
    }
    return *charge;
}

int get_signal(int *rssi, int *ber){
    char buffer[1000], *pos;
    modem_cmd("AT+CSQ\r\n",buffer,sizeof(buffer)-1,50,0);

    pos = strstr(buffer, "+CSQ:");
    if (pos == NULL) {
        message(MSG_WARNING,"Could not read signal quality! (received: \"%s\")",reform(buffer,1));
        *rssi = -1;
        *ber = -1;
    } else {
        sscanf(pos,"+CSQ: %d, %d",rssi,ber);
    }

    return *rssi;
}

void get_sn(char *sn,int len){
    get_string("AT+CGSN\r\n",sn,len);
}

void get_revision(char *rev,int len){
    get_string("AT+CGMR\r\n",rev,len);
}

void get_model(char *model,int len){
    get_string("AT+CGMM\r\n",model,len);
}

void get_manufacturer(char *manuf,int len){
    get_string("AT+CGMI\r\n",manuf,len);
}

void get_imsi(char *imsi,int len){
    char *pos;
    get_string("AT+CIMI\r\n", imsi, len);
    pos = strchr(imsi, ':');
    if (pos)
        memmove(imsi, pos+1, strlen(pos));
}

void get_string(const char *cmd, char *data, int len){
    char buffer[1000];
    char *loc1,*loc2;
    modem_cmd(cmd,buffer,sizeof(buffer)-1,100,0);
    if (strlen(buffer) < strlen(cmd) + 1) {
        message(MSG_WARNING,"Could not parse string from output! (received: \"%s\")",reform(buffer,1));
        data[0]='\0';
        return;
    }
    loc1 = buffer + strlen(cmd);
    while (*loc1 == '\n' || *loc1 == '\r') loc1++;
    
    loc2 = strchr(loc1,'\r');
    if (loc2 == NULL) {
        message(MSG_WARNING,"Could not parse string from output! (received: \"%s\")",reform(buffer,1));
        data[0]='\0';
        return;
    }
    loc2[0] = '\0';
    strncpy(data,loc1,len-1);
    message(MSG_DEBUG,"Parsed string (len=%d): %s", len, reform(data,1));
}
