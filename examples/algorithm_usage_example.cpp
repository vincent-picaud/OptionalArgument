//
// Hypothetical optimization algorithm, with optional arguments:
// - absolute_precision,relative_precision
// - maximum_iterations
// - lower_bounds, upper_bounds
//

#include "OptionalArgument/optional_argument.hpp"

#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

namespace OptionalArgument
{
  template <typename T>
  std::ostream&
  operator<<(std::ostream& out, const std::vector<T>& to_print)
  {
    for (const auto& to_print_i : to_print) out << to_print_i << " ";
    return out;
  }
}  // namespace OptionalArgument

using namespace OptionalArgument;

using Absolute_Precision          = Named_Type<struct Absolute_Precision_Tag, double>;
constexpr auto absolute_precision = typename Absolute_Precision::argument_syntactic_sugar();

using Relative_Precision          = Named_Type<struct Relative_Precision_Tag, double>;
constexpr auto relative_precision = typename Relative_Precision::argument_syntactic_sugar();

using Max_Iterations          = Named_Type<struct Max_Iterations_Tag, size_t>;
constexpr auto max_iterations = typename Max_Iterations::argument_syntactic_sugar();

template <typename T>
using Lower_Bounds = Named_Type<struct Lower_Bounds_Tag, std::vector<T>>;
template <typename T>
constexpr auto lower_bounds = typename Lower_Bounds<T>::argument_syntactic_sugar();

template <typename T>
using Upper_Bounds = Named_Type<struct Upper_Bounds_Tag, std::vector<T>>;
template <typename T>
constexpr auto upper_bounds = typename Upper_Bounds<T>::argument_syntactic_sugar();

template <typename T, typename... USER_OPTIONS>
void
optimization_algorithm(std::vector<T>& x, USER_OPTIONS&&... user_options)
{
  Max_Iterations max_iterations{100};
  Absolute_Precision absolute_precision{1e-10};
  Relative_Precision relative_precision{1e-10};
  std::optional<Lower_Bounds<T>> lower_bound;
  std::optional<Upper_Bounds<T>> upper_bound;

  auto options = take_optional_argument_ref(max_iterations, absolute_precision, relative_precision,
                                            lower_bound, upper_bound);
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);

  std::cerr << "Option values: " << options << std::endl;

  // implementation ...
}

int
main()
{
  const size_t n = 4;
  std::vector<double> x_init(n);

  // Option values: 100 1e-10 1e-10
  optimization_algorithm(x_init);

  // Option values: 50 1e-10 1e-10 0 0 0 0
  optimization_algorithm(x_init, max_iterations = 50,
                         lower_bounds<double> = std::vector<double>(n, 0));

  // Option values: 50 1e-08 1e-10 0 0 0 0  1 1 1 1
  optimization_algorithm(x_init, max_iterations = 50, absolute_precision = 1e-8,
                         lower_bounds<double> = std::vector<double>(n, 0),
                         upper_bounds<double> = std::vector<double>(n, 1));
}
