/*
  ==============================================================================

    NearestNeigbour.h
    Created: 14 Oct 2016 3:22:38pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#ifndef NEARESTNEIGBOUR_H_INCLUDED
#define NEARESTNEIGBOUR_H_INCLUDED

#include <memory>
#include<atomic>

template<typename T>
class NearestNeighbour
{
public:
	
	NearestNeighbour();
	~NearestNeighbour();


	/** Sets the number of nearest neighbours (K) used in the search/scoring algorithm.  
	 * @param newNumNeighbours The number of neighbours (K) compared in the search.
	 */
	void setNumNeighbours(const unsigned int newNumNeighbours);
	
private:
	
	unsigned int numNeighbours;
};


#endif  // NEARESTNEIGBOUR_H_INCLUDED

