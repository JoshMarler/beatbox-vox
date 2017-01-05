/*
  ==============================================================================

    OnsetDetector.cpp
    Created: 28 Jun 2016 6:40:26pm
    Author:  joshua

  ==============================================================================
*/

#include "OnsetDetector.h"
#include "../MathHelpers/MathHelpers.h"
//==============================================================================

template<typename T>
OnsetDetector<T>::OnsetDetector(int initFrameSize, unsigned int initSampleRate)
    : numPreviousValues(10),
      lastOnsetTime(),
      previousValues(std::make_unique<T[]>(numPreviousValues)),
      previousValuesCopy(std::make_unique<T[]>(numPreviousValues)),
      onsetDetectionFunction(initFrameSize),
	  adaptiveWhitener(initFrameSize, initSampleRate)
{
    usingLocalMaximum = true;
	usingWhitening = false;
    threshold = 1.0f;
    largestPeak = 0.0f;
    msBetweenOnsets.store(70);

    //Set to false initially - this will be set to true and left after the first onset is detected.
    firstOnsetDetected = false;    

    meanCoeff.store(0.8f);
    medianCoeff.store(0.8f);
    noiseRatio.store(0.1f);
    
    //Set initial ODF type
    currentODFType.store(AudioClassifyOptions::ODFType::spectralDifference);

    std::fill(previousValues.get(), (previousValues.get() + numPreviousValues), static_cast<T>(0.0));   
    std::fill(previousValuesCopy.get(), (previousValuesCopy.get() + numPreviousValues), static_cast<T>(0.0));   

    setCurrentFrameSize(initFrameSize);
	setSampleRate(initSampleRate);
}

//==============================================================================
template<typename T>
OnsetDetector<T>::~OnsetDetector()
{

}


//==============================================================================
template<typename T>
int OnsetDetector<T>::getCurrentFrameSize() const
{
  return currentFrameSize; 
}

template<typename T>
void OnsetDetector<T>::setCurrentFrameSize(unsigned newFrameSize)
{
    currentFrameSize = newFrameSize;
	
	currentFFTFrame.reset(new T[currentFrameSize]);
	std::fill(currentFFTFrame.get(), currentFFTFrame.get() + currentFrameSize, static_cast<T>(0.0));

    onsetDetectionFunction.setFrameSize(newFrameSize);
	adaptiveWhitener.setFFTFrameSize(newFrameSize);
}

//==============================================================================
template<typename T>
unsigned int OnsetDetector<T>::getSampleRate() const
{
	return sampleRate;
}

template<typename T>
void OnsetDetector<T>::setSampleRate(unsigned int newSampleRate)
{
	sampleRate = newSampleRate;
	adaptiveWhitener.setSampleRate(sampleRate);
}

//==============================================================================

template<typename T>
bool OnsetDetector<T>::getUsingLocalMaximum() const
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
template <typename T>
unsigned OnsetDetector<T>::getMinMsBetweenOnsets() const
{
	return msBetweenOnsets.load();
}

template<typename T>
void OnsetDetector<T>::setMinMsBetweenOnsets(unsigned ms)
{
    msBetweenOnsets.store(ms);
}

//=============================================================================
template<typename T>
int OnsetDetector<T>::getCurrentODFType() const
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
bool OnsetDetector<T>::getUsingAdaptiveWhitening() const
{
	return usingWhitening;
}

template <typename T>
void OnsetDetector<T>::setUsingAdaptiveWhitening(bool newUseWhitening)
{
	usingWhitening = newUseWhitening;
}

template<typename T>
void OnsetDetector<T>::setWhitenerPeakDecayRate(unsigned int newDecayRate)
{
	adaptiveWhitener.setPeakMemoryDecayRate(newDecayRate);
}

//=============================================================================
template<typename T>
bool OnsetDetector<T>::checkForOnset(const T* magnitudeSpectrum, const std::size_t magSpectrumSize)
{
	T featureValue = static_cast<T>(0.0);
	auto hasOnset = false;

	std::copy(magnitudeSpectrum, magnitudeSpectrum + magSpectrumSize, currentFFTFrame.get());

	if (usingWhitening)
		adaptiveWhitener.process(currentFFTFrame.get(), currentFFTFrame.get());

	//Get the onset detection funciton/feature value for peak picking/thresholding
	featureValue = getODFValue();

	//NORMALISATION TEST
	featureValue = featureValue / (1 + featureValue);

	//assert(!MathHelpers::isNaN(featureValue));

	hasOnset = checkForPeak(featureValue);

    return hasOnset;
}
//=============================================================================
template<typename T>
bool OnsetDetector<T>::checkForPeak(T featureValue) 
{
    auto isOnset = false;

    if (usingLocalMaximum) 
    {
        std::copy(previousValues.get(), previousValues.get() + numPreviousValues, previousValuesCopy.get());

        if ((previousValues[0] > noiseRatio) && (previousValues[0] > threshold) && (previousValues[0] > featureValue) && (previousValues[0] > previousValues[1]))
				isOnset = onsetTimeIsValid();
    }
    else
    {
		if (featureValue > noiseRatio && featureValue > threshold)
				isOnset = onsetTimeIsValid();
    }


	threshold = (meanCoeff.load() * MathHelpers::getMean(previousValues.get(), numPreviousValues)) +
				(medianCoeff.load() * MathHelpers::getMedian(previousValuesCopy.get(), numPreviousValues));


	//assert(!MathHelpers::isNaN(threshold));

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
	auto isValid = false;

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
template<typename T>
T OnsetDetector<T>::getODFValue()
{
	T featureValue = static_cast<T>(0.0);

    switch (currentODFType.load())
    {
        case AudioClassifyOptions::ODFType::spectralDifferenceHWR :
            featureValue = onsetDetectionFunction.spectralDifferenceHWR(currentFFTFrame.get(), currentFrameSize);
            break;

        case AudioClassifyOptions::ODFType::spectralDifference :
            featureValue = onsetDetectionFunction.spectralDifference(currentFFTFrame.get(), currentFrameSize);
            break;

        case AudioClassifyOptions::ODFType::highFrequencyContent : 
            featureValue = onsetDetectionFunction.highFrequencyContent(currentFFTFrame.get(), currentFrameSize);
            break;
		
    	default: break;
    }

	return featureValue;
}

//=============================================================================
template class OnsetDetector<float>;
template class OnsetDetector<double>;
