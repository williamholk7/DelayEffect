/*
  ==============================================================================

    DelayEffect.h
    Created: 3 Mar 2021 11:39:11pm
    Author:  William Holk

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Biquad.h"

class DelayEffect{

public:
 
    DelayEffect();
    
    float processSample(float x, int c);
    
    void prepare(float newFs);
    
    void setDelayMS(float newDelayMS);
    
    void setBPM(float newBPM);
    
    void setNoteDuration(float newNoteDuration);
    
    void setFeedbackGain(float newFeedbackGain);
    
    void setLowPassFreq(float newLowPassFreq);
    
private:
    
    float Fs = 48000.f;
    
    float delayMS = 500.f;
    
    float smoothDelay = round(Fs*delayMS/1000.f);
    
    int delaySamples = round(Fs*delayMS/1000.f);
    
    float bpm = 120.f;
    
    float noteDuration = 1.f; // 1 - quarter, 2 - half, 0.5 - 8th, 0.25 - 16th, 0.125 - 16th
    
    float smoothFeedback = 0.0f;
    
    float feedbackGain = 0.0f;
    
    float lowPassFreq = 20000.f;
    
    Biquad filter = Biquad{Biquad::FilterType::LPF,0.7071f};
    
    static const int BUFFERSIZE = 96000;
    float w[2][BUFFERSIZE] = {0.f};
    int writeIndex[2] = {47999,47999};
    
};
