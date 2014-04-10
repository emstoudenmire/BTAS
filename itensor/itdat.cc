#include "itensor/itdat.h"
#include "btas/generic/scal_impl.h"
#include "btas/generic/axpy_impl.h"

using std::make_shared;

namespace itensor {

//
// ITDat
//

void ITDat::
plusEq(const CPtr& other, Ptr& newptr, Real fac) { other->addTo(this,newptr,fac); }


//
// RealITDat
//

RealITDat::
RealITDat(const storage& t)
    :
    t_(t)
    { }

void RealITDat::
plusEqImpl(const RealITDat* d, Ptr& newdat, Real fac)
    { 
    //axpy computes t_ += d->t_ * fac
    btas::axpy(fac,d->t_,t_);
    }

RealITDat::NewPtr RealITDat::
clone() const
    {
    //Should change this to make_unique<RealITDat>(t_);
    //once C++14 make_unique feature is available
    return RealITDat::NewPtr(new RealITDat(t_));
    }

void RealITDat::
fill(Real r, Ptr& newdat)
    {
    t_.fill(r);
    }

void RealITDat::
generate(std::function<Real()> rfunc, Ptr& newdat)
    {
    t_.generate(rfunc);
    }

void RealITDat::
mult(Real r)
    {
    btas::scal(r,t_);
    }

void RealITDat::
map(detail::MapBase* m)
    {
    for(auto& r : t_)
        {
        r = m->rmap(r);
        }
    }

void RealITDat::
print(std::ostream& s, const LogNumber& x) const
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
