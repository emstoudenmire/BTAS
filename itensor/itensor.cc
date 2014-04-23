//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#include "itensor.h"

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
ITensor(Real val) 
    { 
    //TODO implement this constructor
    //ScalarITData class?
    //fill(val);
    }

ITensor::
ITensor(IndexSet&& iset,
        ITData::NewPtr nd,
        LogNumber scale)
    :
    d_(std::move(nd)),
    is_(iset),
    scale_(scale)
    {
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
        return operator=( ITensor(sqr(norm(*this))) );

    btas::varray<bool> contL(size_t(is_.r()),false),
                       contR(size_t(other.is_.r()),false);

    //TODO: replace Lind, Rind with static-allocated arrays
    //TODO: redefine btas::contract to take pair of iterators
    //      to annotations instead of containers
    //      reimplement current version as just as wrapper

    //Set Lind, Rind to zero. Special value 0 marks
    //uncontracted indices. Later will assign unique numbers
    //to these entries in Lind and Rind
    btas::varray<size_t> Lind(size_t(is_.rn()),0),
                         Rind(size_t(other.is_.rn()),0);

    //Count number of contracted indices,
    //set corresponding entries of Lind, Rind
    //to 1,2,...,ncont
    int ncont = 0;
    for(int i = 0; i < is_.rn(); ++i)
    for(int j = 0; j < other.is_.rn(); ++j)
        {
        if(is_[i] == other.is_[j])
            {
            contL[i] = true;
            contR[j] = true;

            ++ncont;
            Lind[i] = ncont;
            Rind[j] = ncont;

            break;
            }
        }

    //Finish making contL, contR for m==1 indices
    int ncont_all = ncont;
    for(int i = is_.rn(); i < is_.r(); ++i)
    for(int j = other.is_.rn(); j < other.is_.r(); ++j)
        {
        if(is_[i] == other.is_[j])
            {
            ++ncont_all;
            contL[i] = true;
            contR[j] = true;
            break;
            }
        }

    //nuniq is total number of unique, uncontracted indices
    //(nuniq all includes m==1 indices)
    const int nuniq = is_.rn()+other.is_.rn()-2*ncont;
    const int nuniq_all = is_.r()+other.is_.r()-2*ncont_all;

    //container in which we will accumulate the new indices
    IndexSet::storage newind(nuniq_all);

    //Go through and assign uncontracted entries of Lind,Rind
    //the integers ncont+1,ncont+2,...
    //Simultaneously fill newind (keeping count "ni")
    size_t ni = 0;
    size_t uu = ncont;
    for(int j = 0; j < is_.rn(); ++j)
        {
        if(!contL[j]) 
            {
            Lind[j] = ++uu;
            newind.at(ni++) = is_[j];
            }
        }
    for(int j = 0; j < other.is_.rn(); ++j)
        {
        if(!contR[j]) 
            {
            Rind[j] = ++uu;
            newind.at(ni++) = other.is_[j];
            }
        }

    //Finish filling up newind with m==1 indices
    for(int j = is_.rn(); j < is_.r(); ++j)
        {
        if(!contL[j]) 
            newind.at(ni++) = is_[j];
        }
    for(int j = other.is_.rn(); j < other.is_.r(); ++j)
        {
        if(!contR[j]) 
            newind.at(ni++) = other.is_[j];
        }

    //TODO: replace Pind with static-allocated array
    btas::varray<size_t> Pind(nuniq);
    for(size_t i = 0, u = 1+ncont; i < nuniq; ++i,++u)
        {
        Pind[i] = u;
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

    IndexSet new_index(std::move(newind),nuniq);

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
checkSameIndOrder(const IndexSet& is1,
                  const IndexSet& is2)
    {
    for(int j = 0; j < is1.rn(); ++j)
        {
        if(is1[j] != is2[j])
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

    if(is_ != other.is_)
        {
        PrintVar(*this);
        PrintVar(other);
        Error("ITensor::operator+=: different Index structure");
        }

    const
    bool same_ind_order = checkSameIndOrder(is_,other.is_);


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
    if(this->empty()) return *this;
    solo();
    scale_.reset();
    d_->fill(r,d_);
    return *this;
    }

ITensor& ITensor::
generate(std::function<Real()> rfunc)
    {
    if(this->empty()) return *this;
    solo();
    scale_.reset();
    d_->generate(rfunc,d_);
    return *this;
    }

void ITensor::
scaleTo(const LogNumber& newscale)
    {
    if(scale_ == newscale) return;
    if(newscale.sign() == 0) Error("Trying to scale an ITensor to a 0 scale");
    solo();
    scale_ /= newscale;
    d_->mult(scale_.real0());
    scale_ = newscale;
    }


void ITensor::
solo()
	{
    if(!d_.unique()) d_ = d_->clone();
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
    s << t.inds() << "\n";
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

    if(T.inds().rn() != 0)
        {
        PrintVar(T.inds());
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
    if(T.inds().rn() != 0)
        {
        PrintVar(T.inds());
        Error("ITensor not a scalar");
        }

    Error("Not implemented");
    return Complex_1;
    }

struct NormVisitor
    {
    Real nrm2 = 0.;

    void
    operator()(Real r) { nrm2 += r*r; }

    void
    operator()(Complex z) { nrm2 += sqr(z.real()) + sqr(z.imag()); }
    };

Real
norm(const ITensor& T)
    {
    NormVisitor N;
    T.visit(N);
    return sqrt(N.nrm2);
    }

//LogNumber
//normLogNum(const ITensor& T)
//    { 
//    return LogNumber(log(T.data().norm())+T.scale().logNum(),+1);
//    }

};
