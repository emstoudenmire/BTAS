//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDATA_FUNCTIONS_H
#define __ITENSOR_ITDATA_FUNCTIONS_H
#include "itensor/global.h"
#include "btas/tensor.h"
#include "itensor/itdata/itdense.h"
#include "btas/tensor_func.h"

namespace itensor {

template <typename F>
struct ApplyIT : public Func1
    {
    ApplyIT(F&& f)
        : f_(f)
        { }

    template <typename T>
    NewData
    apply(ITDense<T>& d) const
        {
        const auto end = d.t_.data()+d.t_.size();
        for(auto it = d.t_.data(); it != end; ++it)
            {
            *it = f_(*it);
            }
        return NewData();
        }

    NewData
    operator()(ITDense<Real>& d) const final { return apply(d); }
    NewData
    operator()(ITDense<Complex>& d) const final { return apply(d); }

    private:
    F& f_;
    };

template <typename RangeFunc>
struct ApplyRange : public Func1
    {
    using Range = btas::Range;

    ApplyRange(const RangeFunc& f)
        : f_(f)
        { }

    template <typename T>
    NewData
    apply(ITDense<T>& d) const
        {
        using storage = typename ITDense<T>::storage;
        storage temp = 
            btas::TensorViewOf<storage>(f_(d.t_.range()),d.t_.storage());
        d.t_ = std::move(temp);
        return NewData();
        }

    NewData
    operator()(ITDense<Real>& d) const final { return apply(d); }
    NewData
    operator()(ITDense<Complex>& d) const final { return apply(d); }

    private:
    const RangeFunc& f_;
    };

struct Fill : public Func1
    {
    Fill(Real r)
        : r_(r)
        { }

    NewData
    operator()(ITDense<Real>& d) const final;
    NewData
    operator()(ITDense<Complex>& d) const final;

    private:
    Real r_;
    };

template <typename F>
struct GenerateIT : public Func1
    {
    GenerateIT(F&& f)
        : f_(f)
        { }

    template <typename T>
    NewData
    apply(ITDense<T>& d) const
        {
        const auto end = d.t_.data()+d.t_.size();
        for(auto it = d.t_.data(); it != end; ++it)
            {
            *it = f_();
            }
        return NewData();
        }

    NewData
    operator()(ITDense<Real>& d) const final { return apply(d); }
    NewData
    operator()(ITDense<Complex>& d) const final { return apply(d); }

    private:
    F& f_;
    };

struct MultComplex : public Func1
    {
    MultComplex(Complex z)
        : z_(z)
        { }

    NewData
    operator()(ITDense<Real>& d) const final;
    NewData
    operator()(ITDense<Complex>& d) const final;

    private:
    Complex z_;
    };

struct MultReal : public Func1
    {
    MultReal(Real r)
        : r_(r)
        { }

    NewData
    operator()(ITDense<Real>& d) const final;
    NewData
    operator()(ITDense<Complex>& d) const final;

    private:
    Real r_;
    };

struct PrintIT : public ConstFunc1
    {
    std::ostream& s_;
    const LogNumber& x_;

    PrintIT(std::ostream& s,
            const LogNumber& x)
        : s_(s), x_(x)
        { }

    void
    operator()(const ITDense<Real>& d) const final;
    void
    operator()(const ITDense<Complex>& d) const final;
    };

template <typename F>
struct VisitIT : public ConstFunc1
    {
    VisitIT(F&& f, const LogNumber& scale)
        : f_(f), scale_fac(scale.real0())
        { }

    template <typename T>
    void
    apply(const ITDense<T>& d) const
        {
        const auto end = d.t_.data()+d.t_.size();
        for(auto it = d.t_.data(); it != end; ++it)
            {
            f_((*it)*scale_fac);
            }
        }

    void
    operator()(const ITDense<Real>& d) const final { apply(d); }
    void
    operator()(const ITDense<Complex>& d) const final { apply(d); }

    private:
    F& f_;
    Real scale_fac;
    };


}; //namespace itensor

#endif

