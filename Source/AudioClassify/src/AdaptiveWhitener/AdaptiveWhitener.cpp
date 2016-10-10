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
AdaptiveWhitener<T>::AdaptiveWhitener(const std::size_t fftFrameSize)
{
	decayRate = 20;
	setFFTFrameSize(fftFrameSize);
}

template<typename T>
AdaptiveWhitener<typename T>::~AdaptiveWhitener()
{
}

//==============================================================================
template <typename T>
void AdaptiveWhitener<T>::setFFTFrameSize(const std::size_t newFFTFrameSize)
{
	peakValues.reset(new T[newFFTFrameSize]);
	std::fill(peakValues.get(), peakValues.get() + newFFTFrameSize, static_cast<T>(0.0));
}

//==============================================================================
template <typename T>
void AdaptiveWhitener<T>::processFFTFrame(const T* inputFrame, T* outputFrame, const std::size_t fftFrameSize)
{
	updatePeaks(inputFrame, fftFrameSize);
	
	std::copy(inputFrame, inputFrame + fftFrameSize, outputFrame);

	for (auto i = 0; i < fftFrameSize; i++)
	{
		//Divide by PSP value - check against divid by zero
		if (!peakValues[i] == 0)
			outputFrame[i] /= peakValues[i];
	}
}

template <typename T>
void AdaptiveWhitener<T>::setPeakDecayTime(const unsigned int newDecayTime)
{
	decayRate = newDecayTime;
}

//==============================================================================
template<typename T>
void AdaptiveWhitener<T>::updatePeaks(const T * inputFrame, const std::size_t fftFrameSize)
{
	for (auto i = 0; i < fftFrameSize; i++)
	{
		if (inputFrame[i] > peakValues[i])
			peakValues[i] = inputFrame[i];
	}
}

//==============================================================================
template class AdaptiveWhitener<float>;
template class AdaptiveWhitener<double>;