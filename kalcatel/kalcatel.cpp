/*
 * kalcatel.cpp
 *
 * main window code
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
#include <unistd.h>

// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>

// include files for KDE
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kiconloader.h>
#include <kio/netaccess.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>

// application specific includes
#include "kalcatel.h"
#include "kalcatelview.h"
#include "kalcateldoc.h"
#include "kalcatelconfigdialog.h"

#include "alcatool/modem.h"
#include "alcatool/mobile_info.h"
#include "alcatool/sms.h"
#include "alcatool/contacts.h"
#include "alcatool/logging.h"

KAlcatelApp::KAlcatelApp(QWidget* , const char* name):KMainWindow(0, name)
{
  config=kapp->config();

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
  initActions();

  readOptions();

  initDocument();
  initView();

//  menuBar();
	

  preferencesDialog = new KAlcatelConfigDialog(this);

  ///////////////////////////////////////////////////////////////////
  // disable actions at startup
/*
  fileSave->setEnabled(false);
  fileSaveAs->setEnabled(false);
  filePrint->setEnabled(false);
*/
  editCut->setEnabled(false);
  editCopy->setEnabled(false);
  editPaste->setEnabled(false);
}

KAlcatelApp::~KAlcatelApp() {
}

void KAlcatelApp::initActions()
{
  fileReadMobileAll = new KAction(i18n("&Everything"), QIconSet(SmallIcon("kalcatel-mobile.png"), BarIcon("kalcatel-mobile.png")), 0, this, SLOT(slotFileReadMobileAll()), actionCollection(),"file_read_mobile");
  fileReadMobileTodo = new KAction(i18n("&Todos"), QIconSet(SmallIcon("kalcatel-todo.png"), BarIcon("kalcatel-todo.png")), 0, this, SLOT(slotFileReadMobileTodo()), actionCollection(),"file_read_mobile_todo");
  fileReadMobileSms = new KAction(i18n("Messa&ges"), QIconSet(SmallIcon("kalcatel-message.png"), BarIcon("kalcatel-message.png")), 0, this, SLOT(slotFileReadMobileSms()), actionCollection(),"file_read_mobile_sms");
  fileReadMobileCalendar = new KAction(i18n("&Calendar"), QIconSet(SmallIcon("kalcatel-calendar.png"), BarIcon("kalcatel-calendar.png")), 0, this, SLOT(slotFileReadMobileCalendar()), actionCollection(),"file_read_mobile_calendar");
  fileReadMobileCalls = new KAction(i18n("Ca&lls"), QIconSet(SmallIcon("kalcatel-call.png"), BarIcon("kalcatel-call.png")), 0, this, SLOT(slotFileReadMobileCalls()), actionCollection(),"file_read_mobile_calls");
  fileReadMobileContactsSim = new KAction(i18n("Contacts from &SIM"), QIconSet(SmallIcon("kalcatel-contact-sim.png"), BarIcon("kalcatel-contact-sim.png")), 0, this, SLOT(slotFileReadMobileContactsSim()), actionCollection(),"file_read_mobile_contacts_sim");
  fileReadMobileContactsMobile = new KAction(i18n("Contacts from &mobile"), QIconSet(SmallIcon("kalcatel-contact-mobile.png"), BarIcon("kalcatel-contact-mobile.png")), 0, this, SLOT(slotFileReadMobileContactsMobile()), actionCollection(),"file_read_mobile_contacts_mobile");

  mobileInfo = new KAction(i18n("&Information"), QIconSet(SmallIcon("kalcatel-mobile.png"), BarIcon("kalcatel-mobile.png")), 0, this, SLOT(slotMobileInfo()), actionCollection(),"mobile_info");

  fileNew = KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection());
  fileOpen = KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  fileOpenRecent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const KURL&)), actionCollection());
  fileSave = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  fileSaveAs = KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
  fileClose = KStdAction::close(this, SLOT(slotFileClose()), actionCollection());
  filePrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  fileQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  editCut = KStdAction::cut(this, SLOT(slotEditCut()), actionCollection());
  editCopy = KStdAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  editPaste = KStdAction::paste(this, SLOT(slotEditPaste()), actionCollection());
  viewToolBar = KStdAction::showToolbar(this, SLOT(slotViewToolBar()), actionCollection());
  viewStatusBar = KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()), actionCollection());

  preferencesEdit = KStdAction::preferences(this, SLOT(slotPreferencesEdit()), actionCollection());
  preferencesSave = KStdAction::saveOptions(this, SLOT(slotPreferencesSave()), actionCollection());
  preferencesToolbars = KStdAction::configureToolbars(this, SLOT(slotPreferencesToolbars()), actionCollection());
  preferencesKeyBindings = KStdAction::keyBindings(this, SLOT(slotConfigureKeys()), actionCollection());

  fileNew->setStatusText(i18n("Creates a new document"));
  fileOpen->setStatusText(i18n("Opens an existing document"));
  fileOpenRecent->setStatusText(i18n("Opens a recently used file"));
  fileSave->setStatusText(i18n("Saves the actual document"));
  fileSaveAs->setStatusText(i18n("Saves the actual document as..."));
  fileClose->setStatusText(i18n("Closes the actual document"));
  filePrint ->setStatusText(i18n("Prints out the actual document"));
  fileQuit->setStatusText(i18n("Quits the application"));
  editCut->setStatusText(i18n("Cuts the selected section and puts it to the clipboard"));
  editCopy->setStatusText(i18n("Copies the selected section to the clipboard"));
  editPaste->setStatusText(i18n("Pastes the clipboard contents to actual position"));
  viewToolBar->setStatusText(i18n("Enables/disables the toolbar"));
  viewStatusBar->setStatusText(i18n("Enables/disables the statusbar"));

  // use the absolute path to your kalcatelui.rc file for testing purpose in createGUI();
  createGUI();

}


void KAlcatelApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  // TODO: add your own items you need for displaying current application status.
/*  KStatusBar *bar = statusBar();
  bar->insertItem(i18n("Ready."), ID_STATUS_MSG, 1);
  bar->setItemAlignment(ID_STATUS_MSG, AlignLeft);
  bar->insertItem(I18N_NOOP("KAlcatel"), ID_DETAIL_MSG, 1);
  bar->setItemAlignment(ID_DETAIL_MSG, AlignLeft);*/
}

void KAlcatelApp::initDocument() {
    doc = new KAlcatelDoc(this);
    if (auto_open_last && !last_file.isEmpty()) {
        doc->openDocument(last_file);
    } else {
        doc->newDocument();
    }
}

void KAlcatelApp::initView()
{ 
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KTMainWindow's view-region and
  // connect the widget to your document to display document contents.

  view = new KAlcatelView(this);
  doc->addView(view);
  setCentralWidget(view);	
  setCaption(doc->URL().fileName(),false);

}

void KAlcatelApp::openDocumentFile(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."), ID_STATUS_MSG);

  if (doc->openDocument( url))
      fileOpenRecent->addURL( url );
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}


KAlcatelDoc *KAlcatelApp::getDocument() const
{
  return doc;
}


void KAlcatelApp::saveOptions() {	
    config->setGroup("General Options");
    config->writeEntry("Geometry", size());
    config->writeEntry("Show Toolbar", viewToolBar->isChecked());
    config->writeEntry("Show Statusbar",viewStatusBar->isChecked());
    config->writeEntry("ToolBarPos", (int) toolBar("mainToolBar")->barPos());
    fileOpenRecent->saveEntries(config,"Recent Files");
    config->writeEntry("Open Last File", auto_open_last);
    last_file = doc->URL();

    if (last_file.fileName()!=i18n("Untitled"))
        config->writeEntry("Last File", last_file.url());

    config->setGroup("Data");
    config->writeEntry("Merge during read", mergeData);
    config->writeEntry("Conflict action", conflictAction);

    config->setGroup("Contacts");
    config->writeEntry("Phone Prefix", phone_prefix);
    config->writeEntry("Use Custom Field as URL", contact_url);

    config->setGroup("Mobile");
    config->writeEntry("Device", mobile_device);
    config->writeEntry("Lock", mobile_lock);
    config->writeEntry("Init", mobile_init);
    config->writeEntry("Baud Rate", mobile_rate);
    config->writeEntry("Debug", mobile_debug);
}


