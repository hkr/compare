#include <iostream>
#include <algorithm>

#include "compare/compare.hpp"

// example data structures to be compared:

namespace {

struct Nested
{
    int a;
    int b;
};

struct Complicated
{
    int x;                            // comparable data member
    Nested n;                         // nested struct with no operator<
    int Do(int x) const { return x; } // member function with args
    int Get() const { return 0; }     // regular 'getter'
};

struct Simple
{
    int a;
    bool b;
    std::tuple<int, char> t;
};
    
bool CompareComplicated(Complicated const& x, Complicated const& y)
{
    auto&& compare = hkr::make_compare(&Complicated::x, &Complicated::Get,
                                       std::bind(&Complicated::Do, std::placeholders::_1, 4),
                                       hkr::compare_using(&Complicated::n, hkr::make_compare(&Nested::a, &Nested::b)));
    return compare(x, y);
}

bool CompareSimple(Simple const& x, Simple const& y)
{
    return hkr::make_compare(&Simple::a, &Simple::b, &Simple::t)(x, y);
}

} // namespace

int main(int argc, const char * argv[])
{
    Simple d[] = { {2, true}, {2, false} };
    std::sort(std::begin(d), std::end(d), hkr::make_compare(&Simple::a, hkr::gt(&Simple::b), &Simple::t));
    std::cout << d[0].b << " " << d[1].b << std::endl;
    
    Complicated a = { 4, { 2, 2 } };
    Complicated b = { 4, { 2, 3 } };
    std::cout << CompareComplicated(a, b) << CompareComplicated(b, a) <<
    CompareComplicated(a, a) << CompareComplicated(b, b) << std::endl;
    
    return 0;
}
