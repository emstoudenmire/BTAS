//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITERPAIR_H
#define __ITENSOR_ITERPAIR_H

namespace itensor {
namespace detail {

template <typename _Iter>
struct IterPair
    {
    using iterator = _Iter;

    IterPair() {}

    IterPair(const _Iter& b,
             const _Iter& e)
        : 
        begin_(b),
        end_(e)
        {}

    IterPair(_Iter&& b,
             _Iter&& e)
        : 
        begin_(b),
        end_(e)
        {}

    template <typename _I>
    IterPair(const IterPair<_I>& IP)
        : 
        begin_(IP.begin_),
        end_(IP.end_)
        {}

    template <typename _I>
    IterPair(IterPair<_I>&& IP)
        : 
        begin_(IP.begin_),
        end_(IP.end_)
        {}


    _Iter
    begin() const { return begin_; }

    _Iter
    end() const { return end_; }

    private:
    _Iter begin_,
          end_;

    };


}; //namespace detail
}; //namespace itensor

#endif

