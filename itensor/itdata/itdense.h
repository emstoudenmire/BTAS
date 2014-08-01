//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDENSE_H
#define __ITENSOR_ITDENSE_H

#include "itensor/itdata/itdata.h"
#include "btas/tensor.h"

namespace itensor {

template <typename T_>
class ITDense : public ITDispatch<ITDense<T_>>
    {
    public:

    using storage = typename btas::Tensor<T_>;
    using Range = typename storage::range_type;

    ITDense() { }

    template<typename... args_>
    explicit
    ITDense(const size_t& first, const args_&... rest)
        : t_(first,rest...)
        { 
        t_.fill(0.);
        }

    template<typename ExtentArray>
    explicit
    ITDense(const ExtentArray& extents)
        : t_(Range(extents))
        { 
        t_.fill(0.);
        }

    explicit
    ITDense(const storage& t)
        : t_(t)
        { }

    template<typename OtherType>
    explicit
    ITDense(const ITDense<OtherType>& d)
        : t_(d.t_)
        { }

    virtual
    ~ITDense() { }

    ////////////

    storage t_;

    ////////////
    };

}; //namespace itensor

#endif

