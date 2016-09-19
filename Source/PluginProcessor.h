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

#include "Parameters/CustomAudioParameter.h"
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

    //==============================================================================

    /** Sets a flag to indicate whether a test sound should be triggered for 
     * each onset detected.This is used as a reference point / listening test when
     * configuring the onset detection settings. 
     * @param useTestSound flag to turn the OnsetDetector test sound on/off
     */
    void setUsingOSDTestSound(bool useTestSound);
    
    //Setup the audio processor parameters
    void setupParameters();
    
    //Get parameter from ID
    AudioProcessorParameter& getParameterFromID(StringRef id);
    
    //Initialise the synth object
    void initialiseSynth();

    void triggerKickDrum(MidiBuffer& midiMessages);
    void triggerSnareDrum(MidiBuffer& midiMessages);
    
    void triggerOSDTestSound(MidiBuffer& midiMessages);
    

    enum soundLabel
    {
        KickDrum = 0, 
        SnareDrum, 
        HiHat
    };

    //Parameter ID strings
    static String paramOSDMeanCoeff;
    static String paramOSDNoiseRatio;
    static String paramOSDMsBetweenOnsets;

    //Returns reference to plugins AudioClassifier for GUI to set classification settings.
    //JWM - NOTE will need to change to accept templated precision. 
    AudioClassifier<float>& getClassifier();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeatboxVoxAudioProcessor);

    float downSamplingRate = 11025.0f;

    const int kickNoteNumber = 12;
    const int snareNoteNumber = 43;
    const int osdTestSoundNoteNumber = 57;

    std::atomic_bool usingOSDTestSound;

    Synthesiser drumSynth;
    Synthesiser osdTestSynth;
    AudioClassifier<float> classifier;
    CatmullRomInterpolator interpolator;

    AudioSampleBuffer downSampledBuffer;
    
    //Parameters
    CustomAudioParameter* osdMeanCoefficient;
    CustomAudioParameter* osdNoiseRatio; 
    CustomAudioParameter* osdMsBetweenOnsets;

};


#endif  // PLUGINPROCESSOR_H_INCLUDED
