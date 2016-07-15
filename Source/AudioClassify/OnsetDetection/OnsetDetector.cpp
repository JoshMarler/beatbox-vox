/*
  ==============================================================================

    OnsetDetector.cpp
    Created: 28 Jun 2016 6:40:26pm
    Author:  joshua

  ==============================================================================
*/

#include "OnsetDetector.h"

//==============================================================================

template<typename T>
OnsetDetector<T>::OnsetDetector(int bufferSize)
{
    usingLocalMaximum = true;
    meanCoeff = 1.5;
    numPreviousValues = 10;
    threshold = 0.1;

    previousValues.resize(numPreviousValues);
    std::fill(previousValues.begin(), previousValues.end(), 0.0);   
}

template<typename T>
OnsetDetector<T>::~OnsetDetector()
{

}

//==============================================================================

template<typename T>
bool OnsetDetector<T>::checkForOnset(T featureValue) 
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

template<typename T>
bool OnsetDetector<T>::getUsingLocalMaximum()
{
    return usingLocalMaximum;
}


template<typename T>
void OnsetDetector<T>::setUsingLocalMaximum(bool newUsingLocalMaximum)
{
    usingLocalMaximum = newUsingLocalMaximum;
}

//=============================================================================
template<typename T>
void OnsetDetector<T>::setCurrentODFType(AudioClassifyOptions::ODFType newODFType)
{
    currentODFType.store(newODFType);   
}

//=============================================================================
template class OnsetDetector<float>;
template class OnsetDetector<double>;
