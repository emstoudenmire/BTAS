#include "test.h"

#include "itensor/itensor.h"

using namespace itensor;
using detail::contains;

TEST_CASE("Complex Numbers")
    {

    SECTION("Basics")
        {
        CHECK_CLOSE(1_i,Complex(0,1),1E-10);

        const auto z1 = 1+1_i;
        CHECK_CLOSE(z1,Complex(1,1),1E-10);

        const auto z2 = 1-1_i;
        CHECK_CLOSE(z2,Complex(1,-1),1E-10);

        const auto z3 = 2_i+3;
        CHECK_CLOSE(z3,Complex(3,2),1E-10);

        const auto z4 = 0.1_i+0.2;
        CHECK_CLOSE(z4,Complex(0.2,0.1),1E-10);
        }

    SECTION("Int Operations")
        {
        const int n = 4;
        const auto z1 = 1_i * n;
        CHECK_CLOSE(z1,Complex(0,n),1E-10);

        //Check a different integral type
        long unsigned int lui = 5;
        const auto z2 = (1+1_i) + lui;
        CHECK_CLOSE(z2,Complex(lui+1,1),1E-10);

        auto z3 = 4.+2.3_i;
        z3 *= 1;
        auto z4 = z3 / 1;
        CHECK_CLOSE(z4,z3,1E-10);
        }

    }
