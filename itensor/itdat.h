//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDAT_H
#define __ITENSOR_ITDAT_H
#include "itensor/global.h"
#include "btas/tensor.h"
#include "itensor/detail/mapwrap.h"

namespace itensor {

class RealITDat;

class ITDat
    {
    public:

    using Ptr = std::shared_ptr<ITDat>;
    using CPtr = std::shared_ptr<const ITDat>;
    using NewPtr = std::unique_ptr<ITDat>;

    ITDat() { }

    virtual
    ~ITDat() { }

    //
    // Public interface
    //

    void
    plusEq(const CPtr& other, Ptr& newdat, Real fac = 1);

    void virtual
    mult(Real r) = 0;

    NewPtr virtual
    clone() const = 0;

    void virtual
    fill(Real r, Ptr& newdat) = 0;

    void virtual
    generate(std::function<Real()> rfunc, Ptr& newdat) = 0;

    void virtual
    map(detail::MapBase*) = 0;

    void virtual
    print(std::ostream& s, const LogNumber& x) const = 0;


    //
    // Other methods to be implemented by derived classes
    //

    virtual
    void
    plusEqImpl(const RealITDat* d, Ptr& newdat, Real fac) = 0;

    //virtual
    //void
    //plusEq_(ComplexITDat* d, Ptr& newdat, Real fac) const = 0;

    private:

    ITDat(const ITDat&); //disabled to prevent copying
    ITDat& operator=(const ITDat&); //disabled to prevent copying

    //
    // "Double-dispatch" related wrapper functions.
    // Will be overloaded by class Dispatch<Derived>.
    // 

    virtual
    void
    addTo(ITDat* d, Ptr& newdat, Real fac) const = 0;

    friend class RealITDat;

    };

template <class Derived>
class Dispatch : public ITDat
    {
    public:
    Dispatch() { }
    virtual
    ~Dispatch() { }
    private:
    using Ptr = ITDat::Ptr;
    void 
    addTo(ITDat* d, Ptr& newdat,  Real fac) const final { d->plusEqImpl(static_cast<const Derived*>(this),newdat,fac); }
    };

class RealITDat : public Dispatch<RealITDat>
    {
    public:

    using Ptr = ITDat::Ptr;
    using NewPtr = ITDat::NewPtr;
    using storage = btas::Tensor<Real>;

    RealITDat() { }

    template<typename... args_>
    explicit
    RealITDat(const size_t& first, const args_&... rest)
        :
        t_(first,rest...)
        { 
        t_.fill(0.);
        }

    template<typename T>
    explicit
    RealITDat(const T& extents)
        :
        t_(storage::range_type(extents))
        { 
        t_.fill(0.);
        }

    explicit
    RealITDat(const storage& t);

    virtual
    ~RealITDat() { }

    private:

    void 
    plusEqImpl(const RealITDat* r, Ptr& newdat, Real fac) override; 

    NewPtr
    clone() const override;

    void
    fill(Real r, Ptr& newdat) override;

    void
    generate(std::function<Real()> rfunc, Ptr& newdat) override;

    void
    map(detail::MapBase* m) override;

    void 
    mult(Real r) override; 

    void 
    print(std::ostream& s, const LogNumber& x) const override; 

    ////////////
    // Data Members

    storage t_;

    ////////////
    };

}; //namespace itensor

#endif

