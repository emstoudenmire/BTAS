/*
 * range.h
 *
 *  Created on: Nov 26, 2013
 *      Author: evaleev
 */

#ifndef BTAS_RANGE_H_
#define BTAS_RANGE_H_

#include <algorithm>
#include <vector>
#include <functional>

#include <btas/varray/varray.h>
#include <btas/range_iterator.h>
#include <btas/array_adaptor.h>
#include <btas/types.h>
#include <btas/index_traits.h>
#include <btas/range_traits.h>

//#include <TiledArray/range.h>

/** @addtogroup BTAS_Range

    \section sec_BTAS_Range Range class
    Range implements the Range TWG concept. It supports dense and strided ranges, with fixed (compile-time) and variable (run-time)
    ranks.

    \subsection sec_BTAS_Range_Synopsis Synopsis
    The following will be valid with the reference implementation of Range. This does not belong to the concept specification,
    and not all of these operations will model the concept, but it is useful for discussion; will eventually be moved elsewhere.
    @code
    // Constructors
    Range<1> r0;         // empty = {}
    Range<1> r1(5);      // [0,5) = {0, 1, 2, 3, 4}
    Range<1> r2(2,4);    // [2,4) = {2, 3}
    Range<1> r3(1,7,2);  // [1,7) with stride 2 = {1, 3, 5}
    assert(r3.rank() == 1);
    Range x(r2,r3);   // r1 x r2 = { {2,1}, {2,3}, {2,5}, {4,1}, {4,3}, {4,5} }
    assert(x.rank() == 2);

    // Operations
    std::cout << x.area() << std::endl;  // will print "6"

    // Iteration
    for(auto& v: r3) {
      std::cout << v << " "; // will print "1 3 5 "
    }
    @endcode
*/

namespace btas {
  //using TiledArray::Range;

    namespace detail {

      template <typename Index, typename WeightArray, typename StartArray>
      inline std::size_t calc_ordinal(const Index& index, const WeightArray& weight, const StartArray& lobound) {
        // Check that the dimensions of the arrays are equal.
        const std::size_t n = rank(index);
        assert(rank(weight) == n);
        assert(rank(lobound) == n);

        // Compute ordinal
        std::size_t o = 0ul;
        for(std::size_t i = 0ul; i < n; ++i)
          o += (index[i] - lobound[i]) * weight[i];

        return o;
      }

    }  // namespace detail

    class Range1 {
      public:
        Range1(std::size_t extent = 0ul) :
          begin_(0ul), end_(extent), stride_(1ul) {}

        Range1(std::size_t begin, std::size_t end, std::size_t stride_ = 1ul) :
        begin_(begin), end_(end), stride_(1ul) {}

        Range1(const Range1& other) :
          begin_(other.begin_), end_(other.end_), stride_(other.stride_)
        { }

        Range1& operator=(const Range1& other) {
          begin_ = other.begin_;
          end_ = other.end_;
          stride_ = other.stride_;
          return *this;
        }

      private:
        std::size_t begin_;
        std::size_t end_;
        std::size_t stride_;
    };

    /// Range data of an N-dimensional tensor
    /// Index rank is a runtime parameter
    template <CBLAS_ORDER _Order = CblasRowMajor,
              typename _Index = btas::varray<std::size_t> >
    class Range {
    public:
      typedef Range Range_; ///< This object type
      typedef std::size_t size_type; ///< Size type
      typedef _Index index_type; ///< Coordinate index type
      typedef index_type extent_type;    ///< Range extent type
      typedef std::size_t ordinal_type; ///< Ordinal type
      typedef RangeIterator<index_type, Range_> const_iterator; ///< Index iterator
      friend class RangeIterator<index_type, Range_>;

    private:
      struct Enabler {};

      template <typename Index>
      void init(const Index& lobound, const Index& upbound) {
        using btas::rank;
        auto n = rank(lobound);
        if (n == 0) return;

        std::size_t volume = 1ul;
        lobound_ = array_adaptor<index_type>::construct(n);
        upbound_ = array_adaptor<index_type>::construct(n);
        weight_ = array_adaptor<extent_type>::construct(n);

        // Compute range data
        if (_Order == CblasRowMajor) {
          for(int i = n - 1; i >= 0; --i) {
            assert(lobound[i] <= upbound[i]);
            lobound_[i] = lobound[i];
            upbound_[i] = upbound[i];
            weight_[i] = volume;
            volume *= (upbound_[i] - lobound_[i]);
          }
        }
        else {
          for(auto i = 0; i != n; ++i) {
            assert(lobound[i] <= upbound[i]);
            lobound_[i] = lobound[i];
            upbound_[i] = upbound[i];
            weight_[i] = volume;
            volume *= (upbound_[i] - lobound_[i]);
          }
        }
      }

