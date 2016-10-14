/*
  ==============================================================================

    OnsetDetectorComponent.cpp
    Created: 17 Sep 2016 5:08:18pm
    Author:  joshua

  ==============================================================================
*/

#include "OnsetDetectorComponent.h"
//==============================================================================

String OnsetDetectorComponent::useLocalMaximumButonID("UseLocalMaximum");
String OnsetDetectorComponent::useOSDTestSoundButtonID ("UseOSDTestSound");
String OnsetDetectorComponent::odfComboBoxID ("ODFComboBox");
String OnsetDetectorComponent::useAdaptWhitenButtonID("UseAdaptWhiten");

OnsetDetectorComponent::OnsetDetectorComponent(BeatboxVoxAudioProcessor& p)
	: processor(p),
	  useLocalMaximumButton("Use Local Maximum"),
      useOSDTestSoundButton("Use Onset Detection Test Sound"),
      odfTypeSelector("ODF Type Selector"),
	  useAdaptWhitenButton("Use Adaptive Whitening")
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



    useLocalMaxLabel.setText("Use Local Maximum", juce::NotificationType::dontSendNotification);
	useLocalMaxLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    useLocalMaxLabel.setColour(Label::textColourId, Colours::greenyellow);
	
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

		
    useAdaptWhitenLabel.setText("Use Adaptive Whitening", juce::NotificationType::dontSendNotification);
    useAdaptWhitenLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    useAdaptWhitenLabel.setColour(Label::textColourId, Colours::greenyellow);

	addAndMakeVisible(useLocalMaxLabel);
    addAndMakeVisible(meanCoeffLabel);
    addAndMakeVisible(medianCoeffLabel);
    addAndMakeVisible(noiseRatioLabel);
    addAndMakeVisible(msBetweenOnsetsLabel);
    addAndMakeVisible(useOSDTestSoundLabel);
    addAndMakeVisible(odfTypeLabel);
	addAndMakeVisible(useAdaptWhitenLabel);

    useLocalMaximumButton.addListener(this);
    useLocalMaximumButton.setComponentID(useLocalMaximumButonID);
    useLocalMaximumButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(useLocalMaximumButton);

    useOSDTestSoundButton.addListener(this);
    useOSDTestSoundButton.setComponentID(useOSDTestSoundButtonID);
    useOSDTestSoundButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(useOSDTestSoundButton);

	useAdaptWhitenButton.addListener(this);
	useAdaptWhitenButton.setComponentID(useAdaptWhitenButtonID);
	useAdaptWhitenButton.setToggleState(false, juce::NotificationType::dontSendNotification);
	addAndMakeVisible(useAdaptWhitenButton);
    
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
	auto r (getLocalBounds());
	auto leftSide(r.removeFromLeft(r.getWidth() / 3));
    auto rightSide(r.removeFromRight(r.getWidth() / 2));

    
	//Rectangle/Bounds for onset detection function type controls
    auto odfTypeArea(rightSide.removeFromTop(rightSide.getHeight() / 5));
    
    odfTypeLabel.setBounds(odfTypeArea.removeFromTop(odfTypeArea.getHeight() / 5));
    odfTypeSelector.setBounds(odfTypeArea.reduced(10, 10));


	//Rectangle/Bounds for adaptive whitening controls
	auto adaptWhitenArea(rightSide.removeFromTop(rightSide.getHeight() / 4));

	useAdaptWhitenLabel.setBounds(adaptWhitenArea.removeFromTop(adaptWhitenArea.getHeight() / 8));
	useAdaptWhitenButton.setBounds(adaptWhitenArea);


	//Rectangle/Bounds for use local maximum controls
	auto useLocalMaxArea(leftSide.removeFromTop(leftSide.getHeight() / 6));

	useLocalMaxLabel.setBounds(useLocalMaxArea.removeFromTop(useLocalMaxArea.getHeight() / 5));
	useLocalMaximumButton.setBounds(useLocalMaxArea);


    //Rectangle/Bounds for meanCoeff controls
    auto meanCoeffArea(leftSide.removeFromTop(leftSide.getHeight() / 5));

    meanCoeffLabel.setBounds(meanCoeffArea.removeFromTop(meanCoeffArea.getHeight() / 5));
    meanCoeffSlider->setBounds(meanCoeffArea);
    
    
    //Rectangle/Bounds for noiseRatio controls
    auto noiseRatioArea(leftSide.removeFromTop(leftSide.getHeight() / 4));

    noiseRatioLabel.setBounds(noiseRatioArea.removeFromTop(noiseRatioArea.getHeight() / 5));
    noiseRatioSlider->setBounds(noiseRatioArea);
    
    
    //Rectangle/Bounds for msBetweenOnsets controls 
    auto msBetweenOnsetsArea(leftSide.removeFromTop(leftSide.getHeight() / 3));

    msBetweenOnsetsLabel.setBounds(msBetweenOnsetsArea.removeFromTop(msBetweenOnsetsArea.getHeight() / 5));
    msBetweenOnsetsSlider->setBounds(msBetweenOnsetsArea);
    
    
    //Rectangle/Bounds for useOSDTestSound controls
    auto medianCoeffArea(leftSide.removeFromTop(leftSide.getHeight() / 2));

    medianCoeffLabel.setBounds(medianCoeffArea.removeFromTop(medianCoeffArea.getHeight() / 5));
    medianCoeffSlider->setBounds(medianCoeffArea); 
    

    //Give remaining leftSide space to useOSDTestSound controls
    useOSDTestSoundLabel.setBounds(leftSide.removeFromTop(leftSide.getHeight() / 5));
    useOSDTestSoundButton.setBounds(leftSide);
}

//==============================================================================
void OnsetDetectorComponent::buttonClicked(Button* button)
{
	auto id = button->getComponentID();

	if (id == useLocalMaximumButonID)
		processor.getClassifier().setOSDUseLocalMaximum(button->getToggleState());
	else if (id == useOSDTestSoundButtonID)
		processor.setUsingOSDTestSound(button->getToggleState());
	else if (id == useAdaptWhitenButtonID)
		processor.getClassifier().setOSDUseAdaptiveWhitening(button->getToggleState());
}
//==============================================================================
void OnsetDetectorComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged->getComponentID() == odfComboBoxID)
    {
        auto& classifier = processor.getClassifier();

        auto odfType = odfTypeSelector.getSelectedId() - 1;
        classifier.setOSDDetectorFunctionType(static_cast<AudioClassifyOptions::ODFType>(odfType));
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