void KAlcatelApp::readOptions(){
    config->setGroup("General Options");

    // bar status settings
    bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
    viewToolBar->setChecked(bViewToolbar);
    slotViewToolBar();

    bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
    viewStatusBar->setChecked(bViewStatusbar);
    slotViewStatusBar();


    // bar position settings
    KToolBar::BarPosition toolBarPos;
    toolBarPos=(KToolBar::BarPosition) config->readNumEntry("ToolBarPos", KToolBar::Top);
    toolBar("mainToolBar")->setBarPos(toolBarPos);
  	
    // initialize the recent file list
    fileOpenRecent->loadEntries(config,"Recent Files");

    QSize size=config->readSizeEntry("Geometry");
    if(!size.isEmpty())
    {
      resize(size);
    }
    auto_open_last = config->readBoolEntry("Open Last File", false);
    last_file = config->readEntry("Last File", "");

    config->setGroup("Data");
    mergeData = config->readNumEntry("Merge during read", 0);
    conflictAction = config->readNumEntry("Conflict action", 2);

    config->setGroup("Contacts");
    phone_prefix = config->readEntry("Phone Prefix", "+420");
    contact_url = config->readNumEntry("Use Custom Field as URL", -1);

    config->setGroup("Mobile");
    mobile_device = config->readPathEntry("Device", "/dev/ttyS1");
    mobile_lock = config->readPathEntry("Lock", "/var/lock/LCK..%s");
    mobile_init = config->readEntry("Init", "AT S7=45 S0=0 L1 V1 X4 &c1 E1 Q0");
    mobile_rate = config->readNumEntry("Baud Rate", 19200);
    mobile_debug = config->readNumEntry("Debug", MSG_DETAIL);
    msg_level = mobile_debug;
}

void KAlcatelApp::saveProperties(KConfig *_cfg)
{
  if(doc->URL().fileName()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    KURL url=doc->URL();	
    _cfg->writeEntry("filename", url.url());
    _cfg->writeEntry("modified", doc->isModified());
    QString tempname = kapp->tempSaveName(url.url());
    QString tempurl= KURL::encode_string(tempname);
    KURL _url(tempurl);
    doc->saveDocument(_url);
  }
}


void KAlcatelApp::readProperties(KConfig* _cfg)
{
  QString filename = _cfg->readEntry("filename", "");
  KURL url(filename);
  bool modified = _cfg->readBoolEntry("modified", false);

  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);
    KURL _url(tempname);
  	
    if(canRecover)
    {
      if (doc->openDocument(_url)) {
          doc->setModified();
          setCaption(_url.fileName(),true);
      }
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      if (doc->openDocument(url))
          setCaption(url.fileName(),false);
    }
  }
}		

bool KAlcatelApp::queryClose()
{
  return doc->saveModified();
}

