#pragma once

#include <cstddef>
#include <cstdint>
#include <string>



namespace carpc::hash
{

   /**
    * @brief Computes a 64-bit FNV-1a hash for a character sequence.
    *
    * This function implements the 64-bit variant of the FNV-1a (Fowler–Noll–Vo)
    * hashing algorithm. It can be used to generate stable hash values for 
    * strings, type identifiers, or other purposes where a fast, deterministic, 
    * non-cryptographic hash is required.
    *
    * This overload operates on a raw character array with a specified length.
    * It is marked `constexpr` and can be evaluated at compile time when used 
    * with string literals.
    *
    * @param str Pointer to the first character of the string to hash.
    * @param len Length of the string in characters (excluding null terminator).
    * @return std::uint64_t 64-bit hash of the input string.
    */
   constexpr std::uint64_t fnv1a_64( const char* str, std::size_t len )
   {
      std::uint64_t hash = 0xcbf29ce484222325ull;
      for( std::size_t i = 0; i < len; ++i )
      {
         hash ^= static_cast< std::uint64_t >( str[i] );
         hash *= 0x100000001b3ull;
      }
      return hash;
   }


   /**
    * @brief Computes a 64-bit FNV-1a hash for a std::string.
    *
    * This inline overload allows computing a hash directly from a std::string.
    * Internally, it forwards to the raw character pointer overload and uses the
    * string's length. This function is evaluated at runtime and is convenient for
    * dynamically generated strings or strings obtained from runtime sources.
    *
    * @param str Reference to the std::string to hash.
    * @return std::uint64_t 64-bit hash of the input string.
    */
   inline std::uint64_t fnv1a_64( const std::string& str )
   {
      return fnv1a_64( str.c_str( ), str.length( ) );
   }

}
