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

// application specific includes
#include "kalcatel.h"
#include "kalcatelview.h"
#include "kalcateldoc.h"

KAlcatelApp::KAlcatelApp(QWidget* , const char* name):KMainWindow(0, name)
{
  config=kapp->config();

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
  initActions();
  initDocument();
  initView();

//  menuBar();
	
  readOptions();

  ///////////////////////////////////////////////////////////////////
  // disable actions at startup
  fileSave->setEnabled(false);
  fileSaveAs->setEnabled(false);
  filePrint->setEnabled(false);
  editCut->setEnabled(false);
  editCopy->setEnabled(false);
  editPaste->setEnabled(false);
}

KAlcatelApp::~KAlcatelApp()
{

}

void KAlcatelApp::initActions()
{
//  fileNewWindow = new KAction(i18n("New &Window"), 0, 0, this, SLOT(slotFileNewWindow()), actionCollection(),"file_new_window");
//MainBarIconSet
  fileReadMobileAll = new KAction(i18n("&Read everything from mobile"), QIconSet(SmallIcon("kalcatel-mobile.png"), BarIcon("kalcatel-mobile.png")), 0, this, SLOT(slotFileReadMobileAll()), actionCollection(),"file_read_mobile");
  fileReadMobileTodo = new KAction(i18n("Read &todos from mobile"), QIconSet(SmallIcon("kalcatel-todo.png"), BarIcon("kalcatel-todo.png")), 0, this, SLOT(slotFileReadMobileTodo()), actionCollection(),"file_read_mobile_todo");
  fileReadMobileSms = new KAction(i18n("Read &SMSs from mobile"), QIconSet(SmallIcon("kalcatel-message.png"), BarIcon("kalcatel-message.png")), 0, this, SLOT(slotFileReadMobileSms()), actionCollection(),"file_read_mobile_sms");
  fileReadMobileCalendar = new KAction(i18n("Read &calendar from mobile"), QIconSet(SmallIcon("kalcatel-calendar.png"), BarIcon("kalcatel-calendar.png")), 0, this, SLOT(slotFileReadMobileCalendar()), actionCollection(),"file_read_mobile_calendar");
  fileReadMobileCalls = new KAction(i18n("Read c&alls from mobile"), QIconSet(SmallIcon("kalcatel-call.png"), BarIcon("kalcatel-call.png")), 0, this, SLOT(slotFileReadMobileCalls()), actionCollection(),"file_read_mobile_calls");
  fileReadMobileContactsSim = new KAction(i18n("Read contacts (S&IM) from mobile"), QIconSet(SmallIcon("kalcatel-contact-sim.png"), BarIcon("kalcatel-contact-sim.png")), 0, this, SLOT(slotFileReadMobileContactsSim()), actionCollection(),"file_read_mobile_contacts_sim");
  fileReadMobileContactsMobile = new KAction(i18n("Read contacts (m&obile) from mobile"), QIconSet(SmallIcon("kalcatel-contact-mobile.png"), BarIcon("kalcatel-contact-mobile.png")), 0, this, SLOT(slotFileReadMobileContactsMobile()), actionCollection(),"file_read_mobile_contacts_mobile");

/*
  fileReadMobileAll = new KAction(i18n("&Read everything from mobile"), QIconSet(UserIcon("mobile.png")), 0, this, SLOT(slotFileReadMobileAll()), actionCollection(),"file_read_mobile");
  fileReadMobileTodo = new KAction(i18n("Read &todos from mobile"), QIconSet(UserIcon("todo.png")), 0, this, SLOT(slotFileReadMobileTodo()), actionCollection(),"file_read_mobile_todo");
  fileReadMobileSms = new KAction(i18n("Read &SMSs from mobile"), QIconSet(UserIcon("message.png")), 0, this, SLOT(slotFileReadMobileSms()), actionCollection(),"file_read_mobile_sms");
  fileReadMobileCalendar = new KAction(i18n("Read &calendar from mobile"), QIconSet(UserIcon("calendar.png")), 0, this, SLOT(slotFileReadMobileCalendar()), actionCollection(),"file_read_mobile_calendar");
  fileReadMobileCalls = new KAction(i18n("Read c&alls from mobile"), QIconSet(UserIcon("call.png")), 0, this, SLOT(slotFileReadMobileCalls()), actionCollection(),"file_read_mobile_calls");
  fileReadMobileContactsSim = new KAction(i18n("Read contacts (S&IM) from mobile"), QIconSet(UserIcon("contact-sim.png")), 0, this, SLOT(slotFileReadMobileContactsSim()), actionCollection(),"file_read_mobile_contacts_sim");
  fileReadMobileContactsMobile = new KAction(i18n("Read contacts (m&obile) from mobile"), QIconSet(UserIcon("contact-mobile.png")), 0, this, SLOT(slotFileReadMobileContactsMobile()), actionCollection(),"file_read_mobile_contacts_mobile");
*/

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

//  fileNewWindow->setStatusText(i18n("Opens a new application window"));
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

void KAlcatelApp::initDocument()
{
  doc = new KAlcatelDoc(this);
  doc->newDocument();
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

  doc->openDocument( url);
  fileOpenRecent->addURL( url );
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}


KAlcatelDoc *KAlcatelApp::getDocument() const
{
  return doc;
}

void KAlcatelApp::saveOptions()
{	
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
  config->writeEntry("Show Toolbar", viewToolBar->isChecked());
  config->writeEntry("Show Statusbar",viewStatusBar->isChecked());
  config->writeEntry("ToolBarPos", (int) toolBar("mainToolBar")->barPos());
  fileOpenRecent->saveEntries(config,"Recent Files");
}


void KAlcatelApp::readOptions()
{
	
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
      doc->openDocument(_url);
      doc->setModified();
      setCaption(_url.fileName(),true);
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->openDocument(url);
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

/*
void KAlcatelApp::slotFileNewWindow()
{
  slotStatusMsg(i18n("Opening a new application window..."));
	
  KAlcatelApp *new_window= new KAlcatelApp();
  new_window->show();

  slotStatusMsg(i18n("Ready."));
}
*/

void KAlcatelApp::slotFileReadMobileAll()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_read_all,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileTodo()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_read_todo,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileContactsSim()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_read_contacts_sim,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileContactsMobile()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_read_contacts_mobile,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileCalendar()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_read_calendar,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileCalls()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_read_calls,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
}

void KAlcatelApp::slotFileReadMobileSms()
{
  slotStatusMsg(i18n("Reading data from mobile..."), ID_STATUS_MSG);
  doc->readMobile(alcatel_read_sms,-1);
  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG, false);
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
        i18n("*.kalc|KAlcatel files\n*|All files"), this, i18n("Open File..."));
    if(!url.isEmpty())
    {
      doc->openDocument(url);
      setCaption(url.fileName(), false);
      fileOpenRecent->addURL( url );
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
    doc->openDocument(url);
    setCaption(url.fileName(), false);
  }

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."), ID_STATUS_MSG);
	
  doc->saveDocument(doc->URL());

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file with a new filename..."), ID_STATUS_MSG);

  KURL url=KFileDialog::getSaveURL(QDir::currentDirPath(),
        i18n("*|All files"), this, i18n("Save as..."));
  if(!url.isEmpty())
  {
    doc->saveDocument(url);
    fileOpenRecent->addURL(url);
    setCaption(url.fileName(),doc->isModified());
  }

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileClose()
{
  slotStatusMsg(i18n("Closing file..."), ID_STATUS_MSG);
	
  close();

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."), ID_STATUS_MSG);

  QPrinter printer;
  if (printer.setup(this))
  {
//    view->print(&printer);
  }

  slotStatusMsg(i18n("Ready."), ID_STATUS_MSG);
}

void KAlcatelApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."), ID_STATUS_MSG);
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if(memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
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
  setUpdatesEnabled(true);
  setEnabled(true);
}

void KAlcatelApp::slotDefaultDetailMsg()
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
//  statusBar()->changeItem(I18N_NOOP("KAlcatel"), ID_DETAIL_MSG);
//  statusBar()->message(text);
}


