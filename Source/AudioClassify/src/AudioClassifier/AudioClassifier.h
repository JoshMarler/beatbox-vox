/*
  ==============================================================================

    AudioClassifier.h
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFIER_H_INCLUDED
#define AUDIOCLASSIFIER_H_INCLUDED

//For windows compatibility with armadillo 64bit
//#if JUCE_WINDOWS

#define ARMA_64BIT_WORD

//#endif

#include <memory>
#include <atomic>

#include <armadillo.h>

#include "../../Gist/src/Gist.h"
#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../OnsetDetection/OnsetDetector.h"
#include "../NaiveBayes/NaiveBayes.h"

template<typename T>
class AudioClassifier
{

public:

    AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds);

    ~AudioClassifier();

    int getCurrentBufferSize() const;
    T getCurrentSampleRate() const;

    void setCurrentBufferSize (int newBufferSize);
    void setCurrentSampleRate (T newSampleRate);
    
    int getCurrentTrainingSound() const;
    
    //Onset detector functions
    void setOSDMeanCoeff(T newMeanCoeff);
    void setOSDMedianCoeff(T newMedianCoeff);
    void setOSDNoiseRatio(T newNoiseRatio);
    void setOSDMsBetweenOnsets(const int ms);
    void setOSDDetectorFunctionType(AudioClassifyOptions::ODFType newODFType);
	void setOSDUseAdaptiveWhitening(bool use);
	void setOSDWhitenerPeakDecayRate(unsigned int newDecayRate);
	void setOSDUseLocalMaximum(bool use);

    void recordTrainingSample(int trainingSound);

    //NOTE - May change this after prototype so that the model is trained incrementally for each sound
    //to avoid calling trainModel with unfinished training data set. Alternativley may have this function
    //return bool for successful or not in training. 
    void trainModel();


	size_t getNumSounds() const;
    
    void setTrainingSetSize(int newTrainingSetSize);
    
    bool checkTrainingSetReady() const;
	bool checkTrainingSoundReady(const unsigned sound) const;

    bool getClassifierReady() const;
    
    bool isTraining() const;

    void processAudioBuffer (const T* buffer, const int numSamples);

    void processCurrentInstance();

    /** Checks whether the current frame has a detected note onset. This can be called to help 
     * with configuring the AudioClassifier oject's OnsetDetector. This function should be called
     * right after a call to processAudioBuffer() in the same frame i.e. before the next processAudioBuffer() 
     * call.
     * @returns true if an note onset has been detected for the frame/previous frame depending on whether
     * the onset detector is using local maximums.
     */
    bool noteOnsetDetected() const;

    //This function will return -1 for unclassified sounds 
    int classify();

private:

//==============================================================================

    int bufferSize = 0;
    int trainingSetSize = 10;
    int trainingCount = 0;
    int numSounds = 0; 
    bool hasOnset = false;

    T sampleRate;

    //Values to indicate which features currently being used by model.
	std::atomic_bool usingRMS {true};
	std::atomic_bool usingPeakEnergy {true};
	std::atomic_bool usingZeroCrossingRate {true};
    std::atomic_bool usingSpecCentroid {true};
    std::atomic_bool usingSpecCrest {true};
    std::atomic_bool usingSpecFlatness {true};
    std::atomic_bool usingSpecRolloff {true};
    std::atomic_bool usingSpecKurtosis {true};
    std::atomic_bool usingMfcc {true};


    //This value indicates the current sound being trained declared atomic as may be set by a GUI thread / user control.
    std::atomic_int currentTrainingSound;

    //Inidicates if the model has been trained with full training set and classifier is ready, declared atomic as may be set by a GUI thread / user control.
    std::atomic_bool classifierReady;

    std::atomic_bool training;

    //Holds states for each sound in model to confirm whether sound's training set has been recorded.
    std::vector<bool> soundsReady;
    
    //Array/Buffer to hold mag spectrum.
    std::unique_ptr<T[]> magSpectrum;

    //Array/Buffer to hold the mel frequency cepstral coefficients.
    std::unique_ptr<T[]> mfccs;
    
    //Holds the training data set.
    arma::Mat<T> trainingData;

    //Holds the trainingData matrix's corresponding label values for labelled training data.
    arma::Row<size_t> trainingLabels;
    
    //Holds the the feature values/vector for the current instance/block.
    arma::Col<T> currentInstanceVector;
    
    Gist<T> gistFeatures;
    OnsetDetector<T> osDetector;
    NaiveBayes<T> nbc;
//==============================================================================
   
    void configTrainingSetMatrix();

    unsigned int calcFeatureVecSize() const;
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
