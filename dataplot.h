/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *                 2018 by Bernd Porr
 *   hennig@cn.stir.ac.uk                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DATAPLOT_H
#define DATAPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

// in samples (1sec)
#define SCALE_UPDATE_PERIOD 250

/// this plot shows the raw input data (spikes or membrane potential)
class DataPlot : public QwtPlot
{
private:
	// internal databuffer
	double* xData;
	double* yData;
	
	// number of data points
	int length;

	// sampling Rate
	double samplingRate;

	// max time on the x axis
	double maxTime;
	
	// curve object
	QwtPlotCurve *dataCurve;

	// ranges
	double maxY,minY;
	
	int updateCtr;

public:

	DataPlot(double _maxtime,
		 double _samplingRate,
		 double _minY,
		 double _maxY,
		 const char* title,
		 QWidget *parent = 0);
	
	void setLength(int length);
	
	void setNewData(double yNew);

};

#endif