      template <typename Index>
      void init(const Index& extent) {
        using btas::rank;
        auto n = rank(extent);
        if (n == 0) return;

        // now I know the rank
        lobound_ = array_adaptor<index_type>::construct(n, 0ul);

        std::size_t volume = 1ul;
        upbound_ = array_adaptor<index_type>::construct(n);
        weight_ = array_adaptor<extent_type>::construct(n);

        // Compute range data
        if (_Order == CblasRowMajor) {
          for(int i = n - 1; i >= 0; --i) {
            upbound_[i] = extent[i];
            weight_[i] = volume;
            volume *= upbound_[i];
          }
        }
        else {
          for(auto i = 0; i != n; ++i) {
            upbound_[i] = extent[i];
            weight_[i] = volume;
            volume *= upbound_[i];
          }
        }

      }

    public:

      /// Default constructor

      /// Construct a range with size and dimensions equal to zero.
      Range() :
        lobound_(), upbound_(), weight_()
      { }

      /// Constructor defined by an upper and lower bound

      /// \tparam Index An array type
      /// \param lobound The lower bounds of the N-dimensional range
      /// \param upbound The upper bound of the N-dimensional range
      /// \throw TiledArray::Exception When the size of \c lobound is not equal to
      /// that of \c upbound.
      /// \throw TiledArray::Exception When lobound[i] >= upbound[i]
      /// \throw std::bad_alloc When memory allocation fails.
      template <typename Index>
      Range(const Index& lobound, const Index& upbound,
            typename std::enable_if<btas::is_index<Index>::value, Enabler>::type = Enabler()) :
        lobound_(), upbound_(), weight_()
      {
        using btas::rank;
        auto n = rank(lobound);
        assert(n == rank(upbound));
        init(lobound, upbound);
      }

      /// Range constructor from size array

      /// \tparam SizeArray An array type
      /// \param extent An array with the extent of each dimension
      /// \throw std::bad_alloc When memory allocation fails.
      template <typename SizeArray>
      Range(const SizeArray& extent,
            typename std::enable_if<btas::is_index<SizeArray>::value, Enabler>::type = Enabler()) :
        lobound_(), upbound_(), weight_()
      {
        using btas::rank;
        upbound_ = array_adaptor<index_type>::construct(rank(extent));
        std::copy(extent.begin(), extent.end(), upbound_.begin());
        init(extent);
      }

      /// Range constructor from a pack of sizes for each dimension

      /// \tparam _size0 A
      /// \tparam _sizes A pack of unsigned integers
      /// \param sizes The size of dimensions 0
      /// \param sizes A pack of sizes for dimensions 1+
      /// \throw std::bad_alloc When memory allocation fails.
      template<typename... _sizes>
      explicit Range(const size_type& size0, const _sizes&... sizes) :
      lobound_(), upbound_(), weight_()
      {
        const size_type n = sizeof...(_sizes) + 1;
        size_type range_extent[n] = {size0, static_cast<size_type>(sizes)...};
        init(range_extent);
      }

      /// Copy Constructor

      /// \param other The range to be copied
      /// \throw std::bad_alloc When memory allocation fails.
      Range(const Range_& other) :
        lobound_(other.lobound_), upbound_(other.upbound_), weight_(other.weight_)
      {
      }

      /// Destructor
      ~Range() { }

      /// Copy assignment operator

      /// \param other The range to be copied
      /// \return A reference to this object
      /// \throw std::bad_alloc When memory allocation fails.
      Range_& operator=(const Range_& other) {
        lobound_ = other.lobound_;
        upbound_ = other.upbound_;
        weight_ = other.weight_;

        return *this;
      }

      /// Resize range to a new upper and lower bound

      /// This can be used to avoid memory allocation
      /// \tparam Index An array type
      /// \param lobound The lower bounds of the N-dimensional range
      /// \param upbound The upper bound of the N-dimensional range
      /// \throw TiledArray::Exception When the size of \c lobound is not equal to
      /// that of \c upbound.
      /// \throw TiledArray::Exception When lobound[i] >= upbound[i]
      /// \throw std::bad_alloc When memory allocation fails.
      template <typename Index>
      Range_& resize(const Index& lobound, const Index& upbound) {
        using btas::rank;
        auto n = rank(lobound);
        assert(n == rank(upbound));
        init(lobound, upbound);
        return *this;
      }


