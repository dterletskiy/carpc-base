#pragma once



#include <string>
#include <limits>
#include <type_traits>

#include "carpc/trace/trace.h"

namespace carpc::utils {

   /**
    * @brief Primary template to map a type T to its string representation.
    * 
    * This template provides a constexpr string representing the type name. 
    * Specializations exist for all standard integral and floating point types.
    * 
    * @tparam T Type to map to string.
    * 
    * @note Unknown or unsupported types return "unknown".
    */
   template< typename T >
   struct TypeName
   {
      static constexpr const char* name = "";
   };

   // Signed integer types
   template< > struct TypeName< char >
      { static constexpr const char* name = "char"; };
   template< > struct TypeName< signed char >
      { static constexpr const char* name = "signed char"; };
   template< > struct TypeName< short >
      { static constexpr const char* name = "short"; };
   template< > struct TypeName< int >
      { static constexpr const char* name = "int"; };
   template< > struct TypeName< long >
      { static constexpr const char* name = "long"; };
   template< > struct TypeName< long long >
      { static constexpr const char* name = "long long"; };

   // Unsigned integer types
   template< > struct TypeName< unsigned char >
      { static constexpr const char* name = "unsigned char"; };
   template< > struct TypeName< unsigned short >
      { static constexpr const char* name = "unsigned short"; };
   template< > struct TypeName< unsigned int >
      { static constexpr const char* name = "unsigned int"; };
   template< > struct TypeName< unsigned long >
      { static constexpr const char* name = "unsigned long"; };
   template< > struct TypeName< unsigned long long >
      { static constexpr const char* name = "unsigned long long"; };

   // Floating point types
   template< > struct TypeName< float >
      { static constexpr const char* name = "float"; };
   template< > struct TypeName< double >
      { static constexpr const char* name = "double"; };
   template< > struct TypeName< long double >
      { static constexpr const char* name = "long double"; };

   // Boolean type
   template< > struct TypeName< bool >
      { static constexpr const char* name = "bool"; };

   /**
    * @brief Convert a type T to its string representation.
    * 
    * This function returns the string literal corresponding to the type T. 
    * It removes any const or volatile qualifiers automatically.
    * 
    * @tparam T Type to convert.
    * @return constexpr const char* A string literal representing the type name.
    * 
    * @note If the type is unknown, returns "unknown".
    * @note Works at compile-time.
    * 
    * @code{.cpp}
    * constexpr auto t1 = utils::to_string<int>();      // "int"
    * constexpr auto t2 = utils::to_string<float>();    // "float"
    * constexpr auto t3 = utils::to_string<const bool>(); // "bool"
    * @endcode
    */
   template< typename T >
   constexpr const char* to_string( )
   {
      return TypeName< std::remove_cv_t< T > >::name; // ignore const/volatile
   }

} // namespace carpc::utils

# define TYPE_INFO( TYPE ) \
   CARPC_TRACE_LOG_TRACE( \
         "'%s' -> '%s' (min = %s / max = %s / sizeof = %zu)" \
         , #TYPE \
         , carpc::utils::to_string< TYPE >( ) \
         , std::to_string( std::numeric_limits< TYPE >::min( ) ).c_str( ) \
         , std::to_string( std::numeric_limits< TYPE >::max( ) ).c_str( ) \
         , sizeof( TYPE ) \
      )



#include <string>
#include <charconv>
#include <limits>
#include <type_traits>
#include <iostream>
#include <cstdlib>

#include "carpc/trace/trace.h"

namespace carpc::utils {

   /**
    * @brief Converts a string to a numeric value of integral or 
    * floating-point type.
    *
    * This function attempts to parse a string and store its value in 
    * the provided
    * numeric variable. It supports all standard integral types (signed and
    * unsigned) and floating-point types (float, double, long double).
    * Conversion errors, including invalid input or overflow/underflow, are
    * detected and reported without throwing exceptions.
    *
    * @tparam T Numeric type to convert to. Must be an integral or 
    * floating-point type.
    *
    * @param value_str The input string containing the numeric value.
    * @param value Reference to the variable where the converted value 
    *              will be stored. If conversion fails, value remains 
    *              default-initialized (T{}).
    * @param pos Optional pointer to a std::size_t variable. If provided, it will
    *            be set to the number of characters processed during conversion.
    * @param base Numeric base for integral types. Ignored for floating-point 
    *             types.
    *             Defaults to 10. Valid values are 2..36.
    *
    * @return true if the conversion was successful and the value is within the
    *         valid range of the type T; false if conversion failed due to 
    *         invalid format, overflow, underflow, or unsupported type.
    *
    * @note This function uses `std::from_chars` for conversion of both 
    *       integral and floating-point types (C++17). For floating-point 
    *       types, full support depends on the standard library implementation.
    *       On failure, no exceptions are thrown.
    *
    * @code{.cpp}
    * int32_t i = 0;
    * if(from_string("12345", i))
    * {
    *     // i == 12345
    * }
    *
    * uint16_t u = 0;
    * if(!from_string("70000", u))
    * {
    *     // Overflow detected, u == 0
    * }
    *
    * float f = 0.0f;
    * if(from_string("3.14", f))
    * {
    *     // f == 3.14f
    * }
    *
    * std::size_t pos = 0;
    * int x = 0;
    * if(from_string("42abc", x, &pos))
    * {
    *     // x == 42, pos == 2
    * }
    * @endcode
    */
   template< typename T >
   bool from_string(
         const std::string& value_str,
         T& value,
         std::size_t* pos = nullptr,
         int base = 10
      )
   {
      static_assert(
            std::is_integral_v< T > || std::is_floating_point_v< T >,
            "from_string supports only integral or floating-point types"
         );

      value = T{ }; // default initialize
      bool result = true;

      const char* begin = value_str.data( );
      const char* end = value_str.data( ) + value_str.size( );

      if constexpr (std::is_integral_v< T >)
      {
         auto [ptr, ec] = std::from_chars( begin, end, value, base );
         if( ec != std::errc( ) )
         {
            // CARPC_TRACE_LOG_ERROR(
            //       "Error: failed to convert '%s' to integral type",
            //       value_str.c_str( )
            //    );
            result = false;
         }
         if( pos )
            *pos = static_cast< std::size_t >( ptr - begin );
      }
      else if constexpr (std::is_floating_point_v< T >)
      {
         // from_chars for float/double/long double
         auto [ptr, ec] = std::from_chars(
               begin, end, value, std::chars_format::general
            );
         if( ec != std::errc( ) )
         {
            // CARPC_TRACE_LOG_ERROR(
            //       "Error: failed to convert '%s' to floating-point type",
            //       value_str.c_str( )
            //    );
            result = false;
         }
         if( pos )
            *pos = static_cast< std::size_t >( ptr - begin );
      }

      return result;
   }

}
