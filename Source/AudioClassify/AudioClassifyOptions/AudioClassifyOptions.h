/*
  ==============================================================================

    AudioClassifyOptions.h
    Created: 15 Jul 2016 10:31:26pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFYOPTIONS_H_INCLUDED
#define AUDIOCLASSIFYOPTIONS_H_INCLUDED

namespace AudioClassifyOptions
{
    
    enum class ClassifierType: int
    {
        naiveBayes,
        neuralNet
    };

    enum class AudioFeature: int 
    {
        spectralCentroid,
        spectralCrest
    };

    enum class ODFType: int
    {
        spectralDifference,
        spectralDifferenceHWR,
        complexSpectralDifference,
        highFrequencyContent
    };
}



#endif  // AUDIOCLASSIFYOPTIONS_H_INCLUDED
