#include "itensor/itdat.h"
#include "btas/generic/scal_impl.h"
#include "btas/generic/axpy_impl.h"

using std::make_shared;

namespace itensor {

//
// ITDat
//

ITDat& ITDat::
plusEq(const CPtr& other, Real fac) { other->addTo_(this,fac); return *this; }

ITDat& ITDat::
mult(Real r) { this->mult_(r); return *this; }

ITDat::NewPtr ITDat::
clone() const { return clone_(); }

void ITDat::
fill(Real r) { fill_(r); }

void ITDat::
print(std::ostream& s, const LogNumber& x) const { print_(s,x); }


//
// RealITDat
//

RealITDat::
RealITDat(const storage& t)
    :
    t_(t)
    { }

void RealITDat::
plusEq_(const RealITDat* d, Real fac)
    { 
    //axpy computes t_ += d->t_ * fac
    btas::axpy(fac,d->t_,t_);
    }

RealITDat::NewPtr RealITDat::
clone_() const
    {
    //Should change this to make_unique<RealITDat>(t_);
    //once C++14 make_unique feature is available
    return RealITDat::NewPtr(new RealITDat(t_));
    }

void RealITDat::
fill_(Real r)
    {
    t_.fill(r);
    }

void RealITDat::
mult_(Real r)
    {
    btas::scal(r,t_);
    }

void RealITDat::
print_(std::ostream& s, const LogNumber& x) const
    {
    s << "}\n";
    Real scalefac = 1.0;
    if(!x.isTooBigForReal()) scalefac = x.real0();
    else s << "  (omitting too large scale factor)\n";

    const auto Tr = t_.range();

    const auto rr = Tr.rank();

    if(rr == 0) return;

    for(const auto I : Tr)
        {
        const Real val = t_(I)*scalefac;
        if(fabs(val) > Global::printScale())
            {
            s << "  ("; 
            for(size_t i = 0, j = 1; j <= I.size(); ++i, ++j)
                {
                s << (1+I[i]);
                if(j < I.size()) s << ",";
                }
            s << ") ";
            s << format(fabs(val) > 1E-10 ? "%.12f\n" : "%.8E\n",val);
            }
        }
    }

}; //namespace itensor
