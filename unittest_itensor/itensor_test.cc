#include "test.h"

#include "itensor/itensor.h"

using namespace itensor;
using detail::contains;

const static Real Epsilon = 1E-10;

Real static
rng()
    {
    static auto rng = std::mt19937(std::time(NULL));
    static auto dist = std::uniform_real_distribution<Real>{0., 1.};
    return dist(rng);
    }

Real static
realPart(Real r) { return r; }
Real static
realPart(Complex z) { return z.real(); }

//Function object returning the square of its argument
//Used in a test below
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

    auto Tn = ITensor();
    auto T0 = ITensor(4.0);
    //Calling randIT(inds...) returns a random ITensor(inds...)
    auto T1 = randIT(l1);
    auto T2 = randIT(l1,l2);
    auto T3 = randIT(l1,l2,s1);
    auto T4 = randIT(l1,l2,s1,s2);

    SECTION("Scalar Constructors and Element Access")
        {
        Real r1 = 0.204;
        ITensor Tr(r1);
        auto x = Tr.real();
        CHECK(fabs(r1-x) < Epsilon);
        Real r2 = -8.43;
        Tr.set(r2);
        x = Tr.real();
        CHECK(fabs(r2-x) < Epsilon);

        Complex z1 = 4+3_i;
        ITensor Tc(z1);
        auto w = Tc.cplx();
        CHECK(std::norm(w-z1) < Epsilon);
        Complex z2 = 8-2_i;
        Tc.set(z2);
        w = Tc.cplx();
        CHECK(std::norm(w-z2) < Epsilon);
        }

    SECTION("Basic Accessors")
        {
        //Default constructed ITensor evaluates to false in boolean context
        CHECK(!Tn);
        CHECK(Tn.r() == 0);

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
                CHECK_CLOSE(x,1.,Epsilon);
                });

        CHECK( !isComplex(T1) );

        const auto z = 1+2_i;
        T2.fill(z);

        CHECK( isComplex(T2) );

        T2.visit([z](auto x) 
                { 
                CHECK_CLOSE(x,z,Epsilon);
                });

        //Scalar ITensor
        T0.fill(2.);
        CHECK_CLOSE(T0.real(), 2., Epsilon);

        T0.fill(2.+2._i);
        CHECK_CLOSE(T0.cplx(), 2.+2._i, Epsilon);
        }
        

    SECTION("Multiply by Real")
        {
        T1.fill(1.);
        const auto fac = 3.1415926536;
        T1 *= fac;
        T1.visit([fac](auto x) 
                { 
                CHECK_CLOSE(x,fac,Epsilon);
                });
        }

    SECTION("Multiply by Complex")
        {
        T1.fill(2.);
        const auto fac = Complex(1,1);
        T1 *= Complex(1,1);
        T1.visit([fac](auto x) 
                { 
                CHECK_CLOSE(x,2*fac,Epsilon);
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
                CHECK_CLOSE(x,val2,Epsilon);
                });

        T2.fill(val);
        T2.apply(Square());
        T2.visit([val2](auto x) 
                { 
                CHECK_CLOSE(x,val2,Epsilon);
                });

        //Can use visit to sum all elements, for example
        T2.fill(1.);
        Real total = 0;
        T2.visit([&total](auto x){ total += realPart(x); }); 
        CHECK_CLOSE(total,T2.inds().dim(),Epsilon);
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

        CHECK_CLOSE(total1,total3,Epsilon);
        CHECK_CLOSE(total2,total3,Epsilon);

        //Check .set method with Real argument
        const auto val1 = rng();
        T2.set(val1,l2(2),l1(1));
        CHECK_CLOSE(T2.real(l2(2),l1(1)), val1, Epsilon);

        //Check .set method with Complex argument
        const auto val2 = Complex(rng(),rng());
        T2.set(val2,l2(2),l1(1));
        CHECK( isComplex(T2) );
        CHECK_CLOSE(T2.cplx(l2(2),l1(1)), val2, Epsilon);
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
        //
        // Rank 2, same index order
        //
        const auto Radd = fac1*T2 + fac2*T2;
        for(int i1 = 1; i1 <= l1.m(); ++i1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
            {
            CHECK_CLOSE( Radd.real(l1(i1),l2(i2)) , (fac1+fac2)*T2.real(l1(i1),l2(i2)) , Epsilon);
            }

        //
        // Rank 2, different index order
        //
        auto T2p = randIT(l2,l1);
        const auto Radd2 = T2+T2p;
        for(int i1 = 1; i1 <= l1.m(); ++i1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
            {
            CHECK_CLOSE( Radd2.real(l1(i1),l2(i2)) , T2.real(l1(i1),l2(i2))+T2p.real(l1(i1),l2(i2)) , Epsilon);
            }

        //
        // Rank 3, different index order
        //
        auto T3p = randIT(s1,l1,l2);
        const auto Radd3 = fac1*T3+fac2*T3p;
        for(int j1 = 1; j1 <= s1.m(); ++j1)
        for(int i1 = 1; i1 <= l1.m(); ++i1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
            {
            CHECK_CLOSE( Radd3.real(s1(j1),l1(i1),l2(i2)) , fac1*T3.real(s1(j1),l1(i1),l2(i2))+fac2*T3p.real(s1(j1),l1(i1),l2(i2)) , Epsilon);
            }

        //
        // Scalar (rank 0)
        //
        ITensor T0a(1.);
        T0a = randIT(T0a);
        ITensor T0b(1.);
        T0a = randIT(T0b);
        auto Radd4 = fac1*T0a + fac2*T0b;
        CHECK_CLOSE( Radd4.real() , fac1*T0a.real() + fac2*T0b.real() , Epsilon );

        ITensor T0c(1.0+1.0_i);
        auto Radd5 = fac1*T0a + fac2*T0c;
        CHECK_CLOSE( Radd5.cplx() , fac1*T0a.cplx() + fac2*T0c.cplx() , Epsilon );
        }

    SECTION("Contract")
        {
        const auto fac1 = rng(),
                   fac2 = rng(),
                   fac3 = rng();
        T3 *= fac1;
        //Automatically contracts over common indices l1, l2
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
            CHECK_CLOSE( val, R1.real(s1(j1)) , Epsilon);
            }

        //Automatically contracts over common index l1
        //(Index l2' of prime(T2,l2) does not match Index l2 of T3)
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
            CHECK_CLOSE( val , R2.real(s1(j1),l2(i2),prime(l2)(i2p)) , Epsilon );
            }


        //
        // Contract scalar tensor with non-scalar
        //
        const auto R3 = fac2 * T3 * T0;
        auto T0real = T0.real();
        for(int j1 = 1; j1 <= s1.m(); ++j1)
        for(int i1 = 1; i1 <= l1.m(); ++i1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
            {
            CHECK_CLOSE( R3.real(l1(i1),l2(i2),s1(j1)), fac2*T3.real(l1(i1),l2(i2),s1(j1))*T0real , Epsilon );
            }

        T0.fill(1.+1._i);
        const auto R4 = fac2 * T0 * T3;
        auto T0cplx = T0.cplx();
        for(int j1 = 1; j1 <= s1.m(); ++j1)
        for(int i1 = 1; i1 <= l1.m(); ++i1)
        for(int i2 = 1; i2 <= l2.m(); ++i2)
            {
            CHECK_CLOSE( R4.cplx(l1(i1),l2(i2),s1(j1)), fac2*T0cplx*T3.real(l1(i1),l2(i2),s1(j1)), Epsilon );
            }

        //
        // Contract scalar ITensor with scalar ITensor
        //
        auto T0a(T0);
        auto T0b(T0);

        T0a.fill(fac1);
        T0b.fill(fac2);
        auto R5 = fac3*T0a*T0b;
        CHECK_CLOSE(R5.real(), fac3*T0a.real()*T0b.real(), Epsilon);

        T0a.fill(fac1*(1.+1._i));
        T0b.fill(fac2);
        auto R6 = fac3*T0a*T0b;
        CHECK_CLOSE(R6.cplx(), fac3*T0a.cplx()*T0b.cplx(), Epsilon);

        T0a.fill(fac1*(1.+1._i));
        T0b.fill(fac2*(-2.+3._i));
        auto R7 = fac3*T0a*T0b;
        CHECK_CLOSE(R7.cplx(), fac3*T0a.cplx()*T0b.cplx(), Epsilon);
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
