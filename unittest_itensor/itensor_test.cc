#include "test.h"

#include "itensor/itensor.h"

using namespace itensor;
using detail::contains;

Real
real_part(Real r) { return r; }
Real
real_part(Complex z) { return z.real(); }

TEST_CASE("ITensor")
    {
    Index l1("l1",3),
          l2("l2",4),
          s1("s1",2,Site),
          s2("s2",2,Site);

    auto T0 = ITensor();
    auto T1 = randomize(ITensor(l1));
    auto T2 = randomize(ITensor(l1,l2));
    auto T3 = randomize(ITensor(l1,l2,s1));
    auto T4 = randomize(ITensor(l1,l2,s1,s2));

    SECTION("Basic Accessors")
        {
        REQUIRE(T0.empty());
        REQUIRE(T0.r() == 0);

        REQUIRE(!T1.empty());
        REQUIRE(T1.r() == 1);

        REQUIRE(!T2.empty());
        REQUIRE(T2.r() == 2);

        REQUIRE(!T3.empty());
        REQUIRE(T3.r() == 3);

        REQUIRE(!T4.empty());
        REQUIRE(T4.r() == 4);
        }

    SECTION("Fill and Generate")
        {
        T1.fill(1.);
        PrintData(T1);

        T2.fill(2.);
        PrintData(T2);
        }

    SECTION("Multiply by Real")
        {
        T1.fill(1.);
        T1 *= 3.1415926536;
        PrintData(T1);
        }

    //SECTION("Apply / Visit")
    //    {
    //    Real total = 0;
    //    //real_part function defined at top of this file:
    //    T2.visit([&total](auto x){ total += real_part(x); }); 

    //    T2.apply([](auto x){ return x*x; }); 
    //    }

    //SECTION("tieIndex")
    //    {
    //    auto tied_s1 = tieIndex(T4,s1,s2);
    //    REQUIRE(tied_s1.r() == 3);
    //    REQUIRE(hasIndex(tied_s1,l1));
    //    REQUIRE(hasIndex(tied_s1,l2));
    //    REQUIRE(hasIndex(tied_s1,s1));

    //    auto tied_s2 = tieIndex(T4,s2,s1);
    //    REQUIRE(tied_s2.r() == 3);
    //    REQUIRE(hasIndex(tied_s2,l1));
    //    REQUIRE(hasIndex(tied_s2,l2));
    //    REQUIRE(hasIndex(tied_s2,s2));
    //    }

    }
