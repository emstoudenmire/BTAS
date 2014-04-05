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
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using boost::format;
using std::shared_ptr;
using std::make_shared;

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
    r_(std::make_shared<storage>(i1.m())),
    is_(i1),
    scale_(1)
	{ 
    }


ITensor::
ITensor(const Index& i1,const Index& i2) 
    :
    r_(std::make_shared<storage>(i1.m(),i2.m())),
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
    r_ = std::make_shared<storage>(storage::range_type(extents));
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
    std::array<size_t,3> ii;
    for(const auto iv : {iv1, iv2, iv3})
        {
        ii[findindex(is_,Index(iv))] = iv.i-1;
        }
    return r_->at(ii);
    }

Real ITensor::
operator()(const IndexVal& iv1, 
           const IndexVal& iv2, 
           const IndexVal& iv3) const
    {
    std::array<size_t,3> ii;
    for(const auto iv : {iv1, iv2, iv3})
        {
        ii[findindex(is_,Index(iv))] = iv.i-1;
        }
    return scale_.real0()*r_->at(ii);
    }

void ITensor::
groupIndices(const std::array<Index,NMAX+1>& indices, int nind, 
             const Index& grouped, ITensor& res) const
    {
    }

void ITensor::
tieIndices(const std::array<Index,NMAX>& indices, int nind,
           const Index& tied)
    {
    //if(nind == 0) Error("No indices given");

    //const int tm = tied.m();
    //
    //std::array<Index,NMAX+1> new_index;
    ////TODO
    ////Check for possible bug: behavior of btas::tieIndex
    ////is to position new tied index at location of first matching
    ////tied index, not necessarily position 1
    //new_index[1] = tied;

    //vector<size_t> totie;

    //int nmatched = 0;
    //int new_r = 1;
    //for(size_t k = 0; k < r(); ++k)
    //    {
    //    const Index& K = is_.index(1+k);
    //    bool is_tied = false;
    //    for(int j = 0; j < nind; ++j)
    //    if(K == indices[j]) 
    //        { 
    //        if(indices[j].m() != tm)
    //            Error("Tied indices must have matching m's");
    //        totie.push_back(k);
    //        is_tied = true;
    //        ++nmatched;
    //        break;
    //        }

    //    if(!is_tied)
    //        {
    //        new_index[++new_r] = K;
    //        }
    //    }

    ////Check that all indices were found
    //if(nmatched != nind)
    //    {
    //    Print(*this);
    //    cout << "indices = " << endl;
    //    for(int j = 0; j < nind; ++j)
    //        cout << indices[j] << endl;
    //    Error("Couldn't find Index to tie");
    //    }

    //IndexSet<Index> new_is(new_index,new_r,1);
    //is_.swap(new_is);

    ////If tied indices have m==1, no work
    ////to do; just replace indices
    //if(tm == 1) return;

    //// Call btas::tieIndex and create the new dat
    //storage_ptr np = std::make_shared<storage>(btas::tieIndex(*r_,totie));
    //r_.swap(np);
    }

