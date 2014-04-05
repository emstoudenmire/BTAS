//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITENSOR_H
#define __ITENSOR_ITENSOR_H
#include "itensor/real.h"
#include "itensor/indexset.h"
#include "btas/tensor.h"

#define Cout std::cout
#define Endl std::endl
#define Format boost::format

namespace itensor {

//
// ITensor
//
class ITensor
    {
    public:

    using storage = btas::Tensor<Real>;
    using storage_ptr = std::shared_ptr<storage>;

    //
    //Accessor Methods
    //

    //Rank of this ITensor (number of indices)
    int 
    r() const { return is_.r(); }

    //true if ITensor is default constructed
    bool 
    empty() const { return !bool(r_); }

    //Enables looping over Indices in a Foreach
    //e.g. Foreach(const Index& I, t.index() ) { ... }
    const IndexSet<Index>&
    indices() const { return is_; }

    //Read-only access to scale factor, used internally for efficient scalar ops
    const LogNumber&
    scale() const { return scale_; }

    //
    //Constructors
    //

    //Construct Null ITensor, isNull returns true
    ITensor();

    //Construct rank 1 ITensor, all entries set to zero
    explicit 
    ITensor(const Index& i1);

    //Construct rank 2 ITensor, all entries set to zero
    ITensor(const Index& i1,const Index& i2);

    //Construct ITensor up to rank 8, entries set to zero
    ITensor(const Index& i1, const Index& i2, const Index& i3,
            const Index& i4 = Index::Null(),
            const Index& i5 = Index::Null(),
            const Index& i6 = Index::Null(),
            const Index& i7 = Index::Null(),
            const Index& i8 = Index::Null());

    //Construct rank 0 ITensor (scalar), value set to val
    explicit
    ITensor(Real val);

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
    operator/=(Real fac) { scale_ /= fac; return *this; }

    ITensor
    operator-() const { ITensor T(*this); T.scale_ *= -1; return T; }

    //Tensor addition and subtraction
    //Summands must have same Indices, in any order
    //Cijk = Aijk + Bkij
    ITensor& 
    operator+=(const ITensor& other);

    ITensor& 
    operator-=(const ITensor& other);


    //
    //Primelevel Methods
    //

    //Set primeLevel of Indices to zero
    ITensor& 
    noprime(IndexType type = All) { is_.noprime(type); return *this; }

    //Set primeLevel of Index I to zero
    ITensor& 
    noprime(const Index& I) { is_.noprime(I); return *this; }

    //Increase primeLevel of Indices by 1 (or optional amount inc)
    ITensor& 
    prime(int inc = 1) { prime(All,inc); return *this;}

    //Increase primeLevel of Indices by 1 (or optional amount inc)
    ITensor& 
    prime(IndexType type, int inc = 1) { is_.prime(type,inc); return *this; }

    //Increase primeLevel of Index I by 1 (or optional amount inc)
    ITensor& 
    prime(const Index& I, int inc = 1) { is_.prime(I,inc); return *this; }

    //Change all Indices having primeLevel plevold to have primeLevel plevnew
    ITensor& 
    mapprime(int plevold, int plevnew, IndexType type = All)
        { is_.mapprime(plevold,plevnew,type); return *this; }

    //
    //Element Access Methods
    //

    //Get scalar value of rank 0 ITensor
    //Throws ITError if r() != 0
    Real
    toReal() const;

    // IndexVal element access
    // Given iv1 = (I1,n1), iv2 = (I2,n2), ...
    // returns component of ITensor such that
    // I1 temporarily set to n1, I2 to n2, etc.
    // Can be used to set components of ITensors
    // as well, for example, T(I1(2),I2(1)) = 3;
    Real& 
    operator()(const IndexVal& iv1);

    Real 
    operator()(const IndexVal& iv1) const;

    Real& 
    operator()(const IndexVal& iv1, const IndexVal& iv2);

    Real 
    operator()(const IndexVal& iv1, const IndexVal& iv2) const;

    Real& 
    operator()(const IndexVal& iv1, const IndexVal& iv2, const IndexVal& iv3);

