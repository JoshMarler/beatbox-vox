/*
  ==============================================================================

    CallbackParameter.cpp
    Created: 20 Sep 2016 9:07:49pm
    Author:  joshua

  ==============================================================================
*/

#include "CallbackParameter.h"

//==============================================================================
CallbackParameter::CallbackParameter(String initParamId, std::function<void(float)> initSetValueCallback, NormalisableRange<float> initRange, float initDefaultVal)
    : AudioProcessorParameterWithID(initParamId, initParamId)
{
    setValueCallback = initSetValueCallback;
    range = initRange;

    defaultValue = initDefaultVal;
    value.store(defaultValue);
}

CallbackParameter::~CallbackParameter()
{

}

//==============================================================================
float CallbackParameter::getValue() const
{
    return range.convertTo0to1(value.load());
}

void CallbackParameter::setValue(float newValue)
{
   value.store(range.convertFrom0to1(newValue));

   setValueCallback(value.load());
}

//==============================================================================
//NOTE: JWM - In slider class could use param.getText(this->getValue()) to set slider label
String CallbackParameter::getText(float v, int /* length */) const
{
    return String(range.convertFrom0to1(v));   
}

float CallbackParameter::getValueForText(const String& text) const
{
    return range.convertTo0to1(text.getFloatValue());
}

//==============================================================================
void CallbackParameter::setLabel(String newLabelText)
{
    labelText = newLabelText;
}

String CallbackParameter::getLabel() const
{
    return labelText;
}
//==============================================================================
String CallbackParameter::getName(int maximumStringLength) const
{
    return this->name;
}

//==============================================================================
float CallbackParameter::getDefaultValue() const
{
    return defaultValue;
}
//==============================================================================

