/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DelayEffectAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         public juce::ComboBox::Listener,
                                         public juce::Button::Listener
{
public:
    DelayEffectAudioProcessorEditor (DelayEffectAudioProcessor&);
    ~DelayEffectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void comboBoxChanged(ComboBox * comboBox) override;
    
    void buttonClicked(Button * button) override;
    
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayEffectAudioProcessor& audioProcessor;
    
    Slider delayKnob;
    
    Slider feedbackKnob;
    
    Slider lowPassKnob;
    
    std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    
    ComboBox noteChoice;
    
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment;
        
    TextButton tempoButton;
        
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayEffectAudioProcessorEditor)
};
