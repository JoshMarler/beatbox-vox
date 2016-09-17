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

namespace MathHelpers
{
    template<typename T>
    T getMean(const T* vec, const std::size_t vecSize)
    {
        auto sum = std::accumulate(vec, (vec + vecSize), 0.0);
        return sum / vecSize;       
    }
}


#endif  // MATHHELPERS_H_INCLUDED
