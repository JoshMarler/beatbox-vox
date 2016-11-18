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

	explicit AdaptiveWhitener(const std::size_t initFFTFrameSize, const unsigned int initSampleRate);
	~AdaptiveWhitener();

	std::size_t getFFTFrameSize() const;
	void setFFTFrameSize(const std::size_t newFFTFrameSize);

	unsigned int getSampleRate() const;
	void setSampleRate(const unsigned int newSampleRate);
	
	void process(const T* inputFrame, T* outputFrame);

	/** Sets the amount of time it take for the whitener to forget the previous peak values for 
	 * the recently processed spectral bins.  
	 *
	 * @param newDecayTime The decay time in 
	 */
	void setPeakMemoryDecayRate(const unsigned int newDecayTime);

private:

	unsigned int sampleRate;
	std::size_t fftFrameSize;

	//The decay rate for the peak bin values in seconds.
	unsigned int decayRate;
	float noiseFloor;
	T memoryRateCoeff;

	//The array of peak magnitude values for each of the FFT/spectral bins
	std::unique_ptr<T[]> peakValues;

	void updateMemoryDecayCoeff();

};



#endif  // ADAPTIVEWHITENER_H_INCLUDED
