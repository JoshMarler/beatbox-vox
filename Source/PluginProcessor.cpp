/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

String BeatboxVoxAudioProcessor::paramOSDMeanCoeff ("osd_meancoeff");
String BeatboxVoxAudioProcessor::paramOSDMedianCoeff ("osd_mediancoeff");
String BeatboxVoxAudioProcessor::paramOSDNoiseRatio ("osd_noiseratio");
String BeatboxVoxAudioProcessor::paramOSDMsBetweenOnsets ("osd_msbetween");

//==============================================================================
BeatboxVoxAudioProcessor::BeatboxVoxAudioProcessor() 
    : classifier(256, downSamplingRate, 2),
      interpolator(),
      downSampledBuffer()
      
{ 
    usingOSDTestSound.store(false);

    setupParameters();
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

//==============================================================================
AudioProcessorValueTreeState& BeatboxVoxAudioProcessor::getValueTreeState()
{
    return *processorState;
}
//==============================================================================
void BeatboxVoxAudioProcessor::setupParameters()
{
    
    processorUndoManager = std::make_unique<UndoManager>();
    processorState = std::make_unique<AudioProcessorValueTreeState>(*this, processorUndoManager.get());
    
    processorState->createAndAddParameter(paramOSDNoiseRatio, 
                                          "OSD Noise Ratio",
                                          "Onset Detector Noise Ratio", 
                                          NormalisableRange<float> (0.01, 1.0, 0.01), 
                                          0.05, 
                                          nullptr, 
                                          nullptr);

    processorState->addParameterListener(paramOSDNoiseRatio, this);



    processorState->createAndAddParameter(paramOSDMeanCoeff, 
                                          "OSD Mean Coefficient",
                                          "Onset Detector Mean Coefficient",
                                          NormalisableRange<float> (0.01, 1.0, 0.01),
                                          1.0, 
                                          nullptr,
                                          nullptr);

    processorState->addParameterListener(paramOSDMeanCoeff, this);


    processorState->createAndAddParameter(paramOSDMedianCoeff, 
                                          "OSD Median Coefficient",
                                          "Onset Detector Median Coefficient",
                                          NormalisableRange<float> (0.01, 1.0, 0.01),
                                          1.0, 
                                          nullptr,
                                          nullptr);

    processorState->addParameterListener(paramOSDMedianCoeff, this);


    processorState->createAndAddParameter(paramOSDMsBetweenOnsets, 
                                          "OSD Ms Between Onsets",
                                          "Onset Detector Ms Between Onsets",
                                          NormalisableRange<float> (0.0, 50.0, 5.0),
                                          0.0,
                                          nullptr,
                                          nullptr);

    processorState->addParameterListener(paramOSDMsBetweenOnsets, this);


    processorState->state = ValueTree("Beatbox Vox");

    auto onsetDetectMeanCallback = [this] (float newMeanCoeff) { this->classifier.setOnsetDetectorMeanCoeff(newMeanCoeff); };
    auto onsetDetectMedianCallback = [this] (float newMedianCoeff) { this->classifier.setOnsetDetectorMedianCoeff(newMedianCoeff); };
    auto onsetDetectNoiseCallback = [this] (float newNoiseRatio) { this->classifier.setOnsetDetectorNoiseRatio(newNoiseRatio); };
    auto onsetDetectMsBetweenCallback = [this] (float newMsBetweenOnsets) { this->classifier.setOSDMsBetweenOnsets(newMsBetweenOnsets); };
    
    paramCallbacks.insert(std::make_pair(paramOSDMeanCoeff, onsetDetectMeanCallback));
    paramCallbacks.insert(std::make_pair(paramOSDMedianCoeff, onsetDetectMedianCallback));
    paramCallbacks.insert(std::make_pair(paramOSDNoiseRatio, onsetDetectNoiseCallback));
    paramCallbacks.insert(std::make_pair(paramOSDMsBetweenOnsets, onsetDetectMsBetweenCallback));
}

//==============================================================================
void BeatboxVoxAudioProcessor::parameterChanged(const String& paramID, float newValue)
{
    auto callback = paramCallbacks.find(paramID);
    callback->second(newValue);
}
//==============================================================================

void BeatboxVoxAudioProcessor::initialiseSynth ()
{
    /** JWM - Quick and dirty sample drum synth for prototype
     *  In future versions will ideally allow user to select sample to use and
     *  also manage sample rate changes effect on loaded samples. 
    */

    WavAudioFormat wavFormat;
    BigInteger kickNoteRange;
    BigInteger snareNoteRange;
    BigInteger osdTestSoundNoteRange;

    drumSynth.clearSounds();
    osdTestSynth.clearSounds();

    std::unique_ptr<AudioFormatReader> readerKickDrum(wavFormat.createReaderFor(new MemoryInputStream(BinaryData::bassdrum_wav,
                                                                                                      BinaryData::bassdrum_wavSize,
                                                                                                      false),
                                                                                                      true));

    std::unique_ptr<AudioFormatReader> readerSnareDrum(wavFormat.createReaderFor(new MemoryInputStream(BinaryData::snaredrum_wav,
                                                                                                       BinaryData::snaredrum_wavSize,
                                                                                                       false),
                                                                                                       true));

    std::unique_ptr<AudioFormatReader> readerOSDTestSound(wavFormat.createReaderFor(new MemoryInputStream(BinaryData::osdTestOne_wav, 
                                                                                                          BinaryData::osdTestOne_wavSize,
                                                                                                          false), 
                                                                                                          true));

    kickNoteRange.setBit(kickNoteNumber);
    snareNoteRange.setBit(snareNoteNumber);
    osdTestSoundNoteRange.setBit(osdTestSoundNoteNumber);

    
    drumSynth.addSound(new SamplerSound("Kick Sound", *readerKickDrum, kickNoteRange, kickNoteNumber, 0.0, 0.0, 5.0));
    drumSynth.addSound(new SamplerSound("Snare Sound", *readerSnareDrum, snareNoteRange, snareNoteNumber, 0.0, 0.0, 5.0));

    drumSynth.addVoice(new SamplerVoice());

    osdTestSynth.addSound(new SamplerSound("OSD Test Sound", *readerOSDTestSound, osdTestSoundNoteRange, osdTestSoundNoteNumber, 0.0, 0.0, 5.0));
    osdTestSynth.addVoice(new SamplerVoice());
}

//==============================================================================
void BeatboxVoxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    downSampledBuffer.setSize(1, samplesPerBlock);
    downSampledBuffer.clear();

    drumSynth.setCurrentPlaybackSampleRate(sampleRate);
    osdTestSynth.setCurrentPlaybackSampleRate(sampleRate);

    classifier.setCurrentBufferSize(samplesPerBlock);
    //classifier.setCurrentSampleRate(sampleRate);

    interpolator.reset();
}

void BeatboxVoxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    interpolator.reset();
    downSampledBuffer.clear();
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

    /**
     * NOTE: Need to confirm that buffer size will not change between prepareToPlay and processBlock/handle 
     * this for downSampleBuffer.
     */
    interpolator.process((downSamplingRate / sampleRate), buffer.getReadPointer(0), downSampledBuffer.getWritePointer(0, 0), numSamples);

    //classifier.processAudioBuffer(buffer.getReadPointer(0)); 
    classifier.processAudioBuffer(downSampledBuffer.getReadPointer(0), numSamples);

    
    //This is used for configuring the onset detector settings from the GUI
    if (classifier.noteOnsetDetected())
    {
        //NOTE: Potentially add a flag to onset detected in an fifo or something for visual response on onset.
        if (usingOSDTestSound.load())
        {
            triggerOSDTestSound(midiMessages);
            osdTestSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());    
        }
    }

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
    midiMessages.addEvent(MidiMessage::noteOn(1, kickNoteNumber, (uint8) 100), 0);
}

//==============================================================================
void BeatboxVoxAudioProcessor::triggerSnareDrum(MidiBuffer& midiMessages)
{
    midiMessages.addEvent(MidiMessage::noteOn(1, snareNoteNumber, (uint8) 100), 0);
}

//==============================================================================
void BeatboxVoxAudioProcessor::triggerOSDTestSound(MidiBuffer& midiMessages)
{
    midiMessages.addEvent(MidiMessage::noteOn(1, osdTestSoundNoteNumber, (uint8) 100), 0);
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
void BeatboxVoxAudioProcessor::setUsingOSDTestSound(bool useTestSound)
{
    usingOSDTestSound.store(useTestSound);
}
//==============================================================================

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BeatboxVoxAudioProcessor();
}
