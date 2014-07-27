#include "test.h"

#include "itensor/itensor.h"

using namespace itensor;
using detail::contains;

Real
real_part(Real r) { return r; }
Real
real_part(Complex z) { return z.real(); }

struct Square
    {
    template <typename T>
    T
    operator()(T x) const { return x*x; }
    };

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
        REQUIRE(!T0);
        REQUIRE(T0.r() == 0);

        REQUIRE(T1);
        REQUIRE(T1.r() == 1);

        REQUIRE(T2);
        REQUIRE(T2.r() == 2);

        REQUIRE(T3);
        REQUIRE(T3.r() == 3);

        REQUIRE(T4);
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

    SECTION("Multiply by Complex")
        {
        T1.fill(2.);
        T1 *= Complex(1,1);
        PrintData(T1);
        }

    SECTION("Apply / Visit")
        {
        T2.apply([](auto x){ return x*x; }); 
        T2.apply(Square());
        PrintData(T2);

        Real total = 0;
        //real_part function defined at top of this file:
        T2.visit([&total](auto x){ total += real_part(x); }); 
        printfln("total = %.20f",total);
        }

    SECTION("tieIndex")
        {
        auto tied_s1 = tieIndex(T4,s1,s2);
        REQUIRE(tied_s1.r() == 3);
        REQUIRE(hasIndex(tied_s1,l1));
        REQUIRE(hasIndex(tied_s1,l2));
        REQUIRE(hasIndex(tied_s1,s1));

        auto tied_s2 = tieIndex(T4,s2,s1);
        REQUIRE(tied_s2.r() == 3);
        REQUIRE(hasIndex(tied_s2,l1));
        REQUIRE(hasIndex(tied_s2,l2));
        REQUIRE(hasIndex(tied_s2,s2));
        }

    SECTION("Add")
        {
        printfln("T2 = %f",T2);
        auto Radd = T2 + T2;
        printfln("Radd = %f",Radd);
        Radd += T2;
        printfln("Radd = %f",Radd);
        printfln("T2 = %f",T2);
        }

    SECTION("Contract")
        {
        auto R = T3 * T2;
        printfln("R = %f",R);
        }

    SECTION("Elements")
        {
        PrintData(T2);
        Elements<Real> eT2a(T2,l1,l2);
        PrintVar(eT2a);
        Elements<Real> eT2b(T2,l2,l1);
        PrintVar(eT2b);
        PrintVar(eT2b(0,0));
        PrintVar(eT2b(0,1));
        PrintVar(eT2b(3,1));
        PrintVar(eT2a(1,3));

        CHECK( eT2a(1,2) == eT2b(2,1) );
        }

    }
