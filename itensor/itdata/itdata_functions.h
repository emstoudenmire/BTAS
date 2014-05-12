//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITDATA_FUNCTIONS_H
#define __ITENSOR_ITDATA_FUNCTIONS_H
#include "itensor/global.h"
#include "btas/tensor.h"
#include "itensor/itdata/itdense.h"

namespace itensor {

struct Fill : public Func1
    {
    Fill(Real r)
        : r_(r)
        { }

    NewData
    operator()(ITDense<Real>& d) const final;

    private:
    Real r_;
    };

struct MultReal : public Func1
    {
    MultReal(Real r)
        : r_(r)
        { }

    NewData
    operator()(ITDense<Real>& d) const final;

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
    };



}; //namespace itensor

#endif

