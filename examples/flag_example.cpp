#include "OptionalArgument/optional_argument.hpp"

using namespace OptionalArgument;

using My_Flag_A = Named_Type<struct My_Flag_A_Tag>;
constexpr auto my_flag_a = My_Flag_A();
using My_Flag_B = Named_Type<struct My_Flag_B_Tag>;
constexpr auto my_flag_b = My_Flag_B();

template <typename... USER_OPTIONS>
void foo(USER_OPTIONS&&... user_options)
{
  std::optional<My_Flag_A> my_flag_a;
  std::optional<My_Flag_B> my_flag_b;

  auto options = take_optional_argument_ref(my_flag_a, my_flag_b);
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);

  std::cout << "Options: " << options << std::endl;
}

int main()
{
  foo();
  foo(my_flag_b);
  foo(my_flag_b, my_flag_a);
}
