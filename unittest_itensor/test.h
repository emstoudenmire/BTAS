#ifndef __ITENSOR_UNITTEST_TEST_H
#define __ITENSOR_UNITTEST_TEST_H

#include "catch.hpp"

#include <complex>

namespace unittest {

double inline
diff(std::complex<double> z1, std::complex<double> z2)
    {
    return abs(z1-z2);
    }

double inline
diff(double r1, double r2)
    {
    return fabs(r1-r2);
    }

double inline
diff(std::complex<double> z1, double r2)
    {
    return abs(z1-std::complex<double>(r2));
    }

double inline
diff(double r1, std::complex<double> z2)
    {
    return abs(z2-std::complex<double>(r1));
    }

};

#define CHECK_CLOSE(X,Y,tol) CHECK(unittest::diff(X,Y) < tol)

#endif
