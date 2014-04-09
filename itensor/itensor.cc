//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#include "itensor.h"

#include <functional>
#include <random>
#include "btas/generic/scal_impl.h"
#include "btas/generic/axpy_impl.h"

using std::array;
using std::ostream;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::cout;
using std::endl;

namespace itensor {

//
// ITensor
//

//
// ITensor Constructors
//

ITensor::
ITensor()  
    : 
    scale_(1)
    { }


ITensor::
ITensor(Real val) 
    :
    scale_(1)
    { 
    }

ITensor::
ITensor(const Index& i1) 
    :
    d_(std::make_shared<RealITDat>(i1.m())),
    is_(i1),
    scale_(1)
	{ 
    }


ITensor::
ITensor(const Index& i1,const Index& i2) 
    :
    d_(std::make_shared<RealITDat>(i1.m(),i2.m())),
    is_(i1,i2),
    scale_(1)
	{ 
    }
    

ITensor::
ITensor(const Index& i1, const Index& i2, const Index& i3,
        const Index& i4, const Index& i5, const Index& i6,
        const Index& i7, const Index& i8)
    :
    scale_(1)
	{
#ifdef DEBUG
    if(i1 == Index::Null())
        Error("i1 is null");
    if(i2 == Index::Null())
        Error("i2 is null");
    if(i3 == Index::Null())
        Error("i3 is null");
#endif
    std::array<Index,NMAX> ii = {{ i1, i2, i3, i4, i5, i6, i7, i8 }};
    std::vector<int> extents = { i1.m(), i2.m(), i3.m() };
    int r = 3;
	for(; r < 8 && ii[r] != Index::Null(); ++r)
        {
        extents.push_back(ii[r].m());
        }
    is_ = IndexSet<Index>(ii,r,0);
    d_ = std::make_shared<RealITDat>(extents);
	}

Real ITensor::
toReal() const 
	{ 
#ifdef DEBUG
    if(this->empty())
        Error("ITensor is empty");
#endif

    if(is_.rn() != 0)
        {
        PrintVar(*this);
        Error("ITensor not a scalar");
        }

	//try {
	//    return r_->v(1)*scale_.real(); 
	//    }
	//catch(const TooBigForReal& e)
	//    {
	//    cout << "too big for real() in toReal" << endl;
	//    cerr << "too big for real() in toReal" << endl;
	//    cout << "r_->v(1) is " << r_->v(1) << endl;
	//    cout << "scale is " << scale() << endl;
	//    cout << "rethrowing" << endl;
	//    throw e;
	//    }
	//catch(TooSmallForReal)
	//    {
	//    //cout << "warning: too small for real() in toReal" << endl;
	//    //cerr << "warning: too small for real() in toReal" << endl;
	//    //cout << "r_->v(1) is " << r_->v(1) << endl;
	//    //cout << "scale is " << scale() << endl;
	//    return 0.;
	//    }
	return NAN; //shouldn't reach this line
	}


//Real& ITensor::
//operator()(const IndexVal& iv1)
//	{
//    solo(); 
//    scaleTo(1);
//    return d_->operator()(iv1.i-1);
//	}
//
//Real ITensor::
//operator()(const IndexVal& iv1) const
//	{
//    return scale_.real()*(d_->operator()(iv1.i-1));
//	}
//
//Real& ITensor::
//operator()(const IndexVal& iv1, const IndexVal& iv2) 
//    {
//    solo(); 
//    scaleTo(1);
//    if(is_[0] == iv1)
//        return d_->at(iv1.i-1,iv2.i-1);
//    else
//        return d_->at(iv2.i-1,iv1.i-1);
//    }
//
//Real ITensor::
//operator()(const IndexVal& iv1, const IndexVal& iv2) const
//    {
//    if(is_[0] == iv1)
//        return scale_.real0()*d_->at(iv1.i-1,iv2.i-1);
//    else
//        return scale_.real0()*d_->at(iv2.i-1,iv1.i-1);
//    }
//
//Real& ITensor::
//operator()(const IndexVal& iv1, 
//           const IndexVal& iv2, 
//           const IndexVal& iv3)
//    {
//    solo(); 
//    scaleTo(1);
//    std::array<size_t,3> ii;
//    for(const auto iv : {iv1, iv2, iv3})
//        {
//        ii[findindex(is_,Index(iv))] = iv.i-1;
//        }
//    return d_->at(ii);
//    }
//
//Real ITensor::
//operator()(const IndexVal& iv1, 
//           const IndexVal& iv2, 
//           const IndexVal& iv3) const
//    {
//    std::array<size_t,3> ii;
//    for(const auto iv : {iv1, iv2, iv3})
//        {
//        ii[findindex(is_,Index(iv))] = iv.i-1;
//        }
//    return scale_.real0()*d_->at(ii);
//    }

void ITensor::
swap(ITensor& other)
    {
    d_.swap(other.d_);
    is_.swap(other.is_);
    scale_.swap(other.scale_);
    }

//const Real* ITensor::
//data() const
//    {
//    if(!d_) Error("ITensor is empty");
//    return r_->data();
//    }

//void ITensor::
//randomize(const OptSet& opts) 
//    { 
//    solo(); 
//    std::mt19937 rgen(std::time(NULL)+getpid());
//    std::uniform_real_distribution<double> dist(-1.0, 1.0);
//    r_->generate(bind(dist, rgen));
//    }

//Real ITensor::
//normNoScale() const 
//    { 
//    return 0;
//    //if(!this->isComplex())
//    //    {
//    //    return Norm(r_->v);
//    //    }
//    //else
//    //    {
//    //    return sqrt(sqr(Norm(r_->v))+sqr(Norm(i_->v)));
//    //    }
//    }

//Real ITensor::
//norm() const 
//    { 
//    if(scale_.isTooBigForReal())
//        {
//        throw TooBigForReal("Scale too large for real in ITensor::norm()");
//        }
//    //If scale_ is too small to be converted to Real,
//    //real0 method will return 0.0
//    return fabs(scale_.real0())*normNoScale();
//    }
//
//LogNumber ITensor::
//normLogNum() const 
//    { 
//    return LogNumber(log(normNoScale())+scale_.logNum(),+1);
//    }

void ITensor::
scaleTo(const LogNumber& newscale)
    {
    if(newscale.sign() == 0) 
        Error("Trying to scale an ITensor to a 0 scale");
    if(scale_ == newscale) return;
    solo();
    scale_ /= newscale;
    d_->mult(scale_.real0());
    scale_ = newscale;
    }


void ITensor::
solo()
	{
    if(!d_.unique())
        { 
        d_ = std::move(d_->clone());
        }
    }

void ITensor::
equalizeScales(ITensor& other)
    {
    if(scale_.sign() != 0)
        {
        other.scaleTo(scale_);
        }
    else //*this is equivalent to zero
        {
        solo();
        d_->fill(0);
        scale_ = other.scale_;
        }
    }

ITensor& ITensor::
operator*=(Real fac)
    {
    if(fac == 0)
        {
        solo();
        d_->fill(0);
        return *this;
        }
    scale_ *= fac;
    return *this;
    }


ITensor& ITensor::
operator*=(const ITensor& other)
    {
    return *this;
    }

bool static
checkSameIndOrder(const IndexSet<Index> is1,
                  const IndexSet<Index> is2)
    {
    for(int j = 0; j < is1.rn(); ++j)
    if(is1[j] != is2[j])
        { 
        return false;
        }
    return true;
    }


ITensor& ITensor::
operator+=(const ITensor& other)
    {
    if(!d_) return operator=(other);
    if(this == &other) return operator*=(2.);
    if(this->scale_.isZero()) return operator=(other);

    const
    bool same_ind_order = checkSameIndOrder(is_,other.is_);

    if(is_ != other.is_)
        {
        printn("this ur = %.10f, other.ur = %.10f",is_.uniqueReal(),is_.uniqueReal());
        PrintVar(*this);
        PrintVar(other);
        Error("ITensor::operator+=: different Index structure");
        }

    Real scalefac = 1;
    if(scale_.magnitudeLessThan(other.scale_)) 
        {
        this->scaleTo(other.scale_); 
        }
    else
        {
        scalefac = (other.scale_/scale_).real();
        }

    solo();

    if(same_ind_order) 
        { 
        d_->plusEq(other.d_,scalefac);
        }
    else // not same_ind_order
        {
        Error("Can currently only add if same index order");
        }

    return *this;
    } 

ITensor& ITensor::
operator-=(const ITensor& other)
    {
    if(this == &other) 
        { 
        scale_ = 0; 
        return *this; 
        }
    scale_.negate();
    operator+=(other); 
    scale_.negate();
    return *this; 
    }

void ITensor::
fill(Real r)
    {
#ifdef DEBUG
    if(this->empty()) Error("Cannot fill default-constructed ITensor.");
#endif
    d_->fill(r);
    }


ostream& 
operator<<(ostream & s, const ITensor& t)
    {
    s << "ITensor r = " << t.r() << ": ";
    s << t.indices() << "\n";
    s << "  {log(scale)[incl in elems]=" << t.scale().logNum();

    const bool ff_set = (std::ios::floatfield & s.flags()) != 0;

    if(ff_set || Global::printdat())
        {
        if(!t.empty())
            {
            t.d_->print(s,t.scale());
            }
        else
            {
            s << " (empty / default constructed)}\n";
            }
        }
    return s;
    }

};