    Real 
    operator()(const IndexVal& iv1, const IndexVal& iv2, const IndexVal& iv3) const;

    //
    //Methods for Mapping to Other Objects
    //

    //
    // groupIndices combines a set of indices (of possibly different sizes) 
    // together, leaving only single grouped Index.
    //
    // RiJ = Ai(jk) <-- Here J represents the grouped pair of indices (jk)
    //                  If j.m() == 5 and k.m() == 7, J.m() == 5*7.
    //
    void 
    groupIndices(const std::array<Index,NMAX+1>& indices, int nind, 
                      const Index& grouped, ITensor& res) const;

    //
    // tieIndices locks a set of indices (of the same size) together,
    // leaving only a single tied Index.
    //
    // Rijl = Aijil <-- here we have tied the 1st and 3rd index of A
    //
    void
    tieIndices(const std::array<Index,NMAX>& indices, int nind,
               const Index& tied);

    void
    tieIndices(const Index& i1, const Index& i2,
               const Index& tied);


    void
    tieIndices(const Index& i1, const Index& i2,
               const Index& i3,
               const Index& tied);


    void
    tieIndices(const Index& i1, const Index& i2,
               const Index& i3, const Index& i4,
               const Index& tied);


    // The trace method sums over the given set of indices
    // (which must all have the same dimension).
    //
    // Rik = Aijkml.trace(j,l,m) = \sum_t Aitktt
    ITensor&
    trace(const Index& i1, 
          const Index& i2 = Index::Null(), 
          const Index& i3 = Index::Null(),
          const Index& i4 = Index::Null(),
          const Index& i5 = Index::Null(),
          const Index& i6 = Index::Null(),
          const Index& i7 = Index::Null(),
          const Index& i8 = Index::Null());

    ITensor&
    trace(const std::array<Index,NMAX>& indices, int nind = -1);


    //
    // expandIndex replaces a smaller index with a bigger one, padding out
    // the elements of the resulting ITensor with zeros as necessary.
    // Say we have a tensor Aij and j has range m. Now expand j with 
    // a larger Index J. The result is RiJ, where
    //        _
    // RiJ = |  Ai(j=J-start+1) for J = start...start+m
    //       |_ 0               otherwise
    //        
    void 
    expandIndex(const Index& small, const Index& big, int start);

    void
    swap(ITensor& other);

    //TODO remove this test
    //ITensor
    //testMethod(const std::array<Index,NMAX>& indices, int nind,
    //           const Index& tied) const;


    //Other Methods -------------------------------------------------

    const Real*
    data() const;

    void 
    randomize(const OptSet& opts = Global::opts());

    Real 
    norm() const;

    LogNumber 
    normLogNum() const;

    Real 
    normNoScale() const;

    void 
    scaleTo(const LogNumber& newscale);

    //
    // Typedefs
    //

    typedef Index 
    IndexT;

    typedef IndexVal 
    IndexValT;

    private:

    //////////////

    //Pointer to ITDat containing tensor data
    storage_ptr r_; //real part

    //Indices, maximum of 8
    IndexSet<Index> is_;

    //scale_ absorbs scalar factors to avoid copying ITDat
    LogNumber scale_; 

    //////////////

    void 
    allocate(int dim);
    void 
    allocate();

    //Disattach self from current ITDat and create own copy instead.
    //Necessary because ITensors logically represent distinct
    //objects even though they may share data
    void 
    solo();

    void
    equalizeScales(ITensor& other);

    friend std::ostream& 
    operator<<(std::ostream & s, const ITensor& T);

    }; // class ITensor

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
    { A.prime(I,inc); return A; }

//Return copy of ITensor with primeLevel of Index I set to zero
template <class Tensor, class IndexT>
Tensor
deprime(Tensor A, const IndexT& I)
    { A.noprime(I); return A; }

std::ostream& 
operator<<(std::ostream & s, const ITensor& T);

}; //namespace itensor

#undef Cout
#undef Endl
#undef Format

#endif
