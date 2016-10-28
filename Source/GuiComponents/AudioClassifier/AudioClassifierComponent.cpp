/*
  ==============================================================================

    AudioClassifierComponent.cpp
    Created: 17 Sep 2016 5:13:37pm
    Author:  joshua

  ==============================================================================
*/

#include "AudioClassifierComponent.h"

//===============================================================================

AudioClassifierComponent::AudioClassifierComponent(BeatboxVoxAudioProcessor& p)
          : processor(p)
{
	recordTrainingSetComponent = std::make_unique<RecordTrainingSetComponent>(processor);
	selectClassifierComponent = std::make_unique<SelectClassifierComponent>(processor);
	delayedEvaluationComponent = std::make_unique<DelayedEvaluationComponent>(processor);

	addAndMakeVisible(recordTrainingSetComponent.get());
	addAndMakeVisible(selectClassifierComponent.get());
	addAndMakeVisible(delayedEvaluationComponent.get());
}


//===============================================================================
AudioClassifierComponent::~AudioClassifierComponent()
{

}

//===============================================================================
void AudioClassifierComponent::paint(Graphics& g)
{
    g.fillAll (Colours::darkgrey);
}

//===============================================================================
void AudioClassifierComponent::resized()
{
	auto bounds = getLocalBounds();
	auto boundsTop = bounds.removeFromTop(bounds.getHeight() / 6);

	boundsTop.reduce(boundsTop.getWidth() / 100, boundsTop.getHeight() / 50);

	selectClassifierComponent->setBounds(boundsTop);

	auto boundsLeft = bounds.removeFromLeft(bounds.getWidth() / 2);
	auto boundsRight = bounds;

	boundsLeft.reduce(boundsLeft.getWidth() / 50, boundsLeft.getHeight() / 75);
	boundsRight.reduce(boundsRight.getWidth() / 50, boundsRight.getHeight() / 75);

	recordTrainingSetComponent->setBounds(boundsLeft);
	delayedEvaluationComponent->setBounds(boundsRight);
}

//===============================================================================
void AudioClassifierComponent::buttonClicked(Button* button)
{

}

//===============================================================================

