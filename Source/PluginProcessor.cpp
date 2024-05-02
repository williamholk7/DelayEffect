/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayEffectAudioProcessor::DelayEffectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), state(*this, nullptr, "Params",
                                createParameterLayout())
#endif
{
}

DelayEffectAudioProcessor::~DelayEffectAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout DelayEffectAudioProcessor::createParameterLayout(){
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<AudioParameterFloat> ("delayMSValue", "Delay", 10.f, 1000.f, 1.f));
    
    params.push_back(std::make_unique<AudioParameterFloat>("feedbackValue", "Feedback", 0.f, 1.f,0.5f));
    
    auto normRange = NormalisableRange<float>(20.0f, 20000.f);
    normRange.setSkewForCentre(2000.0f);
    
    params.push_back(std::make_unique<AudioParameterFloat>("lowPassValue", "LPF", normRange, 20000.f));
    
    params.push_back(std::make_unique<AudioParameterBool>("tempoSync", "Tempo Sync", false));
    
    params.push_back(std::make_unique<AudioParameterChoice>("noteValue", "Note Value", StringArray ("Half", "Quarter", "8th", "16th", "32nd"), 1));
    
    return {params.begin() , params.end()};
}

//==============================================================================
const juce::String DelayEffectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayEffectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayEffectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayEffectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayEffectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayEffectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayEffectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayEffectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayEffectAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayEffectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayEffectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    delay.prepare(sampleRate);
}

void DelayEffectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayEffectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DelayEffectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    buttonValue = *state.getRawParameterValue("tempoSync");
    
    if (buttonValue){
        playHead = this->getPlayHead();
        playHead->getCurrentPosition(currentPositionInfo);
        
        float newBPM = currentPositionInfo.bpm;
        if (bpm != newBPM){
            // update echo
            delay.setBPM(newBPM);
            bpm = newBPM;
        }
        
        delay.setNoteDuration(noteDuration);
    }
    else{ // not tempo sync'd
        float delayMS = *state.getRawParameterValue("delayMSValue");
        delay.setDelayMS(delayMS);
    }
    
    float feedbackGain = *state.getRawParameterValue("feedbackValue");
    delay.setFeedbackGain(feedbackGain);
    
    float lowPassFreq = *state.getRawParameterValue("lowPassValue");
    delay.setLowPassFreq(lowPassFreq);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        for (int n = 0; n < buffer.getNumSamples() ; ++n){
            float x = buffer.getReadPointer(channel)[n];
            x = delay.processSample(x, channel);
            buffer.getWritePointer(channel)[n] = x;
        }
    }
}

//==============================================================================
bool DelayEffectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayEffectAudioProcessor::createEditor()
{
    return new DelayEffectAudioProcessorEditor (*this);
}

//==============================================================================
void DelayEffectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto currentState = state.copyState();
    std::unique_ptr<XmlElement> xml (currentState.createXml());
    copyXmlToBinary(*xml, destData);
    
}

void DelayEffectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<XmlElement> xml (getXmlFromBinary(data, sizeInBytes));
    if(xml && xml->hasTagName(state.state.getType())){
        state.replaceState(ValueTree::fromXml(*xml));
    }
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayEffectAudioProcessor();
}
