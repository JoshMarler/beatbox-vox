/*
  ==============================================================================

    NearestNeighbour.h
    Created: 15 Oct 2016 5:22:21pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef NEARESTNEIGHBOUR_H_INCLUDED
#define NEARESTNEIGHBOUR_H_INCLUDED

//For windows compatibility with armadillo 64bit

#ifdef _WIN64
#define ARMA_64BIT_WORD
#endif

#include<memory>
#include <armadillo.h>

#include "../PreProcessing/PreProcessing.h"

/**
 *
 *
 */
template<typename T>
class NearestNeighbour
{
public:

	/** Constructor - Creates a (K) NearestNeighbour for classifiying instance vectors in real-time.
	 * @param initNumFeatures the number of features/attributes to be used in the model.
	 * @param initNumClasses the number of different classes in the model.
	 * @param initNumInstances the number of instances in the initial training set for the model.
	 */
	NearestNeighbour(unsigned int initNumFeatures, unsigned int initNumClasses, std::size_t initNumInstances);
	
	~NearestNeighbour();


	/** Classifies a given instance based on the Modal class of the K nearest neighbours.
	 * @param instance the instance to be classified.
	 * @return the label/class predicted for the supplied instance.
	 */
	int classify(arma::Col<T>& instance);


	/** Sets the number of nearest neighbours (K) used in the search/scoring algorithm.
	* @param newNumNeighbours The number of neighbours (K) compared in the search.
	*/
	void setNumNeighbours(const unsigned int newNumNeighbours);

	/** Sets the number of instance to be used per class for the training set.
	 * @param newNumInstances the number of instances per class
	 */
	void setNumTrainingInstances(const unsigned int newNumInstances);

	/** Sets the number of features / attributes to be used by training and classifiable instances.
	 * @param newNumFeatures the number of features/attributes to be used.
	 */
	void setNumFeatures(const unsigned int newNumFeatures);


	/** Adds a new training data set to the NearestNeighbour model which will be used
	 * as the model for calculating distances and K nearest neighbours.
	 * @param newTrainingSet the training set to be added to the model.
	 */
	void train(const arma::Mat<T>& newTrainingSet, const arma::Row<size_t>& newLabels);


private:

	/** A simple structure used internally by the NearestNeighbour class.
	 * The Neighbour class/struct is used when sorting the neighbour instance
	 * distances vector into K nearest order. This allows a prediction to be
	 * made based on the modal class/label occurence in the K nearest neighbours.
	 * 
	 * Note: The Neighbour struct/object will not be valid when used in the distance
	 * based nearest neighbour algorithm until it's members label and distance are
	 * explicitly set. When default constructed distance = 0.0 which means if left in 
	 * it's initial state the Neighbour would be at the top of the K nearest list as the
	 * closest training instance. 	
	 */
	struct Neighbour
	{
		size_t label;
		T distance;

		Neighbour()
		{
			label = 0;
			distance = static_cast<T>(0.0);
		}

		Neighbour(size_t label, T distance)
		{
			this->label = label;
			this->distance = distance;
		}

		bool operator < (Neighbour other)
		{
			return this->distance < other.distance;
		}
	};


	/** Calculates the euclidean distance of the referenceInstance from the testInstance
	 * @param testInstance the test instance to determine the euclidean distance of test - reference for.  
	 * @param referenceInstance the reference instance to determine the euclidean distance of test - reference for.
	 *
	 * @return the euclidean distance value for testInstance - referenceInstance. 
	 */
	T euclideanDistance(const arma::Col<T>& testInstance, const arma::Col<T>& referenceInstance);

	unsigned int numInstances;
	unsigned int trainingSetSize;
	unsigned int numFeatures;
	unsigned int numClasses;
	unsigned int numNeighbours;

	std::unique_ptr<Neighbour[]> neighbours;
	std::unique_ptr<size_t[]> neighbourClassCounts;

	arma::Mat<T> trainingSet;
	arma::Row<size_t> labels;

	//Vector of squared differences for use in euclidean distance function
	arma::Col<T> squaredDistances;

	void configureTrainingSetMatrix();
};




#endif  // NEARESTNEIGHBOUR_H_INCLUDED
