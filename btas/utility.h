#ifndef __BTAS_UTILITY_H
#define __BTAS_UTILITY_H 1

namespace btas {
namespace detail {

template <typename A>
void 
print_array(std::ostream& s, const A& a) 
    {
    const std::size_t n = a.size();
    s << "[";
    for(std::size_t i = 0; i < n; ++i) 
        {
        s << a[i];
        if(i != (n-1)) s << ",";
        }
    s << "]";
    }

} // namespace detail
} // namespace btas

#endif // __BTAS_UTILITY_H
