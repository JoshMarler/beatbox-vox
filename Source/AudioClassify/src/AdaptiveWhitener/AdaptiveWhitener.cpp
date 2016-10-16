/*
  ==============================================================================

    AdaptiveWhitener.cpp
    Created: 10 Oct 2016 1:59:31pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#include "AdaptiveWhitener.h"

//==============================================================================
template<typename T>
AdaptiveWhitener<T>::AdaptiveWhitener(const std::size_t initFrameSize, const unsigned int initSampleRate)
{
	//In DB - Minimum noise floor value set to -40db 
	noiseFloor = static_cast<T>(0.01);

	//Set initial default peak decay rate of 15 seconds
	setPeakMemoryDecayRate(10);
	setSampleRate(initSampleRate);
	setFFTFrameSize(initFrameSize);
	
}

template<typename T>
AdaptiveWhitener<typename T>::~AdaptiveWhitener()
{
}

//==============================================================================
template<typename T>
std::size_t AdaptiveWhitener<T>::getFFTFrameSize() const
{
	return fftFrameSize;
}

template <typename T>
void AdaptiveWhitener<T>::setFFTFrameSize(const std::size_t newFFTFrameSize)
{
	fftFrameSize = newFFTFrameSize;

	peakValues.reset(new T[fftFrameSize]);
	std::fill(peakValues.get(), peakValues.get() + fftFrameSize, static_cast<T>(0.0));

	//Update the memory decay coefficient relative to new frame size
	updateMemoryDecayCoeff();
}

//==============================================================================
template<typename T>
unsigned int AdaptiveWhitener<T>::getSampleRate() const
{
	return sampleRate;
}

template<typename T>
void AdaptiveWhitener<T>::setSampleRate(const unsigned int newSampleRate)
{
	sampleRate = newSampleRate;

	//Update the memory decay coefficient relative to new sample rate
	updateMemoryDecayCoeff();
}

//==============================================================================
template <typename T>
void AdaptiveWhitener<T>::process(const T* inputFrame, T* outputFrame)
{
	std::copy(inputFrame, inputFrame + fftFrameSize, outputFrame);

	for (auto i = 0; i < fftFrameSize; i++)
	{
		T val = outputFrame[i];

		/* If value less than previous bin peak this updates val and then the current peak 
		 * value for this FFT bin by multiplying with the memory decay rate coefficient.
		 *
		 * This will be mitigated in the next check if the resulting value falls below the noise
		 * floor and will set the current FFT bins PSP/peak value to the minimum noise floor level.		  
		 */
		if (val < peakValues[i])
			val = val + (peakValues[i] - val) * memoryRateCoeff;

		//Do not allow value to fall below noise floor
		if (val < noiseFloor)
			val = noiseFloor;

		//Update the peak/PSP value for the current bin
		peakValues[i] = val;

		//Divide by PSP value - check against divide by zero for first block
		if (peakValues[i] != static_cast<T>(0.0))
			outputFrame[i] /= peakValues[i];
	}
}

template <typename T>
void AdaptiveWhitener<T>::setPeakMemoryDecayRate(const unsigned int newDecayTime)
{
	decayRate = newDecayTime;
	updateMemoryDecayCoeff();
}

//==============================================================================
template <typename T>
void AdaptiveWhitener<T>::updateMemoryDecayCoeff()
{

	//NOTE: Potentially change these to compile time constance with constexpr
	//log/0.0 - 1.0 db conversions for -60db decay rate calculations
	auto dbRatio = std::pow(10, (-60 / 20));
	auto logDbDecayRatio = std::log(dbRatio);

	//Calculate the memory coefficient from the specified decay rate in seconds
	memoryRateCoeff = (decayRate == 0) ? static_cast<T>(0.0) : std::exp((logDbDecayRatio * fftFrameSize) / (decayRate * sampleRate));
}


//==============================================================================
template class AdaptiveWhitener<float>;
template class AdaptiveWhitener<double>;