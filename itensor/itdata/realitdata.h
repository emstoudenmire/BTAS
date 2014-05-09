//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_REALITDATA_H
#define __ITENSOR_REALITDATA_H

#include "itensor/itdata/itdata.h"

namespace itensor {

class RealITData : public ITDispatch<RealITData>
    {
    public:

    using Ptr = ITData::Ptr;
    using NewPtr = ITData::NewPtr;
    using storage = btas::Tensor<Real>;
    using Range = ITData::Range;

    RealITData() { }

    template<typename... args_>
    explicit
    RealITData(const size_t& first, const args_&... rest);

    template<typename ExtentArray>
    explicit
    RealITData(const ExtentArray& extents);

    explicit
    RealITData(const storage& t);

    virtual
    ~RealITData() { }

    ////////////

    storage t_;

    ////////////
    };

template<typename... args_>
RealITData::
RealITData(const size_t& first, const args_&... rest)
    :
    t_(first,rest...)
    { 
    t_.fill(0.);
    }

template<typename ExtentArray>
RealITData::
RealITData(const ExtentArray& extents)
    :
    t_(storage::range_type(extents))
    { 
    t_.fill(0.);
    }

inline RealITData::
RealITData(const storage& t)
    :
    t_(t)
    { }

}; //namespace itensor

#endif

