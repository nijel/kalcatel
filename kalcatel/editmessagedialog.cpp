/*
 * editmessagedialog.cpp
 *
 * TODO: description
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

#include <qpushbutton.h>
#include <qvbox.h>
#include <qframe.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qmultilineedit.h>
#include <qstringlist.h>
#include <qregexp.h>

#include <kdialog.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kmessagebox.h>

#include "editmessagedialog.h"
#include "phonenumbervalidator.h"
#include "kalcatel.h"

#include "alcasync/pdu.h"
#include "alcasync/sms.h"

#include "selectcontactdialog.h"

EditMessageDialog::EditMessageDialog(AlcatelContactList *cont, QWidget *parent, const char *name ) : KDialog(parent,name, true) {
    contacts = cont;
    QLabel *label;
    QFrame *line;

    reread = false;
    resize(450, 280 );
    setCaption( i18n( "New message" ) );

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSpacing( 6 );
    mainLayout->setMargin( 8 );

    label = new QLabel( i18n("<b>New message</b>"), this );
    mainLayout->addMultiCellWidget(label,0,0,0,4);

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMargin(3);
    mainLayout->addMultiCellWidget(line,1,1,0,4);

    mainLayout->setRowStretch(0, -1);
    mainLayout->setRowStretch(1, -1);

    sendCheck = new QCheckBox(i18n("Send message"), this);
    sendCheck->setChecked(true);
    mainLayout->addMultiCellWidget(sendCheck,2,2,0,1);

    QWhatsThis::add(sendCheck ,i18n("<b>Send message</b><br>When checked message will be sent as you press OK button."));

    writeCheck = new QCheckBox(i18n("Store to mobile as"), this);
    writeCheck->setChecked(true);
    mainLayout->addMultiCellWidget(writeCheck,2,2,2,3);

    QWhatsThis::add(writeCheck ,i18n("<b>Store to mobile</b><br>When checked, message will be written to mobile (SIM) as sent or unsent just as you press OK button."));

    typeCombo = new QComboBox(this);
    typeCombo->insertItem(i18n("Sent"));
    typeCombo->insertItem(i18n("Unsent"));
    mainLayout->addWidget(typeCombo, 2,4);

    QWhatsThis::add( typeCombo,i18n("<b>Type of message</b><br>Which will be type of message stored in mobile?"));

    connect( sendCheck, SIGNAL( toggled(bool) ), this, SLOT( slotSendChanged(bool) ) );
    connect( writeCheck, SIGNAL( toggled(bool) ), this, SLOT( slotWriteChanged(bool) ) );

    label = new QLabel( i18n("Send to:"), this );
    mainLayout->addWidget(label, 3,0);

    sendToCombo = new KComboBox(true, this);
    sendToCombo->setValidator(new PhoneNumberValidator(false, true, sendToCombo));
    mainLayout->addMultiCellWidget(sendToCombo,3,3,1,3);
    QWhatsThis::add(sendToCombo ,i18n("<b>Send to number(s)</b><br>Enter here number(s) of receivers of this message, more numbers should be separated with space, comma or semicolon."));

    QPushButton *buttonContacts = new QPushButton(i18n("Contacts..."), this);
    connect ( buttonContacts, SIGNAL( clicked() ), this, SLOT(selectContacts()));
    mainLayout->addWidget(buttonContacts,3,4);
    QWhatsThis::add(buttonContacts ,i18n("<b>Contacts</b><br>Use this button to add number from your contacts."));

    messageEdit = new QTextEdit(this);
    messageEdit->setTextFormat(Qt::PlainText);
    messageEdit->setWordWrap(QTextEdit::WidgetWidth);

    mainLayout->addMultiCellWidget(messageEdit,4,4,0,4);

    connect( messageEdit, SIGNAL( textChanged() ), this, SLOT( slotTextChanged() ) );
    connect( messageEdit, SIGNAL( returnPressed() ), this, SLOT( slotOK() ) );

    QWhatsThis::add( messageEdit,i18n("<b>Message text</b><br>Write here message text, note that characters with accents will be very probably interpreted another way that you wanted, just few pass correctly."));

    msgInfoLabel = new QLabel( i18n("Used %1 of %2 characters").arg(0).arg(160), this );
    mainLayout->addMultiCellWidget(msgInfoLabel,5,5,0,2);

    mainLayout->addWidget(new QLabel( i18n("Class of message"),this),5,3);
    classCombo = new QComboBox(this);
    classCombo->insertItem(i18n("0 - flash"));
    classCombo->insertItem(i18n("1 - mobile"));
    classCombo->insertItem(i18n("2 - sim"));
    classCombo->insertItem(i18n("3 - terminal"));
    classCombo->setCurrentItem(1);
    mainLayout->addWidget(classCombo, 5,4);

    QWhatsThis::add( classCombo,i18n("<b>Class of message</b><br>Class 0 message just shows on display imediately after receiving. Class 1 and 2 message are normal messages, 1 should be preferably stored into mobile while 2 on SIM card. Class 3 should go to terminal equipment which most mobiles doesn't support."));

    QHBoxLayout *layout = new QHBoxLayout;

    layout->setSpacing( 6 );
    layout->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout->addItem( spacer );

    QPushButton *buttonOK = new QPushButton(i18n("&OK"), this);
    buttonOK->setDefault(true);
    layout->addWidget(buttonOK);

    QPushButton *buttonCancel = new QPushButton(i18n("&Cancel"), this);
    layout->addWidget(buttonCancel);

    mainLayout->addMultiCellLayout( layout, 7,7, 0,4 );

    line = new QFrame( this);
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    mainLayout->addMultiCellWidget( line, 6,6, 0,4 );
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
}

EditMessageDialog::~EditMessageDialog(){
}

void EditMessageDialog::slotCancel() {
    reject();
}

void EditMessageDialog::slotOK() {
    KAlcatelApp *theApp=(KAlcatelApp *) parentWidget();
    int sent = 0, written = 0;
    int failed = 0, s_failed = 0, w_failed = 0;
    int i;
    QString msg;
    char pdu[1024];
    QStringList list = QStringList::split(QRegExp("[; ,]"), sendToCombo->currentText(), false);

    if (list.count() == 0 && sendCheck->isChecked()) {
        KMessageBox::error(this, i18n("You must enter at least one phone number to send message!"), i18n("Error"));
        return;
    }

    if(messageEdit->text().isEmpty()) {
        KMessageBox::error(this, i18n("You must enter at least one character of text!"), i18n("Error"));
        return;
    }


    if (theApp->modemConnect()) {

        if (list.count() == 0) {
            make_pdu("", messageEdit->text().latin1(), 1, classCombo->currentItem(), pdu);
            i = put_message(pdu, (typeCombo->currentItem() == 0) ? MESSAGE_SENT : MESSAGE_UNSENT);
            if (i == -1) {
                msg.append(i18n("Writing message failed!<br>"));
                w_failed++;
                failed++;
            } else {
                msg.append(i18n("Message written, position=%1<br>").arg(i));
                written++;
            }
        } else {
            for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
                if ((*it).find(PhoneNumberValidator::phoneNumberRegExp, 0) == -1) {
                    KMessageBox::error(this, i18n("%1 doesn't look like phone number!").arg(*it), i18n("Error"));
                    msg.append(i18n("%1 doesn't look like phone number!").arg(*it).append("<br>"));
                    failed++;
                } else {
                    make_pdu((*it).latin1(), messageEdit->text().latin1(), 1, classCombo->currentItem(), pdu);

                    if (sendCheck->isChecked() && writeCheck->isChecked()){
                        i = put_message(pdu, (typeCombo->currentItem() == 0) ? MESSAGE_SENT : MESSAGE_UNSENT);
                        if (i == -1) {
                            msg.append(i18n("Writing message for number %1 failed!<br>").arg(*it));
                            w_failed++;
                            failed++;
                        } else {
                            msg.append(i18n("Message written for number %1, position=%2<br>").arg(*it).arg(i));
                            written++;
                        }
                        i = send_message(i);
                        if (i == -1) {
                            msg.append(i18n("Sending to number %1 failed!<br>").arg(*it));
                            s_failed++;
                            failed++;
                        } else {
                            msg.append(i18n("Message sent to number %1, reference id=%2<br>").arg(*it).arg(i));
                            sent++;
                        }
                    } else if (writeCheck->isChecked()){
                        i = put_message(pdu, (typeCombo->currentItem() == 0) ? MESSAGE_SENT : MESSAGE_UNSENT);
                        if (i == -1) {
                            msg.append(i18n("Writing message for number %1 failed!<br>").arg(*it));
                            w_failed++;
                            failed++;
                        } else {
                            msg.append(i18n("Message written for number %1, position=%2<br>").arg(*it).arg(i));
                            written++;
                        }
                    } else if (sendCheck->isChecked()){
                        i = send_message(pdu);
                        if (i == -1) {
                            msg.append(i18n("Sending to number %1 failed!<br>").arg(*it));
                            s_failed++;
                            failed++;
                        } else {
                            msg.append(i18n("Message sent to number %1, reference id=%2<br>").arg(*it).arg(i));
                            sent++;
                        }
                    }
                }
            }
        }
        theApp->modemDisconnect();


        msg.prepend(i18n("<br><b>Detailed list:</b><br>"));
        if (failed == 0) {
            if (sendCheck->isChecked()){
                msg.prepend(i18n("Sent all %1 message(s).<br>").arg(sent));
            }
            if (writeCheck->isChecked()){
                msg.prepend(i18n("Written all %1 message(s).<br>").arg(written));
            }
            KMessageBox::information(this,msg);
            accept();
        } else {
            if (sendCheck->isChecked()){
                msg.prepend(i18n("Sent %1 message(s), %2 failed during sending.<br>").arg(sent).arg(s_failed));
            }
            if (writeCheck->isChecked()){
                msg.prepend(i18n("Written %1 message(s), %2 failed during writing.<br>").arg(written).arg(w_failed));
            }
            msg.prepend(i18n("%1 message(s) failed!<br>").arg(failed));
            if (KMessageBox::questionYesNo(this,msg.append(i18n("<br>Do you want to edit that message again?"))) == KMessageBox::No)
                accept();
        }
    }
}

void EditMessageDialog::slotWriteChanged(bool on) {
    sendCheck->setEnabled(on);
    typeCombo->setEnabled(on);
}

void EditMessageDialog::slotSendChanged(bool on) {
    writeCheck->setEnabled(on);
}

void EditMessageDialog::slotTextChanged() {
    static QString prevtext;
    int para,pos;

    if (messageEdit->length() > 160 || messageEdit->paragraphs() > 1) {
        messageEdit->blockSignals(TRUE);
        messageEdit->getCursorPosition(&para, &pos);
        messageEdit->setText(prevtext);
        messageEdit->setCursorPosition(para, pos);
        messageEdit->blockSignals(FALSE);
    } else {
        prevtext = messageEdit->text();
    }
    msgInfoLabel->setText(i18n("Used %1 of %2 characters").arg(messageEdit->length()).arg(160));
}

void EditMessageDialog::selectContacts() {
    SelectContactDialog dlg(SelectContactDialog::Numbers,StorageAny, StorageAny, contacts, this);
    if (dlg.exec()) {
        sendToCombo->setEditText(sendToCombo->currentText().append(" ").append(dlg.getNumbers()));
    }
}
