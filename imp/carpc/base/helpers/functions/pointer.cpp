#include "carpc/base/helpers/functions/pointer.hpp"



namespace carpc {

   void* const inc( void* const ptr, const std::size_t value )
   {
      return static_cast< void* const >( static_cast< std::uint8_t* const >( ptr ) + value );
   }

   const void* const inc( const void* const ptr, const std::size_t value )
   {
      return static_cast< const void* const >( static_cast< const std::uint8_t* const >( ptr ) + value );
   }

   void* const dec( void* const ptr, const std::size_t value )
   {
      return static_cast< void* const >( static_cast< std::uint8_t* const >( ptr ) - value );
   }

   const void* const dec( const void* const ptr, const std::size_t value )
   {
      return static_cast< const void* const >( static_cast< const std::uint8_t* const >( ptr ) - value );
   }

   std::ptrdiff_t diff( const void* const ptr1, const void* const ptr2 )
   {
      return static_cast< const std::uint8_t* const >( ptr1 ) - static_cast< const std::uint8_t* const >( ptr2 );
   }

} // namespace carpc
