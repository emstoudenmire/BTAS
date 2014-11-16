//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDATA_FUNCTIONS_H
#define __ITENSOR_ITDATA_FUNCTIONS_H
#include "itensor/global.h"
#include "btas/tensor.h"
#include "itensor/itdata/itdense.h"
#include "itensor/itdata/itscalar.h"
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

    template <typename T,
              typename std::enable_if<std::is_same<T,std::result_of_t<F(T)>>::value>::type* = nullptr>
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

    //TODO:
    //Could unify this with ITDense by providing
    //subset of Tensor interface, like data() pointer
    template <typename T,
              typename std::enable_if<std::is_same<T,std::result_of_t<F(T)>>::value>::type* = nullptr>
    NewData
    operator()(ITScalar<T>& d) const
        {
        d.x_ = f_(d.x_);
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

    template <typename T>
    NewData
    operator()(ITScalar<T>& d) const
        {
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

    NewData
    operator()(const ITDense<Real>& a1,
               const ITDense<Complex>& a2) const
        {
        ITDense<Complex> c1(a1);
        return operator()(c1,a2);
        }

    NewData
    operator()(const ITDense<Complex>& a1,
               const ITDense<Real>& a2) const
        {
        ITDense<Complex> c2(a2);
        return operator()(a1,c2);
        }


    //template <typename T1, typename T2>
    //NewData
    //operator()(const ITDense<T1>& a1,
    //           const ITDense<T2>& a2) const
    //    {
    //    using product_type = decltype(::std::declval<T1>() * ::std::declval<T2>());
    //    //static const auto One = product_type(1.),
    //    //                  Zero = product_type(0.);
    //    auto res = new ITDense<product_type>();
    //    //TODO:
    //    Error("Contract not implemented for tensors of different element types.");
    //    //btas::contract(One,a1.t_,Lind_,a2.t_,Rind_,Zero,res->t_,Pind_);
    //    return NewData(res);
    //    }

 
    private:
    const annotation& Lind_,
                      Rind_,
                      Pind_;
    };

struct FillReal
    {
    FillReal(Real r)
        : r_(r)
        { }

    NewData
    operator()(ITDense<Real>& d) const;
    NewData
    operator()(const ITDense<Complex>& d) const;
    NewData
    operator()(ITScalar<Real>& d) const
        {
        d.x_ = r_;
        return NewData();
        }
    NewData
    operator()(const ITScalar<Complex>& d) const
        {
        return NewData(new ITScalar<Real>(r_));
        }

    private:
    Real r_;
    };

struct FillCplx
    {
    FillCplx(Complex z)
        : z_(z)
        { }

    NewData
    operator()(const ITDense<Real>& d) const;
    NewData
    operator()(ITDense<Complex>& d) const;
    NewData
    operator()(const ITScalar<Real>& d) const
        {
        return NewData(new ITScalar<Complex>(z_));
        }
    NewData
    operator()(ITScalar<Complex>& d) const
        {
        d.x_ = z_;
        return NewData();
        }

    private:
    Complex z_;
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
        //TODO: implement for case when return type of f_ != T
        const auto end = d.t_.data()+d.t_.size();
        for(auto it = d.t_.data(); it != end; ++it)
            {
            *it = f_();
            }
        return NewData();
        }

    template <typename T>
    NewData
    operator()(ITScalar<T>& d) const
        {
        auto val = f_();
        using ftype = decltype(val);
        if(std::is_same<ftype,T>::value)
            {
            d.x_ = val;
            }
        else
            {
            auto nd = new ITScalar<ftype>(val);
            return NewData(nd);
            }
        return NewData();
        }

    private:
    F& f_;
    };

