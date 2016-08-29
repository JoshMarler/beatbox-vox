/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BeatboxVoxAudioProcessorEditor::BeatboxVoxAudioProcessorEditor (BeatboxVoxAudioProcessor& p)
    : AudioProcessorEditor (p),
      trainClassifierButton(std::make_unique<TextButton>("Train Model"))
{
    addAndMakeVisible(*trainClassifierButton);

    auto numSounds = p.getClassifier().getNumSounds();

    for (size_t i = 0; i < numSounds; ++i)
    {
        soundButtons.add(new ToggleButton("Sound " + String(i + 1) + " Button"));

        auto button = soundButtons[i];
        button->setRadioGroupId(1);
        button->addListener(this);

        addAndMakeVisible(button);
    }
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    //JWM - set timer for gui update callback
    startTimer(10);
}

BeatboxVoxAudioProcessorEditor::~BeatboxVoxAudioProcessorEditor()
{
}

//==============================================================================
void BeatboxVoxAudioProcessorEditor::timerCallback()
{
}

//==============================================================================
void BeatboxVoxAudioProcessorEditor::buttonClicked(Button* button)
{
    //JWM - Dirty button handling for initial prototype. Will make this all nicer in future. 
    //Probably going to change all this to set parameters with callback functions or using value tree params etc.
    BeatboxVoxAudioProcessor& processor = getProcessor();

    if (button == std::addressof(*trainClassifierButton))
    {
        //Call classifier.Train() ?            
    }
    else if (button->getToggleState())
    {
        auto sound = soundButtons.indexOf(button);

        switch (sound)
        {
            case BeatboxVoxAudioProcessor::soundLabel::KickDrum :
                break;
            case BeatboxVoxAudioProcessor::soundLabel::SnareDrum :
                break;
        }
    }
}


//==============================================================================

void BeatboxVoxAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void BeatboxVoxAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> r (getLocalBounds().reduced(15));

    trainClassifierButton->setBounds(r.removeFromTop(25));

    for(size_t i = 0; i < soundButtons.size(); ++i)
    {
        auto button = soundButtons[i]; 
        button->setBounds(r.removeFromTop(75 + (10 * i)));
    }
}
