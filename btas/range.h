#ifndef __BTAS_RANGE_H
#define __BTAS_RANGE_H 1

#include <array>
#include "btas/utility.h"

namespace btas {

template<std::size_t _N> class Range;

typedef Range<1> Range1;

template<std::size_t _N>
class Range
    {
    public: 

    typedef std::size_t size_type;
    typedef std::array<size_type, _N> array_type;

    Range()
        : size_(0)
        {
        start_.fill(0);
        finish_.fill(0);
        stride_.fill(1);
        }

    /// \return the number of dimensions of this Range
    size_type dim() const { return _N; }

    /// \return true if Range contains no elements
    bool empty() const { return (size_==0); }

    /// \return the total number of elements in the Range
    size_type size() const { return size_; }

    /// \return array containing the starting index of each dimension
    const array_type& start() const { return start_; }

    /// \return array containing the upper bounds of each dimension
    const array_type& finish() const { return finish_; }

    /// \return array containing the strides of each dimension
    const array_type& stride() const { return stride_; }

    private:

    /////////////////////
    array_type start_;
    array_type finish_;
    array_type stride_;
    size_type size_;
    /////////////////////

    };

template<>
class Range<1>
    {
    public: 

    typedef std::size_t size_type;
    typedef size_type array_type;

    Range()
        : start_(0), finish_(0), stride_(0)
        { }

    Range(size_type finish)
        : start_(0), finish_(finish), stride_(1)
        { }

    Range(size_type start, size_type finish, size_type stride = 1)
        : start_(start), finish_(finish), stride_(stride)
        { }

    /// \return the number of dimensions of this Range (==1)
    size_type dim() const { return 1; }

    /// \return true if Range contains no elements
    bool empty() const { return start_==finish_; }

    /// \return the total number of elements in the Range
    size_type size() const 
        { 
        auto extent = finish_-start_;
        return (extent/stride_ + extent%stride_);
        }

    /// \return starting index
    size_type start() const { return start_; }

    /// \return upper bound
    size_type finish() const { return finish_; }

    /// \return stride
    size_type stride() const { return stride_; }

    private:

    /////////////////////
    size_type start_;
    size_type finish_;
    size_type stride_;
    /////////////////////

    };

template<std::size_t N>
std::ostream&
operator<<(std::ostream& s, const Range<N>& r)
    {
    s << "[ ";
    detail::print_array(s,r.start());
    s << ", ";
    detail::print_array(s,r.finish());
    bool nontriv_stride = false;
    for(const auto& st : r.stride())
        {
        if(st != 1)
            {
            nontriv_stride = true;
            break;
            }
        }
    s << " )";
    if(nontriv_stride)
        {
        s << ":";
        detail::print_array(s,r.stride());
        }
    return s;
    }

template<>
std::ostream&
operator<<(std::ostream& s, const Range<1>& r)
    {
    s << "[" << r.start() << "," << r.finish() << ")";
    if(r.stride() != 1) s << ":" << r.stride();
    return s;
    }

} // namespace btas

#endif // __BTAS_RANGE_H
