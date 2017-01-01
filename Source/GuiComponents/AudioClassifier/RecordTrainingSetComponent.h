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
#include "../../PluginProcessor.h"

class RecordTrainingSetComponent : public Component,
								   Timer,
								   ButtonListener,
								   SliderListener,
								   ComboBoxListener
{
public:

	RecordTrainingSetComponent(BeatboxVoxAudioProcessor& p);
	~RecordTrainingSetComponent();

	void paint(Graphics& g) override;
	void resized()override;

	void timerCallback() override;
	void buttonClicked(Button* button) override;
	void sliderValueChanged(Slider* slider) override;
	void comboBoxChanged(ComboBox* comboBOxThatHasChanged) override;



	//Control ID's
	static String headingLabelID;
	static String activateButtonID;
	static String instanceSizeSliderID;
	static String instanceSizeButtonID;
	static String recordButtonID;
	static String saveButtonID;
	static String recordTypeCmbID;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordTrainingSetComponent)

    BeatboxVoxAudioProcessor& processor;

	int soundButtonsGroupID = 1234;

    int currentRecordingSound = -1;

	AudioClassifyOptions::DataSetType currentDataSetType;

	Label headingLabel;
	Label numInstancesLabel;
	Label selectRecordingSoundLabel;

	ComboBox recordTypeCmb;

	ToggleButton activateButton;
	TextButton recordButton;
	TextButton saveButton;
	

	Slider instanceSizeSlider;
	TextButton instanceSizeButton;

	OwnedArray<Button> soundButtons;
	OwnedArray<Label> soundStatusLabels;

	std::string trainingSetsDirectory;
	std::string testSetsDirectory;

	void saveDataSet();

	void setupSoundButtons();
	void setActive(bool active);
	void setNeedsUpdate(bool needsUpdate);
	void setupRecordTypeCmb();
};





#endif  // RECORDTRAININGSETCOMPONENT_H_INCLUDED
