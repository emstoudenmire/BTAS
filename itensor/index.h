//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_INDEX_H
#define __ITENSOR_INDEX_H
#include "itensor/global.h"
#include <memory>

namespace itensor {

enum IndexType { Link, Site, All };

//Forward declarations
struct IndexDat;
class IndexVal;


//
// Index
//
// Represents a tensor index of fixed bond dimension m.
//
// Can be compared with == operator (returns true if both
// are copies of the same Index instance).
//
// To make an Index distinct from other copies, increase its primeLevel.
//
class Index
    {
    public:

    using RNG = std::mt19937;
    using IDType = RNG::result_type;

    //
    // Constructors
    //

    // For a default constructed Index, isNull() returns true.
    Index();

    // Name of Index is used for printing purposes
    explicit
    Index(const std::string& name, 
          int m = 1, 
          IndexType it = Link, 
          int primelevel = 0);

    //
    // Accessor Methods
    //

    // Return the bond dimension
    int 
    m() const;

    // Return the prime level
    int 
    primeLevel() const;

    // Set the prime level to a specified value.
    Index& 
    primeLevel(int plev);

    // Return the IndexType
    IndexType 
    type() const;

    // Return the name of this Index
    std::string 
    name() const;

    // Return the name of this Index with primes removed
    const std::string&
    rawname() const;

    IDType
    id() const;

    // Returns true if Index default initialized
    bool 
    isNull() const;

    // Returns the Arrow direction of this Index
    Arrow 
    dir() const { return Out; }

    void 
    dir(Arrow ndir) const {  }

    //
    // Prime level methods
    //

    // Increase primelevel by 1 (or by optional amount inc)
    Index& 
    prime(int inc = 1);

    // Increase primelevel by 1 (or optional amount inc)
    // if type matches this Index or type==All
    Index& 
    prime(IndexType type, int inc = 1);

    // Set primelevel to zero (optionally only if type matches)
    Index& 
    noprime(IndexType type = All) { prime(type,-primelevel_); return *this; }

    // Switch primelevel from plevold to plevnew
    // Has no effect if plevold doesn't match current primelevel
    Index& 
    mapprime(int plevold, int plevnew, IndexType type = All);

    //
    // Operators
    //

    // Equality (==) operator
    // True if other Index is a copy of this Index and has same primelevel
    bool 
    operator==(const Index& other) const;

    bool 
    operator!=(const Index& other) const { return !operator==(other); }

    // Check if other Index is a copy of this, ignoring primeLevel
    bool 
    noprimeEquals(const Index& other) const;

    // Useful for sorting Index objects
    bool 
    operator<(const Index& other) const;

    // Creates an IndexVal from this Index with value i
    IndexVal 
    operator()(int i) const;

    //
    // Other methods
    //

    // Conjugate this Index.
    // Currently has no effect; exists for forward compatibility
    // with Arrows and interface compatibility with class IQIndex.
    void 
    conj() { } //for forward compatibility with arrows

    // Write Index to binary output stream.
    void 
    write(std::ostream& s) const;

    // Read Index from binary input stream.
    Index& 
    read(std::istream& s);

    private:

    /////////////
    std::shared_ptr<IndexDat> p;
    int primelevel_; 
    /////////////

    }; //class Index


//
// IndexVal
//
// Class pairing an Index of dimension m
// with a specific value i where 1 <= i <= m
//
class IndexVal : public Index
    {
    public:

    int i;

    IndexVal();

    IndexVal(const Index& index, int i_);

    bool
    operator==(const IndexVal& other) const;

    bool
    operator!=(const IndexVal& other) const;

    };

Index inline
conj(Index res) { res.conj(); return res; }

IndexVal inline
conj(IndexVal res) { res.conj(); return res; }

bool inline
operator==(const Index& i, const IndexVal& iv) { return i==Index(iv); }

bool inline
operator==(const IndexVal& iv, const Index& i) { return operator==(i,iv); }

bool inline
operator!=(const Index& i, const IndexVal& iv) { return !operator==(i,iv); }

bool inline
operator!=(const IndexVal& iv, const Index& i) { return !operator==(i,iv); }

//Return a copy of I, increasing primelevel.
template<class T>
T
prime(T I, int inc = 1) { I.prime(inc); return I; }

//Return a copy of I, increasing primelevel if I.type() == type
template<class T>
T 
prime(T I, IndexType type, int inc = 1) { I.prime(type,inc); return I; }

//Return a copy of I with primelevel set to zero.
template<class T>
T
noprime(T I, IndexType type = All) { I.noprime(type); return I; }

//Return a copy of I with prime level changed to plevnew if
//old prime level was plevold. Otherwise has no effect.
template<class T>
T
mapprime(T I, int plevold, int plevnew, IndexType type = All)
    { I.mapprime(plevold,plevnew,type); return I; }


//Returns a string version of this Index's bond dimension.
std::string
showm(const Index& I);

std::ostream& 
operator<<(std::ostream & s, const Index &t);

std::ostream& 
operator<<(std::ostream& s, const IndexVal& iv);

std::ostream& 
operator<<(std::ostream& s, const IndexType& it);

}; //namespace itensor

#endif
