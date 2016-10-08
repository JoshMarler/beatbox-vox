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
          : processor(p),
            recordSoundButton(std::make_unique<TextButton> ("Record Training Sound")),
            trainClassifierButton(std::make_unique<TextButton> ("Train Model"))
{
    trainClassifierButton->setClickingTogglesState(true);
    trainClassifierButton->setColour(TextButton::buttonColourId, Colours::white);
    trainClassifierButton->setColour(TextButton::buttonOnColourId, Colours::greenyellow);
    trainClassifierButton->setEnabled(false);
    trainClassifierButton->addListener(this);
    
    addAndMakeVisible(*trainClassifierButton);

    recordSoundButton->setClickingTogglesState(true);
    recordSoundButton->setColour(TextButton::buttonColourId, Colours::white);
    recordSoundButton->setColour(TextButton::buttonOnColourId, Colours::greenyellow);
    recordSoundButton->addListener(this);
    
    addAndMakeVisible(*recordSoundButton);
    

    auto numSounds = processor.getClassifier().getNumSounds();

    for (size_t i = 0; i < numSounds; ++i)
    {
        soundButtons.add(new ToggleButton("Sound " + String(i + 1) + " Button"));

        auto button = soundButtons[i];
        button->setRadioGroupId(1234);
        button->addListener(this);
        button->setColour(ToggleButton::ColourIds::textColourId, Colours::greenyellow);
        button->setColour(ToggleButton::ColourIds::tickColourId, Colours::greenyellow);

        addAndMakeVisible(button);
    }
    
    //Set timer for gui update callback
    startTimer(10);

}

//===============================================================================
AudioClassifierComponent::~AudioClassifierComponent()
{

}

//===============================================================================
void AudioClassifierComponent::paint(Graphics& g)
{
    g.fillAll (Colours::transparentBlack);
}

//===============================================================================
void AudioClassifierComponent::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	auto r (getLocalBounds().reduced(15));

    recordSoundButton->setBounds(r.removeFromTop(25));

    for (size_t i = 0; i < soundButtons.size(); ++i)
    {
        auto button = soundButtons[i]; 
        button->setBounds(r.removeFromTop(75 + (10 * i)));
    }

    trainClassifierButton->setBounds(r.removeFromBottom(15));

}

//===============================================================================
void AudioClassifierComponent::timerCallback()
{
     auto isTraining = processor.getClassifier().isTraining();
     auto trainingSetReady = processor.getClassifier().checkTrainingSetReady();
     auto classifierReady = processor.getClassifier().getClassifierReady();

     if (trainingSetReady)
         trainClassifierButton->setEnabled(true);

     if (trainClassifierButton->getToggleState())
     {
        if (classifierReady)
            trainClassifierButton->setToggleState(false, NotificationType::dontSendNotification);
     }
         
     //JWM - A little sketchy way of doing things maybe but works for prototype stage. 
     if (recordSoundButton->getToggleState())
     {
        if (isTraining == false)
           recordSoundButton->setToggleState(false, NotificationType::dontSendNotification);
     }

}

//===============================================================================
void AudioClassifierComponent::buttonClicked(Button* button)
{

    if (button->getRadioGroupId() == 1234)
    {
        if (button->getToggleState())
        {
            currentTrainingSound = soundButtons.indexOf(button);
        }
    }
    else if (button == std::addressof(*trainClassifierButton))
    {
        if (button->getToggleState())
            processor.getClassifier().trainModel();    
    }
    else if (button == std::addressof(*recordSoundButton))
    {
        if (button->getToggleState())
            processor.getClassifier().recordTrainingSample(currentTrainingSound);
    }

}

//===============================================================================
