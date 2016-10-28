/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 19 Sep 2014 7:23:32pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef CustomLookAndFeel_H_INCLUDED
#define CustomLookAndFeel_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


class CustomLookAndFeel : public LookAndFeel_V3

{
public:
    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider&) override;
                           
    void drawLinearSliderThumb (Graphics& g, int x, int y, int width, int height,
                                float sliderPos, float minSliderPos, float maxSliderPos,

                                const Slider::SliderStyle style, Slider& slider) override;
	
	void drawButtonBackground(Graphics & g, Button & button, const Colour & backgroundColour, 
								bool isMouseOverButton, bool isButtonDown) override;

	void drawTickBox(Graphics& g, Component& component,
		float x, float y, float w, float h,
		bool ticked, bool isEnabled, bool isMouseOverButton,
		bool isButtonDown) override;
};



#endif  // CustomLookAndFeel_H_INCLUDED
