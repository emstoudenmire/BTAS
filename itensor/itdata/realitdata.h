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

    private:

    void 
    contractEqImpl(Ptr& newdat,
                   const RealITData* other,
                   const btas::varray<size_t>& Tind,
                   const btas::varray<size_t>& Oind,
                   const btas::varray<size_t>& Rind) override;

    void 
    plusEqImpl(const RealITData* r, Ptr& newdat, Real fac) override; 

    NewPtr
    clone() const override;

    public:

    void
    fill(Real r, Ptr& newdat) override;

    void
    generate(std::function<Real()> rfunc, Ptr& newdat) override;

    void
    apply(detail::MapBase* m) override;

    void
    visit(detail::VisitBase* v) const override;

    void 
    mult(Real r) override; 

    void 
    print(std::ostream& s, const LogNumber& x) const override; 

    Range 
    range() const override;

    void virtual
    applyRange(const Range& r) override;

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

}; //namespace itensor

#endif

