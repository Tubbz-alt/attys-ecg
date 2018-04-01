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
#include <QLabel>

#include "AttysComm.h"
#include <qwt_counter.h>
#include <qwt_plot_marker.h>

#include <mutex>

#include "dataplot.h"
#include "ecg_rr_det.h"
#include <Iir.h>

#define IIRORDER 2

class MainWindow : public QWidget
{
  Q_OBJECT
    
  // show the raw serai data here
  DataPlot *dataPlotI;
  DataPlot *dataPlotII;
  DataPlot *dataPlotIII;
  DataPlot *dataPlotBPM;

  DataPlot *dataPlotAccX;
  DataPlot *dataPlotAccY;
  DataPlot *dataPlotAccZ;

  double minAcc;
  double maxAcc;

  double I,II,III;
  double aVR,aVL,aVF;
  double bpm;

  double accX, accY, accZ;
  
  // sampling rate
  double sampling_rate;

  // time counter
  long int sampleNumber = 0;

  Iir::Butterworth::BandStop<IIRORDER>* iirnotch1;
  Iir::Butterworth::HighPass<IIRORDER>* iirhp1;

  Iir::Butterworth::BandStop<IIRORDER>* iirnotch2;
  Iir::Butterworth::HighPass<IIRORDER>* iirhp2;

  ECG_rr_det* rr_det;

  FILE* ecgFile = NULL;
  int recordingOn = 0;
  double tRec = 0;
  QString fileName;

  QPushButton* recordECG;
  QPushButton* clearBPM;
  QPushButton* saveECG;
  QComboBox* notchFreq;
  QLabel* statusLabel;
  QLabel* bpmLabel;
  QComboBox* yRange;

  std::mutex saveFileMutex;

private slots:

  // actions:
  void slotClearBPM();
  void slotRecordECG();
  void slotSaveECG();
  void slotSelectNotchFreq(int);
  void slotSelectYrange(int);

protected:

  /// timer to plot the data
  virtual void timerEvent(QTimerEvent *e);

  struct AttysCallback : AttysCommListener {
	  MainWindow* mainwindow;
	  AttysCallback(MainWindow* _mainwindow) { mainwindow = _mainwindow; };
	  virtual void hasSample(float ts,float *data) {
		  mainwindow->hasData(ts,data);
	  };
  };
  void hasData(float,float *sample);
  AttysCallback* attysCallback;

  struct BPMCallback : ECG_rr_det::RRlistener {
	  MainWindow* mainwindow;
	  BPMCallback(MainWindow* _mainwindow) { mainwindow = _mainwindow; };
	  void hasRpeak(long samplenumber,
			float filtBpm,
			float unFiltBpm,
			double amplitude,
			double confidence) {
		  mainwindow->hasRpeak(samplenumber,filtBpm,unFiltBpm,amplitude,confidence);
	  };
  };
  void hasRpeak(long samplenumber,
		float filtBpm,
		float unFiltBpm,
		double amplitude,
		double confidence);
  BPMCallback* bPMCallback;


  void setNotch(double f);

signals:
  void sweepStarted(int);

public:

  MainWindow( QWidget *parent=0 );
  ~MainWindow();

};

#endif
