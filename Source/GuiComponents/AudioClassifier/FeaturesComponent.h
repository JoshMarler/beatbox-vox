/*
  ==============================================================================

    FeaturesComponent.h
    Created: 28 Oct 2016 10:33:45pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef FEATURESCOMPONENT_H_INCLUDED
#define FEATURESCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"

//==============================================================================
/*
*/
class FeaturesComponent    : public Component,
							 Timer,	
							 ButtonListener
{
public:
    FeaturesComponent(BeatboxVoxAudioProcessor& p);
    ~FeaturesComponent();

    void paint (Graphics&) override;
    void resized() override;

	void timerCallback() override;

	void buttonClicked(Button *button) override;

	static String headingLabelID;
	static String showVariancesButtonID;
	static String updateFeatureVariancesButtonID;

	void setActive(bool active);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeaturesComponent)

	BeatboxVoxAudioProcessor& processor;

	int varianceReduceButtonsGroupID = 1;
	int numFeaturesToUse = 0;

	std::unique_ptr<LookAndFeel> componentLookAndFeel;

	Label headingLabel;


	Label numFeaturesInUseLbl;
	Label numFeaturesInUseVal;

	Label varianceReduceControlsHeadingLabel;
	OwnedArray<Button> varianceReduceValueButtons;
	OwnedArray<Label> varianceReduceLabels;
	TextButton showVariancesButton;
	TextButton updateFeatureVariancesButton;

	void setupVarianceReduceValueButtons();

	void setNeedsUpdate(bool needsUpdate);
};


#endif  // FEATURESCOMPONENT_H_INCLUDED
