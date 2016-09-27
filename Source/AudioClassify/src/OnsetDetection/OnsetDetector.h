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
#include <chrono>

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
        
        void setMedianCoefficient(T newCoeff);
        
        //JWM - Note: Should probably make ms an unsigned value to avoid negatives.
        void setMinMsBetweenOnsets(int ms);

        void setCurrentODFType(AudioClassifyOptions::ODFType newODFType);
        
        bool checkForOnset(const T* magnitudeSpectrum, const std::size_t magSpectrumSize);

    private:

       int bufferSize; 
       int numPreviousValues;
       int framesSinceOnset;
       int medianWindowSize; 
       
       //Members for minimum milliseconds between onsets behaviour.
       std::atomic_int msBetweenOnsets;
       std::chrono::steady_clock::time_point lastOnsetTime;
       bool firstOnsetDetected;


       bool usingLocalMaximum;      
       
       T threshold;
       T largestPeak;

       //Modifiable parameters declared as std::atomic as likely to be set by a GUI thread. 
       std::atomic<T> meanCoeff;
       std::atomic<T> medianCoeff;
       std::atomic<T> noiseRatio;

       std::unique_ptr<T[]> previousValues;

       /**
        * We require a copy of the previousValues array for the median averaging/filtering
        * as the MathHelpers::getMedian function modifys the array which the median is
        * calculate for.
        */
       std::unique_ptr<T[]> previousValuesCopy;


       OnsetDetectionFunction<T> onsetDetectionFunction;

       std::atomic<AudioClassifyOptions::ODFType> currentODFType {AudioClassifyOptions::ODFType::spectralDifference};

       bool checkForPeak(T featureValue);
};



#endif  // ONSETDETECTOR_H_INCLUDED