      /// Rank accessor

      /// \return The rank (number of dimensions) of this range
      /// \throw nothing
      size_t rank() const {
        using btas::rank;
        return rank(lobound_);
      }

      /// Range lobound coordinate accessor

      /// \return A \c size_array that contains the lower bound of this range
      /// \throw nothing
      const index_type& lobound() const { return lobound_; }

      /// Range lobound coordinate accessor

      /// \return A \c size_array that contains the first index in this range
      /// \throw nothing
      const index_type& front() const { return lobound_; }

      /// Range upbound coordinate accessor

      /// \return A \c size_array that contains the upper bound of this range
      /// \throw nothing
      index_type upbound() const {
        return upbound_;
      }

      /// Range size accessor

      /// \return A \c extent_type that contains the extent of each dimension
      /// \throw nothing
      extent_type extent() const {
        index_type ex = array_adaptor<extent_type>::construct(rank());
        for(auto i=0; i<rank(); ++i)
          ex[i] = upbound_[i] - lobound_[i];
        return ex;
      }

      /// Range weight accessor

      /// \return A \c size_array that contains the strides of each dimension
      /// \throw nothing
      const extent_type& weight() const { return weight_; }

      /// Range volume accessor

      /// \return The total number of elements in the range.
      /// \throw nothing
      size_type area() const {
        if (rank())
          return _Order == CblasRowMajor ?
              weight_[0] * (upbound_[0] - lobound_[0]) :
              weight_[rank()-1] * (upbound_[rank()-1] - lobound_[rank()-1]);
        else
          return 0;
      }

      /// Index iterator factory

      /// The iterator dereferences to an index. The order of iteration matches
      /// the data layout of a dense tensor.
      /// \return An iterator that holds the lobound element index of a tensor
      /// \throw nothing
      const_iterator begin() const { return const_iterator(lobound_, this); }

      /// Index iterator factory

      /// The iterator dereferences to an index. The order of iteration matches
      /// the data layout of a dense tensor.
      /// \return An iterator that holds the upbound element index of a tensor
      /// \throw nothing
      const_iterator end() const { return const_iterator(upbound_, this); }

      /// calculate the ordinal index of \c i

      /// Convert a coordinate index to an ordinal index.
      /// \tparam Index A coordinate index type (array type)
      /// \param index The index to be converted to an ordinal index
      /// \return The ordinal index of \c index
      /// \throw When \c index is not included in this range.
      template <typename Index>
      typename std::enable_if<btas::is_index<Index>::value, size_type>::type
      ordinal(const Index& index) const {
        using btas::rank;
        assert(rank(index) == this->rank());
        assert(this->includes(index));
        size_type o = 0;
        const auto end = this->rank();
        for(auto i = 0ul; i < end; ++i)
          o += (index[i] - lobound_[i]) * weight_[i];

        return o;
      }

      /// Check the coordinate to make sure it is within the range.

      /// \tparam Index The coordinate index array type
      /// \param index The coordinate index to check for inclusion in the range
      /// \return \c true when \c i \c >= \c lobound and \c i \c < \c f, otherwise
      /// \c false
      /// \throw TildedArray::Exception When the dimension of this range is not
      /// equal to the size of the index.
      template <typename Index>
      typename std::enable_if<btas::is_index<Index>::value, bool>::type
      includes(const Index& index) const {
        using btas::rank;
        assert(rank(index) == this->rank());
        const auto end = this->rank();
        for(auto i = 0ul; i < end; ++i)
          if((index[i] < lobound_[i]) || (index[i] >= upbound_[i]))
            return false;

        return true;
      }


      void swap(Range_& other) {
        std::swap(lobound_, other.lobound_);
        std::swap(upbound_, other.upbound_);
        std::swap(weight_, other.weight_);
      }

    //private:
    public:

      /// Increment the coordinate index \c i in this range

