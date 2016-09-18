/*
  ==============================================================================

    OnsetDetector.h
    Created: 28 Jun 2016 6:40:26pm
    Author:  joshua

  ==============================================================================
*/

#ifndef ONSETDETECTOR_H_INCLUDED
#define ONSETDETECTOR_H_INCLUDED

#include <atomic>
#include <memory>

#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../../MathHelpers/MathHelpers.h"
#include "../../Gist/src/onset-detection-functions/OnsetDetectionFunction.h"


template<typename T>
class OnsetDetector
{
    public:
        OnsetDetector(int initBufferSize);
        ~OnsetDetector();
        
        int getCurrentBufferSize();
        void setCurrentBufferSize(int newBufferSize);

        void setUsingLocalMaximum(bool newUsingLocalMaximum);
        bool getUsingLocalMaximum();

        void setNoiseRatio(T newNoiseRatio);
        T getNoiseRatio() const;

        void setMeanCoefficient(T newCoeff);
        T getMeanCoefficient() const;
        
        void setCurrentODFType(AudioClassifyOptions::ODFType newODFType);
        
        bool checkForOnset(const T* magnitudeSpectrum, const std::size_t magSpectrumSize);

    private:

       int bufferSize; 
       int numPreviousValues;
       int framesSinceOnset;
       

       bool usingLocalMaximum;      
       
       T threshold;
       T largestPeak;

       //Modifiable parameters declared as std::atomic as likely to be set by a GUI thread. 
       std::atomic<T> meanCoeff;
       std::atomic<T> noiseRatio;

       std::unique_ptr<T[]> previousValues;

       OnsetDetectionFunction<T> onsetDetectionFunction;

       std::atomic<AudioClassifyOptions::ODFType> currentODFType {AudioClassifyOptions::ODFType::spectralDifference};

       bool checkForPeak(T featureValue);
};



#endif  // ONSETDETECTOR_H_INCLUDED
