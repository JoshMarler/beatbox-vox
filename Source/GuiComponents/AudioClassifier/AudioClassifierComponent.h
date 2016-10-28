/*
  ==============================================================================

    AudioClassifierComponent.h
    Created: 17 Sep 2016 5:13:37pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFIERCOMPONENT_H_INCLUDED
#define AUDIOCLASSIFIERCOMPONENT_H_INCLUDED

#include <memory>
#include <string>

#include "../JuceLibraryCode/JuceHeader.h"

#include "../CustomLookAndFeel.h"
#include "../../PluginProcessor.h"

class AudioClassifierComponent : public Component,
                                 Timer,
                                 ButtonListener
{

public:

	explicit AudioClassifierComponent(BeatboxVoxAudioProcessor& p);
    ~AudioClassifierComponent();


    void paint(Graphics&) override;
    void resized() override;

    void timerCallback() override;
    void buttonClicked(Button* button) override;
    
	//ComponentID Strings
	static String saveTrainingDataButtonID;
	static String loadTrainingDateButtonID;

private:
    
    BeatboxVoxAudioProcessor& processor;


	std::unique_ptr<LookAndFeel> componentLookAndFeel;


    //JWM - A little dirty maybe but works for now at prototype stage
    int currentTrainingSound = -1;
	
	std::string trainingSetsDirectory;

    std::unique_ptr<TextButton> recordSoundButton;
    
    //JWM - Test button to swich on classifier training for sound
    std::unique_ptr<TextButton> trainClassifierButton;

	TextButton saveTrainingDataButton;

    OwnedArray<Button> soundButtons;

	OwnedArray<Label> soundReadyLabels;

	TextButton loadTrainingDataButton;
	std::unique_ptr<FileChooser> trainingDataChooser;

	void saveTrainingSet();
	void initialiseTrainingDataChooser();
};



#endif  // AUDIOCLASSIFIERCOMPONENT_H_INCLUDED
