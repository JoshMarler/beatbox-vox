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
	featuresComponent = std::make_unique<FeaturesComponent>(processor);
	bufferHandlingComponent = std::make_unique<BufferHandlingComponent>(processor);

	addAndMakeVisible(recordTrainingSetComponent.get());

	addAndMakeVisible(selectClassifierComponent.get());
	selectClassifierComponent->addChangeListener(this);

	addAndMakeVisible(featuresComponent.get());
	addAndMakeVisible(bufferHandlingComponent.get());
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

	auto centerBounds = bounds.removeFromTop(bounds.getHeight() / 1.5f);

	auto boundsLeft = centerBounds.removeFromLeft(centerBounds.getWidth() / 2);
	auto boundsRight = centerBounds;

	boundsLeft.reduce(boundsLeft.getWidth() / 55, boundsLeft.getHeight() / 75);
	boundsRight.reduce(boundsRight.getWidth() / 55, boundsRight.getHeight() / 75);

	recordTrainingSetComponent->setBounds(boundsLeft);
	featuresComponent->setBounds(boundsRight);

	auto boundsBottom = bounds;
	boundsBottom.reduce(boundsBottom.getWidth() / 100, boundsBottom.getHeight() / 50);
	bufferHandlingComponent->setBounds(boundsBottom);
}

//===============================================================================
void AudioClassifierComponent::changeListenerCallback(ChangeBroadcaster * source)
{
	if (source == selectClassifierComponent.get())
	{
		recordTrainingSetComponent->handleNewTrainingSetLoaded();
		bufferHandlingComponent->handleNewTrainingSetLoaded();
	}
}

//===============================================================================

