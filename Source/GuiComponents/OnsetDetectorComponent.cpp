/*
  ==============================================================================

    OnsetDetectorComponent.cpp
    Created: 17 Sep 2016 5:08:18pm
    Author:  joshua

  ==============================================================================
*/

#include "OnsetDetectorComponent.h"
//==============================================================================

String OnsetDetectorComponent::useOSDTestSoundButtonID ("UseOSDTestSound");
String OnsetDetectorComponent::odfComboBoxID ("ODFComboBox");

OnsetDetectorComponent::OnsetDetectorComponent(BeatboxVoxAudioProcessor& p)
    : processor(p), 
      useOSDTestSoundButton("Use Onset Detection Test Sound"),
      odfTypeSelector("ODF Type Selector")
{

    odfTypeSelector.setComponentID(odfComboBoxID);

    populateODFTypeSelector();
    addAndMakeVisible(odfTypeSelector);
    

    meanCoeffSlider = std::make_unique<Slider> (Slider::LinearHorizontal, Slider::TextBoxRight);
    medianCoeffSlider = std::make_unique<Slider> (Slider::LinearHorizontal, Slider::TextBoxRight);
    noiseRatioSlider = std::make_unique<Slider> (Slider::LinearHorizontal, Slider::TextBoxRight);
    msBetweenOnsetsSlider = std::make_unique<Slider> (Slider::LinearHorizontal, Slider::TextBoxRight);

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


    meanCoeffLabel.setText("Mean Coefficient", juce::NotificationType::dontSendNotification);
    meanCoeffLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    meanCoeffLabel.setColour(Label::textColourId, Colours::greenyellow);

    medianCoeffLabel.setText("Median Coefficient", juce::NotificationType::dontSendNotification);
    medianCoeffLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    medianCoeffLabel.setColour(Label::textColourId, Colours::greenyellow);

    noiseRatioLabel.setText("Noise Ratio", juce::NotificationType::dontSendNotification);
    noiseRatioLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    noiseRatioLabel.setColour(Label::textColourId, Colours::greenyellow);

    msBetweenOnsetsLabel.setText("Ms Between Onsets", juce::NotificationType::dontSendNotification);
    msBetweenOnsetsLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    msBetweenOnsetsLabel.setColour(Label::textColourId, Colours::greenyellow);

    useOSDTestSoundLabel.setText("Enable Onset Detector Test Sound", juce::NotificationType::dontSendNotification);
    useOSDTestSoundLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    useOSDTestSoundLabel.setColour(Label::textColourId, Colours::greenyellow);


    odfTypeLabel.setText("Onset Detection Function", juce::NotificationType::dontSendNotification);
    odfTypeLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    odfTypeLabel.setColour(Label::textColourId, Colours::greenyellow);

    addAndMakeVisible(meanCoeffLabel);
    addAndMakeVisible(medianCoeffLabel);
    addAndMakeVisible(noiseRatioLabel);
    addAndMakeVisible(msBetweenOnsetsLabel);
    addAndMakeVisible(useOSDTestSoundLabel);
    addAndMakeVisible(odfTypeLabel);

    useOSDTestSoundButton.addListener(this);
    useOSDTestSoundButton.setComponentID(useOSDTestSoundButtonID);
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
    Rectangle<int> rightSide(r);

    
    Rectangle<int> odfTypeArea(rightSide.removeFromTop(rightSide.getHeight() / 5));
    
    odfTypeLabel.setBounds(odfTypeArea.removeFromTop(odfTypeArea.getHeight() / 5));
    odfTypeSelector.setBounds(odfTypeArea.reduced(10, 10));

    //Rectangle/Bounds for meanCoeff controls
    Rectangle<int> meanCoeffArea(leftSide.removeFromTop(leftSide.getHeight() / 5));

    meanCoeffLabel.setBounds(meanCoeffArea.removeFromTop(meanCoeffArea.getHeight() / 5));
    meanCoeffSlider->setBounds(meanCoeffArea);
    
    
    //Rectangle/Bounds for noiseRatio controls
    Rectangle<int> noiseRatioArea(leftSide.removeFromTop(leftSide.getHeight() / 4));

    noiseRatioLabel.setBounds(noiseRatioArea.removeFromTop(noiseRatioArea.getHeight() / 5));
    noiseRatioSlider->setBounds(noiseRatioArea);
    
    
    //Rectangle/Bounds for msBetweenOnsets controls 
    Rectangle<int> msBetweenOnsetsArea(leftSide.removeFromTop(leftSide.getHeight() / 3));

    msBetweenOnsetsLabel.setBounds(msBetweenOnsetsArea.removeFromTop(msBetweenOnsetsArea.getHeight() / 5));
    msBetweenOnsetsSlider->setBounds(msBetweenOnsetsArea);

    
    
    //Rectangle/Bounds for useOSDTestSound controls
    Rectangle<int> medianCoeffArea(leftSide.removeFromTop(leftSide.getHeight() / 2));

    medianCoeffLabel.setBounds(medianCoeffArea.removeFromTop(medianCoeffArea.getHeight() / 5));
    medianCoeffSlider->setBounds(medianCoeffArea); 
    
    //Give remaining leftSide space to useOSDTestSound controls
    useOSDTestSoundLabel.setBounds(leftSide.removeFromTop(leftSide.getHeight() / 5));
    useOSDTestSoundButton.setBounds(leftSide);
}

//==============================================================================
void OnsetDetectorComponent::buttonClicked(Button* button)
{
    if (button->getComponentID() == useOSDTestSoundButtonID)
    {
        processor.setUsingOSDTestSound(button->getToggleState()); 
    }
}
//==============================================================================
void OnsetDetectorComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged->getComponentID() == odfComboBoxID)
    {
        auto& classifier = processor.getClassifier();

        auto odfType = odfTypeSelector.getSelectedId() - 1;
        classifier.setOnsetDetectorODFType(static_cast<AudioClassifyOptions::ODFType>(odfType));
    }
}

//==============================================================================
void OnsetDetectorComponent::populateODFTypeSelector()
{
    //odf enumerated values in AudioClassifyOptions begin at 0 so adding 1 to ID's - can't use 0 value for combobox id 
    odfTypeSelector.addItem("Spectral Difference", static_cast<int>(AudioClassifyOptions::ODFType::spectralDifference) + 1); 
    odfTypeSelector.addItem("Spectral Difference HWR", static_cast<int>(AudioClassifyOptions::ODFType::spectralDifferenceHWR) + 1);
    odfTypeSelector.addItem("High Frequency Content", static_cast<int>(AudioClassifyOptions::ODFType::highFrequencyContent) + 1);

    odfTypeSelector.setSelectedId(static_cast<int>(AudioClassifyOptions::ODFType::spectralDifference) + 1);
}

//==============================================================================

