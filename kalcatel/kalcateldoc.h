/*
 * kalcateldoc.h
 *
 * "document" for KAlcatel - here are all data
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

#ifndef KALCATELDOC_H
#define KALCATELDOC_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for QT
#include <qobject.h>
#include <qstring.h>
#include <qlist.h>
#include <qstringlist.h>

// include files for KDE
#include <kurl.h>

#include "alcatelclasses.h"

/** Used for selecting which data we can read
  */
enum AlcReadType {
    alcatel_read_all,
    alcatel_read_todo,
    alcatel_read_sms,
    alcatel_read_calendar,
    alcatel_read_calls,
    alcatel_read_contacts_sim,
    alcatel_read_contacts_mobile
    };

// forward declaration of the KAlcatel classes
class KAlcatelView;
class KAlcatelApp;

/**	KAlcatelDoc provides a document object for a document-view model.
  *
  * The KAlcatelDoc class provides a document object that can be used in conjunction with the classes KAlcatelApp and KAlcatelView
  * to create a document-view model for standard KDE applications based on KApplication and KMainWindow. Thereby, the document object
  * is created by the KAlcatelApp instance and contains the document structure with the according methods for manipulation of the document
  * data by KAlcatelView objects. Also, KAlcatelDoc contains the methods for serialization of the document data from and to files.
  */
class KAlcatelDoc : public QObject
{
  Q_OBJECT
  public:
    /** Constructor for the fileclass of the application
     */
    KAlcatelDoc(QWidget *parent, const char *name=0);
    /** Destructor for the fileclass of the application
     */
    ~KAlcatelDoc();

    /** adds a view to the document which represents the document contents. Usually this is your main view.
     */
    void addView(KAlcatelView *view);
    /** removes a view from the list of currently connected views
     */
    void removeView(KAlcatelView *view);
    /** sets the modified flag for the document after a modifying action on the view connected to the document.
     */
    void setModified(bool _m=true){ modified=_m; };
    /** returns if the document is modified or not. Use this to determine if your document needs saving by the user on closing.
     */
    bool isModified(){ return modified; };
    /** "save modified" - asks the user for saving if the document is modified
     */
    bool saveModified();	
    /** deletes the document's contents
     */
    void deleteContents();
    /** initializes the document generally
     */
    bool newDocument();
    /** closes the acutal document
     */
    void closeDocument();
    /** reads content from mobile
     */
    bool readMobile(AlcReadType what = alcatel_read_all, int category = -1);
    /** loads the document by filename and format and emits the updateViews() signal
     */
    bool openDocument(const KURL& url, const char *format=0);
    /** saves the document under filename and format.
     */	
    bool saveDocument(const KURL& url, const char *format=0);
    /** returns the KURL of the document
     */
    const KURL& URL() const;
    /** sets the URL of the document
     */
	void setURL(const KURL& url);

    /** returns document version
     */
	int getVersion();
    /** returns messages version
     */
	int getSMSVersion();
    /** returns calls version
     */
	int getCallVersion();
    /** returns calendar version
     */
	int getCalendarVersion();
    /** returns todos version
     */
	int getTodoVersion();
    /** returns contacts version
     */
	int getContactVersion();
	
  public slots:
    /** calls repaint() on all views connected to the document object and is called by the view by which the document has been changed.
     * As this view normally repaints itself, it is excluded from the paintEvent.
     */
    void slotUpdateAllViews(KAlcatelView *sender);
 	
  public:	
    /** the list of the views currently connected to the document
     */
    static QList<KAlcatelView> *pViewList;	

    /** todo categories
      */
    AlcatelCategoryList *todo_cats;
    /** contacts categories
      */
    AlcatelCategoryList *contact_cats;

    /** contacts
      */
    AlcatelContactList *contacts;
    /** calendar
      */
    AlcatelCalendarList *calendar;
    /** todos
      */
    AlcatelTodoList *todo;
    /** messages
      */
    AlcatelSMSList *sms;
    /** calls
      */
    AlcatelCallList *calls;
  private:
    /** reads categories from mobile
      */
    int readMobileCategories(AlcatelCategoryList *strList, alc_type sync, alc_type type, alc_type cat);
    /** reads items from mobile
      */
    int readMobileItems(alc_type sync, alc_type type);
    /** the modified flag of the current document
      */
    bool modified;
    /** document url
      */
    KURL doc_url;

    /** document version
      */
    int version;
    /** messages version
      */
    int smsVersion;
    /** calls version
      */
    int callVersion;
    /** contacts version
      */
    int contactVersion;
    /** calendar version
      */
    int calendarVersion;
    /** todos version
      */
    int todoVersion;
};

#endif // KALCATELDOC_H
