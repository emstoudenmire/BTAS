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
    Func1Base() { }
    virtual ~Func1Base() { }

    NewData virtual
    operator()(ITDense<Real>& t) = 0;
    NewData virtual
    operator()(ITDense<Complex>& t) = 0;
    NewData virtual
    operator()(ITScalar<Real>& t) = 0;
    NewData virtual
    operator()(ITScalar<Complex>& t) = 0;

    template <typename T>
    NewData
    operator()(T& t)
        {
        throw ITError("Operation not defined for ITData subtype. [Func1Base]");
        return NewData();
        }
    };

template <typename Callable>
class Func1 : public Func1Base
    {
    Callable& d_;
    public:
    Func1(Callable& d) : d_(d) { }
    virtual ~Func1() { }

    NewData virtual
    operator()(ITDense<Real>& t) { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(ITDense<Complex>& t) { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(ITScalar<Real>& t) { return detail::call<NewData>(d_,t); }
    NewData virtual
    operator()(ITScalar<Complex>& t) { return detail::call<NewData>(d_,t); }
    };

struct ConstFunc1Base
    {
    ConstFunc1Base() { }
    virtual ~ConstFunc1Base() { }

    void virtual
    operator()(const ITDense<Real>& t) = 0;
    void virtual
    operator()(const ITDense<Complex>& t) = 0;
    void virtual
    operator()(const ITScalar<Real>& t) = 0;
    void virtual
    operator()(const ITScalar<Complex>& t) = 0;

    template <typename T>
    void
    operator()(const T& t)
        {
        throw ITError("Operation not defined for ITData subtype. [ConstFunc1Base]");
        }
    };

template <typename Callable>
class ConstFunc1 : public ConstFunc1Base
    {
    Callable& d_;
    public:
    ConstFunc1(Callable& d) : d_(d) { }
    virtual ~ConstFunc1() { }

    void 
    operator()(const ITDense<Real>& t) final { detail::call(d_,t); }
    void 
    operator()(const ITDense<Complex>& t) final { detail::call(d_,t); }
    void 
    operator()(const ITScalar<Real>& t) final { detail::call(d_,t); }
    void 
    operator()(const ITScalar<Complex>& t) final { detail::call(d_,t); }
    };

struct Func2Base
    {
    Func2Base() { }
    virtual ~Func2Base() { }

    NewData virtual
    operator()(const ITData& arg1, const ITDense<Real>& arg2) = 0;
    NewData virtual
    operator()(const ITData& arg1, const ITDense<Complex>& arg2) = 0;
    NewData virtual
    operator()(const ITData& arg1, const ITScalar<Real>& arg2) = 0;
    NewData virtual
    operator()(const ITData& arg1, const ITScalar<Complex>& arg2) = 0;

    template <typename T1,typename T2>
    NewData
    operator()(const T1& t1, const T2& t2) 
        {
        throw ITError("Operation not defined for ITData subtype. [Func2Base]");
        return NewData();
        }
    };

template <typename Callable>
class Func2 : public Func2Base
    {
    Callable& d_;
    public:

    Func2(Callable& d) : d_(d) { }
    virtual ~Func2() { }

    NewData 
    operator()(const ITData& arg1, const ITDense<Real>& arg2) final { return applyTo(arg1,arg2); }
    NewData 
    operator()(const ITData& arg1, const ITDense<Complex>& arg2) final { return applyTo(arg1,arg2); } 
    NewData 
    operator()(const ITData& arg1, const ITScalar<Real>& arg2) final { return applyTo(arg1,arg2); }
    NewData 
    operator()(const ITData& arg1, const ITScalar<Complex>& arg2) final { return applyTo(arg1,arg2); }

    private:

    template<typename DataType>
    NewData
    applyTo(const ITData& arg1, const DataType& arg2);
    };

struct Func2ModBase
    {
    Func2ModBase() { }
    virtual ~Func2ModBase() { }

    NewData virtual
    operator()(PData& arg1, const ITDense<Real>& arg2) = 0;
    NewData virtual
    operator()(PData& arg1, const ITDense<Complex>& arg2) = 0;
    NewData virtual
    operator()(PData& arg1, const ITScalar<Real>& arg2) = 0;
    NewData virtual
    operator()(PData& arg1, const ITScalar<Complex>& arg2) = 0;

    template <typename T1,typename T2>
    NewData
    operator()(T1& t1, const T2& t2)
        {
        throw ITError("Operation not defined for ITData subtype. [Func2ModBase]");
        return NewData();
        }
    };

template <typename Callable>
class Func2Mod : public Func2ModBase
    {
    Callable& d_;
    public:

    Func2Mod(Callable& d) : d_(d) { }
    virtual ~Func2Mod() { }

    NewData 
    operator()(PData& arg1, const ITDense<Real>& arg2) final { return applyTo(arg1,arg2); }
    NewData 
    operator()(PData& arg1, const ITDense<Complex>& arg2) final { return applyTo(arg1,arg2); } 
    NewData 
    operator()(PData& arg1, const ITScalar<Real>& arg2) final { return applyTo(arg1,arg2); }
    NewData 
    operator()(PData& arg1, const ITScalar<Complex>& arg2) final { return applyTo(arg1,arg2); }

    private:

    template<typename DataType>
    NewData
    applyTo(PData& arg1, const DataType& arg2);
    };

class ITData
    {
    public:

    ITData() { }
    virtual ~ITData() { }

    NewData virtual
    clone() const = 0;

    void virtual
    plugInto(ConstFunc1Base& f) const = 0;

    NewData virtual
    plugInto(Func1Base& f) = 0;

    NewData virtual
    plugSecond(Func2ModBase& f, PData& arg1) const = 0;

    NewData virtual
    plugSecond(Func2Base& f, const ITData& arg1) const = 0;
    };

template <class Derived>
struct ITDispatch : public ITData
    {
    ITDispatch() { }
    virtual ~ITDispatch() { }

    private:
    
    NewData
    clone() const final 
        { 
        auto pdt = static_cast<const Derived*>(this);
        return std::make_unique<Derived>(*pdt);
        }

    void
    plugInto(ConstFunc1Base& f) const final
        {
        const Derived& dt = *(static_cast<const Derived*>(this));
        f(dt);
        }
        
    NewData
    plugInto(Func1Base& f) final
        {
        Derived& dt = *(static_cast<Derived*>(this));
        return f(dt);
        }

    NewData
    plugSecond(Func2ModBase& f, PData& arg1) const final
        {
        const Derived& a2 = *(static_cast<const Derived*>(this));
        return f(arg1,a2);
        }

    NewData
    plugSecond(Func2Base& f, const ITData& arg1) const final
        {
        const Derived& a2 = *(static_cast<const Derived*>(this));
        return f(arg1,a2);
        }
    };

template<typename Callable>
template<typename DataType>
NewData Func2<Callable>::
applyTo(const ITData& arg1, const DataType& arg2)
    {
    //std::cout << "In Func2::call" << std::endl;
    auto C = [this,&arg2](const auto& a1) { return detail::call<NewData>(this->d_,a1,arg2); };
    auto f1 = ConstFunc1<decltype(C)>(C);
    return arg1.plugInto(f1);
    }

template<typename Callable>
template<typename DataType>
NewData Func2Mod<Callable>::
applyTo(PData& arg1, const DataType& arg2)
    {
    //std::cout << "In Func2Mod::call" << std::endl;
    auto C = [this,&arg2](auto& a1) { return detail::call<NewData>(this->d_,a1,arg2); };
    auto f1 = Func1<decltype(C)>(C);
    return arg1->plugInto(f1);
    }


//
// applyFunc methods
//

template<typename F>
F
applyFunc(const ITData& arg,
          F f = F())
    {
    ConstFunc1<F> cf1(f);
    arg.plugInto(cf1);
    return f;
    }

template<typename F>
F
applyFunc(const CPData& arg,
          F f = F())
    {
    return applyFunc(*arg,f);
    }

namespace detail {
template<typename F>
NewData
applyFuncImpl(ITData& arg,
              F& f)
    {
    Func1<F> f1(f);
    return arg.plugInto(f1);
    }

};

template<typename F>
F
applyFunc(PData& arg,
          F f = F())
    {
    auto res = detail::applyFuncImpl(*arg,f);
    if(res) arg = std::move(res);
    return f;
    }

template<typename F>
F
applyFunc(NewData& arg,
          F f = F())
    {
    auto res = detail::applyFuncImpl(*arg,f);
    if(res) arg = std::move(res);
    return f;
    }

template<typename F>
F
applyFunc(const ITData& arg1,
          const ITData& arg2,
          F f = F())
    {
    Func2<F> f2(f);
    arg2.plugSecond(f2,arg1);
    return f;
    }

template<typename F>
F
applyFunc(const PData& arg1,
          const PData& arg2,
          F f = F())
    {
    return applyFunc(*arg1,*arg2,f);
    }

template<typename F>
F 
applyFunc(PData& arg1,
          const ITData& arg2,
          F f = F())
    {
    Func2Mod<F> f2m(f);
    auto res = arg2.plugSecond(f2m,arg1);
    if(res) arg1 = std::move(res);
    return f;
    }

template<typename F>
F 
applyFunc(PData& arg1,
          const PData& arg2,
          F f = F())
    {
    return applyFunc(arg1,*arg2,f);
    }

}; //namespace itensor

#endif

