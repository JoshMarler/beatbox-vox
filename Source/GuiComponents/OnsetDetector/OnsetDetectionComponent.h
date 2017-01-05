/*
  ==============================================================================

    OnsetDetectionComponent.h
    Created: 4 Jan 2017 7:57:41pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef ONSETDETECTIONCOMPONENT_H_INCLUDED
#define ONSETDETECTIONCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "./OSDOptionsComponent.h"
#include "./OSDParametersComponent.h"
#include  "./OSDPreProcessingComponent.h"

#include "../../PluginProcessor.h"

//==============================================================================
/*
*/
class OnsetDetectionComponent    : public Component
{
public:
    OnsetDetectionComponent(BeatboxVoxAudioProcessor& p);
    ~OnsetDetectionComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnsetDetectionComponent)

    BeatboxVoxAudioProcessor& processor;

	OSDOptionsComponent osdOptionsComponent;
	OSDParametersComponent osdParametersComponent;
	OSDPreProcessingComponent osdPreProcessingComponent;
};


#endif  // ONSETDETECTIONCOMPONENT_H_INCLUDED
