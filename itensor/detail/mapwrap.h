//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_MAPWRAP_H
#define __ITENSOR_MAPWRAP_H

namespace itensor {
namespace detail {

struct MapBase
    {
    MapBase() {}
    virtual ~MapBase() { }

    Real virtual
    rmap(Real r) const = 0;

    Complex virtual
    cmap(Complex z) const = 0;
    };

template <typename Func>
struct MapWrap : public MapBase
    {
    Func& f;

    MapWrap(Func&& f_) : f(f_) { }
    virtual ~MapWrap() { }

    Real
    rmap(Real r) const override { return f(r); }

    Complex
    cmap(Complex z) const override { return f(z); }
    };

struct VisitBase
    {
    VisitBase() {}
    virtual ~VisitBase() { }

    void virtual
    rvisit(Real r) const = 0;

    void virtual
    cvisit(Complex z) const = 0;
    };

template <typename Func>
struct VisitWrap : public VisitBase
    {
    Func& f;
    Real scale_fac;

    VisitWrap(Func&& f_, const LogNumber& scale) 
        : 
        f(f_)
        { 
        scale_fac = scale.real0();
        }

    virtual ~VisitWrap() { }

    void
    rvisit(Real r) const override { f(r*scale_fac); }

    void
    cvisit(Complex z) const override { f(z*scale_fac); }
    };


}; //namespace detail
}; //namespace itensor

#endif

