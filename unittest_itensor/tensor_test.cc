#include "test.h"

#include "btas/tensor.h"
#include "btas/tensor_func.h"
#include "btas/generic/contract.h"

using btas::Range;
using btas::contract;

using DTensor = btas::Tensor<double>;
using DTensorView = btas::TensorViewOf<DTensor>;

std::ostream& 
operator<<(std::ostream& s, const DTensor& X)
    {
    for(auto i : X.range()) s << i << " " << X(i) << "\n";
    return s;
    }

#include "itensor/print.h"
using itensor::println;
using itensor::printfln;

// Set the elements of a Tensor T such that
// T(i,j,k) = 1ijk
// assuming individual dimensions are all less than 10.
// (The 1 in front is just a placeholder.)
void
fillEls(DTensor& T)
    {
    if(T.rank() == 0) return;
    const double base = pow(10.,T.rank());
    const size_t max_ii = T.rank()-1;
    for(auto I : T.range())
        {
        double &val = T(I);
        val = base;
        for(size_t ii = 0; ii <= max_ii; ++ii)
            {
            val += I[ii]*pow(10.,max_ii-ii);
            }
        }
    }


TEST_CASE("Tensor")
    {

    DTensor T2(3,2);
    fillEls(T2);

    DTensor T3(3,2,4);
    fillEls(T3);


    SECTION("Range")
        {
        Range r0;
        //println("r0.area() = ",r0.area());

        DTensor T0(r0);
        //println("T0.rank() = ",T0.rank());
        }

    SECTION("Fill")
        {
        T3.fill(1.);
        for(auto x : T3) CHECK(x == 1);
        }

    SECTION("Permute")
        {
        auto permutation = { 2, 1, 0 };

        DTensor pT3 = permute(T3,permutation);
        for(size_t i0 = 0; i0 < T3.extent(0); ++i0)
        for(size_t i1 = 0; i1 < T3.extent(1); ++i1)
        for(size_t i2 = 0; i2 < T3.extent(2); ++i2)
            {
            CHECK(pT3(i2,i1,i0) == T3(i0,i1,i2));
            }

        auto pvT3 = permute(T3,permutation);
        for(size_t i0 = 0; i0 < T3.extent(0); ++i0)
        for(size_t i1 = 0; i1 < T3.extent(1); ++i1)
        for(size_t i2 = 0; i2 < T3.extent(2); ++i2)
            {
            CHECK(pvT3(i2,i1,i0) == T3(i0,i1,i2));
            }
        }

    SECTION("Overwrite With TensorView")
        {
        const DTensor origT3(T3);

        auto permutation = { 2, 1, 0 };
        
        //This works correctly:
        DTensor temp = permute(T3,permutation);
        T3 = std::move(temp);

        //This doesn't compile, or work correctly even
        //when problematic Tensor::operator=(_Tensor&&)
        //method is commented out:
        //T3 = permute(T3,permutation);

        for(size_t i0 = 0; i0 < origT3.extent(0); ++i0)
        for(size_t i1 = 0; i1 < origT3.extent(1); ++i1)
        for(size_t i2 = 0; i2 < origT3.extent(2); ++i2)
            {
            CHECK(T3(i2,i1,i0) == origT3(i0,i1,i2));
            }
        }

    SECTION("Contract")
        {
        enum {i,j,k};

        DTensor R;
        contract(1.0,T2,{j,i},T2,{j,k},0.0,R,{i,k});
                    
        }


    }
