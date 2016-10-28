/*
  ==============================================================================

    RecordTrainingSetComponent.h
    Created: 21 Oct 2016 3:23:25pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#ifndef RECORDTRAININGSETCOMPONENT_H_INCLUDED
#define RECORDTRAININGSETCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

class RecordTrainingSetComponent : public Component,
								   Timer,
								   ButtonListener	
{
public:

	RecordTrainingSetComponent();
	~RecordTrainingSetComponent();

	void paint(Graphics& g) override;
	void resized()override;

	void timerCallback() override;
	void buttonClicked(Button* button) override;

	//Control ID's
	static String headingLabelID;
	static String activateButtonID;
	static String instanceSizeSliderID;
	static String recordButtonID;
	static String trainButtonID;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordTrainingSetComponent)

	int soundButtonsGroupID = 1234;

    //JWM - A little dirty maybe but works for now at prototype stage
    int currentTrainingSound = -1;

	Label headingLabel;
	Label numInstancesLabel;
	Label selectRecordingSoundLabel;

	ToggleButton activateButton;
	TextButton recordButton;
	TextButton trainButton;
	

	Slider instanceSizeSlider;

	OwnedArray<Button> soundButtons;
	OwnedArray<Label> soundStatusLabels;

	void setupSoundButtons();
	void setActive(bool active);

};





#endif  // RECORDTRAININGSETCOMPONENT_H_INCLUDED
