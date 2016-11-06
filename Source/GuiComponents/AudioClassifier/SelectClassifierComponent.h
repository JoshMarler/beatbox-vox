/*
  ==============================================================================

    SelectClassifierComponent.h
    Created: 28 Oct 2016 3:13:59pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#ifndef SELECTCLASSIFIERCOMPONENT_H_INCLUDED
#define SELECTCLASSIFIERCOMPONENT_H_INCLUDED

#include "JuceHeader.h"
#include "../../PluginProcessor.h"

class SelectClassifierComponent : public Component,
								  ButtonListener,
								  ComboBoxListener
{
public:
	explicit SelectClassifierComponent(BeatboxVoxAudioProcessor& p);
	~SelectClassifierComponent();

	void paint(Graphics& g) override;
	void resized() override;

	void buttonClicked(Button* button) override;

	void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

	//ComponentID Strings
	static String classifierCmbBoxID;
	static String saveTrainingDataButtonID;
	static String loadTrainingDateButtonID;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectClassifierComponent)

	BeatboxVoxAudioProcessor& processor;

	std::string trainingSetsDirectory;

	Label headingLabel;
	Label classifierParamsLabel;
	ComboBox classifierCmbBox;

	TextButton loadTrainingDataButton;
	TextButton saveTrainingDataButton;

	void setupClassifierCmbBox();
	std::unique_ptr<FileChooser> trainingDataChooser;

	void saveTrainingSet();
	void initialiseTrainingDataChooser();

};




#endif  // SELECTCLASSIFIERCOMPONENT_H_INCLUDED
