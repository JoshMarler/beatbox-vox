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
#include "BufferHandlingComponent.h"
#include "FeaturesComponent.h"

#include "../../PluginProcessor.h"

class AudioClassifierComponent : public Component,
								 ChangeListener
{

public:

	explicit AudioClassifierComponent(BeatboxVoxAudioProcessor& p);
    ~AudioClassifierComponent();

    void paint(Graphics&) override;
    void resized() override;

	void changeListenerCallback(ChangeBroadcaster *source) override;
    
	//ComponentID Strings
	static String saveTrainingDataButtonID;
	static String loadTrainingDateButtonID;

private:
    
    BeatboxVoxAudioProcessor& processor;

	//Core child components
	std::unique_ptr<RecordTrainingSetComponent> recordTrainingSetComponent;
	std::unique_ptr<SelectClassifierComponent> selectClassifierComponent;
	std::unique_ptr<BufferHandlingComponent> bufferHandlingComponent;
	std::unique_ptr<FeaturesComponent> featuresComponent;

};



#endif  // AUDIOCLASSIFIERCOMPONENT_H_INCLUDED
