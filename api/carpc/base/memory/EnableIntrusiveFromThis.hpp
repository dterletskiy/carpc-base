#pragma once

#include "carpc/base/memory/IntrusivePtr.hpp"



#pragma once

#include <cassert>



namespace carpc::memory {

   template< typename T >
   class EnableIntrusiveFromThis
   {
      protected:
         EnableIntrusiveFromThis( ) = default;
         EnableIntrusiveFromThis( const EnableIntrusiveFromThis& ) = default;
         EnableIntrusiveFromThis& operator=( const EnableIntrusiveFromThis& ) = default;
         ~EnableIntrusiveFromThis( ) = default;

      public:
         IntrusivePtr< T > intrusive_from_this( );
         IntrusivePtr< const T > intrusive_from_this( ) const;
   };



   template< typename T >
   IntrusivePtr< T > EnableIntrusiveFromThis< T >::intrusive_from_this( )
   {
#ifndef NO_DEBUG
      assert(
            static_cast< const T* >( this )->ref_count( ) > 0 &&
            "intrusive_from_this() called on unmanaged object"
         );
#endif
      return IntrusivePtr< T >( static_cast< T* >( this ) );
   }

   template< typename T >
   IntrusivePtr< const T > EnableIntrusiveFromThis< T >::intrusive_from_this( ) const
   {
#ifndef NO_DEBUG
      assert(
            static_cast< const T* >( this )->ref_count( ) > 0 &&
            "intrusive_from_this() called on unmanaged object"
         );
#endif
      return IntrusivePtr< const T >( static_cast< const T* >( this ) );
   }

} // namespace carpc::memory
