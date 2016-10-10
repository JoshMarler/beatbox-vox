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

#include "CustomLookAndFeel.h"
#include "../PluginProcessor.h"

class OnsetDetectorComponent : public Component,
                               private ButtonListener,
                               private ComboBoxListener
{

public:
	explicit OnsetDetectorComponent(BeatboxVoxAudioProcessor& p);    
    ~OnsetDetectorComponent();

    void paint(Graphics& g) override;
    void resized() override;

    void buttonClicked(Button* button) override;
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    static String useOSDTestSoundButtonID;
    static String odfComboBoxID;
	static String useAdaptWhitenButtonID;

private:

    BeatboxVoxAudioProcessor& processor;

    Label meanCoeffLabel;
    Label medianCoeffLabel;
    Label noiseRatioLabel;
    Label useOSDTestSoundLabel;
    Label msBetweenOnsetsLabel;
    Label odfTypeLabel;
	Label useAdaptWhitenLabel;

    ToggleButton useOSDTestSoundButton;
    ComboBox odfTypeSelector;
	ToggleButton useAdaptWhitenButton;

    //Parameter sliders
    std::unique_ptr<Slider> meanCoeffSlider;
    std::unique_ptr<Slider> medianCoeffSlider;
    std::unique_ptr<Slider> noiseRatioSlider;
    std::unique_ptr<Slider> msBetweenOnsetsSlider;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> meanCoeffAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> medianCoeffAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> noiseRatioAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> msBetweenOnsetsAttachment;


    void populateODFTypeSelector();

};


#endif  // ONSETDETECTORCOMPONENT_H_INCLUDED
