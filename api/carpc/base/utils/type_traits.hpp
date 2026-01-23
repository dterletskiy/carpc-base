#pragma once

#include <type_traits>
#include <utility>

/**
 * @def DEFINE_CALL_IF_EXISTS_20(NAME)
 *
 * @brief
 * Generates a conditional call wrapper for a member function NAME.
 *
 * This macro defines:
 * - A concept `HAS_NAME<T, Args...>` that checks at compile time
 *   whether type `T` has a callable member function `NAME`
 *   accepting arguments `Args...`.
 * - A function `call_NAME(T&, Args&&...)` that:
 *   - Calls `T::NAME(args...)` if it exists.
 *   - Returns the result wrapped in `std::optional`.
 *   - Returns `std::nullopt` if the function does not exist.
 *
 * The generated API provides a safe, zero-overhead alternative to
 * runtime polymorphism when optional interfaces are required.
 *
 * The wrapper correctly handles:
 * - Overloaded member functions (via argument matching).
 * - `void` and non-void return types.
 * - Perfect forwarding of arguments.
 *
 * This macro is intended for C++20 and later, using `concepts`
 * and `requires` expressions.
 *
 * @note
 * - The return type is always `std::optional<T>` or
 *   `std::optional<std::monostate>` for `void` functions.
 * - If the member function does not exist, the call is discarded
 *   at compile time with no runtime cost.
 *
 * @warning
 * - The macro generates symbols based on NAME.
 *   Avoid collisions in large scopes.
 *
 * @param NAME
 * Name of the member function to check and call.
 *
 * @example
 * Basic usage with non-void return type:
 *
 * @code
 * struct A
 * {
 *    int foo(int x)
 *    {
 *       return x * 2;
 *    }
 * };
 *
 * struct B
 * {
 * };
 *
 * DEFINE_CALL_IF_EXISTS_20(foo)
 *
 * A a;
 * B b;
 *
 * auto r1 = call_foo(a, 10);
 * auto r2 = call_foo(b, 10);
 *
 * // r1.has_value() == true
 * // r1.value() == 20
 *
 * // r2.has_value() == false
 * @endcode
 *
 * @example
 * Usage with void return type:
 *
 * @code
 * struct C
 * {
 *    void bar(int v)
 *    {
 *       // side effects
 *    }
 * };
 *
 * DEFINE_CALL_IF_EXISTS_20(bar)
 *
 * C c;
 *
 * auto r = call_bar(c, 5);
 *
 * // r.has_value() == true
 * // return type is std::optional<std::monostate>
 * @endcode
 *
 * @example
 * Overload selection by argument types:
 *
 * @code
 * struct D
 * {
 *    int foo(int);
 *    double foo(double);
 * };
 *
 * DEFINE_CALL_IF_EXISTS_20(foo)
 *
 * D d;
 *
 * auto r1 = call_foo(d, 10);     // calls foo(int)
 * auto r2 = call_foo(d, 3.14);   // calls foo(double)
 * @endcode
 *
 * @example
 * Generic usage in templates:
 *
 * @code
 * template< typename T >
 * void process(T &obj)
 * {
 *    if(auto r = call_foo(obj, 42))
 *    {
 *       // foo exists and was called
 *    }
 * }
 * @endcode
 *
 * @remarks
 * This macro is especially useful in:
 * - Framework-level code.
 * - Driver or codec pipelines.
 * - Optional protocol or capability detection.
 * - Compile-time feature dispatch.
 */
