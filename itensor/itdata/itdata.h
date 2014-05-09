//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDATA_H
#define __ITENSOR_ITDATA_H
#include "itensor/global.h"
#include "btas/tensor.h"
#include "itensor/detail/mapwrap.h"

namespace itensor {

struct Func1;
struct ConstFunc1;
struct Func2;
struct Func2Mod;

class ITData
    {
    public:

    using Ptr = std::shared_ptr<ITData>;
    using CPtr = std::shared_ptr<const ITData>;
    using NewPtr = std::unique_ptr<ITData>;
    using Range = btas::DEFAULT::range;

    ITData() { }

    virtual
    ~ITData() { }

    NewPtr virtual
    clone() const = 0;

    private:

    ITData(const ITData&); //disabled to prevent copying
    ITData& operator=(const ITData&); //disabled to prevent copying

    void virtual
    plugInto(const ConstFunc1& f) const = 0;
    NewBase virtual
    plugInto(const Func1& f) = 0;

    NewBase virtual
    plugSecond(const Func2Mod& f, const PBase& arg1) const = 0;
    NewBase virtual
    plugFirst(const Func2Mod& f, const TR& arg2) = 0;
    NewBase virtual
    plugFirst(const Func2Mod& f, const TC& arg2) = 0;

    NewBase virtual
    plugSecond(const Func2& f, const CPBase& arg1) const = 0;
    NewBase virtual
    plugFirst(const Func2& f, const TR& arg2) const = 0;
    NewBase virtual
    plugFirst(const Func2& f, const TC& arg2) const = 0;

    template <class Derived>
    friend struct ITDispatch;

    friend void apply(const Func1& f, PBase&);
    friend void apply(const ConstFunc1&, const CPBase&);
    friend void apply(const Func2Mod&, PBase&, const CPBase&);
    friend NewBase apply(const Func2&, const CPBase&, const CPBase&);

    };

template <class Derived>
struct ITDispatch : public Base
    {
    private:
    
    NewBase
    clone() const final 
        { 
        auto pdt = static_cast<const Derived*>(this);
        //Should change this to make_unique<Derived>(t_);
        //once C++14 make_unique feature is available
        return NewBase(new Derived(*pdt));
        }

    void
    plugInto(const ConstFunc1& f) const final
        {
        f(*(static_cast<const Derived*>(this)));
        }
    NewBase
    plugInto(const Func1& f) final
        {
        return f(*(static_cast<Derived*>(this)));
        }

    NewBase
    plugSecond(const Func2Mod& f, const PBase& arg1) const final
        {
        return arg1->plugFirst(f,*(static_cast<const Derived*>(this)));
        }
    NewBase
    plugFirst(const Func2Mod& f, const TR& arg2) final
        {
        return f(*(static_cast<Derived*>(this)),arg2);
        }
    NewBase
    plugFirst(const Func2Mod& f, const TC& arg2) final
        {
        return f(*(static_cast<Derived*>(this)),arg2);
        }

    NewBase
    plugSecond(const Func2& f, const CPBase& arg1) const final
        {
        return arg1->plugFirst(f,*(static_cast<const Derived*>(this)));
        }
    NewBase
    plugFirst(const Func2& f, const TR& arg2) const final
        {
        return f(*(static_cast<const Derived*>(this)),arg2);
        }
    NewBase
    plugFirst(const Func2& f, const TC& arg2) const final
        {
        return f(*(static_cast<const Derived*>(this)),arg2);
        }
    };

void inline
apply(const ConstFunc1& f,
      const CPBase& arg)
    {
    arg->plugInto(f);
    }

void inline
apply(const Func1& f,
      PBase& arg)
    {
    auto res = arg->plugInto(f);
    if(res) arg = std::move(res);
    }

NewBase inline
apply(const Func2& f,
      const CPBase& arg1,
      const CPBase& arg2)
    {
    return arg2->plugSecond(f,arg1);
    }

void inline
apply(const Func2Mod& f,
      PBase&  arg1,
      const CPBase& arg2)
    {
    auto res = arg2->plugSecond(f,arg1);
    if(res) arg1 = std::move(res);
    }


}; //namespace itensor

#endif

