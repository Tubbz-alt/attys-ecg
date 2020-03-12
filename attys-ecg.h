/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *   hennig@cn.stir.ac.uk                                                  *
 *   Copyright (C) 2005-2019 by Bernd Porr                                 *
 *   mail@berndporr.me.uk                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ATTYS_ECG
#define ATTYS_ECG

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include "AttysComm.h"
#include "AttysScan.h"
#include <qwt_counter.h>
#include <qwt_plot_marker.h>

#include <mutex>

#include "dataplot.h"
#include "ecg_rr_det.h"
#include <Iir.h>
#include <Fir1.h>

#define IIRORDER 2

// LMS coefficients
#define HP_CUTOFF 0.25
#define LEARNING_RATE 0.00001
#define LMS_COEFF ((int)(250/HP_CUTOFF))
#define FAULT_THRES 10

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
  long unsigned int sampleNumber = 0;

  Iir::Butterworth::BandStop<IIRORDER> iirnotch1;
  Iir::Butterworth::HighPass<IIRORDER> iirhp1;

  Iir::Butterworth::BandStop<IIRORDER> iirnotch2;
  Iir::Butterworth::HighPass<IIRORDER> iirhp2;

  Iir::Butterworth::HighPass<IIRORDER> iirAcc[3];

  ECG_rr_det* rr_det1;
  ECG_rr_det* rr_det2;

  int rr_det_channel = 0;

  Fir1 **lms1;
  Fir1 **lms2;

  FILE* ecgFile = NULL;
  int recordingOn = 0;
  QString fileName;

  QPushButton* recordECG;
  QPushButton* clearBPM;
  QPushButton* saveECG;
  QComboBox* notchFreq;
  QLabel* statusLabel;
  QLabel* bpmLabel;
  QComboBox* yRange;
  QCheckBox* lmsCheckBox;

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
	  virtual void hasSample(double ts,float *data) {
		  mainwindow->hasData(ts,data);
	  };
  };
  void hasData(double t,float *sample);
  AttysCallback* attysCallback;

  struct BPMCallback : ECG_rr_det::RRlistener {
	  MainWindow* mainwindow;
	  ECG_rr_det* det;
	  BPMCallback(MainWindow* _mainwindow, ECG_rr_det* _det) {
		  mainwindow = _mainwindow;
		  det = _det;
	  };
	  void hasRpeak(long samplenumber,
			float bpm,
			double amplitude,
			double confidence) {
		  mainwindow->hasRpeak(det,samplenumber,bpm,amplitude,confidence);
	  };
  };

  void hasRpeak(ECG_rr_det* det,
		long samplenumber,
		float bpm,
		double amplitude,
		double confidence);
  BPMCallback* bPMCallback1;
  BPMCallback* bPMCallback2;
  
  void setNotch(double f);

signals:
  void sweepStarted(int);

public:

  MainWindow( QWidget *parent=0 );
  ~MainWindow();

private:
	void clearAllRingbuffers() {
		attysScan.getAttysComm(0)->resetRingbuffer();
	}

	double start_time = 0;

	int current_secs = 0;

};

#endif
