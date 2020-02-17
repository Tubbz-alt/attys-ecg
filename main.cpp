/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *             (C) 2013-2018 by Bernd Porr                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "attys-ecg.h"
#include "AttysComm.h"
#include "AttysScan.h"

#include <QApplication>
#include <QSplashScreen>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QPixmap pixmap(":/attys.png");
  QSplashScreen* splash = new QSplashScreen(pixmap);
  splash->setFont(QFont("Helvetica", 10));
  splash->show();
  splash->showMessage("Scanning for paired devices");
  app.processEvents();

  // see if we have any Attys!
  int ret = attysScan.scan(1);
  
  // zero on success and non zero on failure
  if (ret) {
	  app.processEvents();
	  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	  delete splash;
	  return ret;
  }
        
  // none detected
  if (attysScan.nAttysDevices<1) {
	  printf("No Attys present or not paired.\n");
	  splash->showMessage("Cound not connect\nand/or no devices paired.");
	  app.processEvents();
	  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	  delete splash;
	  return -1;
  }

  MainWindow mainWindow;
  mainWindow.show();
  splash->finish(&mainWindow);

  return app.exec();
}
