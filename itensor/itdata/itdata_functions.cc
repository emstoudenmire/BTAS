#include "itensor/itdata/itdata_functions.h"

#include "btas/generic/scal_impl.h"
#include "btas/generic/axpy_impl.h"
#include "btas/tensor_func.h"

namespace itensor {

NewData FillReal::
operator()(ITDense<Real>& d) const
    {
    d.t_.fill(r_);
    return NewData();
    }

NewData FillReal::
operator()(const ITDense<Complex>& d) const
    {
    auto nd = new ITDense<Real>(d.t_.range());
    nd->t_.fill(r_);
    return NewData(nd);
    }

NewData FillCplx::
operator()(const ITDense<Real>& d) const
    {
    auto nd = new ITDense<Complex>(d.t_.range());
    nd->t_.fill(z_);
    return NewData(nd);
    }

NewData FillCplx::
operator()(ITDense<Complex>& d) const
    {
    d.t_.fill(z_);
    return NewData();
    }

NewData MultComplex::
operator()(const ITDense<Real>& d) const
    {
    auto nd = new ITDense<Complex>(d);
    btas::scal(z_,nd->t_);
    return NewData(nd);
    }

NewData MultComplex::
operator()(ITDense<Complex>& d) const
    {
    btas::scal(z_,d.t_);
    return NewData();
    }

NewData MultComplex::
operator()(const ITScalar<Real>& d) const
    {
    auto nd = new ITScalar<Complex>(z_*d.x_);
    return NewData(nd);
    }

NewData MultComplex::
operator()(ITScalar<Complex>& d) const
    {
    d.x_ *= z_;
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
                s_ << val.real() << sgn << fabs(val.imag()) << "_i\n";
                }
            else
                {
                s_ << format("%.8E\n",val);
                }
            }
        }
    }

void PrintIT::
operator()(const ITScalar<Real>& d) const
    {
    s_ << "}\n";
    Real scalefac = 1.0;
    if(!x_.isTooBigForReal()) scalefac = x_.real0();
    else s_ << "  (omitting too large scale factor)\n";

    auto val = d.x_*scalefac;
    if(fabs(val) > 1E-10)
        s_ << "  " << val << "\n";
    else
        s_ << format("  %.8E\n",val);
    }

void PrintIT::
operator()(const ITScalar<Complex>& d) const
    {
    s_ << "}\n";
    Real scalefac = 1.0;
    if(!x_.isTooBigForReal()) scalefac = x_.real0();
    else s_ << "  (omitting too large scale factor)\n";

    auto val = d.x_*scalefac;
    auto sgn = (val.imag() < 0 ? '-' : '+');
    if(std::norm(val) > 1E-10)
        {
        s_ << "  " << val.real() << sgn << fabs(val.imag()) << "_i\n";
        }
    else
        {
        s_ << format("  %.8E%s%.8E_i\n",val.real(),sgn,fabs(val.imag()));
        }
    }


}; //namespace itensor
