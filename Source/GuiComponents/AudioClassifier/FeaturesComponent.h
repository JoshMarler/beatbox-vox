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
class FeaturesComponent    : public Component
{
public:
	explicit FeaturesComponent(BeatboxVoxAudioProcessor& p);
    ~FeaturesComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeaturesComponent)

	BeatboxVoxAudioProcessor& processor;

	Label headingLabel;
};


#endif  // FEATURESCOMPONENT_H_INCLUDED
