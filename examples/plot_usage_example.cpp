//
// Script generator for GnuPlot like software
// (this was one of my initial motivation for this library)
//
#include <iomanip>
#include <iostream>
#include <optional>

#include "OptionalArgument/optional_argument.hpp"

using namespace OptionalArgument;

using Line_Width          = Named_Type<struct Line_Width_Tag, size_t>;
constexpr auto line_width = typename Line_Width::argument_syntactic_sugar();
std::ostream&
operator<<(std::ostream& out, const Line_Width& to_print)
{
  out << "linewidth " << to_print.value();
  return out;
}

using Line_Type          = Named_Type<struct Line_Type_Tag, size_t>;
constexpr auto line_type = typename Line_Type::argument_syntactic_sugar();
std::ostream&
operator<<(std::ostream& out, const Line_Type& to_print)
{
  out << "linetype " << to_print.value();
  return out;
}

using Curve_Title          = Named_Type<struct Curve_Title_Tag, std::string>;
constexpr auto curve_title = typename Curve_Title::argument_syntactic_sugar();
std::ostream&
operator<<(std::ostream& out, const Curve_Title& to_print)
{
  out << "title \"" << to_print.value() << "\"";
  return out;
}

template <typename... USER_OPTIONS>
void
plot(std::ostream& out, const std::string& f, USER_OPTIONS&&... user_options)
{
  std::optional<Line_Width> line_width;
  std::optional<Line_Type> line_type;
  std::optional<Curve_Title> curve_title;
  auto options = take_optional_argument_ref(line_width, line_type, curve_title);
  optional_argument(options, std::forward<USER_OPTIONS>(user_options)...);

  out << "plot " << f << " " << options << std::endl;
}

template <typename... USER_OPTIONS>
void
replot(std::ostream& out, const std::string& f, USER_OPTIONS&&... user_options)
{
  out << "re";
  plot(out, f, std::forward<USER_OPTIONS>(user_options)...);
}

int
main()
{
  // prints:
  // plot sin(x)  linetype 2 title "my curve 1"
  // replot cos(x) linewidth 4 title "my curve 2"
  //
  plot(std::cout, "sin(x)", line_type = 2, curve_title = "my curve 1");
  replot(std::cout, "cos(x)", line_width = 4, curve_title = "my curve 2");
}
