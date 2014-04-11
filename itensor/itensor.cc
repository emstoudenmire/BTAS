//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#include "itensor.h"
#include "itensor/itdata/realitdata.h"

using std::array;
using std::ostream;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::cout;
using std::endl;

namespace itensor {

//
// ITensor Constructors
//

ITensor::
ITensor()  
    { }


ITensor::
ITensor(const Index& i1) 
    :
    d_(std::make_shared<RealITData>(i1.m())),
    is_(i1)
	{ 
    }


ITensor::
ITensor(const Index& i1,const Index& i2) 
    :
    d_(std::make_shared<RealITData>(i1.m(),i2.m())),
    is_(i1,i2)
	{ 
    }
    

ITensor::
ITensor(const Index& i1, const Index& i2, const Index& i3,
        const Index& i4, const Index& i5, const Index& i6,
        const Index& i7, const Index& i8)
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
    d_ = std::make_shared<RealITData>(extents);
	}

ITensor::
ITensor(Real val) 
    { 
    //TODO fix this constructor
    //ScalarITData class?
    fill(val);
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


//const Real* ITensor::
//data() const
//    {
//    if(!d_) Error("ITensor is empty");
//    return r_->data();
//    }




ITensor& ITensor::
operator*=(const ITensor& other)
    {
    if(this->empty() || other.empty())
        Error("Empty ITensor in product");

    if(this == &other)
        {
        ITensor cp_oth(other);
        return operator*=(cp_oth);
        }

    //These hold  regular new indices and the m==1 indices that appear in the result
    IndexSet<Index> new_index;

    static array<const Index*,100> new_index1_;
    size_t nr1_ = 0;

    //
    //Handle m==1 Indices
    //
    for(int k = is_.rn(); k < this->r(); ++k)
        {
        const Index& K = is_[k];
        if(!hasindex(other.is_,K))
            new_index1_[++nr1_] = &K;
        }

    for(int j = other.is_.rn(); j < other.r(); ++j)
        {
        const Index& J = other.is_[j];
        if(!hasindex(this->is_,J))
            new_index1_[++nr1_] = &J;
        }

    //
    //Special cases when one of the tensors
    //has only m==1 indices (effectively a scalar)
    //
    //if(other.is_.rn() == 0)
    //    {
    //    scale_ *= other.scale_;
    //    scale_ *= other.r_->v(1);
    //    for(int j = 0; j < is_.rn(); ++j)
    //        new_index.addindex(is_[j]);
    //    //Keep current m!=1 indices, overwrite m==1 indices
    //    for(int j = 1; j <= nr1_; ++j) 
    //        new_index.addindex( *(new_index1_[j]) );
    //    is_.swap(new_index);
    //    return *this;
    //    }
    //else if(is_.rn() == 0)
    //    {
    //    scale_ *= other.scale_;
    //    scale_ *= r_->v(1);
    //    r_ = other.r_;
    //    for(int j = 1; j <= other.is_.rn(); ++j) 
    //        new_index.addindex( other.is_.index(j) );
    //    for(int j = 1; j <= nr1_; ++j) 
    //        new_index.addindex( *(new_index1_[j]) );
    //    is_.swap(new_index);
    //    return *this;
    //    }


    //TODO: replace Lind, Rind with static-allocated arrays
    btas::varray<size_t> Lind(size_t(is_.rn()),1),
                         Rind(size_t(other.is_.rn()),1);

    size_t ncont = 0; //number of m!=1 indices that match
    for(int j = 0; j < is_.rn(); ++j)
	for(int k = 0; k < other.is_.rn(); ++k)
        {
	    if(is_[j] == other.is_[k])
            {
            ++ncont;
            Lind[j] = 0;
            Rind[k] = 0;
            }
        }

    size_t uu = 0;
    for(auto& x : Lind)
        {
        if(x == 1)
            x = ++uu;
        }
    for(auto& x : Rind)
        {
        if(x == 1)
            x = ++uu;
        }

    const size_t nuniq = is_.rn()+other.is_.rn()-2*ncont;
    //TODO: replace Pind with static-allocated array
    auto Pind = btas::varray<size_t>(nuniq);
    for(size_t i = 0; i < nuniq; ++i)
        {
        Pind[i] = 1+i;
        }

    //cout << "Lind = {";
    //for(auto x : Lind)
    //    {
    //    cout << x << ",";
    //    }
    //cout << "}" << endl;
    //cout << "Rind = {";
    //for(auto x : Rind)
    //    {
    //    cout << x << ",";
    //    }
    //cout << "}" << endl;
    //cout << "Pind = {";
    //for(auto x : Pind)
    //    {
    //    cout << x << ",";
    //    }
    //cout << "}" << endl;
    //exit(0);

    d_->contractEq(d_,other.d_,Lind,Rind,Pind);

    //
    //TODO: improve following lines so that new_index
    //is not repeatedly calling push_back/new
    //(either append/construct all at once for single call to new, 
    //resize new_index ahead of time, or use stack-allocated data (std::array) inside new_index)
    //
    //Put in m!=1 indices
    for(int j = 0; j < this->is_.rn(); ++j)
        {
        if(0 != Lind[j]) 
            new_index.addindex( is_[j] );
        }
    for(int j = 0; j < other.is_.rn(); ++j)
        {
        if(0 != Rind[j]) 
            new_index.addindex( other.is_[j] );
        }
    //Put in m==1 indices
    for(int j = 1; j <= nr1_; ++j) 
        {
        new_index.addindex( *(new_index1_.at(j)) );
        }

    is_.swap(new_index);

    scale_ *= other.scale_;

    scaleOutNorm();

    return *this;
    }

ITensor& ITensor::
operator*=(Real fac)
    {
    if(fac == 0)
        {
        fill(0);
        return *this;
        }
    scale_ *= fac;
    return *this;
    }

ITensor& ITensor::
operator/=(Real fac) 
    { 
    scale_ /= fac; 
    return *this; 
    }

//ITensor& ITensor::
//operator*=(Complex z)
//    {
//    }

//ITensor& ITensor::
//operator/=(Complex z)
//    {
//    }

ITensor ITensor::
operator-() const 
    { 
    ITensor T(*this); 
    T.scale_ *= -1; 
    return T; 
    }

ITensor& ITensor::
noprime(IndexType type) 
    { 
    is_.noprime(type); 
    return *this; 
    }

//Set primeLevel of Index I to zero
ITensor& ITensor::
noprime(const Index& I) 
    { 
    is_.noprime(I); 
    return *this; 
    }

//Increase primeLevel of Indices by 1 (or optional amount inc)
ITensor& ITensor::
prime(int inc) 
    { 
    prime(All,inc); 
    return *this;
    }

//Increase primeLevel of Indices by 1 (or optional amount inc)
ITensor& ITensor::
prime(IndexType type, int inc) 
    { 
    is_.prime(type,inc); 
    return *this; 
    }

//Increase primeLevel of Index I by 1 (or optional amount inc)
ITensor& ITensor::
prime(const Index& I, int inc) 
    { 
    is_.prime(I,inc); 
    return *this; 
    }

//Change all Indices having primeLevel plevold to have primeLevel plevnew
ITensor& ITensor::
mapprime(int plevold, int plevnew, IndexType type)
    { 
    is_.mapprime(plevold,plevnew,type); 
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
        d_->plusEq(other.d_,d_,scalefac);
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
        fill(0);
        return *this; 
        }
    scale_.negate();
    operator+=(other); 
    scale_.negate();
    return *this; 
    }

