// This example shows that it is not mandatory to use the Name_Type
// class. Actually you are free to use any class instead.
//
#include "OptionalArgument/optional_argument.hpp"

#include <iostream>
#include <random>

using namespace OptionalArgument;

struct Sample_Size
{
  size_t n;

  size_t
  value() const
  {
    return n;
  }
};

struct Truncated
{
};

static constexpr auto sample_size = Argument_Syntactic_Sugar<Sample_Size, size_t>();
static constexpr auto truncated   = Truncated();

template <typename... USER_OPTIONS>
void
generate_sample(USER_OPTIONS&&... user_options)
{
  //// Options ////
  //
  Sample_Size sample_size{10};
  std::optional<Truncated> truncated;

  auto options = take_optional_argument_ref(sample_size, truncated);
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);

  //// Implementation ////
  //
  std::random_device rd{};
  std::mt19937 gen{rd()};

  std::normal_distribution<> d{0, 1};

  for (size_t i = 0; i < sample_size.value(); i++)
  {
    auto sample = d(gen);
    if (truncated.has_value())
    {
      sample = std::abs(sample);
    }
    std::cout << sample << std::endl;
  }
  std::cout << std::endl;
}

int
main()
{
  generate_sample();

  generate_sample(truncated);

  generate_sample(truncated, sample_size = 5);
}
