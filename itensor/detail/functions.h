//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_FUNCTIONS_H
#define __ITENSOR_FUNCTIONS_H

namespace itensor {
namespace detail {


template <typename Container, typename Item>
bool
contains(const Container& C,
         const Item& I)
    {
    for(const auto& c : C) 
        {
        if(I == c) return true;
        }
    return false;
    }



}; //namespace detail
}; //namespace itensor

#endif

