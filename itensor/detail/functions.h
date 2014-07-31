//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_FUNCTIONS_H
#define __ITENSOR_FUNCTIONS_H

namespace itensor {
namespace detail {

template <typename Set1,
          typename Set2,
          typename Perm>
void
calc_permutation(const Set1& s1,
                 const Set2& s2,
                 Perm& p)
    {
    for(size_t i2 = 0; i2 < s2.size(); ++i2)
        {
        const auto& v2 = s2[i2];
        bool found = false;
        for(size_t i1 = 0; i1 < s1.size(); ++i1)
            {
            if(v2 == s1[i1])
                {
                p[i1] = i2;
                found = true;
                break;
                }
            }

        if(!found)
            {
            throw ITError("sets are not permutations of each other");
            }
        }
    }

template <typename Set1,
          typename Set2,
          typename RType,
          typename Map>
void
permute_map(const Set1& s1,
            const Set2& s2,
            RType& r,
            Map&& m)
    {
    for(size_t i2 = 0; i2 < s2.size(); ++i2)
        {
        const auto& v2 = s2[i2];
        bool found = false;
        for(size_t i1 = 0; i1 < s1.size(); ++i1)
            {
            if(v2 == s1[i1])
                {
                r[i1] = m(s2[i2]);
                found = true;
                break;
                }
            }

        if(!found)
            {
            throw ITError("sets are not permutations of each other");
            }
        }
    }

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

