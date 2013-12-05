#include <iostream>
#include <iomanip>
#include "btas/slice.h"

using namespace btas;
using namespace std;

#define Print(X) cout << #X << " =\n" << X << endl;

typedef Tensor<double> DTensor;

typedef NDIterator<double*,typename DTensor::shape_type> 
NDIter;

typedef NDIterator<const double*,typename DTensor::shape_type> 
cNDIter;

ostream& 
operator<<(ostream& s, const DTensor& T)
    {
    cNDIter it(T.data(),T.shape(),T.stride());
    for(; it.valid(); ++it)
        {
        s << "  (";
        for(size_t n = 0; n < T.rank()-1; ++n)
            {
            s << it.index(n) << ",";
            }
        s << it.index(T.rank()-1);
        s << ") " <<  *it << "\n";
        }
    return s;
    }

ostream&
operator<<(ostream& s, const typename DTensor::shape_type& S)
    {
    btas::detail::print_array(s,S); 
    return s;
    }

int 
main()
    {
    DTensor T(3,3);
    T(0,0) = 900;
    T(0,1) = 901;
    T(0,2) = 902;
    T(1,0) = 910;
    T(1,1) = 911;
    T(1,2) = 912;
    T(2,0) = 920;
    T(2,1) = 921;
    T(2,2) = 922;

    Print(T);
    
    //
    // Conversions of TensorRef to Tensor
    //

    DTensor sT1; 
    sT1 = slice(T,{0,3,2},3);

    DTensor sT2 = slice(T,{0,3,2},3);

    DTensor sT3(slice(T,{0,3,2},3));

    //
    // Various slice examples
    // 

    cout << "-------------------------------------" << endl;
    cout << "Slicing examples" << endl;
    cout << "-------------------------------------" << endl;


    Print(slice(T,{0,3,2},3));
    Print(slice(T,{1,3,2},3));
    Print(slice(T,{2,3,2},3));
    Print(slice(T,{1,2,2},3));
    Print(slice(T,3,{0,3,2}));
    Print(slice(T,3,{1,3,2}));
    Print(slice(T,3,{1,2,2}));
    Print(slice(T,{1,2,1},{1,2,1}));
    Print(slice(T,{0,2,1},{0,2,1}));
    Print(slice(T,{},{0,2,1}));
    Print(slice(T,{1,3,1},{}));

    //
    // Assignment of slice to slice
    //

    cout << "-------------------------------------" << endl;
    cout << "Slice assignment" << endl;
    cout << "-------------------------------------" << endl;

    {
    auto T1 = T;
    Print(T1);
    Print(slice(T1,{2,3},3));
    Print(slice(T1,3,{1,2}));
    slice(T1,{2,3},3) = slice(T1,3,{1,2});
    cout << "Result of slice(T1,{2,3},3) = slice(T1,3,{1,2});" << endl;
    cout << T1 << endl;
    }

    cout << "-------------------------------------" << endl;

    {
    auto T2 = T;
    Print(T2);
    Print(slice(T2,3,{1,2})); 
    Print(slice(T2,{2,3},3));
    slice(T2,3,{1,2}) =  slice(T2,{2,3},3);
    cout << "Result of slice(T2,3,{1,2}) =  slice(T2,{2,3},3);" << endl;
    cout << T2 << endl;
    }
    
    DTensor F(2,3,3);
    F(0,0,0) = 9000;
    F(0,0,1) = 9001;
    F(0,0,2) = 9002;
    F(0,1,0) = 9010;
    F(0,1,1) = 9011;
    F(0,1,2) = 9012;
    F(0,2,0) = 9020;
    F(0,2,1) = 9021;
    F(0,2,2) = 9022;
    F(1,0,0) = 9100;
    F(1,0,1) = 9101;
    F(1,0,2) = 9102;
    F(1,1,0) = 9110;
    F(1,1,1) = 9111;
    F(1,1,2) = 9112;
    F(1,2,0) = 9120;
    F(1,2,1) = 9121;
    F(1,2,2) = 9122;

    cout << "-------------------------------------" << endl;

    Print(F);

    cout << "-------------------------------------" << endl;
    cout << "Slice of rank 3 tensor" << endl;
    cout << "-------------------------------------" << endl;

    Print(slice(F,2,{0,3,2},3));

    cout << "-------------------------------------" << endl;
    cout << "diag function (returns TensorRef to diagonal)" << endl;
    cout << "-------------------------------------" << endl;

    Print(diag(T));
    Print(diag(F));

    cout << "-------------------------------------" << endl;
    cout << "tie function" << endl;
    cout << "-------------------------------------" << endl;

    Print(tie(F,0,2));
    Print(tie(F,0,1));
    Print(tie(F,1,2));
    Print(tie(F,0,1,2));

    cout << "-------------------------------------" << endl;
    cout << "Composing methods that produce TensorRefs" << endl;
    cout << "-------------------------------------" << endl;

    Print(diag(tie(F,0,1)));
    Print(diag(tie(F,1,2)));
    Print(diag(tie(F,0,1,2)));

    Print(slice(F,{},{0,3,2},{}));
    Print(diag(slice(F,{},{0,3,2},{})));

    Print(diag(tie(T,0,1)));

    Print(diag(diag(T)));

    return 0;
    }
