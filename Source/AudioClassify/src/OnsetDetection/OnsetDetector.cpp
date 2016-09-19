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
    : onsetDetectionFunction(initBufferSize)
{
    usingLocalMaximum = true;
    numPreviousValues = 10;
    threshold = 0.5;
    largestPeak = 0.0;

    meanCoeff.store(1.5);
    noiseRatio.store(0.05);
    
    previousValues.reset(new T[numPreviousValues]);

    std::fill(previousValues.get(), (previousValues.get() + numPreviousValues), static_cast<T>(0.0));   

    setCurrentBufferSize(initBufferSize);
}

//==============================================================================
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
void OnsetDetector<T>::setNoiseRatio(T newNoiseRatio)
{
    noiseRatio.store(newNoiseRatio);
}

template<typename T>
T OnsetDetector<T>::getNoiseRatio() const
{
    return noiseRatio.load();
}
//=============================================================================
template<typename T>
void OnsetDetector<T>::setMeanCoefficient(T newCoeff)
{
    meanCoeff.store(newCoeff);
}

template<typename T>
T OnsetDetector<T>::getMeanCoefficient() const
{
    return meanCoeff.load();
}
//=============================================================================
template<typename T>
void OnsetDetector<T>::setCurrentODFType(AudioClassifyOptions::ODFType newODFType)
{
    currentODFType.store(newODFType);   
}

//=============================================================================
template<typename T>
bool OnsetDetector<T>::checkForOnset(const T* magnitudeSpectrum, const std::size_t magSpectrumSize)
{
    T featureValue = 0;
    bool hasOnset = false;

    featureValue = onsetDetectionFunction.highFrequencyContent(magnitudeSpectrum, magSpectrumSize); 
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

    threshold = (meanCoeff.load() * MathHelpers::getMean(previousValues.get(), numPreviousValues)) +
                (noiseRatio.load() * largestPeak);

    for (auto i = numPreviousValues - 1; i > 0; i--) 
    { 
        previousValues[i] = previousValues[i - 1]; 
    } 

    previousValues[0] = featureValue;

    if (isOnset && (previousValues[1] > largestPeak))
        largestPeak = previousValues[1];
    

    return isOnset;
}

//=============================================================================


//=============================================================================
template class OnsetDetector<float>;
template class OnsetDetector<double>;
