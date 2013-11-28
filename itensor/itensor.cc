//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#include "itensor.h"

#include <functional>
#include <random>

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using boost::format;
using boost::shared_ptr;
using boost::make_shared;

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
    r_(boost::make_shared<RTensor>(i1.m())),
    is_(i1),
    scale_(1)
	{ 
    }


ITensor::
ITensor(const Index& i1,const Index& i2) 
    :
    r_(boost::make_shared<RTensor>(i1.m(),i2.m())),
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
    //                        0   1   2   3   4   5   6    7
	Array<Index,NMAX> ii = {{ i1, i2, i3, i4, i5, i6, i7, i8 }};
    RTensor::shape_type shape = { i1.m(), i2.m(), i3.m() };
    int r = 3;
	for(; r < 8 && ii[r] != Index::Null(); ++r)
        {
        shape.push_back(ii[r].m());
        }
	int alloc_size = -1; 
    is_ = IndexSet<Index>(ii,r,alloc_size,0);
    r_ = boost::make_shared<RTensor>(shape);
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
        Print(*this);
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


Real& ITensor::
operator()(const IndexVal& iv1)
	{
    solo(); 
    scaleTo(1);
    return r_->operator()(iv1.i-1);
	}

Real ITensor::
operator()(const IndexVal& iv1) const
	{
    return scale_.real()*(r_->operator()(iv1.i-1));
	}

Real& ITensor::
operator()(const IndexVal& iv1, const IndexVal& iv2) 
    {
    solo(); 
    scaleTo(1);
    if(is_[0] == iv1)
        return r_->at(iv1.i-1,iv2.i-1);
    else
        return r_->at(iv2.i-1,iv1.i-1);
    }

Real ITensor::
operator()(const IndexVal& iv1, const IndexVal& iv2) const
    {
    if(is_[0] == iv1)
        return scale_.real0()*r_->at(iv1.i-1,iv2.i-1);
    else
        return scale_.real0()*r_->at(iv2.i-1,iv1.i-1);
    }

Real& ITensor::
operator()(const IndexVal& iv1, 
           const IndexVal& iv2, 
           const IndexVal& iv3)
    {
    solo(); 
    scaleTo(1);
    Array<int,3> ii;
    for(const auto iv : {iv1, iv2, iv3})
        {
        ii[findindex(is_,Index(iv))] = iv.i-1;
        }
    return r_->at(ii[0],ii[1],ii[2]);
    }

Real ITensor::
operator()(const IndexVal& iv1, 
           const IndexVal& iv2, 
           const IndexVal& iv3) const
    {
    Array<int,3> ii;
    for(const auto iv : {iv1, iv2, iv3})
        {
        ii[findindex(is_,Index(iv))] = iv.i-1;
        }
    return scale_.real0()*r_->at(ii[0],ii[1],ii[2]);
    }

void ITensor::
groupIndices(const Array<Index,NMAX+1>& indices, int nind, 
             const Index& grouped, ITensor& res) const
    {
    }

void ITensor::
tieIndices(const Array<Index,NMAX>& indices, int nind,
           const Index& tied)
    {
    }

void ITensor::
tieIndices(const Index& i1, const Index& i2,
           const Index& tied)
    {
    Array<Index,NMAX> inds =
        {{ i1, i2, 
           Index::Null(), Index::Null(), 
           Index::Null(), Index::Null(), 
           Index::Null(), Index::Null() }};

    tieIndices(inds,2,tied);
    }

void ITensor::
tieIndices(const Index& i1, const Index& i2,
           const Index& i3,
           const Index& tied)
    {
    Array<Index,NMAX> inds =
        {{ i1, i2, i3,
           Index::Null(), Index::Null(), 
           Index::Null(), Index::Null(), Index::Null() }};

    tieIndices(inds,3,tied);
    }

void ITensor::
tieIndices(const Index& i1, const Index& i2,
           const Index& i3, const Index& i4,
           const Index& tied)
    {
    Array<Index,NMAX> inds =
        {{ i1, i2, i3, i4,
           Index::Null(), Index::Null(), 
           Index::Null(), Index::Null() }};

    tieIndices(inds,4,tied);
    }

ITensor& ITensor::
trace(const Array<Index,NMAX>& indices, int nind)
    {
    return *this;
    } //ITensor::trace