template<typename T, int size>
class GetElt
    {
    using Inds = std::array<int,size>;
    T elt_;
    const Inds& inds_;
    public:

    GetElt(const Inds& inds)
        : inds_(inds)
        { }

    explicit operator T() const { return elt_; }

    template <typename V,
              typename std::enable_if<std::is_convertible<V,T>::value>::type* = nullptr>
    void
    operator()(const ITDense<V>& d)
        {
        elt_ = T(d.t_(inds_));
        }

    template <typename V,
              typename std::enable_if<std::is_convertible<V,T>::value>::type* = nullptr>
    void
    operator()(const ITScalar<V>& d)
        {
        elt_ = T(d.x_);
        }

    template <class D>
    void
    operator()(const D& d)
        {
        throw ITError("ITensor does not have requested element type");
        }

    private:
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
    NewData
    operator()(const ITScalar<Real>& d) const;
    NewData
    operator()(ITScalar<Complex>& d) const;

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

    template <typename T>
    NewData
    operator()(ITScalar<T>& d) const
        {
        d.x_ *= r_;
        return NewData();
        }

    private:
    Real r_;
    };

struct PlusEQ
    {
    using perm = btas::varray<size_t>;

    PlusEQ(Real fac)
        :
        fac_(fac)
        { }

    PlusEQ(const perm& P,
           Real fac)
        :
        P_(P),
        fac_(fac)
        { }

    NewData
    operator()(ITDense<Real>& a1,
               const ITDense<Complex>& a2) const
        {
        auto np = new ITDense<Complex>(a1);
        operator()(*np,a2);
        return NewData(np);
        }

    //TODO: handle this case automatically in btas::axpy
    //      once it is supported
    NewData
    operator()(ITDense<Complex>& a1,
               const ITDense<Real>& a2) const
        {
        ITDense<Complex> a2c(a2);
        operator()(a1,a2c);
        return NewData();
        }

    template <typename T>
    NewData
    operator()(ITDense<T>& a1,
               const ITDense<T>& a2) const
        {
        //axpy computes a1.t_ += a2.t_ * fac
        if(P_.empty())
            {
            btas::axpy(fac_,a2.t_,a1.t_);
            }
        else
            {
            //TODO: inefficient - have to copy permutation
            //      of a2.t_ instead of using a TensorView
            //      because axpy doesn't work for TensorViews
            //      or for mixed Tensor/TensorView arguments
            decltype(a2.t_) a2copy = permute(a2.t_,P_);
            btas::axpy(fac_,a2copy,a1.t_);

            //Would like to do:
            //auto a2view = permute(a2.t_,P_);
            //btas::axpy(fac_,a2view,a1.t_);
            }
        return NewData();
        }

    template <typename T>
    NewData
    operator()(ITScalar<T>& a1,
               const ITScalar<T>& a2) const
        {
        a1.x_ += fac_*a2.x_;
        return NewData();
        }

    template <typename T1, typename T2>
    NewData
    operator()(ITScalar<T1>& a1,
               const ITScalar<T2>& a2) const
        {
        auto res = a1.x_+fac_*a2.x_;
        auto nd = new ITScalar<decltype(res)>(res);
        return NewData(nd);
        }
 
    private:
    perm P_;
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

    void
    operator()(const ITDense<Real>& d) const;
    void
    operator()(const ITDense<Complex>& d) const;
    void
    operator()(const ITScalar<Real>& d) const;
    void
    operator()(const ITScalar<Complex>& d) const;
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

    NewData
    operator()(const ITScalar<Real>& d) const
        {
        auto nd = new ITScalar<Complex>(elt_);
        return NewData(nd);
        }

    NewData
    operator()(ITScalar<Complex>& d) const
        {
        d.x_ = elt_;
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

    NewData
    operator()(ITScalar<Real>& d) const
        {
        d.x_ = elt_;
        return NewData();
        }

    NewData
    operator()(ITScalar<Complex>& d) const
        {
        d.x_ = Complex(elt_,0);
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

    template <typename T>
    NewData
    operator()(const ITScalar<T>& d) const
        {
        f_(d.x_*scale_fac);
        return NewData();
        }

    private:
    F& f_;
    Real scale_fac;
    };


}; //namespace itensor

#endif

