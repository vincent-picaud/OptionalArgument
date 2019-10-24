#include "OptionalArgument/hello.hpp"
#include <gtest/gtest.h>

using namespace OptionalArgument;

TEST(Hello, demo)
{
  ASSERT_EQ(hello(),"Hello OptionalArgument");
}
