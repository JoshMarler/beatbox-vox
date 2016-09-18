/*
  ==============================================================================

    OnsetDetectorComponent.cpp
    Created: 17 Sep 2016 5:08:18pm
    Author:  joshua

  ==============================================================================
*/

#include "OnsetDetectorComponent.h"
//==============================================================================
OnsetDetectorComponent::OnsetDetectorComponent(BeatboxVoxAudioProcessor& p)
    : processor(p)
{

    meanCoeffSlider = std::make_unique<CustomSlider> (processor.getParameterFromID(BeatboxVoxAudioProcessor::paramOSDMeanCoeff));
    noiseRatioSlider = std::make_unique<CustomSlider> (processor.getParameterFromID(BeatboxVoxAudioProcessor::paramOSDNoiseRatio));

    addAndMakeVisible(*meanCoeffSlider);
    addAndMakeVisible(*noiseRatioSlider);

}

//==============================================================================
OnsetDetectorComponent::~OnsetDetectorComponent()
{

}

//==============================================================================
void OnsetDetectorComponent::paint(Graphics& g)
{
    g.fillAll(Colours::transparentBlack);
}

//==============================================================================
void OnsetDetectorComponent::resized()
{
    Rectangle<int> r (getLocalBounds());
}

