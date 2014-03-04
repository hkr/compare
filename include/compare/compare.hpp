//
//  Created by Florian Penzkofer
//
// template to generate *correct* compare functions with strict weak ordering
// to be used with standard library algorithms and data structures such as
// std::map, std::sort

#ifndef HKR_MAKE_COMPARE_HPP
#define HKR_MAKE_COMPARE_HPP

#include "compare/detail/compare_detail.hpp"

namespace hkr {

// wraps an element, combined with a custom compare function (instead of the default 'std::less')
template <typename Access, typename Compare>
detail::compare_using_dispatch<Access, Compare> compare_using(Access const& x, Compare const& comp)
{
    return detail::compare_using_dispatch<Access, Compare>(x, comp);
}

// creates a polymorphic less-than function from a list of fields
// for a description about what is a valid field see 'less_than'
template <typename... As>
detail::compare_dispatch<As...> make_compare(As... as)
{
    return detail::compare_dispatch<As...>(as...);
}

} // namespace hkr

#endif // HKR_MAKE_COMPARE_HPP
