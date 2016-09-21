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
    : processor(p), 
      useOSDTestSoundButton("Use Onset Detection Test Sound")
{

    meanCoeffSlider = std::make_unique<Slider> (Slider::LinearHorizontal, Slider::TextBoxRight);
    noiseRatioSlider = std::make_unique<Slider> (Slider::LinearHorizontal, Slider::TextBoxRight);
    msBetweenOnsetsSlider = std::make_unique<Slider> (Slider::LinearHorizontal, Slider::TextBoxRight);

    addAndMakeVisible(*meanCoeffSlider);
    addAndMakeVisible(*noiseRatioSlider);
    addAndMakeVisible(*msBetweenOnsetsSlider);


    meanCoeffAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getValueTreeState(), 
                                                                                           BeatboxVoxAudioProcessor::paramOSDMeanCoeff,
                                                                                           *meanCoeffSlider);

    noiseRatioAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getValueTreeState(), 
                                                                                            BeatboxVoxAudioProcessor::paramOSDNoiseRatio, 
                                                                                            *noiseRatioSlider);

    msBetweenOnsetsAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getValueTreeState(),
                                                                                                 BeatboxVoxAudioProcessor::paramOSDMsBetweenOnsets, 
                                                                                                 *msBetweenOnsetsSlider);


    meanCoeffLabel.setText("Mean Coefficient", juce::NotificationType::dontSendNotification);
    meanCoeffLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    meanCoeffLabel.setColour(Label::textColourId, Colours::greenyellow);

    noiseRatioLabel.setText("Noise Ratio", juce::NotificationType::dontSendNotification);
    noiseRatioLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    noiseRatioLabel.setColour(Label::textColourId, Colours::greenyellow);

    msBetweenOnsetsLabel.setText("Ms Between Onsets", juce::NotificationType::dontSendNotification);
    msBetweenOnsetsLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    msBetweenOnsetsLabel.setColour(Label::textColourId, Colours::greenyellow);

    useOSDTestSoundLabel.setText("Enable Onset Detector Test Sound", juce::NotificationType::dontSendNotification);
    useOSDTestSoundLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    useOSDTestSoundLabel.setColour(Label::textColourId, Colours::greenyellow);


    addAndMakeVisible(noiseRatioLabel);
    addAndMakeVisible(meanCoeffLabel);
    addAndMakeVisible(msBetweenOnsetsLabel);
    addAndMakeVisible(useOSDTestSoundLabel);

    useOSDTestSoundButton.addListener(this);
    useOSDTestSoundButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(useOSDTestSoundButton);
    
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
    Rectangle<int> leftSide(r.removeFromLeft(getWidth() / 2));


    //Rectangle/Bounds for meanCoeff controls
    Rectangle<int> meanCoeffArea(leftSide.removeFromTop(leftSide.getHeight() / 4));

    meanCoeffLabel.setBounds(meanCoeffArea.removeFromTop(meanCoeffArea.getHeight() / 5));
    meanCoeffSlider->setBounds(meanCoeffArea);
    
    
    //Rectangle/Bounds for noiseRatio controls
    Rectangle<int> noiseRatioArea(leftSide.removeFromTop(leftSide.getHeight() / 3));

    noiseRatioLabel.setBounds(noiseRatioArea.removeFromTop(noiseRatioArea.getHeight() / 5));
    noiseRatioSlider->setBounds(noiseRatioArea);
    
    
    //Rectangle/Bounds for msBetweenOnsets controls 
    Rectangle<int> msBetweenOnsetsArea(leftSide.removeFromTop(leftSide.getHeight() / 2));

    msBetweenOnsetsLabel.setBounds(msBetweenOnsetsArea.removeFromTop(msBetweenOnsetsArea.getHeight() / 5));
    msBetweenOnsetsSlider->setBounds(msBetweenOnsetsArea);

    
    //Rectangle/Bounds for useOSDTestSound controls
    Rectangle<int> useOSDTestSoundArea(leftSide);

    useOSDTestSoundLabel.setBounds(leftSide.removeFromTop(leftSide.getHeight() / 5));
    useOSDTestSoundButton.setBounds(leftSide);
}

//==============================================================================
void OnsetDetectorComponent::buttonClicked(Button* button)
{
    if (button == &useOSDTestSoundButton)
    {
        processor.setUsingOSDTestSound(button->getToggleState()); 
    }
}
//==============================================================================

