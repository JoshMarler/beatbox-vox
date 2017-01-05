/*
  ==============================================================================

    OnsetDetectionComponent.cpp
    Created: 4 Jan 2017 7:57:41pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OnsetDetectionComponent.h"

//==============================================================================
OnsetDetectionComponent::OnsetDetectionComponent(BeatboxVoxAudioProcessor& p)
	: processor(p),
	  osdOptionsComponent(p),
	  osdParametersComponent(p),
	  osdPreProcessingComponent(p)
{
	addAndMakeVisible(osdOptionsComponent);
	addAndMakeVisible(osdParametersComponent);
	addAndMakeVisible(osdPreProcessingComponent);
}

OnsetDetectionComponent::~OnsetDetectionComponent()
{
}

void OnsetDetectionComponent::paint (Graphics& g)
{
	g.setColour(Colours::darkgrey);

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

void OnsetDetectionComponent::resized()
{
	auto bounds(getLocalBounds());
	
	auto boundsTop = bounds.removeFromTop(bounds.getHeight() / 6);
	boundsTop.reduce(boundsTop.getWidth() / 100, boundsTop.getHeight() / 50);
	osdOptionsComponent.setBounds(boundsTop);

	auto boundsLeft = bounds.removeFromLeft(bounds.getWidth() / 2);
	auto boundsRight = bounds;

	boundsLeft.reduce(boundsLeft.getWidth() / 55, boundsLeft.getHeight() / 75);
	boundsRight.reduce(boundsRight.getWidth() / 55, boundsRight.getHeight() / 75);

	osdParametersComponent.setBounds(boundsLeft);
	osdPreProcessingComponent.setBounds(boundsRight);
}
