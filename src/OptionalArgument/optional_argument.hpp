// MIT License
// Copyright (c) 2019 Picaud Vincent, picaud.vincent at gmail dot com
// https://github.com/vincent-picaud/OptionalArgument
//
#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <tuple>
#include <type_traits>

namespace OptionalArgument
{
  //////////////// Count_Type_Occurrence ////////////////
  //
  template <typename T, typename... Ts>
  struct Count_Type_Occurence
      : public std::integral_constant<size_t, (std::is_same_v<T, Ts> + ... + 0)>
  {
  };
  template <typename T, typename... Ts>
  constexpr auto Count_Type_Occurence_v = Count_Type_Occurence<T, Ts...>::value;

  //////////////// Is_Free_Of_Duplicate_Type ////////////////
  //
  template <typename... Ts>
  struct Is_Free_Of_Duplicate_Type
      : public std::integral_constant<bool, ((Count_Type_Occurence<Ts, Ts...>::value == 1) && ...)>
  {
  };

  template <typename... Ts>
  constexpr auto Is_Free_Of_Duplicate_Type_v = Is_Free_Of_Duplicate_Type<Ts...>::value;

  //////////////// Is_Optional ////////////////
  //
  // Tests if T is of the form std::optional<XXX>
  //
  template <typename T>
  struct Is_Optional : std::false_type
  {
  };

  template <typename T>
  struct Is_Optional<std::optional<T>> : std::true_type
  {
  };

  template <typename Ts>
  constexpr auto Is_Optional_v = Is_Optional<Ts>::value;

  //////////////// Optional_Argument ////////////////
  //
  template <typename... OPTIONs>
  struct Optional_Argument : public std::tuple<OPTIONs...>
  {
    using std::tuple<OPTIONs...>::tuple;
  };

  template <typename... OPTIONs>
  std::ostream&
  operator<<(std::ostream& out, const Optional_Argument<OPTIONs...>& options_to_print)
  {
    [[maybe_unused]] auto dispatch = [&](const auto& option) {
      if constexpr (Is_Optional_v<std::decay_t<decltype(option)>>)
      {
        if (option.has_value())
        {
          out << option.value() << " ";
        }
      }
      else
      {
        out << option << " ";
      }
    };

    (dispatch(std::get<OPTIONs>(options_to_print)), ...);
    return out;
  }

  template <typename... OPTIONs>
  Optional_Argument<OPTIONs&...>
  take_optional_argument_ref(OPTIONs&... options)
  {
    return {options...};
  }

  //////////////// Option_Decay_t<T> ////////////////
  //
  // T                 -> T
  // T&                -> T
  // std::optional<T>  -> T
  // std::optional<T>& -> T
  //
  template <typename T>
  struct Option_Decay
  {
    using type = T;
  };
  template <typename T>
  struct Option_Decay<T&>
  {
    using type = T;
  };
  template <typename T>
  struct Option_Decay<std::optional<T>>
  {
    using type = T;
  };
  template <typename T>
  struct Option_Decay<std::optional<T>&>
  {
    using type = T;
  };
  template <typename T>
  using Option_Decay_t = typename Option_Decay<T>::type;

