#include "test.h"

#include "itensor/itensor.h"

using namespace itensor;
using detail::contains;

Real static
rng()
    {
    static std::mt19937 rng(std::time(NULL));
    static auto dist = std::uniform_real_distribution<Real>{0., 1.};
    return dist(rng);
    }

Real static
realPart(Real r) { return r; }
Real static
realPart(Complex z) { return z.real(); }

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
    auto T1 = random(l1);
    auto T2 = random(l1,l2);
    auto T3 = random(l1,l2,s1);
    auto T4 = random(l1,l2,s1,s2);

    SECTION("Basic Accessors")
        {
        //Default constructed ITensor evaluates to false in boolean context
        CHECK(!T0);
        CHECK(T0.r() == 0);

        //Non-default constructed ITensor evaluates to true in boolean context
        CHECK(T1);
        CHECK(T1.r() == 1);

        CHECK(T2);
        CHECK(T2.r() == 2);

        CHECK(T3);
        CHECK(T3.r() == 3);

        CHECK(T4);
        CHECK(T4.r() == 4);

        //Checking "inds()" accessor
        for(const auto& i : T4.inds())
            {
            CHECK( hasIndex(T4,i) );
            }
        }

    SECTION("Fill and Generate")
        {
        //Set all elements of an ITensor to same value
        T1.fill(1.);

        //.visit applies non-modifying function object / lambda to each element
        T1.visit([](auto x) 
                { 
                CHECK_CLOSE(x,1.,1E-10);
                });

        CHECK( !isComplex(T1) );

        const auto z = Complex(1,2);
        T2.fill(z);

        CHECK( isComplex(T2) );

        T2.visit([&z](auto x) 
                { 
                CHECK( abs(Complex(x)-z) < 1E-10 ); 
                });
        }
        

    SECTION("Multiply by Real")
        {
        T1.fill(1.);
        const auto fac = 3.1415926536;
        T1 *= fac;
        T1.visit([fac](auto x) 
                { 
                CHECK_CLOSE(x,fac,1E-10);
                });
        }

    SECTION("Multiply by Complex")
        {
        T1.fill(2.);
        const auto fac = Complex(1,1);
        T1 *= Complex(1,1);
        T1.visit([fac](auto x) 
                { 
                CHECK( abs(Complex(x)-2.*fac) < 1E-10 ); 
                });
        }

    SECTION("Apply / Visit")
        {
        const auto val = 3.;
        const auto val2 = sqr(val);
        T2.fill(val);
        T2.apply([](auto x){ return x*x; }); 
        T2.visit([val2](auto x) 
                { 
                CHECK_CLOSE(x,val2,1E-10);
                });

        T2.fill(val);
        T2.apply(Square());
        T2.visit([val2](auto x) 
                { 
                CHECK_CLOSE(x,val2,1E-10);
                });

        //Can use visit to sum all elements, for example
        T2.fill(1.);
        Real total = 0;
        T2.visit([&total](auto x){ total += realPart(x); }); 
        CHECK_CLOSE(total,T2.inds().dim(),1E-10);
        }

    SECTION("Element Accessors")
        {
        Real total1 = 0.,
             total2 = 0.,
             total3 = 0.;
        for(int i1 = 1; i1 <= l1.m(); ++i1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
            {
            //Interface is independent of Index order,
            //which is why one has to specify Index names
            //to access elements
            total1 += T2.real(l1(i1),l2(i2));
            total2 += T2.real(l2(i2),l1(i1));
            }
        T2.visit([&total3](auto x) { total3 += realPart(x); } );

        CHECK_CLOSE(total1,total3,1E-10);
        CHECK_CLOSE(total2,total3,1E-10);

        //Check .set method with Real argument
        const auto val1 = rng();
        T2.set(val1,l2(2),l1(1));
        CHECK_CLOSE(T2.real(l2(2),l1(1)), val1, 1E-10);

        //Check .set method with Complex argument
        const auto val2 = Complex(rng(),rng());
        T2.set(val2,l2(2),l1(1));
        CHECK( isComplex(T2) );
        CHECK_CLOSE(T2.cplx(l2(2),l1(1)), val2, 1E-10);
        }

    SECTION("tieIndex")
        {
        auto tied_s1 = tieIndex(T4,s1,s2);
        CHECK(tied_s1.r() == 3);
        CHECK(hasIndex(tied_s1,l1));
        CHECK(hasIndex(tied_s1,l2));
        CHECK(hasIndex(tied_s1,s1));

        auto tied_s2 = tieIndex(T4,s2,s1);
        CHECK(tied_s2.r() == 3);
        CHECK(hasIndex(tied_s2,l1));
        CHECK(hasIndex(tied_s2,l2));
        CHECK(hasIndex(tied_s2,s2));
        }

    SECTION("Add")
        {
        const auto fac1 = rng(),
                   fac2 = rng();
        const auto Radd = fac1*T2 + fac2*T2;
        for(int i1 = 1; i1 <= l1.m(); ++i1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
            {
            CHECK( fabs( Radd.real(l1(i1),l2(i2)) - (fac1+fac2)*T2.real(l1(i1),l2(i2)) ) < 1E-10 );
            }
        }

    SECTION("Contract")
        {
        const auto fac1 = rng();
        T3 *= fac1;
        const auto R1 = T3 * T2;

        CHECK( hasIndex(R1,s1) );

        for(int j1 = 1; j1 <= s1.m(); ++j1)
            {
            Real val = 0;
            for(int i1 = 1; i1 <= l1.m(); ++i1)
            for(int i2 = 1; i2 <= l2.m(); ++i2)
                {
                val += T3.real(l1(i1),l2(i2),s1(j1)) * T2.real(l1(i1),l2(i2));
                }
            CHECK( fabs( val - R1.real(s1(j1)) ) < 1E-10 );
            }

        const auto fac2 = rng();
        const auto R2 = fac2 * T3 * prime(T2,l2);

        CHECK( hasIndex(R2,s1) );
        CHECK( hasIndex(R2,l2) );
        CHECK( hasIndex(R2,prime(l2)) );

        for(int j1 = 1; j1 <= s1.m(); ++j1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
        for(int i2p = 1; i2p <= l2.m(); ++i2p)
            {
            Real val = 0;
            for(int i1 = 1; i1 <= l1.m(); ++i1)
                {
                val += fac2 * T3.real(l1(i1),l2(i2),s1(j1)) * T2.real(l1(i1),l2(i2p));
                }
            CHECK( fabs( val - R2.real(s1(j1),l2(i2),prime(l2)(i2p)) ) < 1E-10 );
            }

        }

    //SECTION("Elements")
    //    {
    //    PrintData(T2);
    //    Elements<Real> eT2a(T2,l1,l2);
    //    PrintVar(eT2a);
    //    Elements<Real> eT2b(T2,l2,l1);
    //    PrintVar(eT2b);
    //    PrintVar(eT2b(0,0));
    //    PrintVar(eT2b(0,1));
    //    PrintVar(eT2b(3,1));
    //    PrintVar(eT2a(1,3));

    //    CHECK( eT2a(1,2) == eT2b(2,1) );
    //    }

    }
