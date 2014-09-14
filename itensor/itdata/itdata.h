//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDATA_H
#define __ITENSOR_ITDATA_H
#include "itensor/global.h"
#include "btas/range.h"
#include "btas/defaults.h"
#include "itensor/detail/functions.h"

namespace itensor {

class ITData;

using PData = std::shared_ptr<ITData>;
using CPData = std::shared_ptr<const ITData>;
using NewData = std::unique_ptr<ITData>;

template <typename T_>
class ITDense;

template <typename T_>
class ITScalar;
struct Func1Base
    {
    NewData virtual
    operator()(ITDense<Real>& t) const = 0;
    NewData virtual
    operator()(ITDense<Complex>& t) const = 0;
    NewData virtual
    operator()(ITScalar<Real>& t) const = 0;
    NewData virtual
    operator()(ITScalar<Complex>& t) const = 0;

    template <typename T>
    NewData
    operator()(T& t) const
        {
        throw ITError("Operation not defined for ITData subtype. [Func1Base]");
        return NewData();
        }
    };

template <typename Derived>
struct Func1 : public Func1Base
    {
    Func1(Derived& d) : d_(d) { }

    NewData virtual
    operator()(ITDense<Real>& t) const { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(ITDense<Complex>& t) const { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(ITScalar<Real>& t) const { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(ITScalar<Complex>& t) const { return detail::call<NewData>(d_,t); }

    private:
    Derived& d_;
    };

struct ConstFunc1Base
    {
    NewData virtual
    operator()(const ITDense<Real>& t) const = 0;
    NewData virtual
    operator()(const ITDense<Complex>& t) const = 0;
    NewData virtual
    operator()(const ITScalar<Real>& t) const = 0;
    NewData virtual
    operator()(const ITScalar<Complex>& t) const = 0;

    template <typename T>
    NewData
    operator()(const T& t) const
        {
        throw ITError("Operation not defined for ITData subtype. [ConstFunc1Base]");
        return NewData();
        }
    };

template <typename Derived>
struct ConstFunc1 : public ConstFunc1Base
    {
    ConstFunc1(Derived& d) : d_(d) { }

    NewData virtual
    operator()(const ITDense<Real>& t) const { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(const ITDense<Complex>& t) const { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(const ITScalar<Real>& t) const { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(const ITScalar<Complex>& t) const { return detail::call<NewData>(d_,t); }

    private:
    Derived& d_;
    };

struct Func2Base
    {
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITDense<Complex>& a2) const = 0;
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITDense<Complex>& a2) const = 0;

    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITScalar<Complex>& a2) const = 0;
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITScalar<Complex>& a2) const = 0;

    NewData virtual
    operator()(const ITDense<Real>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITScalar<Complex>& a2) const = 0;
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITScalar<Complex>& a2) const = 0;

    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITDense<Complex>& a2) const = 0;
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITDense<Complex>& a2) const = 0;

    template <typename T1,typename T2>
    NewData
    operator()(const T1& t1, const T2& t2) const
        {
        throw ITError("Operation not defined for ITData subtype. [Func2Base]");
        return NewData();
        }
    };

