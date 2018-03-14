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

	ecgLength = DEFAULT_SWEEP_LENGTH / (1000 / sampling_rate);

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

	initData();

	char styleSheet[] = "padding:0px;margin:0px;border:0px;";

	QHBoxLayout *mainLayout = new QHBoxLayout( this );

	QVBoxLayout *controlLayout = new QVBoxLayout;

	mainLayout->addLayout(controlLayout);
	
	QVBoxLayout *plotLayout = new QVBoxLayout;

	mainLayout->addLayout(plotLayout);
	
	mainLayout->setStretchFactor(controlLayout,1);
	mainLayout->setStretchFactor(plotLayout,4);
	
	// two plots
	RawDataPlot = new DataPlot(xData, yData, ecgLength, 
				   attysScan.attysComm[0]->getADCFullScaleRange(0),
				   -attysScan.attysComm[0]->getADCFullScaleRange(0),
				   this);
	RawDataPlot->setMaximumSize(10000,300);
	RawDataPlot->setStyleSheet(styleSheet);
	plotLayout->addWidget(RawDataPlot);
	RawDataPlot->show();

	plotLayout->addSpacing(20);


	/*---- Buttons ----*/

	// psth functions
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

void MainWindow::initData() {
	//  Initialize data for plots
	for(int i=0; i<MAX_ECG_LENGTH; i++)
	{
		xData[i] = i/(double)sampling_rate*1000;
		yData[i] = 0;
	}
}

void MainWindow::slotSaveECG()
{
	QString fileName = QFileDialog::getSaveFileName();
	
	if( !fileName.isNull() )
	{
		// todo
	}
}

void MainWindow::slotClearECG()
{
	time = 0;
	initData();
}

void MainWindow::slotRunECG()
{
	// toggle ECG recording
	if(recordingOn == 0)
	{
		recordingOn = 1;
	}
	else
	{
		recordingOn = 0;
	}
}

void MainWindow::timerEvent(QTimerEvent *) {
	RawDataPlot->replot();
}


void MainWindow::hasData(float,float *sample)
{
	float y1 = sample[AttysComm::INDEX_Analogue_channel_1];
	float y2 = sample[AttysComm::INDEX_Analogue_channel_2];

	// highpass filtering of the data
	y1=iirhp1->filter(y1);
	y2=iirhp1->filter(y2);

	// removing 50Hz notch
	y1=iirnotch1->filter(y1);
	y2=iirnotch1->filter(y2);

	// plot the data
	RawDataPlot->setNewData(y1);

	// Are we recording?
	if( recordingOn )
	{
		// average the ECG
	}
    
	time++;
}
