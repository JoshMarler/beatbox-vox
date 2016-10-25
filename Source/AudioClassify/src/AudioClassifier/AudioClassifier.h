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

#ifdef _WIN64
#define ARMA_64BIT_WORD
#endif

#include <memory>
#include <string>
#include <atomic>

#include <armadillo.h>

#include "../../Gist/src/Gist.h"
#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../OnsetDetection/OnsetDetector.h"
#include "../NaiveBayes/NaiveBayes.h"
#include "../NearestNeighbour/NearestNeighbour.h"

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
	bool getOSDUsingAdaptiveWhitening();
	void setOSDUseAdaptiveWhitening(bool use);
	void setOSDWhitenerPeakDecayRate(unsigned int newDecayRate);
	bool getOSDUsingLocalMaximum();
	void setOSDUseLocalMaximum(bool use);

	/** This method sets the classifier type/learning algorithm to be used.
	 * @param classifierType the classifier type to be used i.e. AudioClassifyOptions::ClassifierType::knn
	 */
	void setClassifierType(AudioClassifyOptions::ClassifierType classifierType);

    void recordTrainingSample(int trainingSound);

    //NOTE - May change this after prototype so that the model is trained incrementally for each sound
    //to avoid calling trainModel with unfinished training data set. Alternativley may have this function
    //return bool for successful or not in training. 
    void trainModel();
	
	/** Saves the current training data set being used by the model/classifier. 
	 * This can be loaded again on the next application load so that training sets do not have
	 * to be collected again. 
     * Note: This method should NOT be called from the audio/callback thread as it involves file IO and will block.
	 * @param fileName the fully qualified file name/path to save the training set matrix to. i.e. C:\\Models\\model.csv 
     * @param errorString output parameter which will contain an error message if applicable or default "" blank string.
     * @return true for successful save otherwise false.
	 */
	bool saveTrainingSet(const std::string& fileName, std::string& errorString);
	bool loadTrainingSet(const std::string& fileName, std::string& errorString);

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
    int numSounds = 0; 
	unsigned int numFeatures;
    int trainingCount = 0;
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

	std::atomic<AudioClassifyOptions::ClassifierType> currentClassfierType;

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
	NearestNeighbour<T> knn;
//==============================================================================
    void configTrainingSetMatrix();

    unsigned int calcFeatureVecSize() const;
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
