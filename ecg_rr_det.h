#ifndef ECG_RR_DET_H
#define ECG_RR_DET_H

#include <Iir.h>

#define IIRORDER 2


class ECG_rr_det {

    // how fast the adaptive threshold follows changes in ECG
    // amplitude. Realisic values: 0.1 .. 1.0
    // 0.1 = slow recovery after an artefact but no wrong detections
    // 1 = fast recovery after an artefact but possibly wrong detections
    float adaptive_threshold_decay_constant = 0.25F;

    // the threshold for the detection is 0.6 times smaller than the amplitude
    float threshold_factor = 0.6F;

    // bandwidth of the powerline filter
    double notchBW = 2.5; // Hz

    // notch order of the powerline filter
    int notchOrder = 2;

    // 10mV as the threshold the bandpass filtered ECG is an artefact
    double artefact_threshold = 10;

    // ignores 1000 samples to let the filter settle
    int ignoreECGdetector = 1000;

    // adaptive amplitude value of the detector output
    double amplitude = 0.0;

public:
    double getAmplitude() {
        return amplitude;
    }

private:
    long timestamp = 0;

    // previous timestamp
    long t2 = 0;

    // previously detected heartrate
    float prevBPM = 0;

    // timewindow not to detect an R peak
    int doNotDetect = 0;

    int ignoreRRvalue = 2;

    // three HR's are stored and a median filter is applied to them
    float* hrBuffer;
    float* sortBuffer;

    // the R preak detector. This is a matched filter implemented as IIR
    Iir::Butterworth::BandPass<IIRORDER>* ecgDetector;

    // sampling rate in Hz
    float samplingRateInHz;

    // heartrate in BPM after median filtering (3 bpm readings)
    float filtBPM = 0;

    // heartrate in BPM without median filtering (might have 1/2 bpm readings)
    float unfiltBPM = 0;

    int medianFilterSize;

public:
    // constructor
    // provide the sampling rate and the median filter size
    ECG_rr_det(float _samplingrateInHz, int _medianFilterSize) {
        init(_samplingrateInHz, _medianFilterSize);
    }

    // constructor
    // provide the sampling rate
    ECG_rr_det(float _samplingrateInHz) {
        init(_samplingrateInHz, 5);
    }

private:
    void init(float _samplingrateInHz, int _medianFilterSize);

public:
    // this is a callback which is called whenever an R peak is detected
    // gives back the sample number from last reset, the heartrate in bpm (filtered and unfiltered),
    // the amplitude of the R peak in arbitrary units and the confidence of the
    // detection: 1 means just OK, greater than one means more confident
    class RRlistener {
    public:
	    virtual void hasRpeak(long samplenumber,
				  float filtBpm,
				  float unFiltBpm,
				  double amplitude,
				  double confidence) = 0;
    };

private:
    RRlistener* rrListener = NULL;

public:
    void setRrListener(RRlistener* _rrListener) {
	    rrListener = _rrListener;
    }

    // reset detector
    void reset();

    float getFiltBPM() {
        return filtBPM;
    }

    float getUnFiltBPM() {
        return unfiltBPM;
    }

    // detect r peaks
    // input: ECG samples at the specified sampling rate and in V
    void detect(float v);

};


#endif
