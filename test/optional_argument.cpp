#include "OptionalArgument/optional_argument.hpp"

#include <sstream>
#include <vector>

#include <gtest/gtest.h>

using namespace OptionalArgument;

TEST(Optional_Argument, meta)
{
  ASSERT_EQ((Count_Type_Occurence_v<int>), 0);
  ASSERT_EQ((Count_Type_Occurence_v<int, double, int>), 1);
  ASSERT_EQ((Count_Type_Occurence_v<int, int, double, int>), 2);

  ASSERT_EQ((Is_Free_Of_Duplicate_Type_v<>), true);
  ASSERT_EQ((Is_Free_Of_Duplicate_Type_v<int, double, float>), true);
  ASSERT_EQ((Is_Free_Of_Duplicate_Type_v<int, float, double, float>), false);

  ASSERT_TRUE((std::is_same_v<Option_Decay_t<std::optional<double>>, double>));
  ASSERT_TRUE((std::is_same_v<Option_Decay_t<std::optional<double>&>, double>));
  ASSERT_TRUE((std::is_same_v<Option_Decay_t<std::optional<double&>&>, double&>));
  ASSERT_TRUE((std::is_same_v<Option_Decay_t<double&>, double>));
  ASSERT_TRUE((std::is_same_v<Option_Decay_t<double>, double>));
}

TEST(Optional_Argument, basic)
{
  double x{};
  int n{3};

  Optional_Argument<double&, int> opt_arg(x, n);

  ASSERT_EQ(std::get<double&>(opt_arg), 0);
  x = 1;
  ASSERT_EQ(std::get<double&>(opt_arg), 1);

  std::get<int>(opt_arg) = 2;
  ASSERT_EQ(n, 3);

  std::stringstream out;
  out << opt_arg;
  ASSERT_EQ(out.str(), "1 2 ");
}

template <typename... OPTIONS>
auto
foo_scalar(OPTIONS&&... options)
{
  double x{};

  Optional_Argument<double&, int> opt_arg(x, int{2});
  optional_argument(opt_arg, std::forward<OPTIONS>(options)...);

  // CAVEAT: if you return opt_arg, you get a dangling reference (x&
  // where x is a local variable) thus we perform this copy:
  return std::tuple<double, int>(opt_arg);
}

TEST(Optional_Argument, foo_scalar)
{
  auto r_1 = foo_scalar();

  ASSERT_EQ(r_1, std::tuple(0, 2));

  auto r_2 = foo_scalar(2, 1.);
  ASSERT_EQ(r_2, std::tuple(1, 2));

  auto r_3 = foo_scalar(1., 2);
  ASSERT_EQ(r_3, std::tuple(1, 2));
}

//================

TEST(Optional_Argument, take_optional_argument_ref)
{
  double x{};
  std::optional<int> n;

  auto opt_arg = take_optional_argument_ref(x, n);

  ASSERT_EQ(std::get<double&>(opt_arg), 0);
  x = 1;
  ASSERT_EQ(std::get<double&>(opt_arg), 1);
}

//================

template <typename... OPTIONS>
auto
foo_scalar_make(OPTIONS&&... options)
{
  double x{};
  std::optional<int> n;

  // only take reference
  auto opt_arg = take_optional_argument_ref(x, n);
  optional_argument(opt_arg, std::forward<OPTIONS>(options)...);

  return std::tuple(x, n);
}

TEST(Optional_Argument, foo_scalar_make)
{
  auto r_1 = foo_scalar_make();

  ASSERT_EQ(r_1, std::tuple(0, std::optional<int>{}));

  auto r_2 = foo_scalar_make(std::optional<int>(2), 1.);
  ASSERT_EQ(r_2, std::tuple(1, 2));

  auto r_3 = foo_scalar_make(1., std::optional<int>{});
  ASSERT_EQ(r_3, std::tuple(1, std::optional<int>{}));
}

//================

template <typename T>
using Starting_Point_Vector = Named_Type<struct Starting_Point_Vector_Tag, std::vector<T>>;
template <typename T>
constexpr auto starting_point_vector =
    typename Starting_Point_Vector<T>::argument_syntactic_sugar();

template <typename T, typename... OPTIONS>
std::vector<T>
foo_vector(const std::vector<T>&, OPTIONS&&... options)
{
  // CAVEAT: not simply
  //   std::vector<T> v;
  // but a wrapping
  Starting_Point_Vector<T> v;

  auto opt_arg = take_optional_argument_ref(v);
  optional_argument(opt_arg, std::forward<OPTIONS>(options)...);

  if (v.value().size())
  {
    v.value()[0] = 2;
  }
  return v.value();
}

TEST(Optional_Argument, foo_vector_preamble)
{
  Starting_Point_Vector<int> v;

  std::vector<int> y(3, 1);
  v = y;
  ASSERT_EQ(v.value().size(), 3);
  ASSERT_EQ(y.size(), 3);
  v = std::move(y);
  ASSERT_EQ(v.value().size(), 3);
  ASSERT_EQ(y.size(), 0);
}

