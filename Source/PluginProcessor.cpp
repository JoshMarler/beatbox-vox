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
    : onsetDetector(std::make_unique<OnsetDetector>()),
      sineSynth(std::make_unique<Synthesiser>()),
      //gistMFCC(std::make_unique<Gist<float>>(512, 44100)),
      gistOnset(std::make_unique<Gist<float>>(512, 44100)),
     //nbc(std::make_unique<NaiveBayesClassifier<>>()), 
      spectralCentroid(0.0f)
      
{ 
    initialiseSynth(); 
}

BeatboxVoxAudioProcessor::~BeatboxVoxAudioProcessor()
{
    float debug = 0.0;
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
    const int numVoices = 1;    

    for (auto i = numVoices; --i >= 0;)
            sineSynth->addVoice(new SineWaveVoice());

    sineSynth->addSound(new SineWaveSound());
}

//==============================================================================
void BeatboxVoxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //gistMFCC->setSamplingFrequency(sampleRate);
    //gistMFCC->setAudioFrameSize(samplesPerBlock);

    gistOnset->setSamplingFrequency(sampleRate);
    gistOnset->setAudioFrameSize(samplesPerBlock);
    
    magSpectrum.resize(samplesPerBlock / 2);
    std::fill(magSpectrum.begin(), magSpectrum.end(), 0.0f);

    sineSynth->setCurrentPlaybackSampleRate(sampleRate);

    //Testing matrix init - initialzing with rows - testMatrix(2, 0) == row 2 element 0 (third row, first element)
    //testMatrix = {{20, 10}, {30, 10}, {1, 2}};
    //auto sampleOneLabel = testMatrix(2, 0);
    //auto sampleTwoLabel = testMatrix(2,1);
    //auto colOne = testMatrix.col(1);
}

void BeatboxVoxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

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

void BeatboxVoxAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const float sampleRate = getSampleRate();
    const int numSamples = buffer.getNumSamples();

    //Temp var for note duration
    const int noteDuration = static_cast<int> (std::ceil(sampleRate * 1.25f));
    
    //Gist analysis performed
    
    gistOnset->processAudioFrame(buffer.getWritePointer(0), buffer.getNumSamples());
    magSpectrum = gistOnset->getMagnitudeSpectrum();
    float hfc = gistOnset->highFrequencyContent();
    
    bool hasOnset = onsetDetector->checkForOnset(hfc);
    if (hasOnset)
    {
        midiMessages.addEvent(MidiMessage::noteOn(1, 60, (uint8) 100), 0);
    }
    
    if ((startTime + numSamples) >= noteDuration)
    {
        const int offset = jmax(0, jmin((int) (noteDuration - startTime), numSamples - 1));
        midiMessages.addEvent(MidiMessage::noteOff(1, 60), offset);
    }

    startTime = (startTime + numSamples) % noteDuration;
    

    //Render note on sine synth with the ODS triggered MIDI.
    sineSynth->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    

    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
}

float BeatboxVoxAudioProcessor::getSpectralCentroid() const
{
    return spectralCentroid.load();
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
