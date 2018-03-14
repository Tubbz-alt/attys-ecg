/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *   hennig@cn.stir.ac.uk                                                  *
 *   Copyright (C) 2005-2018 by Bernd Porr                                 *
 *   mail@berndporr.me.uk                                                  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "attys-ecg.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QComboBox>
#include <QTimer>

#include "AttysComm.h"
#include "AttysScan.h"

MainWindow::MainWindow( QWidget *parent ) :
    QWidget(parent) {

	attysScan.attysComm[0]->setAdc_samplingrate_index(AttysComm::ADC_RATE_250HZ);
	sampling_rate = attysScan.attysComm[0]->getSamplingRateInHz();

	attysCallback = new AttysCallback(this);
	attysScan.attysComm[0]->registerCallback(attysCallback);

	// set the PGA to max gain
	attysScan.attysComm[0]->setAdc0_gain_index(AttysComm::ADC_GAIN_6);
	
	// 50Hz or 60Hz mains notch filter (see header)
	iirnotch1 = new Iir::Butterworth::BandStop<IIRORDER>;
	assert( iirnotch1 != NULL );
	iirnotch1->setup (IIRORDER, sampling_rate, NOTCH_F, 2.5);
	iirnotch2 = new Iir::Butterworth::BandStop<IIRORDER>;
	assert( iirnotch2 != NULL );
	iirnotch2->setup (IIRORDER, sampling_rate, NOTCH_F, 2.5);

	// highpass
	iirhp1 = new Iir::Butterworth::HighPass<2>;
	assert( iirhp1 != NULL );
	iirhp1->setup (IIRORDER, sampling_rate, 0.5);
	iirhp2 = new Iir::Butterworth::HighPass<2>;
	assert( iirhp2 != NULL );
	iirhp2->setup (IIRORDER, sampling_rate, 0.5);

	char styleSheet[] = "padding:0px;margin:0px;border:0px;";

	QHBoxLayout *mainLayout = new QHBoxLayout( this );

	QVBoxLayout *controlLayout = new QVBoxLayout;

	mainLayout->addLayout(controlLayout);
	
	QVBoxLayout *plotLayout = new QVBoxLayout;

	mainLayout->addLayout(plotLayout);
	
	mainLayout->setStretchFactor(controlLayout,1);
	mainLayout->setStretchFactor(plotLayout,4);

	double maxTime = 5;
	double minRange = -2;
	double maxRange = 2;
	dataPlotI = new DataPlot(maxTime,
				 sampling_rate,
				 minRange,
				 maxRange,
				 "I",
				 this);
	dataPlotI->setMaximumSize(10000,300);
	dataPlotI->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotI);
	dataPlotI->show();

	dataPlotII = new DataPlot(maxTime,
				  sampling_rate,
				  minRange,
				  maxRange,
				  "II",
				  this);
	dataPlotII->setMaximumSize(10000,300);
	dataPlotII->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotII);
	dataPlotII->show();
	
	dataPlotIII = new DataPlot(maxTime,
				   sampling_rate,
				   minRange,
				   maxRange,
				   "III",
				   this);
	dataPlotIII->setMaximumSize(10000,300);
	dataPlotIII->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotIII);
	dataPlotIII->show();

	plotLayout->addSpacing(20);


	/*---- Buttons ----*/

	QGroupBox   *ECGfunGroup  = new QGroupBox( "Actions", this );
	QVBoxLayout *ecgFunLayout = new QVBoxLayout;

	ECGfunGroup->setLayout(ecgFunLayout);
	ECGfunGroup->setAlignment(Qt::AlignJustify);
	ECGfunGroup->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
	controlLayout->addWidget( ECGfunGroup );

	recordECG = new QPushButton(ECGfunGroup);
	recordECG->setText("Rec on/off");
	recordECG->setCheckable(true);
	ecgFunLayout->addWidget(recordECG);
	connect(recordECG, SIGNAL(clicked()), SLOT(slotRunECG()));

	clearECG = new QPushButton(ECGfunGroup);
	clearECG->setText("clear data");
	ecgFunLayout->addWidget(clearECG);
	connect(clearECG, SIGNAL(clicked()), SLOT(slotClearECG()));
	
	saveECG = new QPushButton(ECGfunGroup);
	saveECG->setText("save data");
	ecgFunLayout->addWidget(saveECG);
	connect(saveECG, SIGNAL(clicked()), SLOT(slotSaveECG()));

	// Generate timer event every 50ms
	startTimer(50);

	attysScan.attysComm[0]->start();
}

MainWindow::~MainWindow()
{
	attysScan.attysComm[0]->unregisterCallback();
	attysScan.attysComm[0]->quit();
}

void MainWindow::slotSaveECG()
{
	QString fileName = QFileDialog::getSaveFileName();
	
	if( !fileName.isNull() )
	{
		//
	}
}

void MainWindow::slotClearECG()
{
	sampleNumber = 0;
}

void MainWindow::slotRunECG()
{
	// toggle ECG recording
	if(!recordingOn)
	{
		recordingOn = 1;
	}
	else
	{
		recordingOn = 0;
	}
}

void MainWindow::timerEvent(QTimerEvent *) {
	dataPlotI->replot();
	dataPlotII->replot();
	dataPlotIII->replot();
}


void MainWindow::hasData(float,float *sample)
{
	float y1 = sample[AttysComm::INDEX_Analogue_channel_1];
	float y2 = sample[AttysComm::INDEX_Analogue_channel_2];

	// highpass filtering of the data
	y1=iirhp1->filter(y1);
	y2=iirhp1->filter(y2);

	// removing 50Hz notch
	II=iirnotch1->filter(y1);
	III=iirnotch1->filter(y2);

	I = II - III;	
	aVR = III / 2 - II;
	aVL = II / 2 - III;
	aVF = II / 2 + III / 2;
	
	// plot the data
	dataPlotI->setNewData(I);
	dataPlotII->setNewData(II);
	dataPlotIII->setNewData(III);

	// Are we recording?
	if( ecgFile )
	{
		char s = '\t';
		double t = (double)sampleNumber / sampling_rate;
		fprintf(ecgFile, "%e%c", t, s);
                fprintf(ecgFile, "%e%c", I, s);
                fprintf(ecgFile, "%e%c", II, s);
                fprintf(ecgFile, "%e%c", III, s);
                fprintf(ecgFile, "%e%c", aVR, s);
                fprintf(ecgFile, "%e%c", aVL, s);
                fprintf(ecgFile, "%e%c", aVF, s);
                fprintf(ecgFile, "%f", bpm);
	}
    
	sampleNumber++;
}
