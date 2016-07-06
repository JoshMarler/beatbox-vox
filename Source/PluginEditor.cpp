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
      spectralCentroidLabel(String::empty, "")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible(spectralCentroidLabel);
    spectralCentroidLabel.setColour(Label::textColourId, Colours::red);
    spectralCentroidLabel.setFont(Font(15.0f));

    //JWM - set timer for gui update callback
    startTimer(10);
}

BeatboxVoxAudioProcessorEditor::~BeatboxVoxAudioProcessorEditor()
{
}

//==============================================================================
void BeatboxVoxAudioProcessorEditor::timerCallback()
{
    float centroid = getProcessor().getSpectralCentroid();
    
    //Set spectral centroid label
    spectralCentroidLabel.setText(String(centroid, 2), dontSendNotification);
}
void BeatboxVoxAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void BeatboxVoxAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> r (getLocalBounds().reduced(8));
    spectralCentroidLabel.setBounds(r.removeFromTop(25));
}
