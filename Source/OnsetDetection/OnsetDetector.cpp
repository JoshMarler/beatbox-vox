/*
  ==============================================================================

    OnsetDetector.cpp
    Created: 28 Jun 2016 6:40:26pm
    Author:  joshua

  ==============================================================================
*/

#include "OnsetDetector.h"

//==============================================================================
OnsetDetector::OnsetDetector()
{
    usingLocalMaximum = true;
    meanCoeff = 1.5f;
    numPreviousValues = 10;
    threshold = 0.1f;

    previousValues.resize(numPreviousValues);
    std::fill(previousValues.begin(), previousValues.end(), 0.0f);   
}

OnsetDetector::~OnsetDetector()
{

}

//==============================================================================
bool OnsetDetector::checkForOnset(float featureValue) 
{
    auto isOnset = false;
    
    if (usingLocalMaximum) 
    {
        if (previousValues[0] > threshold && previousValues[0] > featureValue && previousValues[0] > previousValues[1]) 
            isOnset = true;           
        
    }
    else
    {
        if (featureValue > threshold)
            isOnset = true;
    }

    threshold = meanCoeff * MathHelpers::getMean(previousValues);

    //JWM - NOTE: Would be nice to do this with proper iterators later.
    for (auto i = numPreviousValues - 1; i > 0; i--)
    {
        previousValues[i] = previousValues[i - 1];
    }

    previousValues[0] = featureValue;
    

    return isOnset;
}

//=============================================================================
bool OnsetDetector::getUsingLocalMaximum()
{
    return usingLocalMaximum;
}

//=============================================================================
void OnsetDetector::setUsingLocalMaximum(bool newUsingLocalMaximum)
{
    usingLocalMaximum = newUsingLocalMaximum;
}
