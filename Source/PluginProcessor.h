/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <atomic>
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioClassify/src/AudioClassify.h"


//==============================================================================

class BeatboxVoxAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    BeatboxVoxAudioProcessor();
    ~BeatboxVoxAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //JWM - Test function to get spectral centroid value in GUI
    float getSpectralCentroid() const;

    //Initialise the test synth object
    void initialiseSynth();

    void triggerKickDrum(MidiBuffer& midiMessages);
    void triggerSnareDrum(MidiBuffer& midiMessages);

    enum soundLabel
    {
        KickDrum = 0, 
        SnareDrum, 
        HiHat
    };

    //Returns reference to plugins AudioClassifier for GUI to set classification settings.
    //JWM - NOTE will need to change to accept templated precision. 
    const AudioClassifier<float>& getClassifier() const;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeatboxVoxAudioProcessor);


    int startTime = 0;
    std::atomic<float> spectralCentroid;

    Synthesiser drumSynth;
    AudioClassifier<float> classifier;
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
