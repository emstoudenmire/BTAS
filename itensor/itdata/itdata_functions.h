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
#include "btas/generic/contract.h"
#include "btas/generic/axpy_impl.h"

namespace itensor {

template <typename F>
struct ApplyIT
    {
    ApplyIT(F&& f)
        : f_(f)
        { }

    template <typename T>
    NewData
    operator()(ITDense<T>& d) const
        {
        const auto end = d.t_.data()+d.t_.size();
        for(auto it = d.t_.data(); it != end; ++it)
            {
            *it = f_(*it);
            }
        return NewData();
        }

    private:
    F& f_;
    };

template <typename RangeFunc>
struct ApplyRange
    {
    using Range = btas::Range;

    ApplyRange(const RangeFunc& f)
        : f_(f)
        { }

    template <typename T>
    NewData
    operator()(ITDense<T>& d) const
        {
        using storage = typename ITDense<T>::storage;
        storage temp = 
            btas::make_view(f_(d.t_.range()),d.t_.storage());
        d.t_ = std::move(temp);
        return NewData();
        }

    private:
    const RangeFunc& f_;
    };

struct Contract
    {
    using annotation = btas::varray<size_t>;

    Contract(const annotation& Lind,
             const annotation& Rind,
             const annotation& Pind)
        :
        Lind_(Lind),
        Rind_(Rind),
        Pind_(Pind)
        { }

    template <typename T>
    NewData
    operator()(const ITDense<T>& a1,
               const ITDense<T>& a2) const
        {
        static const auto One = T(1.),
                          Zero = T(0.);
        auto res = new ITDense<T>();
        btas::contract(One,a1.t_,Lind_,a2.t_,Rind_,Zero,res->t_,Pind_);
        return NewData(res);
        }

    template <typename T1, typename T2>
    NewData
    operator()(const ITDense<T1>& a1,
               const ITDense<T2>& a2) const
        {
        using product_type = decltype(::std::declval<T1>() * ::std::declval<T2>());
        //static const auto One = product_type(1.),
        //                  Zero = product_type(0.);
        auto res = new ITDense<product_type>();
        Error("Contract not implemented for tensors of different element types.");
        //btas::contract(One,a1.t_,Lind_,a2.t_,Rind_,Zero,res->t_,Pind_);
        return NewData(res);
        }
 
    private:
    const annotation& Lind_,
                      Rind_,
                      Pind_;
    };

struct Fill
    {
    Fill(Real r)
        : r_(r)
        { }

    NewData
    operator()(ITDense<Real>& d) const;
    NewData
    operator()(ITDense<Complex>& d) const;

    private:
    Real r_;
    };

template <typename F>
struct GenerateIT
    {
    GenerateIT(F&& f)
        : f_(f)
        { }

    template <typename T>
    NewData
    operator()(ITDense<T>& d) const
        {
        const auto end = d.t_.data()+d.t_.size();
        for(auto it = d.t_.data(); it != end; ++it)
            {
            *it = f_();
            }
        return NewData();
        }

    private:
    F& f_;
    };

template<typename T, int size>
struct GetElt
    {
    GetElt(const std::array<int,size>& inds)
        : inds_(inds)
        { }

    explicit operator T() const { return elt_; }

    NewData
    operator()(const ITDense<T>& d)
        {
        elt_ = d.t_(inds_);
        return NewData();
        }

    template <typename DType>
    NewData
    operator()(const DType& d)
        {
        throw ITError("ITensor does not have requested element type");
        return NewData();
        }

    private:
    T elt_;
    const std::array<int,size>& inds_;
    };

struct MultComplex
    {
    MultComplex(Complex z)
        : z_(z)
        { }

    NewData
    operator()(const ITDense<Real>& d) const;
    NewData
    operator()(ITDense<Complex>& d) const;

    private:
    Complex z_;
    };

struct MultReal
    {
    MultReal(Real r)
        : r_(r)
        { }

    NewData
    operator()(ITDense<Real>& d) const;
    NewData
    operator()(ITDense<Complex>& d) const;

    private:
    Real r_;
    };

struct PlusEQ
    {
    PlusEQ(Real fac)
        :
        fac_(fac)
        { }

    template <typename T>
    NewData
    operator()(ITDense<T>& a1,
          const ITDense<T>& a2) const
        {
        //axpy computes a1.t_ += a2.t_ * fac
        btas::axpy(fac_,a2.t_,a1.t_);
        return NewData();
        }

    template <typename T1, typename T2>
    NewData
    operator()(ITDense<T1>& a1,
          const ITDense<T2>& a2) const
        {
        Error("+= not implemented for tensors of different element types.");
        return NewData();
        }
 
    private:
    const Real fac_;
    };


struct PrintIT
    {
    std::ostream& s_;
    const LogNumber& x_;

    PrintIT(std::ostream& s,
            const LogNumber& x)
        : s_(s), x_(x)
        { }

    NewData
    operator()(const ITDense<Real>& d) const;
    NewData
    operator()(const ITDense<Complex>& d) const;
    };

template<int size>
struct SetEltComplex
    {
    SetEltComplex(const Complex& elt,
                  const std::array<int,size>& inds)
        : elt_(elt),
          inds_(inds)
        { }

    NewData
    operator()(const ITDense<Real>& d) const
        {
        auto nd = new ITDense<Complex>(d.t_.range());
        std::copy(d.t_.cbegin(),d.t_.cend(),nd->t_.begin());
        nd->t_(inds_) = elt_;
        return NewData(nd);
        }

    NewData
    operator()(ITDense<Complex>& d) const
        {
        d.t_(inds_) = elt_;
        return NewData();
        }

    private:
    const Complex& elt_;
    const std::array<int,size>& inds_;
    };

template<int size>
struct SetEltReal
    {
    SetEltReal(Real elt,
               const std::array<int,size>& inds)
        : elt_(elt),
          inds_(inds)
        { }

    NewData
    operator()(ITDense<Real>& d) const
        {
        d.t_(inds_) = elt_;
        return NewData();
        }

    NewData
    operator()(ITDense<Complex>& d) const
        {
        d.t_(inds_) = Complex(elt_,0);
        return NewData();
        }

    private:
    Real elt_;
    const std::array<int,size>& inds_;
    };

template <typename F>
struct VisitIT
    {
    VisitIT(F&& f, const LogNumber& scale)
        : f_(f), scale_fac(scale.real0())
        { }

    template <typename T>
    NewData
    operator()(const ITDense<T>& d) const
        {
        const auto end = d.t_.data()+d.t_.size();
        for(auto it = d.t_.data(); it != end; ++it)
            {
            f_((*it)*scale_fac);
            }
        return NewData();
        }

    private:
    F& f_;
    Real scale_fac;
    };


}; //namespace itensor

#endif

