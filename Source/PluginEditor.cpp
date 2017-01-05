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
	  componentLookAndFeel(std::make_unique<CustomLookAndFeel>()),
      tabs(TabbedButtonBar::Orientation::TabsAtTop),
      classifierComponent(p),
      onsetDetectComponent(p)
{

	componentLookAndFeel->setColour (TextButton::buttonColourId, Colours::black);
    componentLookAndFeel->setColour (TextButton::textColourOffId, Colours::greenyellow);

    componentLookAndFeel->setColour (TextButton::buttonOnColourId, componentLookAndFeel->findColour (TextButton::textColourOffId));
    componentLookAndFeel->setColour (TextButton::textColourOnId, componentLookAndFeel->findColour (TextButton::buttonColourId));

	componentLookAndFeel->setColour(Slider::ColourIds::thumbColourId, Colours::greenyellow);

	setLookAndFeel(componentLookAndFeel.get());


    tabs.addTab("Onset Detector", Colours::darkgrey, &onsetDetectComponent, false, 0);
    tabs.addTab("Audio Classifier", Colours::darkgrey, &classifierComponent, false, 1);

    addAndMakeVisible(tabs);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 850);
    
    //Set timer for gui update callback
    //startTimer(10);
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
}


//==============================================================================

void BeatboxVoxAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void BeatboxVoxAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	auto r (getLocalBounds());

    tabs.setBounds(r);
}