  //////////////// optional_argument() ////////////////
  //
  template <typename... OPTIONs, typename... USER_OPTIONs>
  void
  optional_argument(Optional_Argument<OPTIONs...>& options, USER_OPTIONs&&... user_options) noexcept
  {
    static_assert(Is_Free_Of_Duplicate_Type_v<Option_Decay_t<USER_OPTIONs>...>);
    static_assert(Is_Free_Of_Duplicate_Type_v<Option_Decay_t<OPTIONs>...>);

    // USER_OPTION might be  empty
    [[maybe_unused]] auto dispatch = [&](auto&& user_option) {
      using USER_OPTION = std::decay_t<decltype(user_option)>;

      constexpr size_t occurence_count =
          Count_Type_Occurence<USER_OPTION, std::remove_reference_t<OPTIONs>...>::value;
      constexpr size_t occurence_count_by_value =
          Count_Type_Occurence<USER_OPTION, OPTIONs...>::value;
      constexpr size_t occurence_count_maybe_optional =
          Count_Type_Occurence<std::optional<USER_OPTION>,
                               std::remove_reference_t<OPTIONs>...>::value;
      constexpr size_t occurence_count_maybe_optional_by_value =
          Count_Type_Occurence<std::optional<USER_OPTION>, OPTIONs...>::value;

      static_assert(((occurence_count <= 1) && (occurence_count_maybe_optional <= 1)),
                    "Internal error, as incompatible with Is_Free_Of_Duplicate_Type");
      static_assert((occurence_count == 1) || (occurence_count_maybe_optional == 1),
                    "Unexpected type");

      if constexpr (occurence_count == 1)
      {
        // If options is a reference use USER_OPTION& too
        //
        std::get<std::conditional_t<occurence_count_by_value, USER_OPTION, USER_OPTION&>>(options) =
            std::forward<decltype(user_option)>(user_option);
      }
      else
      {
        static_assert(occurence_count_maybe_optional == 1);

        // ...ditto...
        //
        std::get<std::conditional_t<occurence_count_maybe_optional_by_value,
                                    std::optional<USER_OPTION>, std::optional<USER_OPTION>&>>(
            options) = std::forward<decltype(user_option)>(user_option);
      }
    };

    (dispatch(std::forward<USER_OPTIONs>(user_options)), ...);
  }

  //////////////// Named_Type ////////////////
  //
  // inspired by
  // https://github.com/joboccara/NamedType/blob/master/named_type_impl.hpp
  //
  template <typename OBJ, typename VALUE = typename OBJ::value_type>
  struct Argument_Syntactic_Sugar
  {
    constexpr OBJ
    operator=(const VALUE& value) const
    {
      return OBJ{value};
    }

    constexpr OBJ
    operator=(VALUE&& value) const
    {
      return OBJ{std::move(value)};
    }

    constexpr Argument_Syntactic_Sugar()                      = default;
    Argument_Syntactic_Sugar(const Argument_Syntactic_Sugar&) = delete;
    Argument_Syntactic_Sugar(Argument_Syntactic_Sugar&&)      = delete;
    Argument_Syntactic_Sugar& operator=(const Argument_Syntactic_Sugar&) = delete;
    Argument_Syntactic_Sugar& operator=(Argument_Syntactic_Sugar&&) = delete;
  };

  template <typename TAG, typename T = void>
  class Named_Type
  {
    // allowing reference brings some complications we do not really
    // need here.
    static_assert(not std::is_reference_v<T>);

   public:
    using value_type = T;

   protected:
    value_type _value;

   public:
    constexpr Named_Type() = default;

    template <typename _T>
    explicit constexpr Named_Type(_T&& value) : _value(std::forward<_T>(value))
    {
    }

    constexpr Named_Type&
    operator=(value_type&& value)
    {
      _value = std::move(value);
      return *this;
    }
    constexpr Named_Type&
    operator=(const value_type& value)
    {
      _value = value;
      return *this;
    }

    constexpr const value_type&
    value() const
    {
      return _value;
    }

    constexpr value_type&
    value()
    {
      return _value;
    }

    using argument_syntactic_sugar = Argument_Syntactic_Sugar<Named_Type>;
  };

  template <typename TAG, typename T>
  std::ostream&
  operator<<(std::ostream& out, const Named_Type<TAG, T>& to_print)
  {
    out << to_print.value();

    return out;
  }

  // Empty specialization
  //
  template <typename TAG>
  struct Named_Type<TAG>
  {
  };

  template <typename TAG>
  std::ostream&
  operator<<(std::ostream& out, const Named_Type<TAG>& to_print)
  {
    out << "On";
    return out;
  }

  ///////////////////////////////////////////////////
  // Extra, added: Thu 21 Nov 2019 12:34:03 PM CET //
  ///////////////////////////////////////////////////
  //

