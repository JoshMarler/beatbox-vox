/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

String BeatboxVoxAudioProcessor::paramOSDMeanCoeff("osd_meancoeff");
String BeatboxVoxAudioProcessor::paramOSDMedianCoeff("osd_mediancoeff");
String BeatboxVoxAudioProcessor::paramOSDNoiseRatio("osd_noiseratio");
String BeatboxVoxAudioProcessor::paramOSDMsBetweenOnsets("osd_msbetween");

//==============================================================================
BeatboxVoxAudioProcessor::BeatboxVoxAudioProcessor()
	: processorState(*this, nullptr),
	  classifier(256, 48000, 2)

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
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int BeatboxVoxAudioProcessor::getCurrentProgram()
{
	return 0;
}

void BeatboxVoxAudioProcessor::setCurrentProgram(int index)
{
}

const String BeatboxVoxAudioProcessor::getProgramName(int index)
{
	return String();
}

void BeatboxVoxAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
AudioProcessorValueTreeState& BeatboxVoxAudioProcessor::getValueTreeState()
{
	return processorState;
}

//==============================================================================
void BeatboxVoxAudioProcessor::setupParameters()
{
	processorState.createAndAddParameter(paramOSDNoiseRatio,
	                                     "OSD Noise Ratio",
	                                     String(),
	                                     NormalisableRange<float>(0.01, 100.0, 0.01),
	                                     40.0f,
	                                     nullptr,
	                                     nullptr);

	processorState.addParameterListener(paramOSDNoiseRatio, this);


	processorState.createAndAddParameter(paramOSDMeanCoeff,
	                                     "OSD Mean Coefficient",
	                                     String(),
	                                     NormalisableRange<float>(0.01, 2.0, 0.01),
	                                     0.8f,
	                                     nullptr,
	                                     nullptr);

	processorState.addParameterListener(paramOSDMeanCoeff, this);


	processorState.createAndAddParameter(paramOSDMedianCoeff,
	                                     "OSD Median Coefficient",
	                                     String(),
	                                     NormalisableRange<float>(0.01, 2.0, 0.01),
	                                     0.8f,
	                                     nullptr,
	                                     nullptr);

	processorState.addParameterListener(paramOSDMedianCoeff, this);


	processorState.createAndAddParameter(paramOSDMsBetweenOnsets,
	                                     "OSD Ms Between Onsets",
	                                     String("ms"),
	                                     NormalisableRange<float>(0.0, 100.0, 5.0),
	                                     70.0f,
	                                     nullptr,
	                                     nullptr);

	processorState.addParameterListener(paramOSDMsBetweenOnsets, this);


	processorState.state = ValueTree(Identifier("BeatboxVox"));

	auto onsetDetectMeanCallback = [this] (float newMeanCoeff) { this->classifier.setOnsetDetectorMeanCoeff(newMeanCoeff); };
	auto onsetDetectMedianCallback = [this] (float newMedianCoeff) { this->classifier.setOnsetDetectorMedianCoeff(newMedianCoeff); };
	auto onsetDetectNoiseCallback = [this] (float newNoiseRatio) { this->classifier.setOnsetDetectorNoiseRatio(newNoiseRatio); };
	auto onsetDetectMsBetweenCallback = [this] (float newMsBetweenOnsets) { this->classifier.setOSDMsBetweenOnsets(static_cast<int>(newMsBetweenOnsets)); };

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

void BeatboxVoxAudioProcessor::initialiseSynth()
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
void BeatboxVoxAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..

	drumSynth.setCurrentPlaybackSampleRate(sampleRate);
	osdTestSynth.setCurrentPlaybackSampleRate(sampleRate);

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
void BeatboxVoxAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	const auto totalNumInputChannels = getTotalNumInputChannels();
	const auto totalNumOutputChannels = getTotalNumOutputChannels();
	const auto sampleRate = getSampleRate();
	const auto numSamples = buffer.getNumSamples();


	//Holds classifier result for this block. 
	int sound = -1;
	auto scaleFactor = 1000.0f;

	//Buffer scale test
	FloatVectorOperations::multiply(buffer.getWritePointer(0), scaleFactor, numSamples);

	classifier.processAudioBuffer(buffer.getReadPointer(0), numSamples);


	//This is used for configuring the onset detector settings from the GUI
	if (classifier.noteOnsetDetected())
	{
		//NOTE: Potentially add a flag to onset detected in an fifo or something for visual response on onset.
		if (usingOSDTestSound.load())
			triggerOSDTestSound(midiMessages);
	}


	sound = classifier.classify();

	switch (sound)
	{
	case soundLabel::KickDrum:
		triggerKickDrum(midiMessages);
		break;
	case soundLabel::SnareDrum:
		triggerSnareDrum(midiMessages);
		break;
	default: break;
	}


	//Now classification complete clear the input buffer/signal - we only want synth output.
	buffer.clear();

	if (usingOSDTestSound.load())
		osdTestSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
	else
		drumSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

	//Not outputting midi so clear after rendering synths
	midiMessages.clear();
}

//==============================================================================
void BeatboxVoxAudioProcessor::triggerKickDrum(MidiBuffer& midiMessages) const
{
	midiMessages.addEvent(MidiMessage::noteOn(1, kickNoteNumber, static_cast<uint8>(100)), 0);
}

//==============================================================================
void BeatboxVoxAudioProcessor::triggerSnareDrum(MidiBuffer& midiMessages) const
{
	midiMessages.addEvent(MidiMessage::noteOn(1, snareNoteNumber, static_cast<uint8>(100)), 0);
}

//==============================================================================
void BeatboxVoxAudioProcessor::triggerOSDTestSound(MidiBuffer& midiMessages) const
{
	midiMessages.addEvent(MidiMessage::noteOn(1, osdTestSoundNoteNumber, static_cast<uint8>(100)), 0);
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
	return new BeatboxVoxAudioProcessorEditor(*this);
}

//==============================================================================
void BeatboxVoxAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	std::unique_ptr<XmlElement> xml(processorState.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void BeatboxVoxAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName(processorState.state.getType()))
			processorState.state = ValueTree::fromXml(*xmlState);
	}
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
