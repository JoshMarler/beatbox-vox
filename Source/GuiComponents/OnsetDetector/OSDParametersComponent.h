/*
  ==============================================================================

    OSDParametersComponent.h
    Created: 4 Jan 2017 8:02:25pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef OSDPARAMETERSCOMPONENT_H_INCLUDED
#define OSDPARAMETERSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"


//==============================================================================
/*
*/
class OSDParametersComponent    : public Component
{
public:
    OSDParametersComponent(BeatboxVoxAudioProcessor& p);
    ~OSDParametersComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSDParametersComponent)

	BeatboxVoxAudioProcessor& processor;

	Label headerLabel;

	Label noiseRatioLabel;
	Label msBetweenOnsetsLabel;
	Label meanCoeffLabel;
	Label medianCoeffLabel;
	
	//Parameter sliders
    std::unique_ptr<Slider> meanCoeffSlider;
    std::unique_ptr<Slider> medianCoeffSlider;
    std::unique_ptr<Slider> noiseRatioSlider;
    std::unique_ptr<Slider> msBetweenOnsetsSlider;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> meanCoeffAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> medianCoeffAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> noiseRatioAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> msBetweenOnsetsAttachment;

};


#endif  // OSDPARAMETERSCOMPONENT_H_INCLUDED