  // Named type with precondition
  // see: example/
  //
  template <typename TAG, typename ASSERT, typename T = void>
  class Named_Assert_Type
  {
    // allowing reference brings some complications we do not really
    // need here.
    static_assert(not std::is_reference_v<T>);

   public:
    using value_type = T;

   protected:
    value_type _value;

   public:
    constexpr Named_Assert_Type() = default;

    template <typename _T>
    explicit constexpr Named_Assert_Type(_T&& value) : _value(std::forward<_T>(value))
    {
      ASSERT()(_value);
    }

    constexpr Named_Assert_Type&
    operator=(const value_type& value)
    {
      _value = value;
      ASSERT()(_value);
      return *this;
    }

    constexpr Named_Assert_Type&
    operator=(value_type&& value)
    {
      _value = std::move(value);
      ASSERT()(_value);
      return *this;
    }

    constexpr const value_type&
    value() const
    {
      return _value;
    }

    constexpr value_type&
    value()
    {
      return _value;
    }

    using argument_syntactic_sugar = Argument_Syntactic_Sugar<Named_Assert_Type>;
  };

  template <typename TAG, typename T>
  std::ostream&
  operator<<(std::ostream& out, const Named_Assert_Type<TAG, T>& to_print)
  {
    out << to_print.value();

    return out;
  }

  //////////////// Named_Std_Function ////////////////
  //
  // Specialization for extended capture
  //
  template <typename TAG, typename OUTPUT, typename... ARGS>
  class Named_Std_Function;

  template <typename TAG, typename OUTPUT, typename... ARGS>
  struct Argument_Syntactic_Sugar<Named_Std_Function<TAG, OUTPUT, ARGS...>,
                                  typename Named_Std_Function<TAG, OUTPUT, ARGS...>::value_type>
  {
    Named_Std_Function<TAG, OUTPUT, ARGS...> operator=(OUTPUT(f)(ARGS...)) const
    {
      return Named_Std_Function<TAG, OUTPUT, ARGS...>{f};
    }
    template <typename _F>
    std::enable_if_t<std::is_invocable_r_v<OUTPUT, std::decay_t<_F>, ARGS...>,
                     Named_Std_Function<TAG, OUTPUT, ARGS...>>
    operator=(_F&& f) const
    {
      return Named_Std_Function<TAG, OUTPUT, ARGS...>{f};
    }

    constexpr Argument_Syntactic_Sugar()                      = default;
    Argument_Syntactic_Sugar(const Argument_Syntactic_Sugar&) = delete;
    Argument_Syntactic_Sugar(Argument_Syntactic_Sugar&&)      = delete;
    Argument_Syntactic_Sugar& operator=(const Argument_Syntactic_Sugar&) = delete;
    Argument_Syntactic_Sugar& operator=(Argument_Syntactic_Sugar&&) = delete;
  };

  template <typename TAG, typename OUTPUT, typename... ARGS>
  class Named_Std_Function
  {
   public:
    using value_type = std::function<OUTPUT(ARGS...)>;

   protected:
    value_type _f;

   public:
    Named_Std_Function() = default;

    template <typename _F, typename = std::enable_if_t<std::is_invocable_r_v<OUTPUT, _F, ARGS...>>>
    explicit Named_Std_Function(_F&& f) : _f{std::forward<_F>(f)}
    {
    }

    template <typename _F,
              typename = std::enable_if_t<std::is_invocable_r_v<OUTPUT, std::decay_t<_F>, ARGS...>>>
    Named_Std_Function&
    operator=(_F&& f)
    {
      _f = std::forward<_F>(f);
      return *this;
    }
    bool
    is_empty() const
    {
      return static_cast<bool>(_f) == false;
    }
    OUTPUT
    operator()(ARGS... args) const { return _f(args...); }

    using argument_syntactic_sugar = Argument_Syntactic_Sugar<Named_Std_Function>;
  };

}  // namespace OptionalArgument
