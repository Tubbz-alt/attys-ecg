/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *   hennig@cn.stir.ac.uk                                                  *
 *   Copyright (C) 2018 by Bernd Porr, mail@berndporr.me.uk                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dataplot.h"

DataPlot::DataPlot(double _maxTime,
		   double _samplingRate,
		   double _minY,
		   double _maxY,
		   const char* title,
		   const char* xAxisLabel,
		   const char* yAxisLabel,
		   QWidget *parent) :
	QwtPlot(parent)
{
	minY = _minY;
	maxY = _maxY;
	maxTime = _maxTime;
	samplingRate = _samplingRate;
	length = maxTime * samplingRate;
	xData = new double[length];
	yData = new double[length];
	reset();
		
	setAxisScale(QwtPlot::yLeft,minY,maxY);

	setTitle(title);
	setAxisTitle(QwtPlot::xBottom, xAxisLabel);
	setAxisTitle(QwtPlot::yLeft, yAxisLabel);

	// Insert new curve for raw data
	dataCurve = new QwtPlotCurve(title);
	dataCurve->setPen( QPen(Qt::blue, 2) );
	dataCurve->setRawSamples(xData, yData, length);
	dataCurve->attach(this);
}

void DataPlot::reset() {
	for(int i=0; i<length; i++) {
                xData[i] = i/samplingRate;
                yData[i] = 0;
	}
}

void DataPlot::setNewData(double yNew) {
	memmove( yData, yData+1, (length - 1) * sizeof(yData[0]) );
	yData[length-1] = yNew;
}