#define DEFINE_CALL_IF_EXISTS_17( NAME )                                      \
                                                                              \
   template< typename T, typename, typename... Args >                         \
   struct HAS_##NAME##_IMPL : std::false_type                                 \
   {                                                                          \
   };                                                                         \
                                                                              \
   template< typename T, typename... Args >                                   \
   struct HAS_##NAME##_IMPL<                                                  \
            T,                                                                \
            std::void_t<                                                      \
               decltype(                                                      \
                  std::declval< T >( ).NAME( std::declval< Args >( )... )     \
               )                                                              \
            >,                                                                \
            Args...                                                           \
         >                                                                    \
      : std::true_type                                                        \
   {                                                                          \
   };                                                                         \
                                                                              \
   template< typename T, typename... Args >                                   \
   using HAS_##NAME = HAS_##NAME##_IMPL< T, void, Args... >;                  \
                                                                              \
   template< typename T, typename... Args >                                   \
   auto call_##NAME##_impl(                                                   \
      T &obj,                                                                 \
      std::true_type,                                                         \
      Args &&... args                                                         \
   )                                                                          \
   {                                                                          \
      using ReturnT =                                                         \
         decltype( obj.NAME( std::forward< Args >( args )... ) );             \
                                                                              \
      if constexpr( std::is_void_v< ReturnT > )                               \
      {                                                                       \
         obj.NAME( std::forward< Args >( args )... );                         \
         return std::optional< std::monostate >( std::monostate{ } );         \
      }                                                                       \
      else                                                                    \
      {                                                                       \
         return std::optional< ReturnT >(                                     \
            obj.NAME( std::forward< Args >( args )... )                       \
         );                                                                   \
      }                                                                       \
   }                                                                          \
                                                                              \
   template< typename T, typename... Args >                                   \
   auto call_##NAME##_impl(                                                   \
      T &,                                                                    \
      std::false_type,                                                        \
      Args &&...                                                              \
   )                                                                          \
   {                                                                          \
      using ReturnT =                                                         \
         std::conditional_t<                                                  \
            true,                                                             \
            std::monostate,                                                   \
            void                                                              \
         >;                                                                   \
                                                                              \
      return std::optional< ReturnT >( );                                     \
   }                                                                          \
                                                                              \
   template< typename T, typename... Args >                                   \
   auto call_##NAME( T &obj, Args &&... args )                                \
   {                                                                          \
      return call_##NAME##_impl(                                              \
         obj,                                                                 \
         HAS_##NAME< T, Args... >{ },                                         \
         std::forward< Args >( args )...                                      \
      );                                                                      \
   }



#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @def DEFINE_CALL_IF_EXISTS_20(NAME)
 *
 * @brief
 * Defines a C++20 compile-time wrapper for conditionally calling a
 * member function if it exists.
 *
 * This macro generates a small API consisting of:
 * - A concept `HAS_NAME<T, Args...>` that checks at compile time
 *   whether type `T` provides a callable member function `NAME`
 *   with arguments `Args...`.
 * - Two overloads of `call_NAME(T&, Args&&...)` selected via
 *   `requires` clauses.
 *
 * If the member function exists, it is invoked and its return value
 * is wrapped into `std::optional`.
 * If the member function does not exist, the call is discarded at
 * compile time and `std::nullopt` is returned.
 *
 * The dispatch is entirely compile-time and introduces no runtime
 * overhead.
 *
 * @details
 * - Uses C++20 `concepts` and `requires` expressions.
 * - Correctly handles both `void` and non-void return types.
 * - Supports overloaded member functions via argument matching.
 * - Preserves value categories using perfect forwarding.
 *
 * The generated functions provide a safe alternative to optional
 * interfaces traditionally implemented via virtual methods or
 * runtime checks.
 *
 * @note
 * - The return type is always `std::optional<R>` or
 *   `std::optional<std::monostate>` for `void` functions.
 * - No code is generated for the non-existing case.
 *
 * @warning
 * - The macro introduces identifiers derived from NAME.
 *   Avoid name collisions in large or public headers.
 *
 * @param NAME
 * Name of the member function to detect and call.
 *
 * @example
 * Conditional call with non-void return type:
 *
 * @code
 * struct A
 * {
 *    int foo(int x)
 *    {
 *       return x * 2;
 *    }
 * };
 *
 * struct B
 * {
 * };
 *
 * DEFINE_CALL_IF_EXISTS_20(foo)
 *
 * A a;
 * B b;
 *
 * auto r1 = call_foo(a, 10);
 * auto r2 = call_foo(b, 10);
 *
 * // r1.has_value() == true
 * // r1.value() == 20
 *
 * // r2.has_value() == false
 * @endcode
 *
 * @example
 * Conditional call with void return type:
 *
 * @code
 * struct C
 * {
 *    void bar(int v)
 *    {
 *       // side effects
 *    }
 * };
 *
 * DEFINE_CALL_IF_EXISTS_20(bar)
 *
 * C c;
 *
 * auto r = call_bar(c, 5);
 *
 * // r.has_value() == true
 * // return type is std::optional<std::monostate>
 * @endcode
 *
 * @example
 * Overload resolution by argument list:
 *
 * @code
 * struct D
 * {
 *    int foo(int);
 *    double foo(double);
 * };
 *
 * DEFINE_CALL_IF_EXISTS_20(foo)
 *
 * D d;
 *
 * auto r1 = call_foo(d, 10);     // calls foo(int)
 * auto r2 = call_foo(d, 3.14);   // calls foo(double)
 * @endcode
 *
 * @example
 * Generic usage in template code:
 *
 * @code
 * template< typename T >
 * void process(T &obj)
 * {
 *    if(auto r = call_foo(obj, 42))
 *    {
 *       // foo exists and was invoked
 *    }
 * }
 * @endcode
 *
 * @remarks
 * This macro is well suited for:
 * - Framework and middleware layers.
 * - Driver and codec pipelines.
 * - Capability-based APIs.
 * - Optional protocol hooks.
 *
 * It provides a clean, readable and maintainable C++20 solution
 * for conditional interface invocation.
 */
