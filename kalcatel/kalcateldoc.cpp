/*****************************************************************************
 * 
 * kalcateldoc.cpp
 *
 * TODO: description
 *
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
 *****************************************************************************/
/* $Id$ */

#include <unistd.h>

// include files for Qt
#include <qdir.h>
#include <qwidget.h>
#include <qstringlist.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/job.h>
#include <kio/netaccess.h>

// application specific includes
#include "kalcateldoc.h"
#include "kalcatel.h"
#include "kalcatelview.h"

#include "alcatelclasses.h"

// alcatool includes
#include "alcatool/modem.h"
#include "alcatool/mobile_info.h"
#include "alcatool/logging.h"
#include "alcatool/version.h"
#include "alcatool/charset.h"
#include "alcatool/sms.h"
#include "alcatool/common.h"
#include "alcatool/pdu.h"
#include "alcatool/alcatel.h"

QList<KAlcatelView> *KAlcatelDoc::pViewList = 0L;

KAlcatelDoc::KAlcatelDoc(QWidget *parent, const char *name) : QObject(parent, name)
{
  if(!pViewList)
  {
    pViewList = new QList<KAlcatelView>();
  }

  pViewList->setAutoDelete(true);
}

KAlcatelDoc::~KAlcatelDoc()
{
}

void KAlcatelDoc::addView(KAlcatelView *view)
{
  pViewList->append(view);
}

void KAlcatelDoc::removeView(KAlcatelView *view)
{
  pViewList->remove(view);
}
void KAlcatelDoc::setURL(const KURL &url)
{
  doc_url=url;
}

const KURL& KAlcatelDoc::URL() const
{
  return doc_url;
}

void KAlcatelDoc::slotUpdateAllViews(KAlcatelView *sender)
{
  KAlcatelView *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
      if(w!=sender)
        w->repaint();
    }
  }

}

bool KAlcatelDoc::saveModified()
{
  bool completed=true;

  if(modified)
  {
    KAlcatelApp *win=(KAlcatelApp *) parent();
    int want_save = KMessageBox::warningYesNoCancel(win,
                                         i18n("The current file has been modified.\n"
                                              "Do you want to save it?"),
                                         i18n("Warning"));
    switch(want_save)
    {
      case KMessageBox::Yes:
           if (doc_url.fileName() == i18n("Untitled"))
           {
             win->slotFileSaveAs();
           }
           else
           {
             saveDocument(URL());
       	   };

       	   deleteContents();
           completed=true;
           break;

      case KMessageBox::No:
           setModified(false);
           deleteContents();
           completed=true;
           break;

      case KMessageBox::Cancel:
           completed=false;
           break;

      default:
           completed=false;
           break;
    }
  }

  return completed;
}

void KAlcatelDoc::closeDocument()
{
  deleteContents();
}

bool KAlcatelDoc::newDocument()
{
  /////////////////////////////////////////////////
  // TODO: Add your document initialization code here
  /////////////////////////////////////////////////
  modified=false;
  doc_url.setFileName(i18n("Untitled"));

  return true;
}

bool KAlcatelDoc::openDocument(const KURL& url, const char *format /*=0*/)
{
  QString tmpfile;
  KIO::NetAccess::download( url, tmpfile );
  /////////////////////////////////////////////////
  // TODO: Add your document opening code here
  /////////////////////////////////////////////////

  KIO::NetAccess::removeTempFile( tmpfile );

  modified=false;
  return true;
}

bool KAlcatelDoc::readMobile(AlcReadType what = alcatel_read_all, int category = -1)
{
/* TODO: this should be in configuration */
char default_device[] = "/dev/ttyS1";
char default_lock[] = "/var/lock/LCK..%s";
char default_init[] = "AT S7=45 S0=0 L1 V1 X4 &c1 E1 Q0";
int default_rate = 19200;
char *devname;

  KAlcatelApp *win=(KAlcatelApp *) parent();

//  win->slotStatusMsg(i18n("Reading data from mobile"),ID_STATUS_MSG);

  msg_level = MSG_DEBUG;

  strcpy(initstring, default_init);
  strcpy(device, default_device);
  devname = strrchr(device, '/');
  devname++;
  sprintf(lockname, default_lock, devname);
  rate=default_rate;

  switch (rate) {
      case 2400:   baudrate=B2400; break;
      case 4800:   baudrate=B4800; break;
      case 9600:   baudrate=B9600; break;
      case 19200:  baudrate=B19200; break;
      case 38400:  baudrate=B38400; break;
      default:
          message(MSG_ERROR,"Ivalid baud rate (%d)!", rate);
          baudrate=B19200;
  }


  if (what == alcatel_read_sms)
  {
    win->slotStatusMsg(i18n("Opening modem"),ID_DETAIL_MSG);
    if (!modem_open()) {
        switch (modem_errno) {
            case ERR_MDM_OPEN:
                KMessageBox::error(win, i18n("Failed opening modem for read/write."), i18n("Error"));
                break;
            case ERR_MDM_LOCK:
                KMessageBox::error(win, i18n("Modem locked."), i18n("Error"));
                break;
            default:
                KMessageBox::error(win, i18n("Failed opening modem.\nUnknown error."), i18n("Error"));
        }
        return false;
    }
    win->slotStatusMsg(i18n("Setting serial port"),ID_DETAIL_MSG);
    modem_setup();
    win->slotStatusMsg(i18n("Initializing modem"),ID_DETAIL_MSG);
    if (!modem_init()) {
        switch (modem_errno) {
            case ERR_MDM_AT:
                KMessageBox::error(win, i18n("Modem doesn't react on AT command."), i18n("Error"));
                break;
            case ERR_MDM_PDU:
                KMessageBox::error(win, i18n("Failed selecting PDU mode."), i18n("Error"));
                break;
            default:
                KMessageBox::error(win, i18n("Failed initializing modem.\nUnknown error."), i18n("Error"));
        }
        return false;
    }

    win->slotStatusMsg(i18n("Reading messages"),ID_DETAIL_MSG);
    get_smss();

    modem_close();
    win->slotStatusMsg(i18n("SMS messages read"),ID_DETAIL_MSG);
  }
  else
  {
    KMessageBox::sorry(win, i18n("Not implemented yet..."), i18n("Sorry"));
  }
  return true;
}

bool KAlcatelDoc::saveDocument(const KURL& url, const char *format /*=0*/)
{
  /////////////////////////////////////////////////
  // TODO: Add your document saving code here
  /////////////////////////////////////////////////

  modified=false;
  return true;
}

void KAlcatelDoc::deleteContents()
{
  /////////////////////////////////////////////////
  // TODO: Add implementation to delete the document contents
  /////////////////////////////////////////////////

}
