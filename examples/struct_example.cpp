// This example shows that it is not mandatory to use the Name_Type
// class. Actually you are free to use any class instead.
//
#include "OptionalArgument/optional_argument.hpp"

#include <iomanip>

using namespace OptionalArgument;

struct A
{
  int n;
};
struct B
{
};

template <typename... USER_OPTIONS>
void
foo(USER_OPTIONS&&... user_options)
{
  A a{10};
  std::optional<B> b;

  auto options = take_optional_argument_ref(a, b);
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);

  std::cout << "Options (1): A.n = " << std::setw(2) << a.n << " B = " << std::boolalpha
            << b.has_value() << std::endl;
  // another way is:
  std::cout << "Options (2): A.n = " << std::setw(2) << std::get<A&>(options).n
            << " B = " << std::boolalpha << std::get<std::optional<B>&>(options).has_value()
            << std::endl;
}

// Another way, using optional argument by value:
//
template <typename... USER_OPTIONS>
void
foo_by_value(USER_OPTIONS&&... user_options)
{
  auto options = Optional_Argument<A, std::optional<B>>{A{10}, std::optional<B>{}};
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);

  std::cout << "Options: A.n = " << std::setw(2) << std::get<A>(options).n
            << " B = " << std::boolalpha << std::get<std::optional<B>>(options).has_value()
            << std::endl;
}
int
main()
{
  foo();
  foo(A{5});
  foo(A{15}, B());

  std::cout << std::endl;

  foo_by_value();
  foo_by_value(A{5});
  foo_by_value(A{15}, B());
}
