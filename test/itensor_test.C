#include "itensor/itensor.h"

using boost::format;
using namespace std;
using namespace itensor;

int 
main(int argc, char* argv[])
    {

    Index i1("i1",2,Site),
          i2("i2",2,Site),
          l2("l2",10);

    Print(i1);
    Print(l2);

    ITensor T(i1,l2);
    T.randomize();

    ITensor T1(l2);
    T1.randomize();

    PrintDat(T1);

    PrintDat(T);

    Print(T(i1(1),l2(1)));
    Print(T(i1(1),l2(2)));
    Print(T(i1(1),l2(3)));
    Print(T(i1(1),l2(4)));

    Print(T(i1(2),l2(5)));
    Print(T(l2(5),i1(2)));

    //
    // tieIndices
    //

    ITensor T2(i2,l2,i1);
    T2.randomize();
    PrintDat(T2);
    T2.tieIndices(i1,i2,i1);
    PrintDat(T2);

    //
    // Addition
    //

    ITensor T3(i1,i2),
            T4(i1,i2);
    T3.randomize();
    T4.randomize();
    T3 *= 0.2;
    T4 *= -1.4;

    auto T5 = T3;
    T5 += T4;

    for(int j1 = 1; j1 <= i1.m(); ++j1)
    for(int j2 = 1; j2 <= i2.m(); ++j2)
        {
        const auto sum = T3(i1(j1),i2(j2)) + T4(i1(j1),i2(j2));
        if(fabs(sum-T5(i1(j1),i2(j2))) > 1E-12)
            {
            cout << format("Error: (T5(%d,%d) == %.5f) != %.5f")
                    % j1 % j2
                    % T5(i1(j1),i2(j2))
                    % sum
                 << endl;
            }
        }

    return 0;
    }
