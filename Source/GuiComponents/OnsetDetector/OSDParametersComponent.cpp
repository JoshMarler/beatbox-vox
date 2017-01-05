/*
  ==============================================================================

    OSDParametersComponent.cpp
    Created: 4 Jan 2017 8:02:25pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OSDParametersComponent.h"

//==============================================================================
OSDParametersComponent::OSDParametersComponent(BeatboxVoxAudioProcessor& p)
	: processor(p)
{
	headerLabel.setText("Parameters", juce::NotificationType::dontSendNotification);
	headerLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headerLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headerLabel);

	meanCoeffSlider = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxRight);
	medianCoeffSlider = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxRight);
	noiseRatioSlider = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxRight);
	msBetweenOnsetsSlider = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxRight);

	addAndMakeVisible(*meanCoeffSlider);
	addAndMakeVisible(*medianCoeffSlider);
	addAndMakeVisible(*noiseRatioSlider);
	addAndMakeVisible(*msBetweenOnsetsSlider);

	meanCoeffAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getValueTreeState(),
		BeatboxVoxAudioProcessor::paramOSDMeanCoeff,
		*meanCoeffSlider);

	medianCoeffAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getValueTreeState(),
		BeatboxVoxAudioProcessor::paramOSDMedianCoeff,
		*medianCoeffSlider);

	noiseRatioAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getValueTreeState(),
		BeatboxVoxAudioProcessor::paramOSDNoiseRatio,
		*noiseRatioSlider);

	msBetweenOnsetsAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getValueTreeState(),
		BeatboxVoxAudioProcessor::paramOSDMsBetweenOnsets,
		*msBetweenOnsetsSlider);


	noiseRatioLabel.setText("Noise Ratio", juce::NotificationType::dontSendNotification);
	noiseRatioLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	noiseRatioLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(noiseRatioLabel);

	msBetweenOnsetsLabel.setText("Ms Between Onsets", juce::NotificationType::dontSendNotification);
	msBetweenOnsetsLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	msBetweenOnsetsLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(msBetweenOnsetsLabel);

	meanCoeffLabel.setText("Mean Coefficient", juce::NotificationType::dontSendNotification);
	meanCoeffLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	meanCoeffLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(meanCoeffLabel);

	medianCoeffLabel.setText("Median Coefficient", juce::NotificationType::dontSendNotification);
	medianCoeffLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	medianCoeffLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(medianCoeffLabel);
}

OSDParametersComponent::~OSDParametersComponent()
{
}

void OSDParametersComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

void OSDParametersComponent::resized()
{
	auto bounds(getLocalBounds());
	bounds.reduce((bounds.getWidth() / 50), (bounds.getHeight()) / 50);

	auto headerSection = bounds.removeFromTop(bounds.getHeight() / 20);
	headerLabel.setBounds(headerSection);

	bounds.reduce(0, bounds.getHeight() / 20);

	auto noiseRatioSection = bounds.removeFromTop(bounds.getHeight() / 6);
	noiseRatioSection.removeFromRight(noiseRatioSection.getWidth() / 6);
	noiseRatioLabel.setBounds(noiseRatioSection.removeFromTop(noiseRatioSection.getHeight() / 5));
	noiseRatioSlider->setBounds(noiseRatioSection);

	auto msBetweenOnsetsArea = bounds.removeFromTop(bounds.getHeight() / 5);
	msBetweenOnsetsArea.removeFromRight(msBetweenOnsetsArea.getWidth() / 6);
	msBetweenOnsetsLabel.setBounds(msBetweenOnsetsArea.removeFromTop(msBetweenOnsetsArea.getHeight() / 5));
	msBetweenOnsetsSlider->setBounds(msBetweenOnsetsArea);

	auto meanCoeffArea = bounds.removeFromTop(bounds.getHeight() / 4);
	meanCoeffArea.removeFromRight(msBetweenOnsetsArea.getWidth() / 6);
	meanCoeffLabel.setBounds(meanCoeffArea.removeFromTop(meanCoeffArea.getHeight() / 5));
	meanCoeffSlider->setBounds(meanCoeffArea);

	auto medianCoeffArea = bounds.removeFromTop(bounds.getHeight() / 3);
	medianCoeffArea.removeFromRight(msBetweenOnsetsArea.getWidth() / 6);
	medianCoeffLabel.setBounds(medianCoeffArea.removeFromTop(meanCoeffArea.getHeight() / 5));
	medianCoeffSlider->setBounds(medianCoeffArea);
}
