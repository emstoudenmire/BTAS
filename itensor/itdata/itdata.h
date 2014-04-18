//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDATA_H
#define __ITENSOR_ITDATA_H
#include "itensor/global.h"
#include "btas/tensor.h"
#include "itensor/detail/mapwrap.h"

namespace itensor {

class RealITData;

class ITData
    {
    public:

    using Ptr = std::shared_ptr<ITData>;
    using CPtr = std::shared_ptr<const ITData>;
    using NewPtr = std::unique_ptr<ITData>;
    using Range = btas::DEFAULT::range;

    ITData() { }

    virtual
    ~ITData() { }

    //
    // Public interface
    //

    void virtual
    contractEq(Ptr& newdat,
               const CPtr& other, 
               const btas::varray<size_t>& Tind,
               const btas::varray<size_t>& Oind,
               const btas::varray<size_t>& Rind
              );

    void
    plusEq(const CPtr& other, Ptr& newdat, Real fac = 1);

    NewPtr virtual
    clone() const = 0;

    void virtual
    fill(Real r, Ptr& newdat) = 0;

    void virtual
    generate(std::function<Real()> rfunc, Ptr& newdat) = 0;

    void virtual
    apply(detail::MapBase*) = 0;

    void virtual
    visit(detail::VisitBase*) const = 0;

    void virtual
    mult(Real r) = 0;

    void virtual
    print(std::ostream& s, const LogNumber& x) const = 0;

    Range virtual
    range() const = 0;

    void virtual
    applyRange(const Range& r) = 0;

    //
    // Other methods to be implemented by derived classes
    //

    void virtual
    plusEqImpl(const RealITData* d, Ptr& newdat, Real fac) = 0;

    //void virtual
    //plusEqImpl(const ComplexITData* d, Ptr& newdat, Real fac) const = 0;

    void virtual
    contractEqImpl(Ptr& newdat,
                   const RealITData* other,
                   const btas::varray<size_t>& Tind,
                   const btas::varray<size_t>& Oind,
                   const btas::varray<size_t>& Rind) = 0;

    private:

    ITData(const ITData&); //disabled to prevent copying
    ITData& operator=(const ITData&); //disabled to prevent copying

    //
    // "Double-dispatch" related wrapper functions.
    // Will be overloaded by class Dispatch<Derived>.
    // 

    void virtual
    addTo(ITData* d, Ptr& newdat, Real fac) const = 0;

    void virtual
    contractWith(ITData* d, 
                 Ptr& newdat,
                 const btas::varray<size_t>& Tind,
                 const btas::varray<size_t>& Oind,
                 const btas::varray<size_t>& Rind
                ) const = 0;

    friend class RealITData;

    };

template <class Derived>
class ITDispatch : public ITData
    {
    public:
    ITDispatch() { }
    virtual
    ~ITDispatch() { }

    private:

    using Ptr = ITData::Ptr;

    void 
    addTo(ITData* d, Ptr& newdat,  Real fac) const final;

    void
    contractWith(ITData* d, 
                 Ptr& newdat,
                 const btas::varray<size_t>& Tind,
                 const btas::varray<size_t>& Oind,
                 const btas::varray<size_t>& Rind
                ) const final;
    };

void inline ITData::
contractEq(Ptr& newdat,
           const CPtr& other, 
           const btas::varray<size_t>& Tind,
           const btas::varray<size_t>& Oind,
           const btas::varray<size_t>& Rind)
    {
    other->contractWith(this,newdat,Tind,Oind,Rind);
    }

void inline ITData::
plusEq(const CPtr& other, Ptr& newptr, Real fac) 
    { 
    other->addTo(this,newptr,fac); 
    }

template <class Derived>
void ITDispatch<Derived>::
addTo(ITData* d, Ptr& newdat,  Real fac) const
    { 
    d->plusEqImpl(static_cast<const Derived*>(this),newdat,fac); 
    }

template <class Derived>
void ITDispatch<Derived>::
contractWith(ITData* d, 
             Ptr& newdat,
             const btas::varray<size_t>& Tind,
             const btas::varray<size_t>& Oind,
             const btas::varray<size_t>& Rind
            ) const
    {
    d->contractEqImpl(newdat,static_cast<const Derived*>(this),Tind,Oind,Rind);
    }

}; //namespace itensor

#endif

