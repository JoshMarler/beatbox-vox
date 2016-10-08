/*
  ==============================================================================

    MathHelpers.h
    Created: 28 Jun 2016 7:30:18pm
    Author:  joshua

  ==============================================================================
*/

#ifndef MATHHELPERS_H_INCLUDED
#define MATHHELPERS_H_INCLUDED

#include <algorithm>

#ifdef _WIN64

#include <numeric>

#endif // _WIN64


namespace MathHelpers
{
    template<typename T>
    T getMean(const T* vec, const std::size_t vecSize)
    {
	    auto end = sizeof(vec[0]) * vecSize;

        auto sum = std::accumulate(vec, (vec + end), static_cast<T>(0.0));
        return sum / vecSize;       
    }

    //Note that this function uses std::n_element internally and will modify the T* vec argument.
    template<typename T>
    T getMedian(T* vec, const std::size_t vecSize)
    {
	    auto middle = vecSize / 2;
        
        std::nth_element(vec, (vec + middle), (vec + vecSize));

        T medianOdd = vec[middle];
        T medianEven = 0;

        if (vecSize % 2 == 1)
        {
            return medianOdd;
        }
        else
        {
            //vec is even - find average of centre values
            std::nth_element(vec, (vec + (middle - 1)), (vec + vecSize)); 
            
            medianEven = vec[middle - 1];
            return (medianEven + medianOdd) / 2;
        }

    }
}



#endif  // MATHHELPERS_H_INCLUDED
