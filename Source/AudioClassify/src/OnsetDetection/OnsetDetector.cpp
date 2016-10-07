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
    : onsetDetectionFunction(initBufferSize),
      lastOnsetTime(),
      numPreviousValues(10),
      previousValues(std::make_unique<T[]>(numPreviousValues)),
      previousValuesCopy(std::make_unique<T[]>(numPreviousValues))
{
    usingLocalMaximum = true;
    threshold = 1.0f;
    largestPeak = 0.0f;
    msBetweenOnsets.store(70);

    //Set to false initially - this will be set to true and left after the first onset is detected.
    firstOnsetDetected = false;    

    meanCoeff.store(0.8f);
    medianCoeff.store(0.8f);
    noiseRatio.store(40.0f);
    
    //Set initial ODF type
    currentODFType.store(AudioClassifyOptions::ODFType::spectralDifference);

    std::fill(previousValues.get(), (previousValues.get() + numPreviousValues), 0.0f);   
    std::fill(previousValuesCopy.get(), (previousValuesCopy.get() + numPreviousValues), 0.0f);   


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
    onsetDetectionFunction.setFrameSize(newBufferSize);
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
void OnsetDetector<T>::setMedianCoefficient(T newCoeff)
{
    medianCoeff.store(newCoeff);
}

//=============================================================================
template<typename T>
void OnsetDetector<T>::setMinMsBetweenOnsets(int ms)
{
    msBetweenOnsets.store(ms);
}

//=============================================================================
template<typename T>
int OnsetDetector<T>::getCurrentODFType()
{
    return static_cast<int>(currentODFType.load());
}

template<typename T>
void OnsetDetector<T>::setCurrentODFType(AudioClassifyOptions::ODFType newODFType)
{
    currentODFType.store(newODFType);   
}

//=============================================================================
template<typename T>
bool OnsetDetector<T>::checkForOnset(const T* magnitudeSpectrum, const std::size_t magSpectrumSize)
{
    T featureValue = 0.0f;
    bool hasOnset = false;

    //JWM - NOTE: Need to check but adaptive whitening of the magnitudeSpectrum would need to occur
    //here before the hfc function call.

    switch (currentODFType.load())
    {
        case AudioClassifyOptions::ODFType::spectralDifferenceHWR :
            featureValue = onsetDetectionFunction.spectralDifferenceHWR(magnitudeSpectrum, magSpectrumSize);
            break;

        case AudioClassifyOptions::ODFType::spectralDifference :
            featureValue = onsetDetectionFunction.spectralDifference(magnitudeSpectrum, magSpectrumSize);
            break;

        case AudioClassifyOptions::ODFType::highFrequencyContent : 
            featureValue = onsetDetectionFunction.highFrequencyContent(magnitudeSpectrum, magSpectrumSize);
            break;
    }

	//Stops initial detection issues when onset detector is loaded and contected to an active/streaming input.
	if (featureValue > noiseRatio)
		hasOnset = checkForPeak(featureValue);

    return hasOnset;
}
//=============================================================================
template<typename T>
bool OnsetDetector<T>::checkForPeak(T featureValue) 
{
    auto isOnset = false;

    //JWM - NOTE: add conditional section here to allow lowpass filtering / normalisation as pre-processing of featureValue
    if (usingLocalMaximum) 
    {
        std::copy(previousValues.get(), previousValues.get() + numPreviousValues, previousValuesCopy.get());

		//(previousValues[0] > noiseRatio) &&
        if ((previousValues[0] > threshold) && (previousValues[0] > featureValue) && (previousValues[0] > previousValues[1]))
				isOnset = onsetTimeIsValid();
    }
    else
    {
		if (featureValue > threshold)
				isOnset = onsetTimeIsValid();
    }


	threshold = (meanCoeff.load() * MathHelpers::getMean(previousValues.get(), numPreviousValues)) +
				(medianCoeff.load() * MathHelpers::getMedian(previousValuesCopy.get(), numPreviousValues));

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
template<typename T>
bool OnsetDetector<T>::onsetTimeIsValid() 
{
    bool isValid = false;

    /** For the first time an onset is detected set firstOnsetDetected = true 
     *  so that the initial lastOnsetTime is valid.
     */
    if (!firstOnsetDetected)
    {
        isValid = true;
		lastOnsetTime = std::chrono::time_point_cast<Ms>(ClockType::now());
        firstOnsetDetected = true;
    }
    else
    {
		auto now = std::chrono::time_point_cast<Ms>(ClockType::now());
        auto msElapsed = std::chrono::duration_cast<Ms>(now - lastOnsetTime);

        if (msElapsed.count() > msBetweenOnsets.load())
        {
            isValid = true;
            lastOnsetTime = std::chrono::time_point_cast<Ms>(ClockType::now());
        }
    }

    return isValid;
}

//=============================================================================
template class OnsetDetector<float>;
template class OnsetDetector<double>;
