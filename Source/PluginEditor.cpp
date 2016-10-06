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
      tabs(TabbedButtonBar::Orientation::TabsAtTop),
      classifierComponent(p),
      onsetDetectComponent(p)
{

    tabs.addTab("Onset Detector", Colours::black, &onsetDetectComponent, false, 0);
    tabs.addTab("Audio Classifier", Colours::black, &classifierComponent, false, 1);

    addAndMakeVisible(tabs);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 600);
    
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
    g.fillAll (Colours::white);
}

void BeatboxVoxAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> r (getLocalBounds());

    tabs.setBounds(r);
}
