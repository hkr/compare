#ifndef HKR_DETAIL_MAKE_COMPARE_HPP
#define HKR_DETAIL_MAKE_COMPARE_HPP

#include <functional>
#include <type_traits>
#include <tuple>

namespace hkr {
namespace detail {
    
// helper used to wrap an element, combined with a custom compare function
template <typename Access, typename Comp, typename Enable = void>
struct compare_using_dispatch;

template <typename Access, typename Comp>
struct compare_using_dispatch<Access, Comp, typename std::enable_if< !std::is_member_pointer<Access>::value>::type>
{
    Comp comp;
    Access get;
    
    compare_using_dispatch(Access const& a, Comp const& c) : comp(c), get(a) {}
    
    template <typename T>
    bool operator()(T const& lhs, T const& rhs) const
    {
        return comp(get(lhs), get(rhs));
    }
};

template <typename Access, typename Comp>
struct compare_using_dispatch<Access, Comp, typename std::enable_if<std::is_member_pointer<Access>::value>::type>
{
    Comp comp;
    Access memPtr;
    
    compare_using_dispatch(Access const& a, Comp const& c) : comp(c), memPtr(a) {}
    
    template <typename T>
    bool operator()(T const& lhs, T const& rhs) const
    {
        return comp(std::mem_fn(memPtr)(lhs), std::mem_fn(memPtr)(rhs));
    }
};

// helper that applies 'std::less' to a pair of elements
template <typename A, typename T>
struct apply_compare
{
    bool operator()(A const& x, T const& lhs, T const& rhs) const
    {
        return apply(x, lhs, rhs, std::is_member_pointer<A>());
    }
    
private:
    
    template <typename L>
    bool less(L const& x, L const& y) const
    {
        return std::less<L>()(x, y);
    }
    
    template <typename X>
    bool apply(X const& x, T const& lhs, T const& rhs, /*is_member_pointer<A>*/ std::false_type) const
    {
        return less(x(lhs), x(rhs));
    }
    
    template <typename X>
    bool apply(X const& x, T const& lhs, T const& rhs, /*is_member_pointer<A>*/ std::true_type) const
    {
        auto&& mf = std::mem_fn(x);
        return less(mf(lhs), mf(rhs));
    }
};

template <typename A, typename Comp, typename T>
struct apply_compare<compare_using_dispatch<A, Comp>, T>
{
    bool operator()(compare_using_dispatch<A, Comp> const& x, T const& lhs, T const& rhs) const
    {
        return x(lhs, rhs);
    }
};

// base case of compile time recursion
template <typename T>
bool less_than(T const& lhs, T const& rhs)
{
    return false;
}

// compares lhs and rhs by the given fields,
// with the first element in the list of arguments being most significant
// a element can be a pointer to data member, pointer to member function or
// a function taking T and returning a type that is comparable using 'std::less'
template <typename T, typename A, typename... As>
bool less_than(T const& lhs, T const& rhs, A a, As... as)
{
    if (detail::apply_compare<A, T>()(a, lhs, rhs))
        return true;
    
    if (detail::apply_compare<A, T>()(a, rhs, lhs))
        return false;
    
    return less_than(lhs, rhs, as...);
}

// polymorphic compare function, created by make_less (see below)
template <typename... As>
struct compare_dispatch
{
    typedef std::tuple<As...> Tup;
    Tup args;
    explicit compare_dispatch(As... as) : args(as...) {}
    
    template<typename T>
    bool operator()(T const& lhs, T const& rhs) const
    {
        return unwrap(lhs, rhs);
    }
    
private:
    template <typename T, typename... Xs>
    typename std::enable_if<sizeof...(Xs) == std::tuple_size<Tup>::value, bool>::type
    unwrap(T const& lhs, T const& rhs, Xs... xs) const
    {
        return less_than(lhs, rhs, xs...);
    }
    
    template <typename T, typename... Xs>
    typename std::enable_if<sizeof...(Xs) != std::tuple_size<Tup>::value, bool>::type
    unwrap(T const& lhs, T const& rhs, Xs... xs) const
    {
        return unwrap(lhs, rhs, xs..., std::get<sizeof...(Xs)>(args));
    }
};

    
// polymorphic compare functions, similar to C++14 (I guess?) std::greater<void>
template <template <typename> class Cmp>
struct polymorphic_compare
{
    template <typename T>
    bool operator()(T const& x, T const& y) const
    {
        return Cmp<T>()(x, y);
    }
};
    
} // namespace detail
} // namespace hkr

#endif // HKR_DETAIL_MAKE_COMPARE_HPP
