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
    msBetweenOnsets.store(20);

    //Set to false initially - this will be set to true and left after the first onset is detected.
    firstOnsetDetected = false;    

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
void OnsetDetector<T>::setMinMsBetweenOnsets(int ms)
{
    msBetweenOnsets.store(ms);
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

    //JWM - NOTE: Need to check but adaptive whitening of the magnitudeSpectrum would need to occur
    //here before the hfc function call.

    featureValue = onsetDetectionFunction.highFrequencyContent(magnitudeSpectrum, magSpectrumSize); 
    hasOnset = checkForPeak(featureValue);

    return hasOnset;
}
//=============================================================================
template<typename T>
bool OnsetDetector<T>::checkForPeak(T featureValue) 
{
    auto isOnset = false;
    std::chrono::duration<float> dur;
    std::chrono::milliseconds msElapsed;

    if (getUsingLocalMaximum()) 
    {
        if (previousValues[0] > threshold && previousValues[0] > featureValue && previousValues[0] > previousValues[1]) 
        {
            /** For the first time an onset is detected set firstOnsetDetected = true 
             *  so that the initial lastOnsetTime is valid.
             *
             *  NOTE: The time based logic below for elapsed ms between onsets should probably be 
             *  refactored out into a seperate private function for this class as it is likely to be called
             *  in at least two seperate places / conditional blocks. 
             */
            if (!firstOnsetDetected)
            {
                isOnset = true;
                lastOnsetTime = std::chrono::high_resolution_clock::now();
                firstOnsetDetected = true;
            }
            else 
            {
                dur = std::chrono::high_resolution_clock::now() - lastOnsetTime;
                msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(dur);

                if (msElapsed.count() > msBetweenOnsets.load())
                {
                    isOnset = true;
                    lastOnsetTime = std::chrono::high_resolution_clock::now();
                }
            }
        }
            
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
