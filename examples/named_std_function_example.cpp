#include "OptionalArgument/optional_argument.hpp"

#include <cassert>
#include <valarray>

using namespace OptionalArgument;

using Objective_Function =
    Named_Std_Function<struct Objective_Function_Tag, double, const std::valarray<double>&>;
constexpr auto objective_function = Argument_Syntactic_Sugar<Objective_Function>();

void
my_algorithm(const Objective_Function& obj_f, std::valarray<double>& x_init)
{
  std::cout << "Value = " << obj_f(x_init) << std::endl;
}

double
Rosenbrock(const std::valarray<double>& x, double c)
{
  assert(x.size() == 2);

  return (1 - x[0]) * (1 - x[0]) + c * (x[1] - x[0] * x[0]) * (x[1] - x[0] * x[0]);
}

double
Rosenbrock(const std::valarray<double>& x)
{
  return Rosenbrock(x, 10);
}

template <typename T>
struct Rosenbrock_as_Struct
{
  double c = 200;

  T
  operator()(const std::valarray<T>& x) const
  {
    assert(x.size() == 2);

    return (1 - x[0]) * (1 - x[0]) + c * (x[1] - x[0] * x[0]) * (x[1] - x[0] * x[0]);
  }
};

int
main()
{
  std::valarray<double> x(2);
  x = -1;

  static_assert(
      std::is_invocable_r_v<double, Rosenbrock_as_Struct<double>, const std::valarray<double>&>);

  // Note that, as there are two Rosenbrock() functions,
  //
  // std::function<double(const std::valarray<double>&)> test = Rosenbrock; fails
  //
  // We do not fails because we provide OUT(f)(ARGS...) entries
  //
  my_algorithm(objective_function = Rosenbrock, x);

  my_algorithm(
      objective_function = [](const std::valarray<double>& x) { return Rosenbrock(x, 100); }, x);

  my_algorithm(objective_function = Rosenbrock_as_Struct<double>(), x);

  Rosenbrock_as_Struct<double> f;
  my_algorithm(objective_function = f, x);
}
