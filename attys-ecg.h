/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *   hennig@cn.stir.ac.uk                                                  *
 *   Copyright (C) 2005-2018 by Bernd Porr                                      *
 *   mail@berndporr.me.uk                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ATTYS_EP
#define ATTYS_EP

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>

#include "AttysComm.h"
#include <qwt_counter.h>
#include <qwt_plot_marker.h>

#include "dataplot.h"
#include <Iir.h>

#define NOTCH_F 50 // filter out 50Hz noise
#define IIRORDER 2

class MainWindow : public QWidget
{
  Q_OBJECT
    
  // show the raw serai data here
  DataPlot *dataPlotI;
  DataPlot *dataPlotII;
  DataPlot *dataPlotIII;

  double I,II,III;
  double aVR,aVL,aVF;
  double bpm;
  
  // sampling rate
  double sampling_rate;

  // time counter
  long int sampleNumber = 0;

  Iir::Butterworth::BandStop<IIRORDER>* iirnotch1;
  Iir::Butterworth::HighPass<IIRORDER>* iirhp1;

  Iir::Butterworth::BandStop<IIRORDER>* iirnotch2;
  Iir::Butterworth::HighPass<IIRORDER>* iirhp2;

  FILE* ecgFile = NULL;
  int recordingOn = 0;

  QPushButton* recordECG;
  QPushButton* clearECG;
  QPushButton* saveECG;

private slots:

  // actions:
  void slotClearECG();
  void slotRunECG();
  void slotSaveECG();

protected:

  /// timer to plot the data
  virtual void timerEvent(QTimerEvent *e);

  struct AttysCallback : AttysComm::CallbackInterface {
	  MainWindow* mainwindow;
	  AttysCallback(MainWindow* _mainwindow) { mainwindow = _mainwindow; };
	  virtual void hasSample(float ts,float *data) {
		  mainwindow->hasData(ts,data);
	  };
  };

  void hasData(float,float *sample);

  AttysCallback* attysCallback;

  void initData();

signals:
  void sweepStarted(int);

public:

  MainWindow( QWidget *parent=0 );
  ~MainWindow();

};

#endif
