/*
 * main.cpp
 *
 * code to initalize main window
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

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "kalcatel.h"

static const char *description = I18N_NOOP("KAlcatel - Application to work with Alcatel 50x and 70x mobile phone.");
	
static KCmdLineOptions options[] =
{
//  { "+[File]", I18N_NOOP("file to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

	KAboutData aboutData( "kalcatel", I18N_NOOP("KAlcatel"),
		VERSION, description, KAboutData::License_GPL,
		"(c) 2002, Michal Cihar",
		i18n("This application allows you to manage contacs, todos and calendar\n"
		"in your Alcatel One Touch 50x and 70x mobile.\n\n"
		"Protocol used by this phone is binary and not documented, so this\n"
		"program could not guarantee any functionality in your case, but it works well for me."),
		"http://cihar.TODO", "cihar@email.cz");
	aboutData.addAuthor("Michal Cihar",0, "cihar@email.cz");
	aboutData.addCredit("Alcatel", I18N_NOOP("for NOT publishing specification of protocol\n"
	    "Their only answer was:\n"
	    "\"Your piece of advice will be helpful enough for us to deliberate\n"
	    "and  get the best service for all our customers.\""),
	    "onetouchen@www.alcatel.com", "http://www.alcatel.com");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication app;
 
  if (app.isRestored())
  {
    RESTORE(KAlcatelApp);
  }
  else 
  {
    KAlcatelApp *kalcatel = new KAlcatelApp();
    kalcatel->show();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
		
		if (args->count())
		{
        kalcatel->openDocumentFile(args->arg(0));
		}
		else
		{
		  kalcatel->openDocumentFile();
		}
		args->clear();
  }

  return app.exec();
}  
