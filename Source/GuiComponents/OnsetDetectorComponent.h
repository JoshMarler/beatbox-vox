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

class OnsetDetectorComponent : public Component,
                               private ButtonListener
{

public:
    OnsetDetectorComponent(BeatboxVoxAudioProcessor& p);
    ~OnsetDetectorComponent();

    void paint(Graphics& g) override;
    void resized() override;

    void buttonClicked(Button* button) override;

private:

    BeatboxVoxAudioProcessor& processor;

    Label meanCoeffLabel;
    Label noiseRatioLabel;
    Label useOSDTestSoundLabel;

    ToggleButton useOSDTestSoundButton;

    //Parameter sliders
    std::unique_ptr<CustomSlider> meanCoeffSlider;
    std::unique_ptr<CustomSlider> noiseRatioSlider;
    
};


#endif  // ONSETDETECTORCOMPONENT_H_INCLUDED
