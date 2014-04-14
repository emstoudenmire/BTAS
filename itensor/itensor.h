//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITENSOR_H
#define __ITENSOR_ITENSOR_H
#include "itensor/real.h"
#include "itensor/indexset.h"

#include "itensor/itdata/itdata.h"
#include "itensor/itdata/realitdata.h"

namespace itensor {

//
// ITensor
//
class ITensor
    {
    public:

    using storage = ITData;
    using storage_ptr = std::shared_ptr<ITData>;
    using IndexT = Index;
    using IndexValT = IndexVal;

    //
    // Constructors
    //

    //Construct Null ITensor, isNull returns true
    ITensor();

    //Construct rank 1 ITensor, all entries set to zero
    explicit 
    ITensor(const Index& i1);

    //Construct rank 2 ITensor, all entries set to zero
    ITensor(const Index& i1,
            const Index& i2);

    //Construct ITensor up to rank 8, entries set to zero
    template <typename... Indices>
    ITensor(const Index& i0, 
            const Index& i1, 
            const Indices&... rest);

    //Construct rank 0 ITensor (scalar), value set to val
    explicit
    ITensor(Real val);

    //TODO: add move constructor and move operator=
    //ITensor(ITensor&& t);
    //ITensor& operator=(ITensor&& t);

    //
    // Accessor Methods
    //

    //Rank of this ITensor (number of indices)
    int 
    r() const { return is_.r(); }

    const IndexSet&
    inds() const { return is_; }

    //true if ITensor is default constructed
    bool 
    empty() const { return !bool(d_); }

    //
    // Operators
    //

    //Contracting product
    //All matching Index pairs automatically contracted
    //Cji = \sum_{k,l} Akjl * Blki
    ITensor& 
    operator*=(const ITensor& other);

    //Multiplication and division by scalar
    ITensor& 
    operator*=(Real fac);

    ITensor& 
    operator/=(Real fac);

    ITensor& 
    operator*=(Complex z);

    ITensor& 
    operator/=(Complex z);

    ITensor
    operator-() const;

    //Tensor addition and subtraction
    //Summands must have same Indices, in any order
    //Cijk = Aijk + Bkij
    ITensor& 
    operator+=(const ITensor& other);

    ITensor& 
    operator-=(const ITensor& other);

    //
    // Index Prime Level Methods
    //

    //Set primeLevel of Indices to zero
    ITensor& 
    noprime(IndexType type = All);

    //Set primeLevel of Index I to zero
    ITensor& 
    noprime(const Index& I);

    //Increase primeLevel of Indices by 1 (or optional amount inc)
    ITensor& 
    prime(int inc = 1);

    //Increase primeLevel of Indices by 1 (or optional amount inc)
    ITensor& 
    prime(IndexType type, int inc = 1);

    //Increase primeLevel of Index I by 1 (or optional amount inc)
    ITensor& 
    prime(const Index& I, int inc = 1);

    //Change all Indices having primeLevel plevold to have primeLevel plevnew
    ITensor& 
    mapprime(int plevold, int plevnew, IndexType type = All);

    //
    // Element Transformation Methods
    //

    ITensor&
    fill(Real r);

    ITensor&
    generate(std::function<Real()> rfunc);

    template <typename Func>
    ITensor&
    map(const Func& f);

    void
    swap(ITensor& other);

    private:

    //////////////

    storage_ptr d_;
    IndexSet is_;
    LogNumber scale_;

    //////////////

    //Disattach self from current ITData and create own copy instead.
    //Necessary because ITensors logically represent distinct
    //tensors even though they may share data (copy-on-write idiom)
    void 
    solo();

    void
    scaleOutNorm();

    void
    equalizeScales(ITensor& other);

    public:

    //
    // Developer / advanced methods
    //
    // The following methods enable external functions
    // to manipulate/copy/inspect ITensors but should 
    // not be explicitly needed for most user code.
    //

    //Scale factor, used internally for efficient scalar ops.
    //Mostly for developer use, not necessary to explicitly involve
    //scale factors in most ITensor operations.
    const LogNumber&
    scale() const { return scale_; }

    const ITData&
    data() const { return *d_; }

    void 
    scaleTo(const LogNumber& newscale);

    }; // class ITensor

template <typename... Indices>
ITensor::
ITensor(const Index& i0, 
        const Index& i1,
        const Indices&... rest)
	{
    const auto size = 2 + sizeof...(rest);
    std::array<Index,size> inds = { i0, i1, static_cast<Index>(rest)...};
    std::array<int,size> extents;
    for(size_t j = 0; j < size; ++j) extents[j] = inds[j].m();
    is_ = IndexSet(inds,size);
    d_ = std::make_shared<RealITData>(extents);
	}

template <typename Func>
ITensor& ITensor::
map(const Func& f)
    {
    //MapWrap<F> creates a new type that holds a functor of
    //type F but which is also a virtual/polymorphic subclass
    //of MapBase so that subclasses of ITData can call f through
    //a uniform function signature
    solo();
    scaleTo(1);
    detail::MapWrap<Func> mw(f);
    d_->map(&mw);
    return *this;
    }

ITensor inline
operator*(ITensor A, const ITensor& B) { A *= B; return A; }

ITensor inline
operator*(ITensor T, Real fac) { T *= fac; return T; }

ITensor inline
operator*(Real fac, ITensor T) { T *= fac; return T; }

ITensor inline
operator/(ITensor T, Real fac) { T /= fac; return T; }

ITensor inline
operator+(ITensor A, const ITensor& B) { A += B; return A; }

ITensor inline
operator-(ITensor A, const ITensor& B) { A -= B; return A; }

//Return copy of ITensor with primeLevel of Index I increased by 1
//(or optional amount inc)
template <class Tensor, class IndexT>
Tensor
prime(Tensor A, const IndexT& I, int inc = 1)
    { 
    A.prime(I,inc); 
    return A; 
    }

//Return copy of ITensor with primeLevel of Index I set to zero
template <class Tensor, class IndexT>
Tensor
noprime(Tensor A, const IndexT& I)
    { 
    A.noprime(I); 
    return A; 
    }

std::ostream& 
operator<<(std::ostream & s, const ITensor& T);

ITensor
randomize(ITensor T, const OptSet& opts = Global::opts());

//Get scalar value of rank 0 ITensor.
//Throws ITError if r() != 0.
//Throws if imaginary part is non-zero.
//TODO: make an external method
Real
toReal(const ITensor& T);

//Get scalar value of complex rank 0 ITensor.
//Throws ITError if r() != 0
//TODO: make an external method
Complex
toComplex(const ITensor& T);

//Compute the norm of an ITensor.
//Thinking of elements as a vector, equivalent to sqrt(v*v).
//Result is equivalent to sqrt(toReal(T*T)) 
//[and similar for complex case] but computed much more efficiently.
Real 
norm(const ITensor& T);

//TODO: add conj (complex conj)

}; //namespace itensor


#endif