bool KAlcatelApp::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void KAlcatelApp::slotFileReadMobileAll()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_all,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileTodo()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_todos,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileContactsSim()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_contacts_sim,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileContactsMobile()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_contacts_mobile,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileCalendar()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_calendar,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileCalls()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_calls,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileSms()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_messages,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotMobileInfo() {
    char manuf[128], model[128], rev[128], sn[128], imsi[128], *s;
    int bat_percent, bat_state, sign_strength, sign_err;
    ContactData *cont;
    slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);

    char *devname;

    msg_level = mobile_debug;

    strcpy(initstring, mobile_init);
    strcpy(device, mobile_device);
    devname = strrchr(device, '/');
    devname++;
    sprintf(lockname, mobile_lock, devname);
    rate = mobile_rate;

    switch (rate) {
        case 2400:   baudrate=B2400; break;
        case 4800:   baudrate=B4800; break;
        case 9600:   baudrate=B9600; break;
        case 19200:  baudrate=B19200; break;
        case 38400:  baudrate=B38400; break;
        default:
            message(MSG_ERROR,"Ivalid baud rate (%d), setting to default (19200)!", rate);
            baudrate=B19200;
    }


    this->slotStatusMsg(i18n("Opening modem"),ID_DETAIL_MSG);
    if (!modem_open()) {
        switch (modem_errno) {
            case ERR_MDM_OPEN:
                KMessageBox::error(this, i18n("Failed opening modem for read/write."), i18n("Error"));
                modem_close();
                return;
            case ERR_MDM_LOCK:
                KMessageBox::error(this, i18n("Modem locked."), i18n("Error"));
                modem_close();
                return;
            default:
                KMessageBox::error(this, i18n("Failed opening modem.\nUnknown error (%1).").arg(modem_errno), i18n("Error"));
                modem_close();
                return;
        }
        return;
    }
    this->slotStatusMsg(i18n("Setting serial port"),ID_DETAIL_MSG);
    modem_setup();
    this->slotStatusMsg(i18n("Initializing modem"),ID_DETAIL_MSG);
    if (!modem_init()) {
        switch (modem_errno) {
            case ERR_MDM_AT:
                KMessageBox::error(this, i18n("Modem doesn't react on AT command."), i18n("Error"));
                modem_close();
                return;
            case ERR_MDM_PDU:
                KMessageBox::error(this, i18n("Failed selecting PDU mode."), i18n("Error"));
                modem_close();
                return;
            default:
                KMessageBox::error(this, i18n("Failed initializing modem.\nUnknown error (%1).").arg(modem_errno), i18n("Error"));
                modem_close();
                return;
        }
        return;
    }

    this->slotStatusMsg(i18n("Reading information"),ID_DETAIL_MSG);

    get_manufacturer(manuf,sizeof(manuf));

    get_model(model,sizeof(model));

    get_revision(rev,sizeof(rev));

    get_sn(sn,sizeof(sn));

    get_imsi(imsi,sizeof(imsi));

    get_battery(&bat_state, &bat_percent);

    get_signal(&sign_strength, &sign_err);

    s = get_smsc();

    select_phonebook(PB_OWN);
    cont = get_contacts(1, 1);

    modem_close();

    KMessageBox::information(this,
          i18n("<table border=\"0\">"
          "<tr><td><b>"
          "Manufacturer:"
          "</b></td><td>"
          "%0"
          "</td></tr>"
          "<tr><td><b>"
          "Model:"
          "</b></td><td>"
          "%1"
          "</td></tr>"
          "<tr><td><b>"
          "Revision:"
          "</b></td><td>"
          "%2"
          "</td></tr>"
          "<tr><td><b>"
          "Serial number:"
          "</b></td><td>"
          "%3"
          "</td></tr>"
          "<tr><td><b>"
          "IMSI:"
          "</b></td><td>"
          "%4"
          "</td></tr>"
          "<tr><td><b>"
          "Battery status:"
          "</b></td><td>"
          "%5"
          "</td></tr>"
          "<tr><td><b>"
          "Battery charge:"
          "</b></td><td>"
          "%6%"
          "</td></tr>"
          "<tr><td><b>"
          "Signal strength:"
          "</b></td><td>"
          "%7"
          "</td></tr>"
          "<tr><td><b>"
          "Signal error rate:"
          "</b></td><td>"
          "%8"
          "</td></tr>"
          "<tr><td><b>"
          "SMS Centre:"
          "</b></td><td>"
          "%9"
          "</td></tr>"
          ).
          arg(manuf).
          arg(model).
          arg(rev).
          arg(sn).
          arg(imsi).
          arg(
            i18n("%1 (%2)").
                arg(bat_state == 0 ? i18n("Supplied from battery") :
                    bat_state == 1 ? i18n("Battery connected but not used") :
                    bat_state == 2 ? i18n("Battery not connected") :
                    bat_state == 3 ? i18n("Error") :
                    i18n("Unknown")
                ).
                arg(bat_state)
          ).
          arg(bat_percent).
          arg(sign_strength == 99 ? i18n("Unknown") : QString(mobil_signal_info[sign_strength])).
          arg(sign_err == 99 ? i18n("Unknown") : QString("%1").arg(sign_err)).
          arg(s).
          append(cont[0].pos == -1 ? QString() : i18n("<tr><td><b>"
              "Own number:"
              "</b></td><td>"
              "%1"
              "</td></tr>").
              arg(cont[0].number)).
          append(i18n("</table>"))
          ,
          i18n("Mobile information"));
    free(cont);

    slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileNew()
{
  slotStatusMsg(i18n("Creating new document..."), ID_STATUS_MSG);

  if(!doc->saveModified())
  {
     // here saving wasn't successful

  }
  else
  {	
    doc->newDocument();		
    setCaption(doc->URL().fileName(), false);
  }

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."), ID_STATUS_MSG);
	
  if(!doc->saveModified())
  {
     // here saving wasn't successful

  }
  else
  {	
    KURL url=KFileDialog::getOpenURL(QString::null,
        i18n("*.kalc|KAlcatel files (*.kalc)\n*|All files"), this, i18n("Open File..."));
    if(!url.isEmpty())
    {
      if (doc->openDocument(url)) {
          setCaption(url.fileName(), false);
          fileOpenRecent->addURL( url );
      }
    }
  }
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileOpenRecent(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."), ID_STATUS_MSG);

  if(!doc->saveModified())
  {
     // here saving wasn't successful
  }
  else
  {
    if (doc->openDocument(url))
        setCaption(url.fileName(), false);
  }

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileSave() {
    slotStatusMsg(i18n("Saving file..."), ID_STATUS_MSG);
  	
    if (doc->URL().fileName() == i18n("Untitled")) {
        slotFileSaveAs();
    } else {
        doc->saveDocument(doc->URL());
    };

    slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileSaveAs() {
    slotStatusMsg(i18n("Saving file with a new filename..."), ID_STATUS_MSG);

    KURL url=KFileDialog::getSaveURL(QDir::currentDirPath(),
          i18n("*.kalc|KAlcatel files (*.kalc)\n*|All files"), this, i18n("Save as..."));
    if(!url.isEmpty()) {
        QString filename = url.fileName();
        if (!filename.contains('.')) {
            filename.append(".kalc");
            url.setFileName(filename);
        }
        if (KIO::NetAccess::exists(url)) {
            int want_save = KMessageBox::warningYesNoCancel(this,
                                                 i18n("The selected file exists.\n"
                                                      "Do you want to overwrite it?"),
                                                 i18n("Warning"));
            switch(want_save){
                case KMessageBox::Yes:
                    break;
                case KMessageBox::No:
                    slotFileSaveAs();
                    return;
                    break;
                case KMessageBox::Cancel:
                    slotStatusMsg(i18n("Canceled."), ID_STATUS_MSG);
                    return;
                    break;
                default:
                    slotFileSaveAs();
                    return;
                    break;
            }
        }

        doc->saveDocument(url);
        fileOpenRecent->addURL(url);
        setCaption(url.fileName(),doc->isModified());
    }

    slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileClose() {
    slotStatusMsg(i18n("Closing file..."), ID_STATUS_MSG);
  	
    close();
}

void KAlcatelApp::slotFilePrint() {
    slotStatusMsg(i18n("Printing..."), ID_STATUS_MSG);

    QPrinter printer;
    if (printer.setup(this)) {
        view->print(&printer);
    }

    slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileQuit() {
    slotStatusMsg(i18n("Exiting..."), ID_STATUS_MSG);
    saveOptions();
    // close the first window, the list makes the next one the first again.
    // This ensures that queryClose() is called on each window to ask for closing
    KMainWindow* w;
    if(memberList) {
        for(w=memberList->first(); w!=0; w=memberList->first()) {
            // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
            // the window and the application stay open.
            if(!w->close())
                break;
      }
    }	
}

void KAlcatelApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting selection..."), ID_STATUS_MSG);

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to clipboard..."), ID_STATUS_MSG);

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotEditPaste()
{
  slotStatusMsg(i18n("Inserting clipboard contents..."), ID_STATUS_MSG);

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotViewToolBar()
{
  slotStatusMsg(i18n("Toggling toolbar..."), ID_STATUS_MSG);
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if(!viewToolBar->isChecked())
  {
    toolBar("mainToolBar")->hide();
  }
  else
  {
    toolBar("mainToolBar")->show();
  }		

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotViewStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."), ID_STATUS_MSG);
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if(!viewStatusBar->isChecked())
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}


//void KAlcatelApp::slotStatusMsg(const QString &text, int which = ID_STATUS_MSG, int clearDetail = true)
void KAlcatelApp::slotStatusMsg(const QString &text, int which, int clearDetail = true)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  QString *msg = new QString();
  setDisabled(true);
  statusBar()->clear();
  if (which == ID_STATUS_MSG) {
        statusText = text;
        msg->append(text);
        if (!clearDetail) {
            msg->append(I18N_NOOP("("));
            msg->append(detailText);
            msg->append(I18N_NOOP(")"));
        }
  } else {
        detailText = text;
        msg->append(statusText);
        msg->append(I18N_NOOP("("));
        msg->append(text);
        msg->append(I18N_NOOP(")"));
  }
//  statusBar()->changeItem(text, which);
  statusBar()->message(*msg);
  setEnabled(true);
  setUpdatesEnabled(true);
  update();
}

void KAlcatelApp::slotDefaultDetailMsg()
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
//  statusBar()->changeItem(I18N_NOOP("KAlcatel"), ID_DETAIL_MSG);
//  statusBar()->message(text);
}

void KAlcatelApp::slotPreferencesEdit() {
    preferencesDialog->exec();
}

void KAlcatelApp::slotPreferencesSave() {
    saveOptions();
}

void KAlcatelApp::slotConfigureKeys() {
    KKeyDialog::configureKeys(actionCollection(), xmlFile());
}

void KAlcatelApp::slotPreferencesToolbars() {
    KEditToolbar dlg(actionCollection());
    if (dlg.exec()){
        createGUI();
    }
}
