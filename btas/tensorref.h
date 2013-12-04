#ifndef __BTAS_TENSORREF_H
#define __BTAS_TENSORREF_H 1

#include <btas/tensor.h>
#include <btas/nditerator.h>

#include "btas/range.h"

namespace btas {


//
// Forward declarations of slice functions
//

template<class _Tensor>
class TensorRef;

template <typename _T>
TensorRef<_T>
slice(_T& T, const Range1& r1, const Range1& r2);


//
// class TensorRef
//

/// view of tensor data without ownership
template<class _Tensor>
class TensorRef 
    {
    public:

    //
    //  Concepts derived from _Tensor class
    //

    typedef typename _Tensor::value_type value_type; ///< element type
    typedef typename _Tensor::container_type container_type; ///< type of array storing data
    typedef typename _Tensor::size_type size_type; ///< size type
    typedef typename _Tensor::shape_type shape_type; ///< type of array for index shapes
    typedef NDIterator<value_type*,shape_type> iterator; ///< iterator over elements
    typedef NDIterator<const value_type*,shape_type> const_iterator; ///< const iterator over elements

    public:

    /// \return number of indices (tensor rank)
    size_type
    rank() const { return iter_.shape().size(); }

    /// \return number of elements
    size_type
    size() const { return iter_.size(); }

    /// \return shape object
    const shape_type& shape() const { return iter_.shape(); }

    /// \return n-th shape
    const typename shape_type::value_type&
    shape(const size_type& n) const { return iter_.shape(n); }

    /// \return stride object
    const shape_type&
    stride() const { return stride_; }

    /// \return n-th stride
    const typename shape_type::value_type&
    stride(const size_type& n) const { return stride_[n]; }

    /// test whether storage is empty
    bool
    empty() const { return !iter_.valid(); }

    /// \return const iterator first
    const_iterator
    begin() const { return iter_; }

    /// \return const iterator last
    const_iterator
    end() const { return iter_.end(); }

    /// \return const iterator first even if this is not itself const
    const_iterator
    cbegin() const { return iter_; }

    /// \return const iterator last even if this is not itself const
    const_iterator
    cend() const { return iter_.end(); }

    /// \return iterator first
    iterator
    begin() { return iter_; }

    /// \return iterator last
    iterator
    end() { return iter_.end(); }

    /// copy data elementwise from one TensorRef to another
    TensorRef& 
    operator=(const TensorRef& other)
        {
        //TODO: if contiguous use std::copy
        //if(contig_ && other.contig_) { ... }
        auto t = iter_;
        auto o = other.iter_;
        for(; t.valid(); ++t, ++o)
            {
            *t = *o;
            }
        return *this;
        }

    /// \return element without shape check
    template<typename... _args>
    const value_type& 
    operator() (const size_type& first, const _args&... rest) const
        {
        return *(iter_ + _address<0>(first,rest...));
        }

    /// \return element without shape check (rank() == general)
    const value_type& 
    operator() (const shape_type& index) const
        {
        return *(iter_ + _address(index));
        }

    /// access element without shape check
    template<typename... _args>
    value_type& 
    operator() (const size_type& first, const _args&... rest)
        {
        return *(iter_ + _address<0>(first,rest...));
        }

    /// access element without shape check (rank() == general)
    value_type& 
    operator() (const shape_type& index)
        {
        return *(iter_ + _address(index));
        }

    /// \return element without shape check
    template<typename... _args>
    const value_type& 
    at (const size_type& first, const _args&... rest) const
        {
        assert(_check_range<0>(first, rest...));
        return *(iter_ + _address<0>(first, rest...));
        }

    /// \return element without shape check (rank() == general)
    const value_type& 
    at (const shape_type& index) const
        {
        assert(_check_range(index));
        return *(iter_ + _address(index));
        }

    /// access element without shape check
    template<typename... _args>
    value_type& 
    at (const size_type& first, const _args&... rest)
        {
        assert(_check_range<0>(first, rest...));
        return *(iter_ + _address<0>(first, rest...));
        }

    /// access element without shape check (rank() == general)
    value_type& 
    at (const shape_type& index)
        {
        assert(_check_range(index));
        return *(iter_ + _address(index));
        }

