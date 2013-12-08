#ifndef __BTAS_GEMM_IMPL_H
#define __BTAS_GEMM_IMPL_H 1

#include <algorithm>
#include <numeric>
#include <iterator>
#include <type_traits>

#include <btas/tensor_traits.h>
#include <btas/types.h>

#include <btas/generic/numeric_type.h>
#include <btas/generic/tensor_iterator_wrapper.h>

#include <btas/generic/scal_impl.h>

namespace btas {

template<bool _Finalize> struct gemm_impl { };

template<> struct gemm_impl<true>
{
   typedef unsigned long size_type;

   /// GEMM implementation
   template<typename _T, class _IteratorA, class _IteratorB, class _IteratorC>
   static void call (
      const CBLAS_ORDER& order,
      const CBLAS_TRANSPOSE& transA,
      const CBLAS_TRANSPOSE& transB,
      const unsigned long& Msize,
      const unsigned long& Nsize,
      const unsigned long& Ksize,
      const _T& alpha,
            _IteratorA itrA,
      const unsigned long& LDA,
            _IteratorB itrB,
      const unsigned long& LDB,
      const _T& beta,
            _IteratorC itrC,
      const unsigned long& LDC)
   {
      // For column-major order, recall this as C^T = B^T * A^T in row-major order
      if (order == CblasColMajor)
      {
         gemm_impl<true>::call(CblasRowMajor, transB, transA, Nsize, Msize, Ksize, alpha, itrB, LDB, itrA, LDA, beta, itrC, LDC);

         return;
      }

      if (beta != NumericType<_T>::one())
      {
         scal (Msize*Nsize, beta, itrC, 1);
      }

      // A:NoTrans / B:NoTrans
      if (transA == CblasNoTrans && transB == CblasNoTrans)
      {
         auto itrB_save = itrB;
         auto itrC_save = itrC;
         for (size_type i = 0; i < Msize; ++i)
         {
            itrB = itrB_save;
            for (size_type k = 0; k < Ksize; ++k, ++itrA)
            {
               itrC = itrC_save;
               for (size_type j = 0; j < Nsize; ++j, ++itrB, ++itrC)
               {
                  (*itrC) += alpha * (*itrA) * (*itrB);
               }
            }
            itrC_save += Nsize;
         }
      }
      // A:NoTrans / B:Trans
      else if (transA == CblasNoTrans && transB != CblasNoTrans)
      {
         auto itrA_save = itrA;
         auto itrB_save = itrB;
         for (size_type i = 0; i < Nsize; ++i)
         {
            itrB = itrB_save;
            for (size_type j = 0; j < Msize; ++j, ++itrC)
            {
               itrA = itrA_save;
               for (size_type k = 0; k < Ksize; ++k, ++itrA, ++itrB)
               {
                  (*itrC) += alpha * (*itrA) * (*itrB);
               }
            }
            itrA_save += Ksize;
         }
      }
      // A:Trans / B:NoTrans
      else if (transA != CblasNoTrans && transB == CblasNoTrans)
      {
         auto itrB_save = itrB;
         auto itrC_save = itrC;
         for (size_type k = 0; k < Ksize; ++k)
         {
            itrC = itrC_save;
            for (size_type i = 0; i < Msize; ++i, ++itrA)
            {
               itrB = itrB_save;
               for (size_type j = 0; j < Nsize; ++j, ++itrB, ++itrC)
               {
                  (*itrC) += alpha * (*itrA) * (*itrB);
               }
            }
            itrB_save += Nsize;
         }
      }
      // A:Trans / B:Trans
      else if (transA != CblasNoTrans && transB != CblasNoTrans)
      {
         auto itrA_save = itrA;
         auto itrC_save = itrC;
         for (size_type j = 0; j < Nsize; ++j, ++itrC_save)
         {
            itrA = itrA_save;
            for (size_type k = 0; k < Ksize; ++k, ++itrB)
            {
               itrC = itrC_save;
               for (size_type i = 0; i < Msize; ++i, ++itrA, itrC += Nsize)
               {
                  (*itrC) += alpha * (*itrA) * (*itrB);
               }
            }
         }
      }
   }

#ifdef _HAS_CBLAS

   static void call (
      const CBLAS_ORDER& order,
      const CBLAS_TRANSPOSE& transA,
      const CBLAS_TRANSPOSE& transB,
      const unsigned long& Msize,
      const unsigned long& Nsize,
      const unsigned long& Ksize,
      const float& alpha,
      const float* itrA,
      const unsigned long& LDA,
      const float* itrB,
      const unsigned long& LDB,
      const float& beta,
            float* itrC,
      const unsigned long& LDC)
   {
      cblas_sgemm(order, transA, transB, Msize, Nsize, Ksize, alpha, itrA, LDA, itrB, LDB, beta, itrC, LDC);
   }

   static void call (
      const CBLAS_ORDER& order,
      const CBLAS_TRANSPOSE& transA,
      const CBLAS_TRANSPOSE& transB,
      const unsigned long& Msize,
      const unsigned long& Nsize,
      const unsigned long& Ksize,
      const double& alpha,
      const double* itrA,
      const unsigned long& LDA,
      const double* itrB,
      const unsigned long& LDB,
      const double& beta,
            double* itrC,
      const unsigned long& LDC)
   {
      cblas_dgemm(order, transA, transB, Msize, Nsize, Ksize, alpha, itrA, LDA, itrB, LDB, beta, itrC, LDC);
   }

   static void call (
      const CBLAS_ORDER& order,
      const CBLAS_TRANSPOSE& transA,
      const CBLAS_TRANSPOSE& transB,
      const unsigned long& Msize,
      const unsigned long& Nsize,
      const unsigned long& Ksize,
      const std::complex<float>& alpha,
      const std::complex<float>* itrA,
      const unsigned long& LDA,
      const std::complex<float>* itrB,
      const unsigned long& LDB,
      const std::complex<float>& beta,
            std::complex<float>* itrC,
      const unsigned long& LDC)
   {
      cblas_cgemm(order, transA, transB, Msize, Nsize, Ksize, &alpha, itrA, LDA, itrB, LDB, &beta, itrC, LDC);
   }

   static void call (
      const CBLAS_ORDER& order,
      const CBLAS_TRANSPOSE& transA,
      const CBLAS_TRANSPOSE& transB,
      const unsigned long& Msize,
      const unsigned long& Nsize,
      const unsigned long& Ksize,
      const std::complex<double>& alpha,
      const std::complex<double>* itrA,
      const unsigned long& LDA,
      const std::complex<double>* itrB,
      const unsigned long& LDB,
      const std::complex<double>& beta,
            std::complex<double>* itrC,
      const unsigned long& LDC)
   {
      cblas_zgemm(order, transA, transB, Msize, Nsize, Ksize, &alpha, itrA, LDA, itrB, LDB, &beta, itrC, LDC);
   }

#endif // _HAS_CBLAS

};

template<> struct gemm_impl<false>
{
   typedef unsigned long size_type;

   template<typename _T, class _IteratorA, class _IteratorB, class _IteratorC>
   static void call (
      const CBLAS_ORDER& order,
      const CBLAS_TRANSPOSE& transA,
      const CBLAS_TRANSPOSE& transB,
      const unsigned long& Msize,
      const unsigned long& Nsize,
      const unsigned long& Ksize,
      const _T& alpha,
            _IteratorA itrA,
      const unsigned long& LDA,
            _IteratorB itrB,
      const unsigned long& LDB,
      const _T& beta,
            _IteratorC itrC,
      const unsigned long& LDC)
   {
      // currently, column-major order has not yet been supported at this level
      assert(order == CblasRowMajor);

      // A:NoTrans / B:NoTrans
      if (transA == CblasNoTrans && transB == CblasNoTrans)
      {
         auto itrB_save = itrB;
         auto itrC_save = itrC;
         for (size_type i = 0; i < Msize; ++i)
         {
            itrB = itrB_save;
            for (size_type k = 0; k < Ksize; ++k, ++itrA)
            {
               itrC = itrC_save;
               for (size_type j = 0; j < Nsize; ++j, ++itrB, ++itrC)
               {
                  gemm(order, transA, transB, alpha, *itrA, *itrB, beta, *itrC);
               }
            }
            itrC_save += Nsize;
         }
      }
      // A:NoTrans / B:Trans
      else if (transA == CblasNoTrans && transB != CblasNoTrans)
      {
         auto itrA_save = itrA;
         auto itrB_save = itrB;
         for (size_type i = 0; i < Nsize; ++i)
         {
            itrB = itrB_save;
            for (size_type j = 0; j < Msize; ++j, ++itrC)
            {
               itrA = itrA_save;
               for (size_type k = 0; k < Ksize; ++k, ++itrA, ++itrB)
               {
                  gemm(order, transA, transB, alpha, *itrA, *itrB, beta, *itrC);
               }
            }
            itrA_save += Ksize;
         }
      }
      // A:Trans / B:NoTrans
      else if (transA != CblasNoTrans && transB == CblasNoTrans)
      {
         auto itrB_save = itrB;
         auto itrC_save = itrC;
         for (size_type k = 0; k < Ksize; ++k)
         {
            itrC = itrC_save;
            for (size_type i = 0; i < Msize; ++i, ++itrA)
            {
               itrB = itrB_save;
               for (size_type j = 0; j < Nsize; ++j, ++itrB, ++itrC)
               {
                  gemm(order, transA, transB, alpha, *itrA, *itrB, beta, *itrC);
               }
            }
            itrB_save += Nsize;
         }
      }
      // A:Trans / B:Trans
      else if (transA != CblasNoTrans && transB != CblasNoTrans)
      {
         auto itrA_save = itrA;
         auto itrC_save = itrC;
         for (size_type j = 0; j < Nsize; ++j, ++itrC_save)
         {
            itrA = itrA_save;
            for (size_type k = 0; k < Ksize; ++k, ++itrB)
            {
               itrC = itrC_save;
               for (size_type i = 0; i < Msize; ++i, ++itrA, itrC += Nsize)
               {
                  gemm(order, transA, transB, alpha, *itrA, *itrB, beta, *itrC);
               }
            }
         }
      }
   }
};

//  ================================================================================================

/// Generic implementation of BLAS GEMM in terms of C++ iterator
template<typename _T, class _IteratorA, class _IteratorB, class _IteratorC>
void gemm (
   const CBLAS_ORDER& order,
   const CBLAS_TRANSPOSE& transA,
   const CBLAS_TRANSPOSE& transB,
   const unsigned long& Msize,
   const unsigned long& Nsize,
   const unsigned long& Ksize,
   const _T& alpha,
         _IteratorA itrA,
   const unsigned long& LDA,
         _IteratorB itrB,
   const unsigned long& LDB,
   const _T& beta,
         _IteratorC itrC,
   const unsigned long& LDC)
{
   typedef unsigned long size_type;

   typedef std::iterator_traits<_IteratorA> __traits_A;
   typedef std::iterator_traits<_IteratorB> __traits_B;
   typedef std::iterator_traits<_IteratorC> __traits_C;

   typedef typename __traits_A::value_type value_type;

   static_assert(std::is_same<value_type, typename __traits_B::value_type>::value, "value type of B must be the same as that of A");
   static_assert(std::is_same<value_type, typename __traits_C::value_type>::value, "value type of C must be the same as that of A");

   static_assert(std::is_same<typename __traits_A::iterator_category, std::random_access_iterator_tag>::value,
                 "iterator A must be a random access iterator");

   static_assert(std::is_same<typename __traits_B::iterator_category, std::random_access_iterator_tag>::value,
                 "iterator B must be a random access iterator");

   static_assert(std::is_same<typename __traits_C::iterator_category, std::random_access_iterator_tag>::value,
                 "iterator C must be a random access iterator");

   gemm_impl<std::is_same<value_type, _T>::value>::call(order, transA, transB, Msize, Nsize, Ksize, alpha, itrA, LDA, itrB, LDB, beta, itrC, LDC);
}

//  ================================================================================================

/// Generic interface of BLAS-GEMM
/// \param order storage order of tensor in matrix view (CblasRowMajor, CblasColMajor)
/// \param transA transpose directive for tensor A (CblasNoTrans, CblasTrans, CblasConjTrans)
/// \param transB transpose directive for tensor B (CblasNoTrans, CblasTrans, CblasConjTrans)
/// \param alpha scalar value to be multiplied to A * B
/// \param A input tensor
/// \param B input tensor
/// \param beta scalar value to be multiplied to C
/// \param C output tensor which can be empty tensor but needs to have rank info (= size of shape).
/// Iterator is assumed to be consecutive (or, random_access_iterator) , thus e.g. iterator to map doesn't work.
template<
   typename _T,
   class _TensorA, class _TensorB, class _TensorC,
   class = typename std::enable_if<
      is_tensor<_TensorA>::value &
      is_tensor<_TensorB>::value &
      is_tensor<_TensorC>::value
   >::type
>
void gemm (
   const CBLAS_ORDER& order,
   const CBLAS_TRANSPOSE& transA,
   const CBLAS_TRANSPOSE& transB,
   const _T& alpha,
   const _TensorA& A,
   const _TensorB& B,
   const _T& beta,
         _TensorC& C)
{
   typedef unsigned long size_type;

   // check element type
   typedef typename _TensorA::value_type value_type;
   static_assert(std::is_same<value_type, typename _TensorB::value_type>::value, "value type of B must be the same as that of A");
   static_assert(std::is_same<value_type, typename _TensorC::value_type>::value, "value type of C must be the same as that of A");

   if (A.empty() || B.empty())
   {
      scal(beta, C);
      return;
   }

   // get contraction rank
   const size_type rankA = A.rank();
   const size_type rankB = B.rank();
   const size_type rankC = C.rank();

   const size_type K = (rankA+rankB-rankC)/2;
   const size_type M = rankA-K;
   const size_type N = rankB-K;

   // get shapes
   const typename _TensorA::shape_type& shapeA = A.shape();
   const typename _TensorB::shape_type& shapeB = B.shape();
         typename _TensorC::shape_type  shapeC = C.shape(); // if C is empty, this gives { 0,...,0 }

   size_type Msize = 0; // Rows count of C
   size_type Nsize = 0; // Cols count of C
   size_type Ksize = 0; // Dims to be contracted

   size_type LDA = 0; // Leading dims of A
   size_type LDB = 0; // Leading dims of B
   size_type LDC = 0; // Leading dims of C

   // to minimize forks by if?
   if      (transA == CblasNoTrans && transB == CblasNoTrans)
   {
      Msize = std::accumulate(shapeA.begin(), shapeA.begin()+M, 1ul, std::multiplies<size_type>());
      Nsize = std::accumulate(shapeB.begin()+K, shapeB.end(),   1ul, std::multiplies<size_type>());
      Ksize = std::accumulate(shapeA.begin()+M, shapeA.end(),   1ul, std::multiplies<size_type>());

      for (size_type i = 0; i < M; ++i) shapeC[i]   = shapeA[i];
      for (size_type i = 0; i < N; ++i) shapeC[M+i] = shapeB[K+i];

      assert(std::equal(shapeA.begin()+M, shapeA.end(), shapeB.begin()));

      if(order == CblasRowMajor)
      {
         LDA = Ksize;
         LDB = Nsize;
      }
      else
      {
         LDA = Msize;
         LDB = Ksize;
      }
   }
   else if (transA == CblasNoTrans && transB != CblasNoTrans)
   {
      Msize = std::accumulate(shapeA.begin(), shapeA.begin()+M, 1ul, std::multiplies<size_type>());
      Nsize = std::accumulate(shapeB.begin(), shapeB.begin()+N, 1ul, std::multiplies<size_type>());
      Ksize = std::accumulate(shapeA.begin()+M, shapeA.end(),   1ul, std::multiplies<size_type>());

      for (size_type i = 0; i < M; ++i) shapeC[i]   = shapeA[i];
      for (size_type i = 0; i < N; ++i) shapeC[M+i] = shapeB[i];

      assert(std::equal(shapeA.begin()+M, shapeA.end(), shapeB.begin()+N));

      if(order == CblasRowMajor)
      {
         LDA = Ksize;
         LDB = Ksize;
      }
      else
      {
         LDA = Msize;
         LDB = Nsize;
      }
   }
   else if (transA != CblasNoTrans && transB == CblasNoTrans)
   {
      Msize = std::accumulate(shapeA.begin()+K, shapeA.end(),   1ul, std::multiplies<size_type>());
      Nsize = std::accumulate(shapeB.begin()+K, shapeB.end(),   1ul, std::multiplies<size_type>());
      Ksize = std::accumulate(shapeA.begin(), shapeA.begin()+K, 1ul, std::multiplies<size_type>());

      for (size_type i = 0; i < M; ++i) shapeC[i]   = shapeA[K+i];
      for (size_type i = 0; i < N; ++i) shapeC[M+i] = shapeB[K+i];

      assert(std::equal(shapeA.begin(), shapeA.begin()+K, shapeB.begin()));

      if(order == CblasRowMajor)
      {
         LDA = Msize;
         LDB = Nsize;
      }
      else
      {
         LDA = Ksize;
         LDB = Ksize;
      }
   }
   else if (transA != CblasNoTrans && transB != CblasNoTrans)
   {
      Msize = std::accumulate(shapeA.begin()+K, shapeA.end(),   1ul, std::multiplies<size_type>());
      Nsize = std::accumulate(shapeB.begin(), shapeB.begin()+N, 1ul, std::multiplies<size_type>());
      Ksize = std::accumulate(shapeA.begin(), shapeA.begin()+K, 1ul, std::multiplies<size_type>());

      for (size_type i = 0; i < M; ++i) shapeC[i]   = shapeA[K+i];
      for (size_type i = 0; i < N; ++i) shapeC[M+i] = shapeB[i];

      assert(std::equal(shapeA.begin(), shapeA.begin()+K, shapeB.begin()+N));

      if(order == CblasRowMajor)
      {
         LDA = Msize;
         LDB = Ksize;
      }
      else
      {
         LDA = Ksize;
         LDB = Nsize;
      }
   }

   if(order == CblasRowMajor)
   {
      LDC = Nsize;
   }
   else
   {
      LDC = Msize;
   }

   // resize / scale
   if (C.empty())
   {
      C.resize(shapeC);
      NumericType<value_type>::fill(C.begin(), C.end(), NumericType<value_type>::zero());
   }
   else
   {
      assert(std::equal(shapeC.begin(), shapeC.end(), C.shape().begin()));
   }

   auto itrA = tbegin(A);
   auto itrB = tbegin(B);
   auto itrC = tbegin(C);

   gemm (order, transA, transB, Msize, Nsize, Ksize, alpha, itrA, LDA, itrB, LDB, beta, itrC, LDC);
}

} // namespace btas

#endif // __BTAS_GEMM_IMPL_H
