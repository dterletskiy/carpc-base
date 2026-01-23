#pragma once

#include <cstddef>



namespace carpc::math
{

   /**
    * @brief Checks if a given value is a power of 2.
    *
    * This function returns true if the input value `v` is a power of 2.
    * Zero is not considered a power of 2.
    *
    * @param v The value to check.
    * @return true if `v` is a power of 2 and non-zero, false otherwise.
    *
    * @note This function is constexpr and noexcept, so it can be evaluated
    *       at compile-time and does not throw exceptions.
    */
   constexpr bool is_pow2( std::size_t v ) noexcept
   {
      return v && !( v & ( v - 1 ) );
   }

   /**
    * @brief Rounds a value up to the nearest power of 2.
    *
    * Given an input value `v`, this function returns the smallest power of 2
    * that is greater than or equal to `v`.
    *
    * @param v The value to round up. If `v` is 0, the function returns 1.
    * @return The smallest power of 2 greater than or equal to `v`.
    *
    * @note This function is constexpr and noexcept, so it can be evaluated
    *       at compile-time and does not throw exceptions.
    * @note The implementation works for all `size_t` sizes (32-bit and 64-bit).
    */
   constexpr std::size_t round_pow2( std::size_t v ) noexcept
   {
      if(v == 0)
         return 1;
      --v;
      v |= v >> 1;
      v |= v >> 2;
      v |= v >> 4;
      v |= v >> 8;
      v |= v >> 16;
   #if SIZE_MAX > 0xFFFFFFFF
      v |= v >> 32;
   #endif
      return v + 1;
   }


}
