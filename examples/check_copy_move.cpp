// An "artificial" example explicitly showing copy and move operations
// -> checks that no useless copy nor hidden sharing exists
#include "OptionalArgument/optional_argument.hpp"

using namespace OptionalArgument;

struct Algorithm_Parameters
{
  Algorithm_Parameters() { std::cerr << "Constructor" << std::endl; }
  Algorithm_Parameters(const Algorithm_Parameters&)
  {
    std::cerr << "Copy constructor" << std::endl;
  }
  Algorithm_Parameters(Algorithm_Parameters&&) { std::cerr << "Move constructor" << std::endl; }
  Algorithm_Parameters&
  operator=(const Algorithm_Parameters&)
  {
    std::cerr << "Copy assignment" << std::endl;
    return *this;
  }
  Algorithm_Parameters&
  operator=(Algorithm_Parameters&&)
  {
    std::cerr << "Move assignment" << std::endl;
    return *this;
  }
};

constexpr auto algorithm_parameters =
    Argument_Syntactic_Sugar<Algorithm_Parameters, Algorithm_Parameters>();

template <typename... USER_OPTIONS>
void
foo(USER_OPTIONS&&... user_options)
{
  Algorithm_Parameters algorithm_parameters;                                // (1)
                                                                            //
  auto options = take_optional_argument_ref(algorithm_parameters);          //
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);  // (2)
}

template <typename... USER_OPTIONS>
void
foo_optional(USER_OPTIONS&&... user_options)
{
  std::optional<Algorithm_Parameters> algorithm_parameters;                 // (3)
                                                                            //
  auto options = take_optional_argument_ref(algorithm_parameters);          //
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);  // (4)
}

int
main()
{
  std::cerr << "================ Check move ================" << std::endl;
  foo(algorithm_parameters = Algorithm_Parameters());  // (5)

  // Constructor      <-> [5]
  // Move constructor <-> [5]
  // Constructor      <-> [1]
  // Move assignment  <-> [2]

  std::cerr << "================ Check copy ================" << std::endl;
  Algorithm_Parameters param;         // (6)
  foo(algorithm_parameters = param);  // (7)

  // Constructor      <-> [6]
  // Copy constructor <-> [7]
  // Constructor      <-> [1]
  // Move assignment  <-> [2]

  std::cerr << "**************** With optional ****************" << std::endl;
  std::cerr << "================ Empty ================" << std::endl;
  foo_optional();

  // nothing

  std::cerr << "================ Check move ================" << std::endl;
  foo_optional(algorithm_parameters = Algorithm_Parameters());  // (8)

  // Constructor      <-> [8]
  // Move constructor <-> [8]
  // Move constructor <-> [4]

  std::cerr << "================ Check copy ================" << std::endl;
  Algorithm_Parameters param_optional;                  // (9)
  foo_optional(algorithm_parameters = param_optional);  // (10)

  // Constructor      <-> [9]
  // Copy constructor <-> [10]
  // Move constructor <-> [4]
}
