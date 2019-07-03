#include "ecg_rr_det.h"

#include <math.h>
#include <assert.h>

// wavelet for 250Hz sampling rate
static const double waveletDB3[] = {
	1.10265752e-02,
	2.67449277e-02,
	4.19878574e-02,
	6.03947231e-02,
	7.61275365e-02,
	9.21548684e-02,
	1.11568926e-01,
	1.32278887e-01,
	6.45829680e-02,
	-3.97635130e-02,
	-1.38929884e-01,
	-2.62428322e-01,
	-3.62246804e-01,
	-4.62843343e-01,
	-5.89607507e-01,
	-7.25363076e-01,
	-3.36865858e-01,
	2.67715108e-01,
	8.40176767e-01,
	1.55574430e+00,
	1.18688954e+00,
	4.20276324e-01,
	-1.51697311e-01,
	-9.42076108e-01,
	-7.93172332e-01,
	-3.26343710e-01,
	-1.24552779e-01,
	2.12909254e-01,
	1.75770320e-01,
	1.47523075e-02,
	8.22192707e-03,
	-3.02920592e-02,
	-2.21119497e-02,
	7.30703025e-03,
	2.83200488e-03,
	-1.16759765e-03
};

ECG_rr_det::ECG_rr_det() {
	ecgDetector = new Fir1(waveletDB3);
	assert(NULL != ecgDetector);
	reset();
}

ECG_rr_det::~ECG_rr_det() {
	delete ecgDetector;
}


void ECG_rr_det::reset() {
        amplitude = 0;
        t2 = 0;
        timestamp = 0;
        doNotDetect = (int) samplingRateInHz;
        ignoreECGdetector = (int) samplingRateInHz;
}

// detect r peaks
// input: ECG samples at the specified sampling rate and in V
void ECG_rr_det::detect(float v) {
	double h = 1000 * v;
	h = ecgDetector->filter(h);
	if (ignoreECGdetector > 0) {
		ignoreECGdetector--;
		return;
	}
	h = h * h;
	if (sqrt(h) > artefact_threshold) {
		// ignore signal for 1 sec
		ignoreECGdetector = ((int) samplingRateInHz);
		//Log.d(TAG,"artefact="+(Math.sqrt(h)));
		ignoreRRvalue = 2;
		return;
	}
	if (h > amplitude) {
		amplitude = h;
	}
	amplitude = amplitude - adaptive_threshold_decay_constant * amplitude / samplingRateInHz;
	
	if (doNotDetect > 0) {
		doNotDetect--;
	} else {
		double threshold = threshold_factor * amplitude;
		if (h > threshold) {
			float t = (timestamp - t2) / samplingRateInHz;
			float bpm = 1 / t * 60;
			if ((bpm > 30) && (bpm < 250)) {
				if (ignoreRRvalue > 0) {
					ignoreRRvalue--;
				} else {
					if (bpm > 0) {
						if (((bpm * 1.5) < prevBPM) || ((bpm * 0.75) > prevBPM)) {
							ignoreRRvalue = 3;
						} else {
							if (rrListener != NULL) {
								rrListener->hasRpeak(timestamp,
										     bpm,
										     amplitude, h / threshold);
							}
						}
						prevBPM = bpm;
					}
				}
			} else {
				ignoreRRvalue = 3;
			}
			t2 = timestamp;
			// advoid 1/5 sec
			doNotDetect = (int) samplingRateInHz / 5;
		}
	}
	timestamp++;
}
