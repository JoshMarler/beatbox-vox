/*
  ==============================================================================

    NearestNeighbour.cpp
    Created: 15 Oct 2016 5:22:21pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include <cmath>
#include <algorithm>

#include "NearestNeighbour.h"


//=======================================================================================================
template<typename T>
NearestNeighbour<T>::NearestNeighbour(unsigned int initNumFeatures, unsigned int initNumClasses, std::size_t initNumInstances)
	: trainingSet(initNumFeatures, (initNumInstances * initNumClasses), arma::fill::zeros),
	  labels(initNumInstances * initNumClasses, arma::fill::zeros),
	  squaredDistances(initNumFeatures, arma::fill::zeros)
{
	numClasses = initNumClasses;
	trainingSetSize = initNumInstances * initNumClasses;

	numInstances = initNumInstances;
	numFeatures = initNumFeatures;

	neighbourClassCounts = std::make_unique<size_t[]>(numClasses);
	std::fill(neighbourClassCounts.get(), neighbourClassCounts.get() + numClasses, 0);

	auto neighboursSize = trainingSetSize;
	neighbours = std::make_unique<Neighbour[]>(neighboursSize);

	//Set default 3 K nearest neighbours
	setNumNeighbours(5);
}

//=======================================================================================================
template<typename T>
NearestNeighbour<T>::~NearestNeighbour()
{

}

//=======================================================================================================
template<typename T>
int NearestNeighbour<T>::classify(arma::Col<T>& instance)
{
	//Normalise instance values to 0 - 1 for distance calculation
	PreProcessing::normalise(instance);

	//Reset the counts for class occurrences in the k nearest neighbours collection
	std::fill(neighbourClassCounts.get(), neighbourClassCounts.get() + numClasses, 0);

	for (std::size_t i = 0; i < trainingSet.n_cols; ++i)
	{
		T distance = euclideanDistance(instance, trainingSet.col(i));

		//Get the class label for the current training instance
		const auto classLabel = labels[i];
		
		neighbours[i].label = classLabel;
		neighbours[i].distance = distance;
	}

	std::nth_element(neighbours.get(), neighbours.get() + numNeighbours, neighbours.get() + trainingSetSize);

	for (auto i = 0; i < numNeighbours; ++i)
	{
		const auto classLabel = neighbours[i].label;
		++neighbourClassCounts[classLabel];
	}
	
	auto max = std::max_element(neighbourClassCounts.get(), neighbourClassCounts.get() + numClasses);
	auto label = std::distance(neighbourClassCounts.get(), max);

	return label;
}

//=======================================================================================================
template<typename T>
void NearestNeighbour<T>::setNumNeighbours(const unsigned int newNumNeighbours)
{
	numNeighbours = newNumNeighbours;
}

//=======================================================================================================
template<typename T>
void NearestNeighbour<T>::setNumTrainingInstances(const unsigned int newNumInstances)
{
	numInstances = newNumInstances;

	trainingSetSize = numInstances * numClasses;

	auto neighboursSize = trainingSetSize;
	neighbours.reset(new Neighbour[neighboursSize]);

	configureTrainingSetMatrix();
}

//=======================================================================================================
template<typename T>
void NearestNeighbour<T>::setNumFeatures(const unsigned int newNumFeatures)
{
	numFeatures = newNumFeatures;
	configureTrainingSetMatrix();
}

//=======================================================================================================
/* JWM - NOTE: May not be neccessary to use references here as requires the training data's life time 
 * to match this objects. The train method should be being called from the message/GUI thread in theory so making 
 * a copy at this point may not be an issue. This will need to be taken into consideration at library stage in particular. 
*/
template<typename T>
void NearestNeighbour<T>::train(const arma::Mat<T>& newTrainingSet, const arma::Row<size_t>& newLabels)
{
	trainingSet = newTrainingSet;
	labels = newLabels;

	//Normalise trainingSet values to 0 - 1 for distance calculation
	PreProcessing::normalise(trainingSet);
}

//=======================================================================================================
template<typename T>
T NearestNeighbour<T>::euclideanDistance(const arma::Col<T>& testInstance, const arma::Col<T>& referenceInstance)
{
	T distance = static_cast<T>(0.0);
	
	squaredDistances = arma::square((testInstance - referenceInstance));
	distance = std::sqrt(arma::accu(squaredDistances));

	return distance;
}

//=======================================================================================================
template<typename T>
void NearestNeighbour<T>::configureTrainingSetMatrix()
{
	trainingSet.set_size(numFeatures, trainingSetSize);
	trainingSet.zeros();

	labels.set_size(trainingSetSize);

	for (auto i = 0; i < labels.n_elem; ++i)
	{
		//Consider making trainingLabel <int> rather than unsigned to init with -1 label vals
		labels[i] = 0;
	}
}
//=======================================================================================================
template class NearestNeighbour<float>;
template class NearestNeighbour<double>;