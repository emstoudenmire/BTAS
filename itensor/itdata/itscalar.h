//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITSCALAR_H
#define __ITENSOR_ITSCALAR_H

#include "itensor/itdata/itdata.h"

namespace itensor {

template <typename T>
class ITScalar : public ITDispatch<ITScalar<T>>
    {
    public:

    ITScalar() : x_(0) { }

    ITScalar(T x) : x_(x) { }

    virtual
    ~ITScalar() { }

    ////////////

    T x_;

    ////////////
    };

}; //namespace itensor

#endif

