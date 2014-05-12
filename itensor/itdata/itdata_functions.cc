#include "itensor/itdata/itdata_functions.h"

#include "btas/generic/scal_impl.h"
#include "btas/generic/axpy_impl.h"
#include "btas/generic/contract.h"
#include "btas/tensor_func.h"

namespace itensor {

NewData Fill::
operator()(ITDense<Real>& d) const
    {
    d.t_.fill(r_);
    return NewData();
    }

NewData Fill::
operator()(ITDense<Complex>& d) const
    {
    const auto z = Complex(r_,0.);
    d.t_.fill(z);
    return NewData();
    }

NewData MultComplex::
operator()(ITDense<Real>& d) const
    {
    //TODO: would like btas::Tensor copy construct to work here
    //ITDense<Complex> nd(d);

    ITDense<Complex> nd(d.t_.range());
    std::copy(d.t_.cbegin(),d.t_.cend(),nd.t_.begin());

    btas::scal(z_,nd.t_);

    return NewData(new ITDense<Complex>(std::move(nd)));
    }

NewData MultComplex::
operator()(ITDense<Complex>& d) const
    {
    btas::scal(z_,d.t_);
    return NewData();
    }

NewData MultReal::
operator()(ITDense<Real>& d) const
    {
    btas::scal(r_,d.t_);
    return NewData();
    }

NewData MultReal::
operator()(ITDense<Complex>& d) const
    {
    btas::scal(Complex(r_,0),d.t_);
    return NewData();
    }

void PrintIT::
operator()(const ITDense<Real>& d) const
    {
    s_ << "}\n";
    Real scalefac = 1.0;
    if(!x_.isTooBigForReal()) scalefac = x_.real0();
    else s_ << "  (omitting too large scale factor)\n";

    const auto Tr = d.t_.range();

    const auto rr = Tr.rank();

    if(rr == 0) return;

    for(const auto I : Tr)
        {
        const Real val = d.t_(I)*scalefac;
        //if(fabs(val) > Global::printScale())
            {
            s_ << "  ("; 
            for(size_t i = 0, j = 1; j <= I.size(); ++i, ++j)
                {
                s_ << (1+I[i]);
                if(j < I.size()) s_ << ",";
                }
            s_ << ") ";

            if(fabs(val) > 1E-10)
                s_ << val << "\n";
            else
                s_ << format("%.8E\n",val);
            }
        }
    }

void PrintIT::
operator()(const ITDense<Complex>& d) const
    {
    s_ << "}\n";
    Real scalefac = 1.0;
    if(!x_.isTooBigForReal()) scalefac = x_.real0();
    else s_ << "  (omitting too large scale factor)\n";

    const auto Tr = d.t_.range();

    const auto rr = Tr.rank();

    if(rr == 0) return;

    for(const auto I : Tr)
        {
        const auto val = d.t_(I)*scalefac;
        //if(fabs(val) > Global::printScale())
            {
            s_ << "  ("; 
            for(size_t i = 0, j = 1; j <= I.size(); ++i, ++j)
                {
                s_ << (1+I[i]);
                if(j < I.size()) s_ << ",";
                }
            s_ << ") ";

            if(std::norm(val) > 1E-10)
                {
                const auto sgn = (val.imag() < 0 ? '-' : '+');
                s_ << format("%.10f%s%.10fi\n",val.real(),sgn,val.imag());
                }
            else
                {
                s_ << format("%.8E\n",val);
                }
            }
        }
    }

//void RealITData::
//contractEqImpl(Ptr& newdat,
//               const RealITData* other,
//               const btas::varray<size_t>& Tind,
//               const btas::varray<size_t>& Oind,
//               const btas::varray<size_t>& Rind)
//    {
//    storage res;
//    btas::contract(1.,t_,Tind,other->t_,Oind,0.,res,Rind);
//    t_.swap(res);
//    }
//
//void RealITData::
//plusEqImpl(const RealITData* d, Ptr& newdat, Real fac)
//    { 
//    //axpy computes t_ += d->t_ * fac
//    btas::axpy(fac,d->t_,t_);
//    }
//
//RealITData::NewPtr RealITData::
//clone() const
//    {
//    //Should change this to make_unique<RealITData>(t_);
//    //once C++14 make_unique feature is available
//    return RealITData::NewPtr(new RealITData(t_));
//    }
//
//void RealITData::
//fill(Real r, Ptr& newdat)
//    {
//    t_.fill(r);
//    }
//
//void RealITData::
//generate(std::function<Real()> rfunc, Ptr& newdat)
//    {
//    t_.generate(rfunc);
//    }
//
//void RealITData::
//apply(detail::MapBase* m)
//    {
//    m->rmap(t_.data(),t_.data()+t_.size());
//    }
//
//void RealITData::
//visit(detail::VisitBase* v) const
//    {
//    v->rvisit(t_.data(),t_.data()+t_.size());
//    }
//
//
//ITData::Range RealITData::
//range() const
//    {
//    return t_.range();
//    }
//
//void RealITData::
//applyRange(const Range& r)
//    {
//    //TODO: directly assign t_ after btas Tensor = TensorView assignment bug fixed
//    storage temp = btas::TensorViewOf<storage>(r,t_.storage());
//    t_ = std::move(temp);
//    }

}; //namespace itensor
