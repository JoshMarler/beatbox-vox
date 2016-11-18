/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <atomic>
#include <map>

#include "../JuceLibraryCode/JuceHeader.h"

#include "Dsp/Dsp.h"
#include "AudioClassify/src/AudioClassify.h"


//==============================================================================

class BeatboxVoxAudioProcessor  : public AudioProcessor,
                                  public AudioProcessorValueTreeState::Listener
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
    void parameterChanged(const String& paramID, float newValue) override;
    
    //==============================================================================
    AudioProcessorValueTreeState& getValueTreeState();

    //==============================================================================

    /** Sets a flag to indicate whether a test sound should be triggered for 
     * each onset detected.This is used as a reference point / listening test when
     * configuring the onset detection settings. 
     * @param useTestSound flag to turn the OnsetDetector test sound on/off
     */
    void setUsingOSDTestSound(bool useTestSound);

    //==============================================================================
    //Initialise the synth object
    void initialiseSynth();

    //==============================================================================
    //Returns reference to plugins AudioClassifier for GUI to set classification settings.
    //JWM - NOTE should eventually change to accept templated precision. 
    AudioClassifier<float>& getClassifier();

    //==============================================================================
    enum soundLabel
    {
        KickDrum = 0, 
        SnareDrum, 
        HiHat
    };

    //==============================================================================
    //Parameter ID strings
    static String paramOSDMeanCoeff;
    static String paramOSDMedianCoeff;
    static String paramOSDNoiseRatio;
    static String paramOSDMsBetweenOnsets;


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeatboxVoxAudioProcessor);
	
    //==============================================================================
    const int kickNoteNumber = 12;
    const int snareNoteNumber = 43;
	const int hihatNoteNumber = 50;
	const int noiseNoteNumber = 54;
    const int osdTestSoundNoteNumber = 57;

	//Flag to indicate whether to trigger the onset detector test sound for setting sensitivity by ear.
    std::atomic_bool usingOSDTestSound;

    //==============================================================================
	/**
	 * A map of callbacks to be used for parameter changes. Associates
	 * a param ID with a function callback.
	 */
    std::map<String, std::function<void(float)>> paramCallbacks;

    //==============================================================================
    Synthesiser drumSynth;
    Synthesiser osdTestSynth;

    //==============================================================================
    AudioProcessorValueTreeState processorState;
    //UndoManager processorUndoManager;

    //==============================================================================
    AudioClassifier<float> classifier;

    //==============================================================================

    //Setup the audio processor parameters
    void setupParameters();

	/**
	 * Quick and dirty midi note generation functions to respond to classification. 
	 * NOTE: Would change in future for more fully featured/production version. 
	 */
    void triggerKickDrum(MidiBuffer& midiMessages) const;
    void triggerSnareDrum(MidiBuffer& midiMessages) const;
	void triggerHiHat(MidiBuffer& midiMessages) const;
	void triggerNoise(MidiBuffer& midiMessages) const;
    void triggerOSDTestSound(MidiBuffer& midiMessages) const;
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
