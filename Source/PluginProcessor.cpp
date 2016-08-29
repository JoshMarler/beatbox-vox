/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BeatboxVoxAudioProcessor::BeatboxVoxAudioProcessor() 
    : spectralCentroid(0.0f),
      classifier(256, 44800, 2)
      
{ 
    initialiseSynth(); 
}

BeatboxVoxAudioProcessor::~BeatboxVoxAudioProcessor()
{
}

//==============================================================================
const String BeatboxVoxAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BeatboxVoxAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BeatboxVoxAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double BeatboxVoxAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BeatboxVoxAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BeatboxVoxAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BeatboxVoxAudioProcessor::setCurrentProgram (int index)
{
}

const String BeatboxVoxAudioProcessor::getProgramName (int index)
{
    return String();
}

void BeatboxVoxAudioProcessor::changeProgramName (int index, const String& newName)
{
}

void BeatboxVoxAudioProcessor::initialiseSynth ()
{
    /** JWM - Quick and dirty sample drum synth for prototype
     *  In future versions will ideally allow user to select sample to use and
     *  also manage sample rate changes effect on loaded samples. 
    */

    WavAudioFormat wavFormat;
    BigInteger kickNoteRange;
    BigInteger snareNoteRange;

    drumSynth.clearSounds();

    std::unique_ptr<AudioFormatReader> readerKickDrum(wavFormat.createReaderFor(new MemoryInputStream(BinaryData::bassdrum_wav,
                                                                                                      BinaryData::bassdrum_wavSize,
                                                                                                      false),
                                                                                                      true));

    std::unique_ptr<AudioFormatReader> readerSnareDrum(wavFormat.createReaderFor(new MemoryInputStream(BinaryData::snaredrum_wav,
                                                                                                       BinaryData::snaredrum_wavSize,
                                                                                                       false),
                                                                                                       true));
    kickNoteRange.setBit(12);
    snareNoteRange.setBit(43);
    
    //kickNoteRange.setRange(0, 64, true);
    //snareNoteRange.setRange(65, 63, true);
    
    drumSynth.addSound(new SamplerSound("Kick Sound", *readerKickDrum, kickNoteRange, 12, 0.0, 0.1, 5.0));
    drumSynth.addSound(new SamplerSound("Snare Sound", *readerSnareDrum, snareNoteRange, 43, 0.0, 0.1, 5.0));

    drumSynth.addVoice(new SamplerVoice());
}

//==============================================================================
void BeatboxVoxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    drumSynth.setCurrentPlaybackSampleRate(sampleRate);

    classifier.setCurrentBufferSize(samplesPerBlock);
    classifier.setCurrentSampleRate(sampleRate);

}

void BeatboxVoxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
#ifndef JucePlugin_PreferredChannelConfigurations
bool BeatboxVoxAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet)
{
    // Reject any bus arrangements that are not compatible with your plugin

    const int numChannels = preferredSet.size();

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

//==============================================================================
void BeatboxVoxAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const float sampleRate = getSampleRate();
    const int numSamples = buffer.getNumSamples();
    
    //Holds classifier result for this block. 
    int sound = -1;

    //SYNTH SAMPLE TEST
    /** if (++startTime == 100)   */
    /** {  */
    /**     triggerSnareDrum(midiMessages);  */
    /**     startTime = 0;   */
    /** }  */

    classifier.processAudioBuffer(buffer.getReadPointer(0)); 

    sound = classifier.classify();
    
    switch(sound)
    {
        case soundLabel::KickDrum :
            triggerKickDrum(midiMessages);
            break;
        case soundLabel::SnareDrum :
            triggerSnareDrum(midiMessages);
            break;
    }

    //Now classification complete clear the input buffer/signal - we only want synth output.
    for (int i = 0; i < getTotalNumInputChannels(); ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    //Render note on sine synth with the ODS triggered MIDI.
    drumSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

}

//==============================================================================
void BeatboxVoxAudioProcessor::triggerKickDrum(MidiBuffer& midiMessages)
{
    midiMessages.addEvent(MidiMessage::noteOn(1, 12, (uint8) 100), 0);
}

//==============================================================================
void BeatboxVoxAudioProcessor::triggerSnareDrum(MidiBuffer& midiMessages)
{
    midiMessages.addEvent(MidiMessage::noteOn(1, 43, (uint8) 100), 0);
}

//==============================================================================
float BeatboxVoxAudioProcessor::getSpectralCentroid() const
{
    return spectralCentroid.load();
}

//==============================================================================
AudioClassifier<float>& BeatboxVoxAudioProcessor::getClassifier() 
{
    return classifier;
}


//==============================================================================
bool BeatboxVoxAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BeatboxVoxAudioProcessor::createEditor()
{
    return new BeatboxVoxAudioProcessorEditor (*this);
}

//==============================================================================
void BeatboxVoxAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BeatboxVoxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BeatboxVoxAudioProcessor();
}
