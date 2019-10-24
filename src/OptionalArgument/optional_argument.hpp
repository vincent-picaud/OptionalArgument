#include <optional>
#include <tuple>
#include <type_traits>

namespace OA
{

  //////////////// Always_False ////////////////
  //
  // Usage example: static_assert(Always_False<T>);
  //
  template <class T>
  struct Always_False : std::false_type
  {
  };

  //////////////// Count_Type_Occurrence ////////////////
  //
  // See /test/
  //
  template <typename T, typename... Ts>
  struct Count_Type_Occurence : public std::integral_constant<size_t, (std::is_same_v<T, Ts> + ...)>
  {
  };
  template <typename T, typename... Ts>
  constexpr auto Count_Type_Occurence_v = Count_Type_Occurence<T, Ts...>::value;

  //////////////// Is_Free_Of_Duplicate_Type ////////////////
  //
  // See /test/
  //
  template <typename... Ts>
  struct Is_Free_Of_Duplicate_Type : public std::integral_constant<bool, ((Count_Type_Occurence<Ts, Ts...>::value == 1) && ...)>
  {
  };

  template <typename... Ts>
  constexpr auto Is_Free_Of_Duplicate_Type_v = Is_Free_Of_Duplicate_Type<Ts...>::value;

  //////////////// Optional Arguments ////////////////
  //

  template <typename... OPTIONs, typename... USER_OPTIONs>
  void optional_arguments(std::tuple<OPTIONs...>& options, USER_OPTIONs&&... user_options)
  {
    static_assert(Is_Free_Of_Duplicate_Type_v<USER_OPTIONs...>);
    static_assert(Is_Free_Of_Duplicate_Type_v<OPTIONs...>);

    auto dispatch = [&](auto&& user_option) {
      using USER_OPTION = std::decay_t<decltype(user_option)>;

      constexpr size_t occurence_count = Count_Type_Occurence<USER_OPTION, OPTIONs...>::value;
      constexpr size_t occurence_count_maybe_optional = Count_Type_Occurence<std::optional<USER_OPTION>, OPTIONs...>::value;

      static_assert(((occurence_count <= 1) && (occurence_count_maybe_optional <= 1)), "Internal error, as incompatible with Is_Free_Of_Duplicate_Type_v<OPTIONs...>");

      if constexpr (occurence_count == 1)
      {
        std::get<USER_OPTION>(options) = std::forward<decltype(user_option)>(user_option);
      }
      else if constexpr (occurence_count_maybe_optional == 1)
      {
        std::get<std::optional<USER_OPTION>>(options) = std::forward<decltype(user_option)>(user_option);
      }
      else
      {
        // ((occurence_count == 0) && (occurence_count_maybe_optional == 0))
        static_assert(Always_False<USER_OPTION>::value, "Unexpected type");
      }
    };
    (void)dispatch;  // prevents compiler warning that happens when
                     // USER_OPTION is empty
    (dispatch(std::forward<USER_OPTIONs>(user_options)), ...);
  }

}  // namespace OA
