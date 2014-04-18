#include "itensor/itdata/realitdata.h"

#include "btas/generic/scal_impl.h"
#include "btas/generic/axpy_impl.h"
#include "btas/generic/contract.h"
#include "btas/tensor_func.h"

namespace itensor {


//
// RealITData
//

RealITData::
RealITData(const storage& t)
    :
    t_(t)
    { }

void RealITData::
contractEqImpl(Ptr& newdat,
               const RealITData* other,
               const btas::varray<size_t>& Tind,
               const btas::varray<size_t>& Oind,
               const btas::varray<size_t>& Rind)
    {
    storage res;
    btas::contract(1.,t_,Tind,other->t_,Oind,0.,res,Rind);
    t_.swap(res);
    }

void RealITData::
plusEqImpl(const RealITData* d, Ptr& newdat, Real fac)
    { 
    //axpy computes t_ += d->t_ * fac
    btas::axpy(fac,d->t_,t_);
    }

RealITData::NewPtr RealITData::
clone() const
    {
    //Should change this to make_unique<RealITData>(t_);
    //once C++14 make_unique feature is available
    return RealITData::NewPtr(new RealITData(t_));
    }

void RealITData::
fill(Real r, Ptr& newdat)
    {
    t_.fill(r);
    }

void RealITData::
generate(std::function<Real()> rfunc, Ptr& newdat)
    {
    t_.generate(rfunc);
    }

void RealITData::
mult(Real r)
    {
    btas::scal(r,t_);
    }

void RealITData::
apply(detail::MapBase* m)
    {
    for(auto& r : t_)
        {
        r = m->rmap(r);
        }
    }

void RealITData::
visit(detail::VisitBase* v) const
    {
    for(auto r : t_)
        {
        v->rvisit(r);
        }
    }

void RealITData::
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
        //if(fabs(val) > Global::printScale())
            {
            s << "  ("; 
            for(size_t i = 0, j = 1; j <= I.size(); ++i, ++j)
                {
                s << (1+I[i]);
                if(j < I.size()) s << ",";
                }
            s << ") ";

            if(fabs(val) > 1E-10)
                s << val << "\n";
            else
                s << format("%.8E\n",val);
            }
        }
    }

ITData::Range RealITData::
range() const
    {
    return t_.range();
    }

void RealITData::
applyRange(const Range& r)
    {
    //TODO: directly assign t_ after btas Tensor = TensorView assignment bug fixed
    storage temp = btas::TensorViewOf<storage>(r,t_.storage());
    t_ = std::move(temp);
    }

}; //namespace itensor
