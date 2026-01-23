#pragma once

#include "carpc/base/memory/IntrusivePtr.hpp"



namespace carpc::memory {

   template< typename T, typename... Args >
   IntrusivePtr< T > make_intrusive( Args&&... args )
   {
      return IntrusivePtr< T >(
            new T( std::forward< Args >( args )... )
         );
   }

} // namespace carpc::memory
