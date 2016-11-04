/*
  ==============================================================================

    DelayedEvaluationComponent.h
    Created: 28 Oct 2016 9:00:48pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef DELAYEDEVALUATIONCOMPONENT_H_INCLUDED
#define DELAYEDEVALUATIONCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"

//==============================================================================

/**
 *
 */
class DelayedEvaluationComponent : public Component,
								   ButtonListener,
								   SliderListener
{
public:
	explicit DelayedEvaluationComponent(BeatboxVoxAudioProcessor& p);
    ~DelayedEvaluationComponent();

    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button* button) override;
	void sliderValueChanged(Slider* slider) override;

	//Component ID's
	static String headingLabelID;
	static String activateButtonID;
	static String bufferDelaySliderID;
	static String bufferDelayUpdateButtonID;
	static String useEarlyAttackButtonID;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayedEvaluationComponent)

    BeatboxVoxAudioProcessor& processor;
	
	//heading section controls
	Label headingLabel;
	ToggleButton activateButton;

	Label numSamplesUsedLbl;
	Label numSamplesUsedVal;

	Label numBuffersUsedLbl;
	Label numBuffersUsedVal;

	Label earlyAttackLbl;
	ToggleButton useEarlyAttackButton;

	Label bufferDelayLabel;
	Slider bufferDelaySlider;
	TextButton bufferDelayUpdateButton;

	void setActive(bool active);
	void setUseEarlyAttack(bool use);
	void setNeedsUpdate(bool needsUpdate);
};


#endif  // DELAYEDEVALUATIONCOMPONENT_H_INCLUDED
