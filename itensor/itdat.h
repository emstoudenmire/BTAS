//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDAT_H
#define __ITENSOR_ITDAT_H
#include "itensor/global.h"
#include "btas/tensor.h"

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

    ITDat&
    plusEq(const CPtr& other, Real fac = 1);

    ITDat&
    mult(Real r);

    NewPtr 
    clone() const;

    void
    fill(Real r);

    void
    print(std::ostream& s, const LogNumber& x) const;


    //
    // Methods to be overridden by implementations
    //

    public:

    virtual
    void
    plusEq_(const RealITDat* d, Real fac) = 0;

    //virtual
    //void
    //plusEq_(ComplexITDat* d, Real fac) const = 0;

    private:

    virtual
    NewPtr
    clone_() const = 0;

    virtual
    void
    fill_(Real r) = 0;

    virtual
    void
    mult_(Real r) = 0;

    virtual
    void
    print_(std::ostream& s, const LogNumber& x) const = 0;


    ITDat(const ITDat&); //disabled to prevent copying
    ITDat& operator=(const ITDat&); //disabled to prevent copying

    //
    // "Double-dispatch" related wrapper functions.
    // Will be overloaded by class Dispatch<Derived>.
    // 

    virtual
    void
    addTo_(ITDat* d,Real fac) const = 0;

    };

template <class Derived>
class Dispatch : public ITDat
    {
    public:
    Dispatch() { }
    virtual
    ~Dispatch() { }
    private:
    void 
    addTo_(ITDat* d, Real fac) const final { d->plusEq_(static_cast<const Derived*>(this),fac); }
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
    plusEq_(const RealITDat* r, Real fac) override; 

    NewPtr
    clone_() const override;

    void
    fill_(Real r) override;

    void 
    mult_(Real r) override; 

    void 
    print_(std::ostream& s, const LogNumber& x) const override; 

    ////////////
    // Data Members

    storage t_;

    ////////////
    };

}; //namespace itensor

#endif

