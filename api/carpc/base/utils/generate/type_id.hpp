#pragma once



#include <random>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <functional>

#include "carpc/base/utils/hash.hpp"

namespace carpc::utils::generate::type_id {

   /**
    * @brief Returns a unique, human-readable name for the type T.
    * 
    * This function generates a string representing the type T. Depending on
    * the build configuration and compiler, the implementation uses different
    * mechanisms:
    * 
    * - If RTTI is enabled (CARPC_BUILD_RTTI_ENABLED is defined), 
    *    it uses `typeid(T).name()`.
    * - If RTTI is disabled:
    *    - On GCC or Clang, it uses `__PRETTY_FUNCTION__`.
    *    - On MSVC, it uses `__FUNCSIG__`.
    *    - Otherwise, it falls back to a generated string using the 
    *       function's address.
    * 
    * The returned value is stored as a static variable, so the string is
    * initialized only once per type T and remains valid for the lifetime
    * of the program.
    * 
    * @tparam T The type for which the name is generated.
    * @return const std::string& A reference to the generated type name.
    */
   template< typename T >
   const std::string& name( )
   {
      static const std::string value =
      #ifdef CARPC_BUILD_RTTI_ENABLED
         typeid( T ).name( );
      #else
         #if defined( __clang__ ) || defined( __GNUC__ )
            __PRETTY_FUNCTION__;
         #elif defined( _MSC_VER )
            __FUNCSIG__;
         #else
            // fallback:
            "type_id_fallback_" + std::to_string(
                  reinterpret_cast< std::uintptr_t >( &name< T > )
               );
         #endif
      #endif

      return value;
   }

   /**
    * @brief Returns a stable 64-bit hash value for the type T.
    * 
    * This function computes a hash of the string returned by `name<T>()` using
    * the FNV-1a algorithm (64-bit variant). The hash can be used as a type
    * identifier for purposes such as IPC message routing, compile-time type
    * maps, or runtime type registration.
    * 
    * The value is stored in a static variable, so the hash is computed only
    * once per type T and reused in subsequent calls.
    * 
    * @tparam T The type for which the hash is generated.
    * @return std::uint64_t A 64-bit hash value representing the type T.
    */
   template< typename T >
   std::uint64_t hash( )
   {
      static const std::string n = name< T >( );
      static const std::uint64_t value =
         hash::fnv1a_64( n );

      return value;
   }

} // namespace carpc::utils::generate::type_id