TEST(Optional_Argument, foo_vector)
{
  std::vector<int> x, y(3, 1);

  std::vector<int> v_1 = foo_vector(x);
  ASSERT_EQ(v_1.size(), 0);

  std::vector<int> v_2 = foo_vector(x, starting_point_vector<int> = y);
  ASSERT_EQ(v_2.size(), 3);
  ASSERT_EQ(v_2[0], 2);
  ASSERT_EQ(v_2[1], 1);
  // Checks that y is copied
  ASSERT_EQ(y.size(), 3);
  ASSERT_EQ(y[0], 1);

  // Checks that y is moved
  std::vector<int> v_3 = foo_vector(x, starting_point_vector<int> = std::move(y));
  ASSERT_EQ(v_3.size(), 3);
  ASSERT_EQ(v_3[2], 1);
  ASSERT_EQ(y.size(), 0);
}

using Flag          = Named_Type<struct Starting_Point_Vector_Tag, bool>;
constexpr auto flag = typename Flag::argument_syntactic_sugar() = true;

template <typename... OPTIONS>
bool
foo_flag(const double, OPTIONS&&... options)
{
  Flag flag = false;

  auto opt_arg = take_optional_argument_ref(flag);
  optional_argument(opt_arg, std::forward<OPTIONS>(options)...);

  return flag.value();
}

TEST(Optional_Argument, foo_flag)
{
  bool b_1 = foo_flag(1.);
  ASSERT_EQ(b_1, false);

  bool b_2 = foo_flag(1., flag);
  ASSERT_EQ(b_2, true);
}

//////////////// Named_Assert ////////////////
//
template <typename T>
struct Assert_Positive
{
  void
  operator()(const T& t) const
  {
    if (t <= 0) throw std::string("not positive");
  }
};

using Absolute_Precision =
    Named_Assert_Type<struct Absolute_Precision_Tag, Assert_Positive<double>, double>;
constexpr auto absolute_precision = typename Absolute_Precision::argument_syntactic_sugar();

double
my_algorithm(const Absolute_Precision& absolute_precision)
{
  return absolute_precision.value();
}

TEST(Optional_Argument, Named_Assert)
{
  ASSERT_EQ(my_algorithm(absolute_precision = 1e-6), 1e-6);

  ASSERT_THROW(my_algorithm(absolute_precision = -1e-6), std::string);
}

//////////////// Named_Std_Function ////////////////
//

using Objective_Function =
    Named_Std_Function<struct Objective_Function_Tag, double, const std::vector<double>&>;
constexpr auto objective_function = Argument_Syntactic_Sugar<Objective_Function>();

double
my_algorithm(const Objective_Function& obj_f, std::vector<double>& x_init)
{
  return obj_f(x_init);
}

double
Rosenbrock(const std::vector<double>& x, double c)
{
  assert(x.size() == 2);

  return (1 - x[0]) * (1 - x[0]) + c * (x[1] - x[0] * x[0]) * (x[1] - x[0] * x[0]);
}

double
Rosenbrock(const std::vector<double>& x)
{
  return Rosenbrock(x, 10);
}

template <typename T>
struct Rosenbrock_as_Struct
{
  double c = 200;

  T
  operator()(const std::vector<T>& x) const
  {
    assert(x.size() == 2);

    return (1 - x[0]) * (1 - x[0]) + c * (x[1] - x[0] * x[0]) * (x[1] - x[0] * x[0]);
  }
};

TEST(Optional_Argument, Empty_Named_Std_Function)
{
  Objective_Function empty_f;

  ASSERT_TRUE(empty_f.is_empty());
}

TEST(Optional_Argument, Named_Std_Function)
{
  std::vector<double> x(2, -1);

  ASSERT_EQ(my_algorithm(my_algorithm(objective_function = Rosenbrock, x)), 44);

  auto lambda = [](const std::vector<double>& x) { return Rosenbrock(x, 100); };
  ASSERT_EQ(my_algorithm(objective_function = lambda, x), 404);

  const auto r = my_algorithm(
      objective_function = [](const std::vector<double>& x) { return Rosenbrock(x, 100); }, x);
  ASSERT_EQ(r, 404);

  ASSERT_EQ(my_algorithm(objective_function = Rosenbrock_as_Struct<double>(), x), 804);

  Rosenbrock_as_Struct<double> f;
  ASSERT_EQ(my_algorithm(objective_function = f, x), 804);
}

TEST(Optional_Argument, Lambda_to_Named_Std_Function)
{
  using Adam_Alpha_Schedule =
      Named_Std_Function<struct Adam_Alpha_Schedule_Tag, double, const size_t>;
  static constexpr auto Adam_alpha_schedule =
      typename Adam_Alpha_Schedule::argument_syntactic_sugar();
  static constexpr auto Adam_alpha_constant_schedule = [](const double alpha) {
    auto to_return = [alpha](const size_t) -> double { return alpha; };
    return to_return;
  };

  Adam_Alpha_Schedule alpha_schedule = (Adam_alpha_schedule = Adam_alpha_constant_schedule(0.01));

  ASSERT_EQ(alpha_schedule(10), 0.01);
  ASSERT_EQ(alpha_schedule(20), 0.01);

  Adam_Alpha_Schedule alpha_schedule_2 = Adam_alpha_constant_schedule(0.02);
}
