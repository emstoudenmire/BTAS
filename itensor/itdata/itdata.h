//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDATA_H
#define __ITENSOR_ITDATA_H
#include "itensor/global.h"
#include "btas/range.h"
#include "btas/defaults.h"

namespace itensor {

class ITData;

using PData = std::shared_ptr<ITData>;
using CPData = std::shared_ptr<const ITData>;
using NewData = std::unique_ptr<ITData>;

template <typename T_>
class ITDense;

struct Func1Base
    {
    NewData virtual
    operator()(ITDense<Real>& t) const = 0;
    NewData virtual
    operator()(ITDense<Complex>& t) const = 0;

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
    NewData virtual
    operator()(ITDense<Real>& t) const final
        {
        return static_cast<const Derived*>(this)->apply(t);
        }
    NewData virtual
    operator()(ITDense<Complex>& t) const final
        {
        return static_cast<const Derived*>(this)->apply(t);
        }
    };

struct ConstFunc1Base
    {
    NewData virtual
    operator()(const ITDense<Real>& t) const = 0;
    NewData virtual
    operator()(const ITDense<Complex>& t) const = 0;

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
    NewData virtual
    operator()(const ITDense<Real>& t) const final
        {
        return static_cast<const Derived*>(this)->apply(t);
        }
    NewData virtual
    operator()(const ITDense<Complex>& t) const final
        {
        return static_cast<const Derived*>(this)->apply(t);
        }
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
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITDense<Real>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITDense<Complex>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITDense<Real>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
    NewData virtual
    operator()(const ITDense<Complex>& a1,const ITDense<Complex>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
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
    NewData virtual
    operator()(ITDense<Real>& a1,const ITDense<Real>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
    NewData virtual
    operator()(ITDense<Real>& a1,const ITDense<Complex>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITDense<Real>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
    NewData virtual
    operator()(ITDense<Complex>& a1,const ITDense<Complex>& a2) const final
        {
        return static_cast<const Derived*>(this)->apply(a1,a2);
        }
    };

template <typename T2>
struct CurryFunc2 : public ConstFunc1<CurryFunc2<T2>>
    {
    CurryFunc2(const Func2Base& f,
           const T2& a2)
        :
        f_(f),
        a2_(a2)
        { }

    template <typename T1>
    NewData
    apply(const T1& a1) const
        {
        return f_(a1,a2_);
        }

    private:
    const Func2Base& f_;
    const T2& a2_;
    };

template <typename T2>
struct CurryFunc2Mod : public Func1<CurryFunc2Mod<T2>>
    {
    CurryFunc2Mod(const Func2ModBase& f,
                  const T2& a2)
        :
        f_(f),
        a2_(a2)
        { }

    template <typename T1>
    NewData
    apply(T1& a1) const
        {
        return f_(a1,a2_);
        }

    private:
    const Func2ModBase& f_;
    const T2& a2_;
    };


class ITData
    {
    public:

    ITData() { }

    virtual
    ~ITData() { }

    NewData virtual
    clone() const = 0;

    private:

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

    template <class Derived>
    friend struct ITDispatch;

    friend NewData applyFunc(const ConstFunc1Base&, const ITData&);
    friend NewData applyFunc(const ConstFunc1Base&, const CPData&);
    friend void applyFunc(const Func1Base& f, PData&);
    friend void applyFunc(const Func1Base& f, NewData&);
    friend NewData applyFunc(const Func2Base&, const ITData&, const ITData&);
    friend NewData applyFunc(const Func2Base&, const PData&, const PData&);
    friend void applyFunc(const Func2ModBase&, PData&, const ITData&);
    friend void applyFunc(const Func2ModBase&, PData&, const PData&);
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
        return f(*(static_cast<const Derived*>(this)));
        }
        
    NewData
    plugInto(const Func1Base& f) final
        {
        return f(*(static_cast<Derived*>(this)));
        }

    NewData
    plugSecond(const Func2ModBase& f, PData& arg1) const final
        {
        const auto& dt = *(static_cast<const Derived*>(this));
        return arg1->plugInto(CurryFunc2Mod<Derived>{f,dt});
        }

    NewData
    plugSecond(const Func2Base& f, const ITData& arg1) const final
        {
        const auto& dt = *(static_cast<const Derived*>(this));
        return arg1.plugInto(CurryFunc2<Derived>{f,dt});
        }
    };

NewData inline
applyFunc(const ConstFunc1Base& f,
          const ITData& arg)
    {
    return arg.plugInto(f);
    }

NewData inline
applyFunc(const ConstFunc1Base& f,
          const CPData& arg)
    {
    return arg->plugInto(f);
    }

void inline
applyFunc(const Func1Base& f,
          PData& arg)
    {
    auto res = arg->plugInto(f);
    if(res) arg = std::move(res);
    }

void inline
applyFunc(const Func1Base& f,
          NewData& arg)
    {
    auto res = arg->plugInto(f);
    if(res) arg = std::move(res);
    }

NewData inline
applyFunc(const Func2Base& f,
          const ITData& arg1,
          const ITData& arg2)
    {
    return arg2.plugSecond(f,arg1);
    }

NewData inline
applyFunc(const Func2Base& f,
          const PData& arg1,
          const PData& arg2)
    {
    return arg2->plugSecond(f,*arg1);
    }

void inline
applyFunc(const Func2ModBase& f,
          PData&  arg1,
          const ITData& arg2)
    {
    auto res = arg2.plugSecond(f,arg1);
    if(res) arg1 = std::move(res);
    }

void inline
applyFunc(const Func2ModBase& f,
          PData&  arg1,
          const PData& arg2)
    {
    auto res = arg2->plugSecond(f,arg1);
    if(res) arg1 = std::move(res);
    }

}; //namespace itensor

#endif

