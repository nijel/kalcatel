/*****************************************************************************
 * 
 * kalcatelview.h
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

#ifndef KALCATELVIEW_H
#define KALCATELVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for Qt
#include <qvbox.h>

//#include <qwidget.h>
#include <kjanuswidget.h>

typedef enum {
    alc_contacts,
    alc_contacts_sim,
    alc_contacts_mobile,
    alc_contacts_mobile_cat,
    alc_todos,
    alc_calendar,
    alc_calls,
    alc_calls_type,
    alc_messages,
    alc_messages_in,
    alc_messages_out
    } AlcListType;

class KAlcatelDoc;

/** The KAlcatelView class provides the view widget for the KAlcatelApp instance.	
 * The View instance inherits QWidget as a base class and represents the view object of a KTMainWindow. As KAlcatelView is part of the
 * docuement-view model, it needs a reference to the document object connected with it by the KAlcatelApp class to manipulate and display
 * the document structure provided by the KAlcatelDoc class.
 * 	
 * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
 * @version KDevelop version 0.4 code generation
 */
class KAlcatelView : public KJanusWidget
{
  Q_OBJECT
  public:
    /** Constructor for the main view */
    KAlcatelView(QWidget *parent = 0, const char *name=0);
    /** Destructor for the main view */
    ~KAlcatelView();

    /** returns a pointer to the document connected to the view instance. Mind that this method requires a KAlcatelApp instance as a parent
     * widget to get to the window document pointer by calling the KAlcatelApp::getDocument() method.
     *
     * @see KAlcatelApp#getDocument
     */
    KAlcatelDoc *getDocument() const;

    /** contains the implementation for printing functionality */
//    void print(QPrinter *pPrinter);
	
  private:
    QVBox *todo, *calendar,
        *contacts, *contacts_sim, *contacts_mobile,
        *calls, *calls_ld, *calls_mc, *calls_rc,
        *messages, *msg_read, *msg_unread, *msg_sent, *msg_unsent;
    KListView *todo_list, *calendar_list,
        *contacts_list, *contacts_sim_list, *contacts_mobile_list,
        *calls_list, *calls_ld_list, *calls_mc_list, *calls_rc_list,
        *messages_list, *msg_read_list, *msg_unread_list, *msg_sent_list, *msg_unsent_list;

    KListView *createListView(QVBox *parent, AlcListType type);

/*
    QGrid *todo;
    QGrid *calendar;
    QGrid *contacts;
    QGrid *calls;
    QGrid *messages;
	*/
};

#endif // KALCATELVIEW_H