      /// \param[in,out] i The coordinate index to be incremented
      /// \throw TiledArray::Exception When the dimension of i is not equal to
      /// that of this range
      /// \throw TiledArray::Exception When \c i or \c i+n is outside this range
      void increment(index_type& i) const {

        if (_Order == CblasRowMajor) {
          for(int d = int(rank()) - 1; d >= 0; --d) {
            // increment coordinate
            ++i[d];

            // break if done
            if(i[d] < upbound_[d])
              return;

            // Reset current index to lobound value.
            i[d] = lobound_[d];
          }
        }
        else { // col-major
          for(auto d = 0ul; d != rank(); ++d) {
            // increment coordinate
            ++i[d];

            // break if done
            if(i[d] < upbound_[d])
              return;

            // Reset current index to lobound value.
            i[d] = lobound_[d];
          }
        }

        // if the current location is outside the range, make it equal to range end iterator
        std::copy(upbound_.begin(), upbound_.end(), i.begin());
      }

#if 0
      /// Advance the coordinate index \c i by \c n in this range

      /// \param[in,out] i The coordinate index to be advanced
      /// \param n The distance to advance \c i
      /// \throw TiledArray::Exception When the dimension of i is not equal to
      /// that of this range
      /// \throw TiledArray::Exception When \c i or \c i+n is outside this range
      void advance(index& i, std::ptrdiff_t n) const {
        const size_type o = ord(i) + n;
        i = idx(o);
      }

      /// Compute the distance between the coordinate indices \c first and \c last

      /// \param first The lobounding position in the range
      /// \param last The ending position in the range
      /// \return The difference between first and last, in terms of range positions
      /// \throw TiledArray::Exception When the dimension of \c first or \c last
      /// is not equal to that of this range
      /// \throw TiledArray::Exception When \c first or \c last is outside this range
      std::ptrdiff_t distance_to(const index& first, const index& last) const {
        TA_ASSERT(includes(first));
        TA_ASSERT(includes(last));
        return ord(last) - ord(first);
      }
#endif

      index_type lobound_; ///< range origin
      index_type upbound_; ///< range extent

      // optimization details
      extent_type weight_; ///< Dimension weights (strides)
    }; // class Range

    /// Exchange the values of the give two ranges.
    template <CBLAS_ORDER _Order,
              typename _Index>
    inline void swap(Range<_Order,_Index>& r0, Range<_Order,_Index>& r1) { // no throw
      r0.swap(r1);
    }


    /// Range equality comparison

    /// \param r1 The first range to be compared
    /// \param r2 The second range to be compared
    /// \return \c true when \c r1 represents the same range as \c r2, otherwise
    /// \c false.
    template <CBLAS_ORDER _Order,
              typename _Index>
    inline bool operator ==(const Range<_Order,_Index>& r1, const Range<_Order,_Index>& r2) {
      return ((r1.lobound() == r2.lobound()) && (r1.extent() == r2.extent()));
    }

    /// Range inequality comparison

    /// \param r1 The first range to be compared
    /// \param r2 The second range to be compared
    /// \return \c true when \c r1 does not represent the same range as \c r2,
    /// otherwise \c false.
    template <CBLAS_ORDER _Order,
              typename _Index>
    inline bool operator !=(const Range<_Order,_Index>& r1, const Range<_Order,_Index>& r2) {
      return ! operator ==(r1, r2);
    }

    /// Range output operator

    /// \param os The output stream that will be used to print \c r
    /// \param r The range to be printed
    /// \return A reference to the output stream
    template <CBLAS_ORDER _Order,
              typename _Index>
    inline std::ostream& operator<<(std::ostream& os, const Range<_Order,_Index>& r) {
      os << "[ ";
      array_adaptor<_Index>::print(r.lobound(), os);
      os << ", ";
      array_adaptor<_Index>::print(r.upbound(), os);
      os << " )_" << (_Order == CblasRowMajor ? "R" : "C");
      return os;
    }

#if 0
      template <typename Archive>
      typename madness::enable_if<madness::archive::is_input_archive<Archive> >::type
      serialize(const Archive& ar) {
        // Get number of dimensions
        size_type n = 0ul;
        ar & n;

        // Get range data
        realloc_arrays(n);
        ar & madness::archive::wrap(lobound_.data(), n * 4ul) & volume_;
      }

      template <typename Archive>
      typename madness::enable_if<madness::archive::is_output_archive<Archive> >::type
      serialize(const Archive& ar) const {
        const size_type n = dim();
        ar & n & madness::archive::wrap(lobound_.data(), n * 4ul) & volume_;
      }

#endif


      template <CBLAS_ORDER _Order,
                typename _Index>
      class boxrange_iteration_order< btas::Range<_Order,_Index> > {
        public:
          enum {row_major = -1, other = 0, column_major = 1};

          static constexpr int value = (_Order == CblasRowMajor) ? row_major : column_major;
      };
}

#endif /* BTAS_RANGE_H_ */