template <typename Derived>
struct Func2 : public Func2Base
    {
    Func2(Derived&& d) : d_(d) { }

    NewData virtual
    operator()(const ITDense<Real>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    NewData virtual
    operator()(const ITDense<Real>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITScalar<Real>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(const ITScalar<Complex>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    private:
    Derived& d_;
    };

struct Func2ModBase
    {
    NewData virtual
    operator()(ITDense<Real>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(ITDense<Real>& a1,const ITDense<Complex>& a2) const = 0;
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITDense<Complex>& a2) const = 0;

    NewData virtual
    operator()(ITScalar<Real>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(ITScalar<Real>& a1,const ITScalar<Complex>& a2) const = 0;
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITScalar<Complex>& a2) const = 0;

    NewData virtual
    operator()(ITDense<Real>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(ITDense<Real>& a1,const ITScalar<Complex>& a2) const = 0;
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITScalar<Real>& a2) const = 0;
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITScalar<Complex>& a2) const = 0;

    NewData virtual
    operator()(ITScalar<Real>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(ITScalar<Real>& a1,const ITDense<Complex>& a2) const = 0;
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITDense<Real>& a2) const = 0;
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITDense<Complex>& a2) const = 0;

    template <typename T1,typename T2>
    NewData
    operator()(T1& t1, const T2& t2) const
        {
        throw ITError("Operation not defined for ITData subtype. [Func2ModBase]");
        return NewData();
        }
    };

template <typename Derived>
struct Func2Mod : public Func2ModBase
    {
    Func2Mod(Derived&& d) : d_(d) { }

    NewData virtual
    operator()(ITDense<Real>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITDense<Real>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    NewData virtual
    operator()(ITScalar<Real>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITScalar<Real>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    NewData virtual
    operator()(ITDense<Real>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITDense<Real>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITScalar<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITScalar<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    NewData virtual
    operator()(ITScalar<Real>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITScalar<Real>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITDense<Real>& a2) const final { return detail::call<NewData>(d_,a1,a2); }
    NewData virtual
    operator()(ITScalar<Complex>& a1,const ITDense<Complex>& a2) const final { return detail::call<NewData>(d_,a1,a2); }

    private:
    Derived& d_;
    };

class ITData
    {
    public:

    ITData() { }

    virtual
    ~ITData() { }

    NewData virtual
    clone() const = 0;

    //ITData(const ITData&) = delete; //disabled to prevent copying
    //ITData& operator=(const ITData&) = delete; //disabled to prevent copying

    NewData virtual
    plugInto(const ConstFunc1Base& f) const = 0;

    NewData virtual
    plugInto(const Func1Base& f) = 0;

    NewData virtual
    plugSecond(const Func2ModBase& f, PData& arg1) const = 0;

    NewData virtual
    plugSecond(const Func2Base& f, const ITData& arg1) const = 0;
    };

template <class Derived>
struct ITDispatch : public ITData
    {
    private:
    
    NewData
    clone() const final 
        { 
        auto pdt = static_cast<const Derived*>(this);
        //Should change this to make_unique<Derived>(t_);
        //once C++14 make_unique feature is available
        return NewData(new Derived(*pdt));
        }

    NewData
    plugInto(const ConstFunc1Base& f) const final
        {
        const auto& dt = *(static_cast<const Derived*>(this));
        return f(dt);

        //auto hold = [&dt](const auto& functor) { return functor(dt); };
        //return f.plugInto(hold); //f.plugInto virtual, can't be templated
        }
        
    NewData
    plugInto(const Func1Base& f) final
        {
        return f(*(static_cast<Derived*>(this)));
        }

    NewData
    plugSecond(const Func2ModBase& f, PData& arg1) const final
        {
        const auto& a2 = *(static_cast<const Derived*>(this));
        auto C = [&f,&a2](auto& a1) { return f(a1,a2); };
        return arg1->plugInto(Func1<decltype(C)>(C));
        }

    NewData
    plugSecond(const Func2Base& f, const ITData& arg1) const final
        {
        const auto& a2 = *(static_cast<const Derived*>(this));
        auto C = [&f,&a2](const auto& a1) { return f(a1,a2); };
        return arg1.plugInto(ConstFunc1<decltype(C)>(C));
        }

    //
    // Think of external template function "apply" as replacing
    // the Func1Base or ConstFunc1Base objects, and think of f,a1,a2
    // as the arguments of apply with the detail that f is a FuncWrap
    // instead of an ITData
    //
    // Test use of "apply" in the f(a1) case first [apply(f,a1)]
    //
    //NewData
    //plugThird(const FuncWrap& f, const ITData& arg1) const final
    //    {
    //    const auto& a2 = *(static_cast<const Derived*>(this));
    //    auto C = [&f,&a2](const auto& a1) { return apply(f,a1,a2); };
    //    return arg1.plugSecond(f,ConstFunc1<decltype(C)>(C));
    //    }
    };

template<typename F>
NewData
applyFunc(F&& f,
          const ITData& arg)
    {
    //ConstFunc1<decltype(f)> cf1(std::forward<F>(f));
    ConstFunc1<decltype(f)> cf1(f);
    return arg.plugInto(cf1);
    }


template<typename F>
NewData
applyFunc(F&& f,
          const CPData& arg)
    {
    //ConstFunc1<decltype(f)> cf1(std::forward<F>(f));
    ConstFunc1<decltype(f)> cf1(f);
    return arg->plugInto(cf1);
    }

template<typename F>
void
applyFunc(F&& f,
          PData& arg)
    {
    //Func1<decltype(f)> f1(std::forward<F>(f));
    Func1<decltype(f)> f1(f);
    auto res = arg->plugInto(f1);
    if(res) arg = std::move(res);
    }

template<typename F>
void
applyFunc(F&& f,
          NewData& arg)
    {
    //Func1<decltype(f)> f1(std::forward<F>(f));
    Func1<decltype(f)> f1(f);
    auto res = arg->plugInto(f1);
    if(res) arg = std::move(res);
    }

template<typename F>
NewData
applyFunc(F&& f,
          const ITData& arg1,
          const ITData& arg2)
    {
    Func2<decltype(f)> f2(std::forward<F>(f));
    return arg2.plugSecond(f2,arg1);
    }

template<typename F>
NewData
applyFunc(F&& f,
          const PData& arg1,
          const PData& arg2)
    {
    Func2<decltype(f)> f2(std::forward<F>(f));
    return arg2->plugSecond(f2,*arg1);
    }

template<typename F>
void 
applyFunc(F&& f,
          PData&  arg1,
          const ITData& arg2)
    {
    Func2Mod<decltype(f)> f2m(std::forward<F>(f));
    auto res = arg2.plugSecond(f2m,arg1);
    if(res) arg1 = std::move(res);
    }

template<typename F>
void 
applyFunc(F&& f,
          PData&  arg1,
          const PData& arg2)
    {
    Func2Mod<decltype(f)> f2m(std::forward<F>(f));
    auto res = arg2->plugSecond(f2m,arg1);
    if(res) arg1 = std::move(res);
    }

}; //namespace itensor

#endif

