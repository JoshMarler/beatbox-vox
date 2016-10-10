/*
  ==============================================================================

    AdaptiveWhitener.h
    Created: 10 Oct 2016 1:59:31pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#ifndef ADAPTIVEWHITENER_H_INCLUDED
#define ADAPTIVEWHITENER_H_INCLUDED

#include <memory>
#include <chrono>

template <typename T>
class AdaptiveWhitener
{

public:

	explicit AdaptiveWhitener(const std::size_t fftFrameSize);
	~AdaptiveWhitener();

	void setFFTFrameSize(const std::size_t newFFTFrameSize);
	
	void processFFTFrame(const T* inputFrame, T* outputFrame, const std::size_t fftFrameSize);

	void setPeakDecayTime(const unsigned int newDecayTime);

private:

	//The decay rate for the peak bin values in seconds.
	unsigned int decayRate;

	//The array of peak magnitude values for each of the fft/spectral bins
	std::unique_ptr<T[]> peakValues;

	void updatePeaks(const T* inputFrame, const std::size_t fftFrameSize);

};



#endif  // ADAPTIVEWHITENER_H_INCLUDED
