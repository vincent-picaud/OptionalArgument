#include "OptionalArgument/optional_argument.hpp"

#include <cassert>

using namespace OptionalArgument;

template <typename T>
struct Assert_Positive
{
  void
  operator()(const T& t) const
  {
    assert(t > 0);
  }
};

using Absolute_Precision =
    Named_Assert_Type<struct Absolute_Precision_Tag, Assert_Positive<double>, double>;
constexpr auto absolute_precision = typename Absolute_Precision::argument_syntactic_sugar();

void
my_algorithm(const Absolute_Precision& absolute_precision)
{
}

int
main()
{
  my_algorithm(absolute_precision = +1e-6);
  my_algorithm(absolute_precision = -1e-6);  // run-time assert fail
}
