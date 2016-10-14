/*
  ==============================================================================

    NearestNeigbour.cpp
    Created: 14 Oct 2016 3:22:38pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#include "NearestNeigbour.h"


//=======================================================================================================
template<typename T>
NearestNeighbour<T>::NearestNeighbour()
{
	//Set default 3 K nearest neighbours
	setNumNeighbours(3);
}

template<typename T>
NearestNeighbour<T>::~NearestNeighbour()
{
	
}

//=======================================================================================================
template<typename T>
void NearestNeighbour<T>::setNumNeighbours(const unsigned int newNumNeighbours)
{
	numNeighbours = newNumNeighbours;
}
//=======================================================================================================
template class NearestNeighbour<float>;
template class NearestNeighbour<double>;