ITensor& ITensor::
trace(const Index& i1, const Index& i2,
      const Index& i3, const Index& i4,
      const Index& i5, const Index& i6,
      const Index& i7, const Index& i8)
    {
    Array<Index,NMAX> inds = {{ i1, i2, i3, i4,
                                i5, i6, i7, i8 }};
    return trace(inds);
    }

void ITensor::
expandIndex(const Index& small, const Index& big, int start)
    {
    }

void ITensor::
swap(ITensor& other)
    {
    r_.swap(other.r_);
    is_.swap(other.is_);
    scale_.swap(other.scale_);
    }

const Real* ITensor::
data() const
    {
    if(!r_) Error("ITensor is empty");
    return r_->data();
    }

void ITensor::
randomize(const OptSet& opts) 
    { 
    solo(); 
    std::mt19937 rgen(std::time(NULL)+getpid());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    r_->generate(bind(dist, rgen));
    }

Real ITensor::
normNoScale() const 
    { 
    return 0;
    //if(!this->isComplex())
    //    {
    //    return Norm(r_->v);
    //    }
    //else
    //    {
    //    return sqrt(sqr(Norm(r_->v))+sqr(Norm(i_->v)));
    //    }
    }

Real ITensor::
norm() const 
    { 
    if(scale_.isTooBigForReal())
        {
        throw TooBigForReal("Scale too large for real in ITensor::norm()");
        }
    //If scale_ is too small to be converted to Real,
    //real0 method will return 0.0
    return fabs(scale_.real0())*normNoScale();
    }

LogNumber ITensor::
normLogNum() const 
    { 
    return LogNumber(log(normNoScale())+scale_.logNum(),+1);
    }

void ITensor::
scaleTo(const LogNumber& newscale)
    {
    if(newscale.sign() == 0) 
        Error("Trying to scale an ITensor to a 0 scale");
    if(scale_ == newscale) return;
    solo();
    scale_ /= newscale;
    //r_->operator*=(scale_.real0());
    scale_ = newscale;
    }


void ITensor::
allocate(int dim) 
    { 
    //r_ = boost::make_shared<ITDat>(dim); 
    }

void ITensor::
allocate() 
    { 
    //r_ = boost::make_shared<ITDat>(); 
    }

void ITensor::
solo()
	{
    if(!r_.unique())
        { 
        //boost::shared_ptr<ITDat> newr = boost::make_shared<ITDat>();
        //newr->v = r_->v;
        //r_.swap(newr);
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
        r_->fill(0);
        scale_ = other.scale_;
        }
    }

ITensor& ITensor::
operator*=(Real fac)
    {
    if(fac == 0)
        {
        solo();
        r_->fill(0);
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


ITensor& ITensor::
operator+=(const ITensor& other)
    {
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


ostream& 
operator<<(ostream & s, const ITensor& t)
    {
    s << "ITensor r = " << t.r() << ": ";
    s << t.indices() << "\n";
    s << "  {log(scale)[incl in elems]=" << t.scale().logNum();

    if(Global::printdat())
        {
        s << "\n";
        Real scale = 1.0;
        if(t.scale().isFiniteReal()) scale = t.scale().real();
        else s << "  (omitting too large scale factor)" << endl;

        //
        // Following code just quick and dirty up to rank 3;
        // need to write general-rank code
        //
        const Real* pv = t.data();
        if(t.r() == 1)
            {
            for(size_t i1 = 0; i1 < t.indices()[0].m(); ++i1)
                {
                s << format("  (%d) %.10f\n") 
                     % (i1+1)
                     % (pv[i1]*scale);
                }
            }
        else
        if(t.r() == 2)
            {
            size_t ind = 0;
            for(int i1 = 1; i1 <= t.indices()[0].m(); ++i1)
            for(int i2 = 1; i2 <= t.indices()[1].m(); ++i2)
                {
                s << format("  (%d,%d) %.10f\n") 
                     % i1 % i2 
                     % (pv[ind]*scale);
                ++ind;
                }
            }
        else
        if(t.r() == 3)
            {
            size_t ind = 0;
            for(int i1 = 1; i1 <= t.indices()[0].m(); ++i1)
            for(int i2 = 1; i2 <= t.indices()[1].m(); ++i2)
            for(int i3 = 1; i3 <= t.indices()[2].m(); ++i3)
                {
                s << format("  (%d,%d,%d) %.10f\n") 
                     % i1 % i2 % i3
                     % (pv[ind]*scale);
                ++ind;
                }
            }
        }

    return s;
    }
