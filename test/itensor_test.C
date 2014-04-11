#include "itensor/itensor.h"
#include "btas/btas.h"
#include "btas/tensor.h"

using namespace std;
using namespace itensor;

using DTensor = btas::Tensor<double>;

struct Square
    {
    template<class T>
    T
    operator()(T x) const { return x*x; }
    };

template <typename _Tensor>
void print(const _Tensor& X)
{
for(auto i : X.range()) 
    {
    cout << i << " " << X(i) << "\n";
    }
cout << endl;
}

int 
main(int argc, char* argv[])
    {

    Index i1("i1",2,Site),
          i2("i2",2,Site),
          l2("l2",10);

    PrintVar(i1);
    PrintVar(l2);

    ITensor T(i1,l2);
    T = randomize(T);
    PrintData(T);
    T.map(Square());
    //In C++14, can say:
    //T.map([](auto x) { return x*x; });
    PrintData(T);


    ITensor T1(i1,l2);
    T1 = randomize(T1);
    PrintData(T1);

    T *= prime(T1,l2);

    PrintData(T);

    Index jj("jj",3);
    auto rT3 = randomize(ITensor(jj));
    printn("rT3=%.10f",rT3);
    printn("norm(rT3)=%.10f",norm(rT3));


    DTensor A(2,2,2),
            B(2,2,2),
            C;

    A.fill(1);
    B.fill(1);
    cout << "A = \n"; print(A);
    cout << "B = \n"; print(B);

    cout << "dot = " << dot(A,B) << endl;

    //array<size_t,2> aa = {0,0},
    //                bb = {0,0};
    //array<size_t,0> cc;
    //contract(1,A,aa,B,bb,0,C,cc);
    enum {I,J,K,L};
    contract(1.,A,{I,J},B,{K,L},0.,C,{I,J,K,L});

    printn("C.rank() = %d",C.rank());
    cout << "*C.data() = " << *(C.data()) << endl;
    cout << "C = \n"; print(C);

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
