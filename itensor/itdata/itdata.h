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

struct Func1
    {
    NewData virtual
    operator()(ITDense<Real>& t) const = 0;
    };

struct ConstFunc1
    {
    void virtual
    operator()(const ITDense<Real>& t) const = 0;
    };

struct Func2
    {
    NewData virtual
    operator()(const ITDense<Real>& a1,const ITDense<Real>& a2) const = 0;
    };

struct Func2Mod
    {
    NewData virtual
    operator()(ITDense<Real>& a1,const ITDense<Real>& a2) const = 0;
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
    plugInto(const ConstFunc1& f) const = 0;
    NewData virtual
    plugInto(const Func1& f) = 0;

    NewData virtual
    plugSecond(const Func2Mod& f, PData& arg1) const = 0;
    NewData virtual
    plugFirst(const Func2Mod& f, const ITDense<Real>& arg2) = 0;

    NewData virtual
    plugSecond(const Func2& f, const ITData& arg1) const = 0;
    NewData virtual
    plugFirst(const Func2& f, const ITDense<Real>& arg2) const = 0;

    template <class Derived>
    friend struct ITDispatch;

    friend void applyFunc(const Func1& f, PData&);
    friend void applyFunc(const ConstFunc1&, const ITData&);
    friend void applyFunc(const Func2Mod&, PData&, const ITData&);
    friend NewData applyFunc(const Func2&, const ITData&, const ITData&);
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
    plugInto(const ConstFunc1& f) const final
        {
        f(*(static_cast<const Derived*>(this)));
        }
    NewData
    plugInto(const Func1& f) final
        {
        return f(*(static_cast<Derived*>(this)));
        }

    NewData
    plugSecond(const Func2Mod& f, PData& arg1) const final
        {
        return arg1->plugFirst(f,*(static_cast<const Derived*>(this)));
        }
    NewData
    plugFirst(const Func2Mod& f, const ITDense<Real>& arg2) final
        {
        return f(*(static_cast<Derived*>(this)),arg2);
        }

    NewData
    plugSecond(const Func2& f, const ITData& arg1) const final
        {
        return arg1.plugFirst(f,*(static_cast<const Derived*>(this)));
        }
    NewData
    plugFirst(const Func2& f, const ITDense<Real>& arg2) const final
        {
        return f(*(static_cast<const Derived*>(this)),arg2);
        }
    };

void inline
applyFunc(const ConstFunc1& f,
          const ITData& arg)
    {
    arg.plugInto(f);
    }

void inline
applyFunc(const Func1& f,
          PData& arg)
    {
    auto res = arg->plugInto(f);
    if(res) arg = std::move(res);
    }

NewData inline
applyFunc(const Func2& f,
          const ITData& arg1,
          const ITData& arg2)
    {
    return arg2.plugSecond(f,arg1);
    }

void inline
applyFunc(const Func2Mod& f,
          PData&  arg1,
          const ITData& arg2)
    {
    auto res = arg2.plugSecond(f,arg1);
    if(res) arg1 = std::move(res);
    }


}; //namespace itensor

#endif

