/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayEffect.h"

//==============================================================================
/**
*/
class DelayEffectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayEffectAudioProcessor();
    ~DelayEffectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float noteDuration = 1.f;
    
    Biquad filter = Biquad{Biquad::FilterType::LPF,0.7071f};
    
    AudioProcessorValueTreeState state;
    
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    bool buttonValue = *state.getRawParameterValue("tempoSync");
    
private:
    
    DelayEffect delay;
    
    AudioPlayHead* playHead;
    
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    
    float bpm = 120.f;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayEffectAudioProcessor)
};
