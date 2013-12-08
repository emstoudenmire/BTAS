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

    ITensor T2(i2,l2,i1);
    T2.randomize();
    PrintDat(T2);
    T2.tieIndices(i1,i2,i1);
    PrintDat(T2);


    return 0;
    }