    /// swap this and x
    void 
    swap(TensorRef& x)
        {
        iter_.swap(x.iter_);
        std::swap(contig_,x.contig_);
        }

    /// \return bare const pointer to the first element of data_
    /// this enables to call BLAS functions
    const value_type*
    data() const
        {
        return iter_.start();
        }

    /// \return bare pointer to the first element of data_
    /// this enables to call BLAS functions
    value_type* 
    data()
        {
        return iter_.start();
        }

    /// fill all elements by val
    void
    fill(const value_type& val)
        {
        std::fill(iter_, iter_.end(), val);
        }

    /// generate all elements by gen()
    template<class Generator>
    void 
    generate(Generator gen)
        {
        std::generate(iter_, iter_.end(), gen);
        }

    private:

    //
    //  Constructors
    //
    //  TensorRef has private constructors and assignment
    //  to prevent it from outliving the Tensor data it references.
    //

    /// default constructor
    TensorRef() { }

    /// construct from Tensor
    explicit
    TensorRef(const _Tensor& T)
        : 
        iter_(T.data(),T.shape(),T.stride()),
        stride_(T.stride()),
        contig_(true)
        { }

    /// construct from NDIterator
    explicit
    TensorRef(const iterator& iter)
        : 
        iter_(iter)
        { 
        _set_stride();
        __check_contig();
        }

    /// move construct from NDIterator
    TensorRef(iterator&& iter)
        : iter_(iter)
        { 
        _set_stride();
        __check_contig();
        }

    /// move assign from NDIterator
    TensorRef&
    operator=(iterator&& iter)
        {
        iter_.swap(iter);
        _set_stride();
        __check_contig();
        }

    /// assign from Tensor
    TensorRef&
    operator=(const _Tensor& T)
        {
        iter_ = iterator(T.data(),T.shape(),T.stride());
        stride_ = T.stride();
        contig_ = true;
        return *this;
        }


    // copy constructor not implemented to avoid extending lifetime
    TensorRef(const TensorRef&);


    //
    //  Supporting functions
    //

    /// \return address pointed by index
    template<size_type i, typename... _args>
    size_type
    _address (const size_type& first, const _args&... rest) const
        {
        return first*iter_.stride(i) + _address<i+1>(rest...);
        }

    /// \return address pointed by index
    template<size_type i>
    size_type
    _address (const size_type& first) const
        {
        return first*stride(i);
        }

    /// \return address pointed by index
    size_type
    _address (const shape_type& index) const
        {
        assert(index.size() == rank());
        size_type adr = 0;
        for(size_type i = 0; i < rank(); ++i) 
            {
            adr += stride(i)*index[i];
            }
        return adr;
        }

   /// calculate stride_ from given shape_
   void
   _set_stride ()
        {
        stride_.resize(rank());
        size_type str = 1;
        for(size_type i = rank()-1; i > 0; --i) 
            {
            stride_[i] = str;
            str *= shape(i);
            }
        stride_[0] = str;
        }

    /// test whether index is in range
    template<size_type i, typename... _args>
    bool
    _check_range (const size_type& first, const _args&... rest) const
        {
        return (first >= 0 && first < shape(i) && _check_range<i+1>(rest...));
        }

    /// test whether index is in range
    template<size_type i>
    bool
    _check_range (const size_type& first) const
        {
        return (first >= 0 && first < shape(i));
        }

    /// test whether index is in range
    bool
    _check_range (const shape_type& index)
        {
        assert(index.size() == rank());
        typename shape_type::iterator r = shape().begin();
        return std::all_of(index.begin(), index.end(), [&r] (const typename shape_type::value_type& i) { return (i >= 0 && i < *r++); });
        }

    bool
    __check_contig() const
        {
        //TODO
        return false;
        }

    ///////////////////////
    // Data members

    iterator iter_; ///< iterator over referenced data
    shape_type stride_; ///< stride
    bool contig_; ///< true if data referenced is contiguous in memory

    ///////////////////////

    public:

    //
    // slice function declarations of friendship
    //

    friend TensorRef<_Tensor>
    slice <> (_Tensor& T, const Range1& r1, const Range1& r2);

    };


} // namespace btas

#endif // __BTAS_TENSORREF_H
