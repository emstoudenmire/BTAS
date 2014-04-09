#include "itensor/itensor.h"

using namespace std;
using namespace itensor;

int 
main(int argc, char* argv[])
    {

    Index i1("i1",2,Site),
          i2("i2",2,Site),
          l2("l2",10);

    PrintVar(i1);
    PrintVar(l2);

    ITensor T(i1,l2);
    T.fill(3.14);
    T *= 5;
    //T.randomize();
    PrintData(T);

    ITensor T1(i1,l2);
    T1.fill(-3.14);
    T1 *= 5;
    //T1.randomize();
    PrintData(T1);

    //T += T1; //this works ok
    auto R = T - T1;
    PrintData(R);


    exit(0);



    //
    // Element access
    //

    //PrintVar(T(i1(1),l2(1)));
    //PrintVar(T(i1(1),l2(2)));
    //PrintVar(T(i1(1),l2(3)));
    //PrintVar(T(i1(1),l2(4)));

    //PrintVar(T(i1(2),l2(5)));
    //PrintVar(T(l2(5),i1(2)));

    ITensor T3(i1,i2,l2);
    //T3(i2(1),i1(1),l2(4)) = -9.92737;
    //T3(i2(2),i1(1),l2(4)) = -2.83781;
    T3 *= 1.2;
    PrintData(T3);

    //
    // tieIndices
    //

    //ITensor T2(i2,l2,i1);
    //T2.randomize();
    //PrintData(T2);
    //T2.tieIndices(i1,i2,i1);
    //PrintData(T2);

    //
    // Addition
    //

    ITensor T4(i1,i2),
            T5(i1,i2);
    //T4.randomize();
    //T5.randomize();
    T4 *= 0.2;
    T5 *= -1.4;

    auto T6 = T4;
    T6 += T5;

    //for(int j1 = 1; j1 <= i1.m(); ++j1)
    //for(int j2 = 1; j2 <= i2.m(); ++j2)
    //    {
    //    const auto sum = T4(i1(j1),i2(j2)) + T5(i1(j1),i2(j2));
    //    if(fabs(sum-T6(i1(j1),i2(j2))) > 1E-12)
    //        {
    //        printn("Error: (T6(%d,%d) == %.5f) != %.5f",j1,j2,T6(i1(j1),i2(j2)),sum);
    //        }
    //    }

    return 0;
    }
