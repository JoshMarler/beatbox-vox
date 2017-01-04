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

#include <armadillo.h>

#include <memory>
#include <atomic>

#include "../../Gist/src/Gist.h"

#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../AudioDataSet/AudioDataSet.h"

#include "../OnsetDetection/OnsetDetector.h"
#include "../FeatureExtractor/FeatureExtractor.h"

#include "../NaiveBayes/NaiveBayes.h"
#include "../NearestNeighbour/NearestNeighbour.h"

//==============================================================================
template<typename T>
class AudioClassifier
{

public:

	//==============================================================================
    AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds, int initNumInstances);
    ~AudioClassifier();

	//==============================================================================
    int getCurrentBufferSize() const;
    T getCurrentSampleRate() const;

    void setCurrentBufferSize (int newBufferSize);
    void setCurrentSampleRate (T newSampleRate);
    
	//==============================================================================
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

	//==============================================================================
	void setNumBuffersDelayed(unsigned int newNumDelayed);
	int getNumBuffersDelayed() const;

	void setSTFTFramesPerBuffer(const unsigned int newNumSTFTFrames);
	int getSTFTFramesPerBuffer() const;

	int getSTFTFrameSize() const;

	/** This method sets the classifier type/learning algorithm to be used.
	 * @param classifierType the classifier type to be used i.e. AudioClassifyOptions::ClassifierType::knn
	 */
	void setClassifierType(AudioClassifyOptions::ClassifierType classifierType);

	AudioClassifyOptions::ClassifierType getClassifierType() const;

	/**
	 *
	 */
    void setSoundRecording(int trainingSound, AudioClassifyOptions::DataSetType dataSetType);
    int getCurrentSoundRecording() const;

    void train();
	
	float test(std::vector<std::pair<unsigned int, unsigned int>>& outputResults);

	/** Saves the current data set being used by the model/classifier. 
	 * This can be loaded again on the next application load so that data sets do not have to be re-recorded. 
     * Note: This method should NOT be called from the audio/callback thread as it involves file IO and will block.
	 * @param fileName the fully qualified file name/path to save AudioDataSet to. i.e. C:\\AudioDataSets/Set1.bin
     * @param errorString output parameter which will contain an error message if applicable or default "" blank string.
     * @return true for successful save otherwise false.
	 */
	bool saveDataSet(const std::string& fileName, AudioClassifyOptions::DataSetType dataSetType, std::string& errorString);

	/** Loads the specified data set according to the file name / path passed in. 
     * Note: This method should NOT be called from the audio/callback thread as it involves file IO and will block.
	 * @param fileName the fully qualified file name/path for the data set to load. i.e. C:\\Models\\model.csv
     * @param errorString output parameter which will contain an error message if applicable or default "" blank string.
     * @return true for successful load otherwise false.
	 */
	bool loadDataSet(const std::string& fileName, AudioClassifyOptions::DataSetType dataSetType, std::string& errorString);


	/** @return the number of sounds currently being used in the model. */
	size_t getNumSounds() const;
    
	/** Sets the number of instances to be used per sound for the data set. 
	 * The model/classifier will need to be re-trained and a fresh training set recorded if dataSetType == trainingSet.
	 * If dataSetType == testSet the test set will need to be re-recorded	
	 * @param newNumInstances the number of instance to be recorded per sound for the data set.
	 */
	void setInstancesPerSound(int newNumInstances, AudioClassifyOptions::DataSetType dataSetType);
	int getInstancesPerSound(AudioClassifyOptions::DataSetType dataSetType);

	int getDataSetSize(AudioClassifyOptions::DataSetType dataSetType) const;

	bool checkDataSetReady(AudioClassifyOptions::DataSetType dataSetType) const;
	bool checkSoundReady(int sound, AudioClassifyOptions::DataSetType dataSetType) const;

    bool getClassifierReady() const;
    
    bool isRecording() const;

    void processAudioBuffer (const T* buffer, const int numSamples);


    /** Checks whether the current frame has a detected note onset. This can be called to help 
     * with configuring the AudioClassifier oject's OnsetDetector. This function should be called
     * right after a call to processAudioBuffer() in the same frame i.e. before the next processAudioBuffer() 
     * call.
     * @returns true if a note onset has been detected for the frame/previous frame depending on whether
     * the onset detector is using local maximums.
     */
    bool noteOnsetDetected() const;

    //This function will return -1 for unclassified sounds 
    int classify();

	void reduceFeaturesByVariance(unsigned numFeaturesToTake);
	int getNumFeaturesUsed();

private:

	//==============================================================================
    int bufferSize = 0;
	int delayedBufferSize = 0;

	//==============================================================================
	int numDelayedBuffers = 0;
	unsigned int delayedProcessedCount = 0;
	unsigned int stftFramesPerBuffer = 1;
	unsigned int stftProcessedCount = 0;

	//==============================================================================
	//Holds the number of features processed so far for the current instance
	unsigned int featuresProcessedCount = 0;
	unsigned int featuresProcessedCountReduced = 0;
	//==============================================================================
	int trainingInstancesPerSound = 0;
	int testInstancesPerSound = 0;

	//==============================================================================
    int numSounds = 0; 
	
	//==============================================================================
	bool hasOnset = false;

	//==============================================================================
	unsigned reducedVarianceSize = 0;
	//==============================================================================
	T sampleRate = static_cast<T>(0.0);
	
	//==============================================================================
	std::atomic<AudioClassifyOptions::ClassifierType> currentClassfierType;

	//==============================================================================
	/* Classifier current state variables */

    //Indicates the current sound being recorded.
	std::atomic_int currentSoundRecording;

	//Classifier is ready to use true/false.
    std::atomic_bool classifierReady;

	//Indicates whether currently recording test/training data.
    std::atomic_bool recordingTrainingData;
	std::atomic_bool recordingTestData;
	//==============================================================================

    //Array/Buffer to hold mag spectrum used for onset detection.
    std::unique_ptr<T[]> magSpectrumOSD;

	//==============================================================================
    Gist<T> gistOSD;
    OnsetDetector<T> osDetector;
	FeatureExtractor<T> featureExtractor;
    NaiveBayes<T> nbc;
	NearestNeighbour<T> knn;

	//==============================================================================
	/**
	 * NOTE: Eventually need to change to atomic shared pointers which 
	 * can be swapped and garbage collected to avoid torn object states when refering to
	 * data sets on the callback / processBlock thread. Will need to swap atomically when 
	 * dataset parameters change. 
	 */
	std::unique_ptr<AudioDataSet<T>> trainingSet;
	std::unique_ptr<AudioDataSet<T>> trainingSetReduced;

	std::unique_ptr<AudioDataSet<T>> testSet;
	std::unique_ptr<AudioDataSet<T>> testSetReduced;

    //Holds the the feature values/vector for the current instance/block.
	arma::Col<T> currentInstanceVector;
	arma::Col<T> currentInstanceVectorReduced;

	//==============================================================================
	void setupStft();

    void processCurrentInstance();
	void processCurrentInstanceReduced();

	void resetClassifierState();

	void configureDataSets();

	//==============================================================================
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
