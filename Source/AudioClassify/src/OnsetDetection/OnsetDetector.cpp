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
OnsetDetector<T>::OnsetDetector(int initBufferSize)
    : onsetDetectionFunction(std::make_unique<OnsetDetectionFunction<T>>(initBufferSize))
{
    usingLocalMaximum = true;
    meanCoeff = 1.5;
    numPreviousValues = 10;
    threshold = 0.1;

    previousValues.resize(numPreviousValues);
    std::fill(previousValues.begin(), previousValues.end(), 0.0);   

    setCurrentBufferSize(initBufferSize);
}

template<typename T>
OnsetDetector<T>::~OnsetDetector()
{

}


//==============================================================================

template<typename T>
int OnsetDetector<T>::getCurrentBufferSize()
{
  return bufferSize; 
}

template<typename T>
void OnsetDetector<T>::setCurrentBufferSize(int newBufferSize)
{
    bufferSize = newBufferSize;
}

//==============================================================================

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

template<typename T>
bool OnsetDetector<T>::checkForOnset(std::vector<T> magnitudeSpectrum)
{
    T featureValue = 0;
    bool hasOnset = false;

    featureValue = onsetDetectionFunction->highFrequencyContent(magnitudeSpectrum); 
    hasOnset = checkForPeak(featureValue);

    return hasOnset;
}
//=============================================================================
template<typename T>
bool OnsetDetector<T>::checkForPeak(T featureValue) 
{
    auto isOnset = false;
    
    if (getUsingLocalMaximum()) 
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


//=============================================================================
template class OnsetDetector<float>;
template class OnsetDetector<double>;