//ITensor ITensor::
//testMethod(const std::array<Index,NMAX>& indices, int nind,
//           const Index& tied) const
    //{
    //if(nind == 0) Error("No indices given");

    //const int tm = tied.m();
    //
    //std::array<Index,NMAX+1> new_index;
    ////TODO
    ////Check for possible bug: behavior of btas::tieIndex
    ////is to position new tied index at location of first matching
    ////tied index, not necessarily position 1
    //new_index[1] = tied;

    //vector<size_t> totie;

    //int nmatched = 0;
    //int new_r = 1;
    //for(size_t k = 0; k < r(); ++k)
    //    {
    //    const Index& K = is_.index(1+k);
    //    bool is_tied = false;
    //    for(int j = 0; j < nind; ++j)
    //    if(K == indices[j]) 
    //        { 
    //        if(indices[j].m() != tm)
    //            Error("Tied indices must have matching m's");
    //        totie.push_back(k);
    //        is_tied = true;
    //        ++nmatched;
    //        break;
    //        }

    //    if(!is_tied)
    //        {
    //        new_index[++new_r] = K;
    //        }
    //    }

    ////Check that all indices were found
    //if(nmatched != nind)
    //    {
    //    Print(*this);
    //    cout << "indices = " << endl;
    //    for(int j = 0; j < nind; ++j)
    //        cout << indices[j] << endl;
    //    Error("Couldn't find Index to tie");
    //    }

    //IndexSet<Index> new_is(new_index,new_r,1);
    //ITensor res;
    //res.is_.swap(new_is);

    ////If tied indices have m==1, no work
    ////to do; just replace indices
    //if(tm == 1) 
    //    {
    //    res.scale_ = this->scale_;
    //    res.r_ = this->r_;
    //    return res;
    //    }

    //// Call btas::tieIndex and create the new dat
    //storage_ptr np = std::make_shared<storage>(btas::tieIndex(*r_,totie));
    //res.r_.swap(np);

    //return res;
    //}

void ITensor::
tieIndices(const Index& i1, const Index& i2,
           const Index& tied)
    {
    std::array<Index,NMAX> inds =
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
    std::array<Index,NMAX> inds =
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
    std::array<Index,NMAX> inds =
        {{ i1, i2, i3, i4,
           Index::Null(), Index::Null(), 
           Index::Null(), Index::Null() }};

    tieIndices(inds,4,tied);
    }

ITensor& ITensor::
trace(const std::array<Index,NMAX>& indices, int nind)
    {
    return *this;
    } //ITensor::trace


ITensor& ITensor::
trace(const Index& i1, const Index& i2,
      const Index& i3, const Index& i4,
      const Index& i5, const Index& i6,
      const Index& i7, const Index& i8)
    {
    std::array<Index,NMAX> inds = {{ i1, i2, i3, i4,
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
    btas::scal(scale_.real0(),*r_);
    scale_ = newscale;
    }


void ITensor::
allocate(int dim) 
    { 
    //r_ = std::make_shared<ITDat>(dim); 
    }

void ITensor::
allocate() 
    { 
    //r_ = std::make_shared<ITDat>(); 
    }

void ITensor::
solo()
	{
    if(!r_.unique())
        { 
        auto newr = std::make_shared<storage>(*r_);
        r_.swap(newr);
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
    if(!r_) return operator=(other);

    if(this == &other) return operator*=(2.);

    if(this->scale_.isZero()) return operator=(other);

    const
    bool same_ind_order = checkSameIndOrder(is_,other.is_);

    if(is_ != other.is_)
        {
        cerr << format("this ur = %.10f, other.ur = %.10f\n")%is_.uniqueReal()%other.is_.uniqueReal();
        Print(*this);
        Print(other);
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
        //axpy computes *r_ += *(other.r_) * scalefac
        btas::axpy(scalefac,*(other.r_),*r_);
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


ostream& 
operator<<(ostream & s, const ITensor& t)
    {
    s << "ITensor r = " << t.r() << ": ";
    s << t.indices() << "\n";
    s << "  {log(scale)[incl in elems]=" << t.scale().logNum();

    if(Global::printdat())
        {
        s << "\n";
        Real scalefac = 1.0;
        if(t.scale().isFiniteReal()) scalefac = t.scale().real();
        else s << "  (omitting too large scale factor)" << endl;

        const auto& T = *(t.r_);
        auto Tr = T.range();

        const auto rr = T.rank();

        if(rr == 0) return s;

        for(const auto I : Tr)
            {
            const Real val = T(I)*scalefac;
            if(fabs(val) > Global::printScale())
                {
                s << "  ("; 
                for(size_t i = 0, j = 1; j <= I.size(); ++i, ++j)
                    {
                    s << (1+I[i]);
                    if(j < I.size()) s << ",";
                    }
                s << ") ";
                s << format(fabs(val) > 1E-10 ? "%.12f\n" : "%.8E\n") % val;
                }
            }
        }

    return s;
    }

};
