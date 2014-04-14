//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_INDEXSET_H
#define __ITENSOR_INDEXSET_H
#include "itensor/index.h"
#include "itensor/detail/iterpair.h"

namespace itensor {

template <class IndexT>
class IndexSetT;

class IQIndex;

using IndexSet = IndexSetT<Index>;
using IQIndexSet = IndexSetT<IQIndex>;

//
// IndexSetT
//

template <class IndexT>
class IndexSetT
    {
    public:

    using storage = std::vector<IndexT>;
    using const_iterator = typename storage::const_iterator;
    using IPair = detail::IterPair<const_iterator>;

    IndexSetT();

    explicit
    IndexSetT(const IndexT& i1);

    IndexSetT(const IndexT& i0, const IndexT& i1);

    template<typename... Args>
    IndexSetT(const IndexT& i0, const IndexT& i1, const Args&... rest);

    template <class Iterable> 
    explicit
    IndexSetT(const Iterable& ii, int size = -1);

    //Move-construct IndexSetT directly from storage container.
    //Assumes container already sorted with rn m>1 indices at
    //the front, all m==1 indices at the back.
    IndexSetT(storage&& v, int rn);

    //
    // Accessor Methods
    //

    int
    r() const { return index_.size(); }

    int
    rn() const { return rn_; }

    IPair
    nInds() const { return IPair(index_.begin(),index_.begin()+rn_); }

    IPair
    m1Inds() const { return IPair(index_.begin()+rn_,index_.end()); }

    const IndexT&
    operator[](int j) const;

    int
    dim() const;

    IndexT
    front() const;

    IndexT
    back() const;

    const_iterator
    begin() const { return index_.begin(); }

    const_iterator
    end() const { return index_.end(); }

    operator const storage&() const { return index_; }

    Real
    uniqueReal() const { return ur_; }

    bool
    operator==(const IndexSetT& other) const
        { return fabs(ur_ - other.ur_) <= UniqueRealAccuracy; }

    bool
    operator!=(const IndexSetT& other) const
        { return fabs(ur_ - other.ur_) > UniqueRealAccuracy; }

    bool
    operator<(const IndexSetT& other) const { return ur_ < other.ur_; }

    //
    // Primelevel Methods
    //

    void 
    prime(int inc = 1) { prime(All,inc); }

    void 
    prime(IndexType type, int inc = 1);

    void 
    prime(const IndexT& I, int inc = 1);

    void 
    noprime(IndexType type = All);

    void 
    noprime(const IndexT& I);

    void 
    mapprime(int plevold, int plevnew, IndexType type = All);

    //
    // Other Methods
    //

    void
    swap(IndexSetT& other);

    void
    clear();

    void
    conj();

    void
    read(std::istream& s);

    void
    write(std::ostream& s) const;

    //using Ptr = typename std::shared_ptr<IndexSetT<IndexT>>;
    //static const Ptr& Null()
    //    {
    //    static Ptr Null_ = std::make_shared<IndexSetT<IndexT>>();
    //    return Null_;
    //    }

    private:

    //////////

    storage index_;
    int rn_;
    Real ur_;

    /////////

    void
    setUniqueReal();

