/*
  ==============================================================================

    AudioClassifier.h
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFIER_H_INCLUDED
#define AUDIOCLASSIFIER_H_INCLUDED

#include <map>
#include "mlpack/core.hpp"
#include "mlpack/methods/naive_bayes/naive_bayes_classifier.hpp"
#include "../../Gist/src/Gist.h"
#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../OnsetDetection/OnsetDetector.h"

//JWM - need to deal with namespace conflict against juce::Timer
//using namespace mlpack;
using namespace arma;

template<typename T>
class AudioClassifier
{

public:

    AudioClassifier(int initBufferSize, T initSampleRate);

    ~AudioClassifier();

    int getCurrentBufferSize();
    T getCurrentSampleRate();

    void setCurrentBufferSize (int newBufferSize);
    void setCurrentSampleRate (T newSampleRate);
    
    //JWM - NOTE: may change this to getCurrentTrainingSoundVal rather than label - returns the numeric label value used internally by the classifier.
    int getCurrentTrainingSoundLabel();
    std::string getCurrentTrainingSoundName();

    void setCurrentTrainingSound (int newTrainingSound);
    void setCurrentTrainingSound (std::string newTrainingSound);

    void setTrainingSetSize(int newTrainingSetSize);

    bool getClassifierReady();

    
    void processAudioBuffer (T* buffer);



     //nbc(std::make_unique<NaiveBayesClassifier<>>()), 


private:

//==============================================================================

    int bufferSize;
    int trainingSetSize;
    T sampleRate;

    bool spectralCrestIsEnabled;

    //JWM - This value indicates the current sound being trained declared atomic as may be set by a GUI thread / user control 
    std::atomic_int currentTrainingSound;

    //JWM - Inidicates if the model has been trained with full training set and classifier is ready, declared atomic as may be set by a GUI thread / user control
    std::atomic_bool classifierReady;


    //Vector to hold mag spectrum
    std::vector<T> magSpectrum;

    std::unique_ptr<Gist<T>> gistFeatures; 
    
    std::unique_ptr<OnsetDetector<T>> osDetector;
        
    std::map<AudioClassifyOptions::AudioFeature, bool> audioFeatures = {
                                                      {AudioClassifyOptions::AudioFeature::spectralCentroid, true},
                                                      {AudioClassifyOptions::AudioFeature::spectralCrest, true},
                                                      {AudioClassifyOptions::AudioFeature::spectralFlatness, true},
                                                      {AudioClassifyOptions::AudioFeature::spectralRolloff, true},
                                                      {AudioClassifyOptions::AudioFeature::spectralKurtois, true},
                                                      {AudioClassifyOptions::AudioFeature::mfcc, true}};

    std::map<int, std::string> soundLabels;
    

    Mat<T> trainingData;
//==============================================================================

    void setClassifierReady (bool ready);
   
    void configTrainingSetMatrix();
    //JWM - creates an instance column/vector to be classifier or added to training set.
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
