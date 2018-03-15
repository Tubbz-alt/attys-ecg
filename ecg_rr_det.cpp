#include "ecg_rr_det.h"

#include <math.h>
#include <algorithm>

void ECG_rr_det::init(float _samplingrateInHz, int _medianFilterSize) {
	samplingRateInHz = _samplingrateInHz;
	medianFilterSize = _medianFilterSize;
	hrBuffer = new float[medianFilterSize];
	sortBuffer = new float[medianFilterSize];
	// this fakes an R peak so we have a matched filter!
	ecgDetector = new Iir::Butterworth::BandPass<IIRORDER>;
	ecgDetector->setup(2, samplingRateInHz, 20, 15);
	reset();
}

void ECG_rr_det::reset() {
        amplitude = 0;
        t2 = 0;
        timestamp = 0;
        doNotDetect = (int) samplingRateInHz;
        ignoreECGdetector = (int) samplingRateInHz;
        for (int i=0; i < medianFilterSize; i++){
            hrBuffer[i] = 0;
        }
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
	// above 0.5mV
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
					for (int i = 0; i < (medianFilterSize - 1); i++) {
						hrBuffer[i + 1] = hrBuffer[i];
					}
					hrBuffer[0] = bpm;
					unfiltBPM = bpm;
					for (int i = 0; i < medianFilterSize; i++) {
						sortBuffer[i] = hrBuffer[i];
					}
					std::sort(sortBuffer,sortBuffer+medianFilterSize);
					//Arrays.sort(sortBuffer);
					filtBPM = sortBuffer[(int) floor(medianFilterSize / 2)];
					if (filtBPM > 0) {
						// still missed a heartbeat?
						if (abs(filtBPM*2-prevBPM)<5) {
							// that's most likely a missed heartbeat because it's
							// exactly half of the previous heartrate
							ignoreRRvalue = 3;
						} else {
							if (rrListener != NULL) {
								rrListener->hasRpeak(timestamp,
										     filtBPM,
										     unfiltBPM,
										     amplitude, h / threshold);
							}
						}
						prevBPM = filtBPM;
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