    template <class Iterable>
    void
    sortIndices(const Iterable& I, int size);

    };

template<class IndexT>
IndexSetT<IndexT>::
IndexSetT()
    :
    rn_(0),
    ur_(0)
    { }

template<class IndexT>
IndexSetT<IndexT>::
IndexSetT(const IndexT& i1)
    :
    index_(1,i1),
    rn_((i1.m() == 1 ? 0 : 1)),
    ur_(i1.uniqueReal())
    { 
#ifdef DEBUG
    if(i1 == IndexT::Null())
        Error("i1 is null");
#endif
    }

template<class IndexT>
IndexSetT<IndexT>::
IndexSetT(const IndexT& i1, const IndexT& i2)
    :
    index_(2),
    ur_(i1.uniqueReal() + i2.uniqueReal())
    { 
#ifdef DEBUG
    if(i1 == IndexT::Null())
        Error("i1 is null");
    if(i2 == IndexT::Null())
        Error("i2 is null");
#endif
	if(i1.m()==1) 
	    {
	    index_[0] = i2; 
        index_[1] = i1; 
	    rn_ = (i2.m() == 1 ? 0 : 1);
	    }
	else 
	    { 
	    index_[0] = i1; 
        index_[1] = i2; 
	    rn_ = (i2.m() == 1 ? 1 : 2); 
	    }
    }

template<class IndexT>
template<typename... Args>
IndexSetT<IndexT>::
IndexSetT(const IndexT& i0,
         const IndexT& i1,
         const Args&... rest)
    { 
    const auto size = 2 + sizeof...(rest);
    std::array<IndexT,size> inds = { i0, i1, static_cast<IndexT>(rest)...};
    sortIndices(inds,size,0);
    setUniqueReal();
    }

template <class IndexT>
template <class Iterable>
IndexSetT<IndexT>::
IndexSetT(const Iterable& ii, int size)
    { 
    sortIndices(ii,(size < 0 ? ii.size() : size));
    setUniqueReal();
    }

template <class IndexT>
IndexSetT<IndexT>::
IndexSetT(storage&& v, int rn)
    :
    index_(v),
    rn_(rn)
    {
    setUniqueReal();
#ifdef DEBUG
    int count_n = 0;
    for(const auto& i : index_)
        {
        if(i.m()==1) break;
        ++count_n;
        }
    if(rn_ != count_n) 
        {
        printn("rn_ = %d, count_n = %d",rn_,count_n);
        Error("Wrong rn passed to IndexSet constructor.");
        }
    for(int j = count_n; j < int(index_.size()); ++j)
        {
        if(index_[j].m()!=1) Error("Unsorted indices passed to IndexSet constructor");
        }
#endif
    }

//template <class IndexT>
//template <class Container1, class Container2> 
//IndexSetT<IndexT>::
//IndexSetT(const Container1& mg1_inds, size_t size1,
//         const Container2& meq1_inds, size_t size2)
//    :
//    index_(size1+size2)
//    {
//    }

template <class IndexT>
const IndexT& IndexSetT<IndexT>::
operator[](int j) const 
    { 
#ifdef DEBUG
    return index_.at(j); 
#else
    return index_[j]; 
#endif
    }

template <class IndexT>
int IndexSetT<IndexT>::
dim() const
    {   
    int d = 1;
    for(int j = 0; j < rn_; ++j)
        d *= index_[j].m();
    return d;
    }

template <class IndexT>
IndexT IndexSetT<IndexT>::
front() const
    {
#ifdef DEBUG
    if(index_.empty())
        Error("Empty IndexSetT");
#endif
    return index_.front();
    }

template <class IndexT>
IndexT IndexSetT<IndexT>::
back() const
    {
#ifdef DEBUG
    if(index_.empty())
        Error("Empty IndexSetT");
#endif
    return index_.back();
    }

template <class IndexT>
void IndexSetT<IndexT>::
noprime(IndexType type)
    {
    ur_ = 0;
    for(size_t j = 0; j < index_.size(); ++j)
        {
#ifdef DEBUG
        //Check if calling noprime is ok
        //Error if it causes duplicate indices
        if(type == All || index_[j].type() == type)
            {
            for(size_t k = 0; k < index_.size(); ++k)
                {
                if(type != All && index_[k].type() != type) continue;
                if(k != j && index_[j].noprimeEquals(index_[k]))
                    {
                    //PrintVar(*this);
                    throw ITError("Calling noprime would lead to duplicate indices");
                    }
                }
            }
#endif
        index_[j].noprime(type);
        ur_ += index_[j].uniqueReal();
        }
	}

template <class IndexT>
void IndexSetT<IndexT>::
noprime(const IndexT& I)
    {
    size_t j = (I.m() == 1 ? rn_ : 0);
    for(; j < index_.size(); ++j) 
        {
        if(index_[j] == I)
            {
#ifdef DEBUG
            //Check if calling noprime is ok
            //Error if it causes duplicate indices
            for(size_t k = 0; k < index_.size(); ++k)
                {
                if(k != j && index_[j].noprimeEquals(index_[k]))
                    {
                    throw ITError("Calling noprime leads to duplicate indices");
                    }
                }
#endif
            index_[j].noprime();
            setUniqueReal();
            return;
            }
        }
    PrintVar(*this);
    PrintVar(I);
    Error("IndexSetT::prime: index not found.");
    }

template <class IndexT>
void IndexSetT<IndexT>::
prime(IndexType type, int inc)
	{
    ur_ = 0;
    for(auto& J : index_)
        {
        J.prime(type,inc);
        ur_ += J.uniqueReal();
        }
	}

template <class IndexT>
void IndexSetT<IndexT>::
prime(const IndexT& I, int inc)
    {
#ifdef DEBUG
    if(I.isNull())
        {
        Error("Request to prime null index");
        }
#endif
    for(size_t j = (I.m() == 1 ? rn_ : 0); j < index_.size(); ++j) 
        if(index_[j] == I)
        {
        index_[j].prime(inc);
        setUniqueReal();
        return;
        }
    PrintVar(*this);
    PrintVar(I);
    Error("IndexSetT::prime: index not found.");
    }

template <class IndexT>
void IndexSetT<IndexT>::
mapprime(int plevold, int plevnew, IndexType type)
	{
    ur_ = 0;
    for(auto& J : index_)
        {
        J.mapprime(plevold,plevnew,type);
        ur_ += J.uniqueReal();
        }
	}


//
// Methods for Manipulating IndexSetTs
//

template <class IndexT>
void IndexSetT<IndexT>::
setUniqueReal()
	{
    ur_ = 0;
    for(const auto& J : index_)
        ur_ += J.uniqueReal();
	}

template <class IndexT>
void IndexSetT<IndexT>::
swap(IndexSetT& other)
    {
    index_.swap(other.index_);
    std::swap(rn_,other.rn_);
    std::swap(ur_,other.ur_);
    }

template <class IndexT>
void IndexSetT<IndexT>::
clear()
    {
    index_.clear();
    rn_ = 0;
    ur_ = 0;
    }

template <class IndexT>
void IndexSetT<IndexT>::
conj()
    {
    for(auto& J : index_)
        J.conj();
    }

template <class IndexT>
void IndexSetT<IndexT>::
read(std::istream& s)
    {
    size_t size = 0;
    s.read((char*) &size,sizeof(size));
    s.read((char*) &rn_,sizeof(rn_));
    ur_ = 0;
    index_.resize(size);
    for(size_t j = 0; j < size; ++j) 
        {
        index_[j].read(s);
        ur_ += index_[j].uniqueReal();
        }
    }

template <class IndexT>
void IndexSetT<IndexT>::
write(std::ostream& s) const
    {
    size_t size = index_.size();
    s.write((char*) &size,sizeof(size));
    s.write((char*) &rn_,sizeof(rn_));
    for(const auto& J : index_)
        J.write(s);
    }

template <class IndexT>
template <class Iterable>
void IndexSetT<IndexT>::
sortIndices(const Iterable& I, int size)
    {
    index_.resize(size);

    rn_ = 0;
    auto j = size-1;

    for(const auto& i : I)
        {
        if(i.m() > 1) 
            {
            index_[rn_] = i; 
            ++rn_;
            }
        else
            {
            index_[j] = i;
            --j;
            }
        }
    }

//
//
// IndexSetT helper methods
//
//

template<class IndexT>
Arrow
dir(const IndexSetT<IndexT>& is, const IndexT& I)
    {
    for(int j = 0; j < is.r(); ++j)
        {
        if(is[j] == I) 
            return is[j].dir();
        }
    Error("dir: Index not found");
    return In;
    }


template <class IndexT>
const IndexT&
findDir(const IndexSetT<IndexT>& iset, Arrow dir)
    {
    for(int j = 0; j < iset.r(); ++j)
        if(iset[j].dir() == dir) return iset[j];
    Error("Couldn't find index with specified dir");
    return IndexT::Null();
    }

//
// Given IndexSetT<IndexT> iset and IndexT I,
// return int j such that iset[j] == I.
// If not found, throws an ITError.
//
template <class IndexT>
int
findIndex(const IndexSetT<IndexT>& iset, const IndexT& I)
    {
    int j = (I.m()==1 ? iset.rn() : 0);
    for(; j < iset.r(); ++j)
        {
        if(iset[j] == I) return j;
        }
    PrintVar(I);
    Error("Index I not found");
    return 0;
    }

template <class IndexT>
const IndexT&
findtype(const IndexSetT<IndexT>& iset, IndexType t)
	{
    for(int j = 0; j < iset.r(); ++j)
        if(iset[j].type() == t) return iset[j];
    Error("findtype failed."); 
    return IndexT::Null();
	}


template <class IndexT>
bool
hasIndex(const IndexSetT<IndexT>& iset, const IndexT& I)
	{
    int j = (I.m()==1 ? iset.rn() : 0);
    for(; j < iset.r(); ++j)
        {
        if(iset[j] == I) return true;
        }
    return false;
	}

template <class IndexT>
bool
hasType(const IndexSetT<IndexT>& iset, IndexType t)
	{
    for(int j = 0; j < iset.r(); ++j)
        if(iset[j].type() == t) return true;
    return false;
	}

template <class IndexT>
int
minM(const IndexSetT<IndexT>& iset)
    {
    if(iset.rn() < iset.r()) return 1;

    int mm = iset[0].m();
    for(int j = 1; j < iset.rn(); ++j)
        mm = min(mm,iset[j].m());

    return mm;
    }

template <class IndexT>
int
maxM(const IndexSetT<IndexT>& iset)
    {
    if(iset.rn() == 0) return 1;

    int mm = iset[0].m();
    for(int j = 1; j < iset.rn(); ++j)
        mm = max(mm,iset[j].m());

    return mm;
    }

template <class IndexT>
std::ostream&
operator<<(std::ostream& s, const IndexSetT<IndexT>& is)
    {
    for(const auto& i : is) s << i << "\n"; 
    return s;
    }

template <> inline
std::ostream&
operator<<(std::ostream& s, const IndexSetT<Index>& is)
    {
    int i = 0; 
    for(; i < is.r()-1; ++i) { s << is[i] << ", "; } 
    if(is.r() != 0) { s << is[i]; } //print last one
    return s;
    }


}; //namespace itensor

#endif
