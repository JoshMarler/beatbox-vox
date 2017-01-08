/*
  ==============================================================================

    BUFFERHANDLINGCOMPONENT.h
    Created: 28 Oct 2016 9:00:48pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef BUFFERHANDLINGCOMPONENT_H_INCLUDED
#define BUFFERHANDLINGCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"

//==============================================================================
class BufferHandlingComponent : public Component,
								   ButtonListener,
								   SliderListener
{
public:
	explicit BufferHandlingComponent(BeatboxVoxAudioProcessor& p);
    ~BufferHandlingComponent();

    void paint (Graphics&) override;
    void resized() override;


	void buttonClicked(Button* button) override;
	void sliderValueChanged(Slider* slider) override;

	/**
	 * Eventually it may be more effective to have AudioClassifier use more of JUCE
	 * and set AudioClassifier to be some form of change broadcaster. Then the various Audio Classifier/Beatbox Vox
	 * child components could take in a reference to the audio classifier and add themselves as a listener to
	 * broadcasted state changes such as numStftFrames updates etc.
	 * 
	 * If time allows before project end this may be added. 
	 */
	void handleNewTrainingSetLoaded();

	//Component ID's
	static String headingLabelID;
	static String activateButtonID;
	static String bufferDelaySliderID;
	static String bufferDelayUpdateButtonID;
	static String stftNumFramesSliderID;
	static String stftFramesUpdateButtonID;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BufferHandlingComponent)

    BeatboxVoxAudioProcessor& processor;
	
	//heading section controls
	Label headingLabel;

	ToggleButton activateButton;

	//Delayed evaluation controls
	Label numSamplesUsedLbl;
	Label numSamplesUsedVal;

	Label numBuffersUsedLbl;
	Label numBuffersUsedVal;

	Label bufferDelayLabel;
	Slider bufferDelaySlider;
	TextButton bufferDelayUpdateButton;

	//STFT Controls
	Label stftNumFramesLabel;
	Slider stftNumFramesSlider;
	TextButton stftFramesUpdateButton;

	Label stftFramesPerBufferLbl;
	Label stftFramePerBufferVal;

	Label stftFrameSizeLbl;
	Label stftFrameSizeVal;

	void setActive(bool active) const;
	static void setNeedsUpdate(bool needsUpdate, Button& button);
	
	void updateStatusControls();
	void updateStatusLabels();
};


#endif  // BUFFERHANDLINGCOMPONENT_H_INCLUDED
