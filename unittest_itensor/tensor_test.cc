#include "test.h"

#include "btas/tensor.h"
#include "btas/tensor_func.h"

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

    DTensor T(3,2,4);
    fillEls(T);

    SECTION("Fill")
        {
        T.fill(1.);
        for(auto x : T) CHECK(x == 1);
        }

    SECTION("Overwrite With TensorView")
        {
        auto permutation = { 2, 1, 0 };
        DTensor temp = permute(T,permutation);
        T = std::move(temp);

        //T = permute(T,permutation);
        println("T = \n",T);
        }



    }
