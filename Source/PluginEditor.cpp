/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayEffectAudioProcessorEditor::DelayEffectAudioProcessorEditor (DelayEffectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 250);
    
    delayKnob.setBounds(100, 50, 150, 150);
    delayKnob.setCentrePosition(250, 170);
    delayKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 75, 25);
    delayKnob.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    delayKnob.setTextValueSuffix("ms");
    addAndMakeVisible(delayKnob);
    
    sliderAttachments.emplace_back(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.state, "delayMSValue", delayKnob));
    
    noteChoice.addListener(this);
    noteChoice.addItem("Half", 1);
    noteChoice.addItem("Quarter", 2);
    noteChoice.addItem("8th", 3);
    noteChoice.addItem("16th", 4);
    noteChoice.addItem("32nd", 5);
    noteChoice.setBounds(325, 25, 120, 40);
    noteChoice.setCentrePosition(250, 25);
    addAndMakeVisible(noteChoice);
    
    comboBoxAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.state, "noteValue", noteChoice);
    
    tempoButton.setBounds(200, 55, 100, 40);
    tempoButton.setColour(TextButton::buttonColourId, Colours::red);
    tempoButton.setColour(TextButton::buttonOnColourId, Colours::green);
    tempoButton.addListener(this);
    tempoButton.setClickingTogglesState(true);
    tempoButton.onClick = [this]() { };
    tempoButton.setButtonText("Tempo Sync");
    addAndMakeVisible(tempoButton);
    
    buttonAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.state, "tempoSync", tempoButton);
    
    feedbackKnob.setBounds(50, 50, 110, 110);
    feedbackKnob.setCentrePosition(100, 125);
    feedbackKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 75, 25);
    feedbackKnob.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(feedbackKnob);
    
    sliderAttachments.emplace_back(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.state, "feedbackValue", feedbackKnob));
    
    lowPassKnob.setBounds(50, 50, 110, 110);
    lowPassKnob.setCentrePosition(400, 125);
    lowPassKnob.setTextValueSuffix("Hz");
    lowPassKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 75, 25);
    lowPassKnob.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(lowPassKnob);
    
    sliderAttachments.emplace_back(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.state, "lowPassValue", lowPassKnob));
    
    delayKnob.setEnabled(!tempoButton.getToggleState());
    noteChoice.setEnabled(tempoButton.getToggleState());
    
}

DelayEffectAudioProcessorEditor::~DelayEffectAudioProcessorEditor()
{
}

//==============================================================================
void DelayEffectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    g.drawText("Feedback", 70, 55, 100, 25, NULL);
    
    g.drawText("LPF", 388, 55, 100, 25, NULL);
}

void DelayEffectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void DelayEffectAudioProcessorEditor::comboBoxChanged(ComboBox *comboBox){
   
    if (comboBox == &noteChoice){
        if(noteChoice.getSelectedId() == 1){
            // half note
            audioProcessor.noteDuration = 2.f;
        }
        if(noteChoice.getSelectedId() == 2){
            // quarter note
            audioProcessor.noteDuration = 1.f;
        }
        if(noteChoice.getSelectedId() == 3){
            // 8th note
            audioProcessor.noteDuration = 0.5f;
        }
        if(noteChoice.getSelectedId() == 4){
            // 16th note
            audioProcessor.noteDuration = 0.25f;
        }
        if(noteChoice.getSelectedId() == 5){
            // 32nd note
            audioProcessor.noteDuration = 0.125f;
        }
    }
}

void DelayEffectAudioProcessorEditor::buttonClicked(Button * button){
    
    if (button == &tempoButton){
        delayKnob.setEnabled(!tempoButton.getToggleState());
        noteChoice.setEnabled(tempoButton.getToggleState());
    }
    
}
