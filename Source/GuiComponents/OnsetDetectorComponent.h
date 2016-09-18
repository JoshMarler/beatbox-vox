/*
  ==============================================================================

    OnsetDetectorComponent.h
    Created: 17 Sep 2016 5:08:18pm
    Author:  joshua

  ==============================================================================
*/

#ifndef ONSETDETECTORCOMPONENT_H_INCLUDED
#define ONSETDETECTORCOMPONENT_H_INCLUDED

#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"

#include "CustomSlider.h"
#include "CustomLookAndFeel.h"

#include "../PluginProcessor.h"

class OnsetDetectorComponent : public Component
{

public:
    OnsetDetectorComponent(BeatboxVoxAudioProcessor& p);
    ~OnsetDetectorComponent();

    void paint(Graphics& g) override;
    void resized() override;

private:

    BeatboxVoxAudioProcessor& processor;

};


#endif  // ONSETDETECTORCOMPONENT_H_INCLUDED
