/*
  ==============================================================================

    CallbackParameter.h
    Created: 20 Sep 2016 9:07:49pm
    Author:  joshua

  ==============================================================================
*/

#ifndef CALLBACKPARAMETER_H_INCLUDED
#define CALLBACKPARAMETER_H_INCLUDED

#include <atomic>

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class CallbackParameter : public AudioProcessorParameterWithID
{
public:
    //==============================================================================

    CallbackParameter(String initParamId, std::function<void(float)> initSetValueCallback, NormalisableRange<float> initRange, float initDefaultVal);
    ~CallbackParameter();

    //==============================================================================

    float getValue() const override;
    void setValue(float newValue) override;

    float getDefaultValue() const override;
    String getName(int maximumStringLength) const override;
    
    void setLabel(String newLabelText);
    String getLabel() const override;
    
    float getValueForText (const String& text) const override;
    String getText(float value, int = 0 /*Maximum String Length - default value provided/optional*/) const override;
    //==============================================================================

private:

    float defaultValue;
    NormalisableRange<float> range;
    
    std::atomic<float> value;
    
    String labelText = "";
    
    std::function<void(float)> setValueCallback = nullptr;
};



#endif  // CALLBACKPARAMETER_H_INCLUDED
