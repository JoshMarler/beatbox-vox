/*
  ==============================================================================

    OnsetDetector.h
    Created: 28 Jun 2016 6:40:26pm
    Author:  joshua

  ==============================================================================
*/

#ifndef ONSETDETECTOR_H_INCLUDED
#define ONSETDETECTOR_H_INCLUDED

#include "JuceHeader.h"
#include "../MathHelpers/MathHelpers.h"

class OnsetDetector
{
    public:
        OnsetDetector();
        ~OnsetDetector();
        
        bool checkForOnset(float featureValue);

        void setUsingLocalMaximum(bool newUsingLocalMaximum);
        bool getUsingLocalMaximum();
        
    private:
       int numPreviousValues;
       bool usingLocalMaximum;      
       
       float threshold;
       float meanCoeff;

       std::vector<float> previousValues;
};



#endif  // ONSETDETECTOR_H_INCLUDED
