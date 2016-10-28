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

#include "RecordTrainingSetComponent.h"
#include "SelectClassifierComponent.h"
#include "DelayedEvaluationComponent.h"

#include "../../PluginProcessor.h"

class AudioClassifierComponent : public Component,
                                 ButtonListener
{

public:

	explicit AudioClassifierComponent(BeatboxVoxAudioProcessor& p);
    ~AudioClassifierComponent();

    void paint(Graphics&) override;
    void resized() override;

    void buttonClicked(Button* button) override;
    
	//ComponentID Strings
	static String saveTrainingDataButtonID;
	static String loadTrainingDateButtonID;

private:
    
    BeatboxVoxAudioProcessor& processor;

	//Core child components
	std::unique_ptr<RecordTrainingSetComponent> recordTrainingSetComponent;
	std::unique_ptr<SelectClassifierComponent> selectClassifierComponent;
	std::unique_ptr<DelayedEvaluationComponent> delayedEvaluationComponent;

};



#endif  // AUDIOCLASSIFIERCOMPONENT_H_INCLUDED