ITensor& ITensor::
fill(Real r)
    {
#ifdef DEBUG
    if(this->empty()) Error("Cannot fill default-constructed ITensor.");
#endif
    solo();
    scale_.reset();
    d_->fill(r,d_);
    return *this;
    }

ITensor& ITensor::
generate(std::function<Real()> rfunc)
    {
#ifdef DEBUG
    if(this->empty()) Error("Cannot generate elements of default-constructed ITensor.");
#endif
    solo();
    scale_.reset();
    d_->generate(rfunc,d_);
    return *this;
    }

void ITensor::
swap(ITensor& other)
    {
    d_.swap(other.d_);
    is_.swap(other.is_);
    scale_.swap(other.scale_);
    }

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
    if(!d_.unique()) d_ = std::move(d_->clone());
    }

void ITensor::
scaleOutNorm()
    {
    //Real f = normNoScale();
    ////If norm already 1 return so
    ////we don't have to call solo()
    //if(fabs(f-1) < 1E-12) return;

    //if(f != 0)
    //    {
    //    solo();
    //    d_->mult(1./f);
    //    scale_ *= f;
    //    }
    //else //norm == zero
    //    {
    //    scale_ = LogNumber(1.);
    //    d_->fill(0.,d_);
    //    }
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
        fill(0);
        scale_ = other.scale_;
        }
    }


ostream& 
operator<<(ostream & s, const ITensor& t)
    {
    s << "ITensor r = " << t.r() << ": ";
    s << t.indices() << "\n";
    s << "  {log(scale)[incl in elems]=" << t.scale().logNum();

    //Checking whether std::ios::floatfield is set enables 
    //printing the contents of an ITensor when using the printf
    //format string %f (or another float-related format string)
    const bool ff_set = (std::ios::floatfield & s.flags()) != 0;

    if(ff_set || Global::printdat())
        {
        if(!t.empty()) t.data().print(s,t.scale());
        else           s << " (empty / default constructed)}\n";
        }
    return s;
    }

ITensor
randomize(ITensor T, const OptSet& opts)
    {
    //std::uniform_real_distribution<Real> dist(-1.,1.);
    T.generate(Global::random);
    return T;
    }

Real
toReal(const ITensor& T)
	{ 
#ifdef DEBUG
    if(T.empty()) Error("ITensor is empty / default constructed");
#endif

    if(T.indices().rn() != 0)
        {
        PrintVar(T.indices());
        Error("ITensor not a scalar");
        }

    Error("Not implemented");

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

Complex
toComplex(const ITensor& T)
    {
#ifdef DEBUG
    if(T.empty()) Error("ITensor is empty / default constructed");
#endif
    if(T.indices().rn() != 0)
        {
        PrintVar(T.indices());
        Error("ITensor not a scalar");
        }

    Error("Not implemented");
    return Complex_1;
    }

Real
norm(const ITensor& T)
    {
    return fabs(T.scale().real0()) * T.data().norm();
    }

//LogNumber
//normLogNum(const ITensor& T)
//    { 
//    return LogNumber(log(T.data().norm())+T.scale().logNum(),+1);
//    }

};
