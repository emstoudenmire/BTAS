#ifndef __BTAS_SLICE_H
#define __BTAS_SLICE_H 1

#include "btas/range.h"
#include "btas/tensorref.h"

namespace btas {

/// construct view of tensor T with permuted indices
/// defining std::vector<size_t> perm = {2,0,1};
/// calling P = permute(T,perm) implies T(i,j,k) == P(k,i,j)
template <class _Tensor, class ArrayType>
TensorRef<_Tensor>
permute(_Tensor& T, 
        const ArrayType& perm)
    {
    typedef typename TensorRef<_Tensor>::shape_type shape_type;

    shape_type pshape(T.rank()),
               pstride(T.rank());
    for(size_t i = 0; i < T.rank(); ++i)
        {
        pshape[i] = T.shape(perm[i]);
        pstride[i] = T.stride(perm[i]);
        }
    return make_ref<_Tensor>( NDIterator<_Tensor>(pshape,pstride,T.begin()) );
    }

/// construct view of tensor T with permuted indices
/// calling P = permute(T,2,0,1) implies T(i,j,k) == P(k,i,j)
template <class _Tensor, typename... _args>
TensorRef<_Tensor>
permute(_Tensor& T, 
    const typename _Tensor::size_type& i0, 
    const _args&... rest)
    {
    typedef typename TensorRef<_Tensor>::size_type size_type;
    const auto size = 1 + sizeof...(rest);
    std::array<size_type,size> perm = { i0, static_cast<size_type>(rest)...};
    return permute(T,perm);
    }

/// construct TensorRef representing the diagonal elements of T, that is, T(i,i,i,i)
template <class _Tensor>
TensorRef<_Tensor>
diag(_Tensor& T)
    {
    typedef typename TensorRef<_Tensor>::size_type size_type;

    size_type stride = 0,
              shape = (size_type)-1;
    for(size_type i = 0; i < T.rank(); ++i)
        {
        stride += T.stride(i);
        shape = std::min(shape,T.shape(i));
        }
    return make_ref<_Tensor>( NDIterator<_Tensor>({shape},{stride},T.begin()) );
    }

//template <class _Tensor>
//TensorRef<_Tensor>
//diag(TensorRef<_Tensor>&& T)
//    {
//    typedef typename TensorRef<_Tensor>::size_type size_type;
//
//    size_type stride = 0,
//              shape = (size_type)-1;
//    for(size_type i = 0; i < T.rank(); ++i)
//        {
//        stride += T.begin().stride(i);
//        shape = std::min(shape,T.begin().shape(i));
//        }
//    return make_ref<_Tensor>( NDIterator<_Tensor>({shape},{stride},begin(T.begin())) );
//    }

/// construct TensorRef "fusing" the specified indices of T
/// passed as an array, for example:
/// std::vector<std::size_t> inds = { 0, 2 };
/// tie(T,inds,inds.size())(i,j) = T(i,j,i)
template <class _Tensor, typename ArrayType>
TensorRef<_Tensor>
tieIndex(_Tensor& T, 
         const ArrayType& inds)
    {
    typedef typename TensorRef<_Tensor>::shape_type shape_type;
    typedef typename TensorRef<_Tensor>::size_type size_type;

    size_type size = inds.size();
    auto new_r = (1+T.rank())-size;
    size_type tpos = T.rank()+1,
              tstride = 0,
              tshape = T.shape(0);
    shape_type tied_shape(new_r),
               tied_stride(new_r);

    for(auto i : inds)
        {
        tpos = std::min(tpos,i);
        tstride += T.stride(i);
        tshape = std::min(tshape,T.shape(i));
        }
    if(tpos >= new_r) tpos = new_r-1;

    for(size_type k = 0, nk = 0; nk < new_r; ++k)
        {
        if(nk == tpos)
            {
            tied_shape[tpos] = tshape;
            tied_stride[tpos] = tstride;
            ++nk;
            continue;
            }

        bool tied = false;
        for(auto i : inds) 
            { 
            if(k == i) 
                {
                tied = true;
                break;
                }
            }

        if(!tied)
            {
            tied_shape[nk] = T.shape(k);
            tied_stride[nk] = T.stride(k);
            ++nk;
            }
        }

    return make_ref<_Tensor>( NDIterator<_Tensor>(tied_shape,tied_stride,T.begin()) );
    }

/// construct TensorRef "fusing" the specified indices of T
/// for example tie(T,0,2)(i,j) = T(i,j,i)
template <class _Tensor, typename... _args>
TensorRef<_Tensor>
tieIndex(_Tensor& T, 
    const typename _Tensor::size_type& i0, 
    const typename _Tensor::size_type& i1, 
    const _args&... rest)
    {
    typedef typename TensorRef<_Tensor>::size_type size_type;
    const auto size = 2 + sizeof...(rest);
    std::array<size_type,size> inds = { i0, i1, static_cast<size_type>(rest)...};
    return tieIndex(T,inds);
    }


//
// Slice - restrict tensor ranges to only subranges possibly with strides other than 1
// 

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0)
    {
    return slice_impl(T,r0);
    }

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0,
      const Range1& r1)
    {
    return slice_impl(T,r0,r1);
    }

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0,
      const Range1& r1,
      const Range1& r2)
    {
    return slice_impl(T,r0,r1,r2);
    }

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0,
      const Range1& r1,
      const Range1& r2,
      const Range1& r3)
    {
    return slice_impl(T,r0,r1,r2,r3);
    }

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0,
      const Range1& r1,
      const Range1& r2,
      const Range1& r3,
      const Range1& r4)
    {
    return slice_impl(T,r0,r1,r2,r3,r4);
    }

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0,
      const Range1& r1,
      const Range1& r2,
      const Range1& r3,
      const Range1& r4,
      const Range1& r5)
    {
    return slice_impl(T,r0,r1,r2,r3,r4,r5);
    }

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0,
      const Range1& r1,
      const Range1& r2,
      const Range1& r3,
      const Range1& r4,
      const Range1& r5,
      const Range1& r6)
    {
    return slice_impl(T,r0,r1,r2,r3,r4,r5,r6);
    }

template <class _Tensor>
TensorRef<_Tensor>
slice(_Tensor& T, 
      const Range1& r0,
      const Range1& r1,
      const Range1& r2,
      const Range1& r3,
      const Range1& r4,
      const Range1& r5,
      const Range1& r6,
      const Range1& r7)
    {
    return slice_impl(T,r0,r1,r2,r3,r4,r5,r6,r7);
    }

template <class _Tensor, typename... _args>
TensorRef<_Tensor>
slice_impl(_Tensor& T, const Range1& r0, const _args&... rest)
    {
    typedef typename TensorRef<_Tensor>::shape_type shape_type;
    typedef typename TensorRef<_Tensor>::size_type size_type;

    const auto size = 1 + sizeof...(rest);
    std::array<Range1,size> ranges = { r0, static_cast<Range1>(rest)... };
    shape_type slice_shape(size),
               slice_stride(size);
    size_type offset = 0;
    for(size_type i = 0; i < size; ++i)
        {
        if(ranges[i].size() == 0)
            {
            slice_shape[i] = T.shape(i);
            slice_stride[i] = T.stride(i);
            }
        else
            {
            slice_shape[i] = ranges[i].size();
            slice_stride[i] = T.stride(i)*ranges[i].stride();
            offset += T.stride(i)*ranges[i].start();
            }
        }

    return make_ref<_Tensor>( NDIterator<_Tensor>(slice_shape,slice_stride,T.begin()+offset) );
    }




} // namespace btas

#endif // __BTAS_SLICE_H
