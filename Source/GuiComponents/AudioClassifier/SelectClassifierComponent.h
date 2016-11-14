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
#include "TestClassifierComponent.h"

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
	static String testClassifierButtonID;
	static String trainClassifierButtonID;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectClassifierComponent)

	BeatboxVoxAudioProcessor& processor;

	std::string trainingSetsDirectory;

	Label headingLabel;
	ComboBox classifierCmbBox;

	TextButton loadTrainingDataButton;
	TextButton saveTrainingDataButton;

	TextButton testClassifierButton;
	TextButton trainClassifierButton;

	void setupClassifierCmbBox();
	
	//At some point may change saveDataChoser to be a dialog windows with FileNameComponent to force .csv suffix etc.
	std::unique_ptr<FileChooser> trainingDataChooser;
	std::unique_ptr<FileChooser> saveDataChooser;

	void saveTrainingSet(std::string fileName);
	void initialiseTrainingDataChooser();
	void initialiseSaveDataChooser();

	std::unique_ptr<TestClassifierComponent> testComponent;

};




#endif  // SELECTCLASSIFIERCOMPONENT_H_INCLUDED