#define DEFINE_CALL_IF_EXISTS_20( NAME )                          \
                                                                  \
   template< typename T, typename... Args >                       \
   concept HAS_##NAME =                                           \
      requires( T t, Args... args )                               \
      {                                                           \
         t.NAME( std::forward< Args >( args )... );               \
      };                                                          \
                                                                  \
   template< typename T, typename... Args >                       \
   requires HAS_##NAME< T, Args... >                              \
   auto call_##NAME( T &obj, Args &&... args )                    \
   {                                                              \
      using Result =                                              \
         decltype( obj.NAME( std::forward< Args >( args )... ) ); \
      if constexpr( std::is_void_v< Result > )                    \
      {                                                           \
         obj.NAME( std::forward< Args >( args )... );             \
         return std::optional< std::monostate >{                  \
               std::monostate{ }                                  \
            };                                                    \
      }                                                           \
      else                                                        \
      {                                                           \
         return std::optional< Result >{                          \
               obj.NAME( std::forward< Args >( args )... )        \
            };                                                    \
      }                                                           \
   }                                                              \
                                                                  \
   template< typename T, typename... Args >                       \
   requires ( !HAS_##NAME< T, Args... > )                         \
   auto call_##NAME(T &, Args &&...)                              \
   {                                                              \
      return std::optional< std::monostate >{ };                  \
   }




#define DEFINE_CALL_IF_EXISTS DEFINE_CALL_IF_EXISTS_20


#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @namespace carpc
 *
 * @brief
 * Namespace containing compile-time utilities for optional
 * member function invocation.
 *
 * The facilities provided here allow calling a member function
 * only if it exists on a given type, with zero runtime overhead.
 *
 * The implementation is based on C++20 concepts and `requires`
 * expressions and does not rely on macros or SFINAE.
 */
namespace carpc {

   //------------------------------------------------------------
   // Extract class type from pointer-to-member-function
   //------------------------------------------------------------

   /**
    * @struct MemberClass
    *
    * @brief
    * Extracts the owning class type from a pointer-to-member-function.
    *
    * This trait is used internally to deduce the class on which a
    * member function pointer is defined.
    *
    * @tparam T
    * Pointer-to-member-function type.
    */
   template< typename >
   struct MemberClass;

   /**
    * @specialization MemberClass<R (C::*)(Args...)>
    *
    * @brief
    * Specialization for non-const member functions.
    */
   template< typename R, typename C, typename... Args >
   struct MemberClass< R (C::*)(Args...) >
   {
      using type = C;
   };

   /**
    * @specialization MemberClass<R (C::*)(Args...) const>
    *
    * @brief
    * Specialization for const-qualified member functions.
    */
   template< typename R, typename C, typename... Args >
   struct MemberClass< R (C::*)(Args...) const >
   {
      using type = C;
   };

   //------------------------------------------------------------
   // Concept: check if method can be invoked
   //------------------------------------------------------------

   /**
    * @concept CanInvoke
    *
    * @brief
    * Checks whether a member function can be invoked on a given object
    * with the specified arguments.
    *
    * This concept verifies at compile time that:
    * - The provided type is a pointer-to-member-function.
    * - The object type derives from the member's owning class.
    * - The member function is callable with the given arguments.
    *
    * @tparam MethodT
    * Pointer-to-member-function type.
    *
    * @tparam Method
    * Pointer-to-member-function value.
    *
    * @tparam C
    * Class type owning the member function.
    *
    * @tparam T
    * Object type used for invocation.
    *
    * @tparam Args
    * Argument types forwarded to the member function.
    */
   template<
         typename MethodT,
         MethodT Method,
         typename C,
         typename T,
         typename... Args
      >
   concept CanInvoke =
      std::is_member_function_pointer_v< MethodT > &&
      std::is_base_of_v< C, std::remove_reference_t< T > > &&
      requires( T t, Args... args )
      {
         ( t.*Method )( std::forward< Args >( args )... );
      };

   //------------------------------------------------------------
   // invoke_if_exists
   //------------------------------------------------------------

   /**
    * @brief
    * Conditionally invokes a member function if it exists.
    *
    * If the member function specified by `Method` is callable on the
    * provided object with the given arguments, it is invoked and the
    * result is returned wrapped in `std::optional`.
    *
    * If the function does not exist, `std::nullopt` is returned and
    * no code is generated for the call.
    *
    * @tparam MethodT
    * Pointer-to-member-function type.
    *
    * @tparam Method
    * Pointer-to-member-function value.
    *
    * @tparam C
    * Class owning the member function.
    *
    * @tparam T
    * Object type.
    *
    * @tparam Args
    * Argument types.
    *
    * @param obj
    * Object on which the member function is conditionally invoked.
    *
    * @param args
    * Arguments forwarded to the member function.
    *
    * @return
    * - `std::optional<R>` if the member function exists and returns `R`.
    * - `std::optional<std::monostate>` if the function returns `void`.
    * - `std::nullopt` if the function does not exist.
    */
   template<
         typename MethodT,
         MethodT Method,
         typename C,
         typename T,
         typename... Args
      >
   auto invoke_if_exists( T &obj, Args &&... args )
   {
      if constexpr( !CanInvoke< MethodT, Method, C, T, Args... > )
      {
         return std::optional< std::monostate >{ };
      }
      else
      {
         using Result =
            decltype( ( obj.*Method )( std::forward< Args >( args )... ) );

         if constexpr( std::is_void_v< Result > )
         {
            ( obj.*Method )( std::forward< Args >( args )... );
            return std::optional< std::monostate >{ std::monostate{ } };
         }
         else
         {
            return std::optional< Result >{
                  ( obj.*Method )( std::forward< Args >( args )... )
               };
         }
      }
   }

   //------------------------------------------------------------
   // MethodInvoker
   //------------------------------------------------------------

   /**
    * @struct MethodInvoker
    *
    * @brief
    * Callable object wrapping a conditional member function invocation.
    *
    * This type enables a compact and readable call syntax while
    * preserving compile-time dispatch and zero runtime overhead.
    *
    * @tparam MethodT
    * Pointer-to-member-function type.
    *
    * @tparam Method
    * Pointer-to-member-function value.
    */
   template< typename MethodT, MethodT Method >
   struct MethodInvoker
   {
      using Class =
         typename MemberClass< MethodT >::type;

      /**
       * @brief
       * Invokes the wrapped member function conditionally.
       *
       * @param obj
       * Object on which the member function is invoked.
       *
       * @param args
       * Arguments forwarded to the member function.
       */
      template< typename T, typename... Args >
      auto operator( )( T &obj, Args &&... args ) const
      {
         return invoke_if_exists<
               MethodT,
               Method,
               Class
            >( obj, std::forward< Args >( args )... );
      }
   };

   //------------------------------------------------------------
   // Variable template
   //------------------------------------------------------------

   /**
    * @var method
    *
    * @brief
    * Variable template providing a convenient entry point for creating
    * a `MethodInvoker`.
    *
    * This variable template binds a specific member function at
    * compile time and allows invoking it conditionally via a simple
    * function-call syntax.
    *
    * @tparam MethodT
    * Pointer-to-member-function type.
    *
    * @tparam Method
    * Pointer-to-member-function value.
    */
   template< typename MethodT, MethodT Method >
   inline constexpr MethodInvoker< MethodT, Method > method{ };

} // namespace carpc

/**
 * @def CARPC_METHOD(sig, cls, name)
 *
 * @brief
 * Convenience macro to simplify creation of a MethodInvoker for
 * overloaded member functions.
 *
 * This macro resolves overloads explicitly using `static_cast`
 * and forwards the result to `carpc::method`.
 *
 * @param sig
 * Exact member function signature.
 *
 * @param cls
 * Class name owning the member function.
 *
 * @param name
 * Member function name.
 *
 * @example
 * @code
 * auto r =
 *    CARPC_METHOD(int (WithFoo::*)(int), WithFoo, foo)(obj, 10);
 * @endcode
 */
#define CARPC_METHOD(sig, cls, name) \
   carpc::method< sig, static_cast< sig >(&cls::name) >
