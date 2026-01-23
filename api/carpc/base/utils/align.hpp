#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <limits>



namespace carpc::utils
{

   /**
    * @brief Safely aligns a value to the next multiple of alignment.
    * 
    * This function works for any integral type and handles overflow.
    * If alignment is 0, the original value is returned.
    * If overflow would occur, the result is saturated to the type max.
    * For power-of-2 alignment, a fast bitwise method is used.
    * 
    * @tparam T Integral type of value and alignment.
    * @param value The value to align.
    * @param alignment The alignment boundary (any positive number).
    * @return T The aligned value, or max if overflow occurs.
    */
   template< typename T >
   constexpr T align_safe( T value, T alignment )
   {
      static_assert(
            std::is_integral_v< T >,
            "align_safe requires integral type"
         );

      if( alignment == 0 )
         return value; // No alignment needed

      // Check for overflow in value + alignment - 1
      if( value > std::numeric_limits< T >::max( ) - ( alignment - 1 ) )
         return std::numeric_limits< T >::max( ); // Saturate to max value

      // Fast path if alignment is power of 2
      if( ( alignment & ( alignment - 1 ) ) == 0 )
         return ( value + alignment - 1 ) & ~( alignment - 1 );

      // General case
      return (( value + alignment - 1 ) / alignment) * alignment;
   }

   /**
    * @brief Quickly aligns a value to the next multiple of alignment.
    * 
    * This function works for any integral type but does not check for
    * overflow. If alignment is 0, the original value is returned. 
    * For power-of-2 alignment, a fast bitwise method is used. Otherwise,
    * a general division/multiplication is used.
    * 
    * @tparam T Integral type of value and alignment.
    * @param value The value to align.
    * @param alignment The alignment boundary (any positive number).
    * @return T The aligned value.
    */
   template< typename T >
   constexpr inline T align_fast( T value, T alignment )
   {
      static_assert(
            std::is_integral_v< T >,
            "align_fast requires integral type"
         );

      return alignment == 0 ? value
            : ( ( alignment & ( alignment - 1 ) ) == 0 ? 
                ( value + alignment - 1 ) & ~( alignment - 1 ) :
                (( value + alignment - 1 ) / alignment) * alignment );
   }

   /**
    * @brief Alias for align_fast function.
    * 
    * This function behaves exactly like align_fast and is provided
    * as a convenient general-purpose name for alignment operations.
    * 
    * @tparam T Integral type of value and alignment.
    * @param value The value to align.
    * @param alignment The alignment boundary (any positive number).
    * @return T The aligned value.
    */
   template< typename T >
   constexpr inline T align( T value, T alignment )
   {
      return align_fast( value, alignment );
   }

}
