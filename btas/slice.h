#ifndef __BTAS_SLICE_H
#define __BTAS_SLICE_H 1

#include "btas/range.h"
#include "btas/tensorref.h"

namespace btas {

/// construct TensorRef representing the diagonal elements of T, that is, T(i,i,i,i)
template <class _Tensor>
TensorRef<_Tensor>
diag(_Tensor& T)
    {
    typedef typename TensorRef<_Tensor>::size_type size_type;
    typedef typename TensorRef<_Tensor>::iterator NDIter;

    size_type stride = 0,
              shape = (size_type)-1;
    for(size_type i = 0; i < T.rank(); ++i)
        {
        stride += T.stride(i);
        shape = std::min(shape,T.shape(i));
        }
    return make_ref<_Tensor>( NDIter(T.data(),{shape},{stride}) );
    }

/// construct TensorRef "fusing" the specified indices of T
/// for example tie(T,0,2)(i,j) = T(i,j,i)
template <class _Tensor, typename... _args>
TensorRef<_Tensor>
tie(_Tensor& T, 
    const typename _Tensor::size_type& i0, 
    const typename _Tensor::size_type& i1, 
    const _args&... rest)
    {
    typedef typename TensorRef<_Tensor>::shape_type shape_type;
    typedef typename TensorRef<_Tensor>::size_type size_type;
    typedef typename TensorRef<_Tensor>::iterator NDIter;

    const auto size = 2 + sizeof...(rest);
    size_type inds[size] = { i0, i1, static_cast<size_type>(rest)...};

    auto new_r = (1+T.rank())-size;
    size_type tpos = T.rank()+1,
              tstride = 0,
              tshape = (size_type)-1;
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

    return make_ref<_Tensor>( NDIter(T.data(),tied_shape,tied_stride) );
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
    typedef typename TensorRef<_Tensor>::iterator NDIter;

    const auto size = 1 + sizeof...(rest);
    Range1 ranges[size] = { r0, rest...};
    shape_type slice_shape(size),
               slice_stride(size);
    size_type offset = 0;
    for(size_type i = 0; i < size; ++i)
        {
        slice_shape[i] = ranges[i].size();
        slice_stride[i] = T.stride(i)*ranges[i].stride();
        offset += T.stride(i)*ranges[i].start();
        }

    return make_ref<_Tensor>( NDIter(T.data()+offset,slice_shape,slice_stride) );
    }




} // namespace btas

#endif // __BTAS_SLICE_H
