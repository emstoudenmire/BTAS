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
    void virtual
    operator()(const ITDense<Real>& t) const = 0;
    void virtual
    operator()(const ITDense<Complex>& t) const = 0;
    };

template <typename Derived>
struct ConstFunc1 : public ConstFunc1Base
    {
    void virtual
    operator()(const ITDense<Real>& t) const final
        {
        static_cast<const Derived*>(this)->apply(t);
        }
    void virtual
    operator()(const ITDense<Complex>& t) const final
        {
        static_cast<const Derived*>(this)->apply(t);
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

    void virtual
    plugInto(const ConstFunc1Base& f) const = 0;
    NewData virtual
    plugInto(const Func1Base& f) = 0;

    NewData virtual
    plugSecond(const Func2ModBase& f, PData& arg1) const = 0;
    NewData virtual
    plugFirst(const Func2ModBase& f, const ITDense<Real>& arg2) = 0;
    NewData virtual
    plugFirst(const Func2ModBase& f, const ITDense<Complex>& arg2) = 0;

    NewData virtual
    plugSecond(const Func2Base& f, const ITData& arg1) const = 0;
    NewData virtual
    plugFirst(const Func2Base& f, const ITDense<Real>& arg2) const = 0;
    NewData virtual
    plugFirst(const Func2Base& f, const ITDense<Complex>& arg2) const = 0;

    template <class Derived>
    friend struct ITDispatch;

    friend void applyFunc(const ConstFunc1Base&, const ITData&);
    friend void applyFunc(const ConstFunc1Base&, const CPData&);
    friend void applyFunc(const Func1Base& f, PData&);
    friend void applyFunc(const Func1Base& f, NewData&);
    friend NewData applyFunc(const Func2Base&, const ITData&, const ITData&);
    friend void applyFunc(const Func2ModBase&, PData&, const ITData&);
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

    void
    plugInto(const ConstFunc1Base& f) const final
        {
        f(*(static_cast<const Derived*>(this)));
        }
    NewData
    plugInto(const Func1Base& f) final
        {
        return f(*(static_cast<Derived*>(this)));
        }

    NewData
    plugSecond(const Func2ModBase& f, PData& arg1) const final
        {
        return arg1->plugFirst(f,*(static_cast<const Derived*>(this)));
        }
    NewData
    plugFirst(const Func2ModBase& f, const ITDense<Real>& arg2) final
        {
        return f(*(static_cast<Derived*>(this)),arg2);
        }
    NewData
    plugFirst(const Func2ModBase& f, const ITDense<Complex>& arg2) final
        {
        return f(*(static_cast<Derived*>(this)),arg2);
        }

    NewData
    plugSecond(const Func2Base& f, const ITData& arg1) const final
        {
        return arg1.plugFirst(f,*(static_cast<const Derived*>(this)));
        }
    NewData
    plugFirst(const Func2Base& f, const ITDense<Real>& arg2) const final
        {
        return f(*(static_cast<const Derived*>(this)),arg2);
        }
    NewData
    plugFirst(const Func2Base& f, const ITDense<Complex>& arg2) const final
        {
        return f(*(static_cast<const Derived*>(this)),arg2);
        }
    };

void inline
applyFunc(const ConstFunc1Base& f,
          const ITData& arg)
    {
    arg.plugInto(f);
    }

void inline
applyFunc(const ConstFunc1Base& f,
          const CPData& arg)
    {
    arg->plugInto(f);
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

void inline
applyFunc(const Func2ModBase& f,
          PData&  arg1,
          const ITData& arg2)
    {
    auto res = arg2.plugSecond(f,arg1);
    if(res) arg1 = std::move(res);
    }


}; //namespace itensor

#endif

