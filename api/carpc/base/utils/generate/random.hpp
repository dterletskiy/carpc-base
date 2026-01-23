#pragma once



#include <random>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <functional>

namespace carpc::utils::generate::random
{

   inline std::mt19937& engine( )
   {
      static std::mt19937 eng
      {
         std::random_device{ }( )
      };
      return eng;
   }

   /**
    * @brief Generate a random alphanumeric string.
    *
    * This function creates a string consisting of random characters
    * taken from the set of digits [0-9], uppercase letters [A-Z],
    * and lowercase letters [a-z].
    *
    * @param length The desired length of the generated string.
    * @return std::string A string of length @p length containing random alphanumeric characters.
    *
    * @note The generated string is not cryptographically secure. For secure random strings,
    *       consider using a cryptographic library.
    */
   std::string text( const std::size_t length );

   /**
    * @brief Generate a random integer of type T within the inclusive range [begin, end].
    *
    * This function works for both signed and unsigned integral types.
    *
    * @tparam T An integral type (e.g., int, long, std::size_t, uint32_t).
    * @param begin The minimum possible value to generate (inclusive).
    * @param end The maximum possible value to generate (inclusive).
    * @return T A randomly selected integer of type T in the range [begin, end].
    *
    * @note The function is noexcept. If begin > end, behavior is undefined.
    */
   template< typename T >
   T number( const T begin, const T end ) noexcept
   {
      static_assert(
         std::is_integral_v< T >,
         "number<T>: T must be integral"
      );

      std::uniform_int_distribution< T > dist
      (
         begin,
         end
      );

      return dist( engine( ) );
   }

   /**
    * @brief Generate a random floating-point number of type T within 
    * the range [begin, end).
    *
    * This function works for floating-point types such as float or double.
    *
    * @tparam T A floating-point type (float or double).
    * @param begin The minimum possible value to generate (inclusive).
    * @param end The maximum possible value to generate (exclusive).
    * @return T A randomly selected floating-point number of type T in 
    *    the range [begin, end).
    *
    * @note The generated number uses uniform real distribution.
    *       The function is noexcept. For unsigned-only ranges, use 
    *       real_positive().
    */
   template< typename T >
   T real( const T begin, const T end ) noexcept
   {
      static_assert(
         std::is_floating_point_v< T >,
         "real<T>: T must be floating point"
      );

      std::uniform_real_distribution< T > dist
      (
         begin,
         end
      );

      return dist( engine( ) );
   }

} // namespace carpc::generate::random
