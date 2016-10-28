/*
  ==============================================================================

    DelayedEvaluationComponent.h
    Created: 28 Oct 2016 9:00:48pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef DELAYEDEVALUATIONCOMPONENT_H_INCLUDED
#define DELAYEDEVALUATIONCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"

//==============================================================================
/*
*/
class DelayedEvaluationComponent    : public Component
{
public:
    DelayedEvaluationComponent(BeatboxVoxAudioProcessor& p);
    ~DelayedEvaluationComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayedEvaluationComponent)

	Label headingLabel;
};


#endif  // DELAYEDEVALUATIONCOMPONENT_H_INCLUDED
