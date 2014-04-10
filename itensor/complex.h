//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_COMPLEX_H
#define __ITENSOR_COMPLEX_H

#include "itensor/real.h"
#include <complex>

namespace itensor {

using Complex = std::complex<Real>;

static const Complex Complex_1 = Complex(1,0);
static const Complex Complex_i = Complex(0,1);

}; //namespace itensor


#endif
