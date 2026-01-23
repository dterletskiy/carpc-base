#pragma once

#include <atomic>
#include <ostream>
#include "carpc/base/types/core/StrongIntegral.hpp"



namespace carpc {

   /**
    * @defgroup id_api ID API
    *
    * @brief Strongly typed unique identifiers based on TStrongIntegral.
    */

   /**
    * @page id_concept TID concept
    *
    * @brief Strongly typed ID concept.
    *
    * TID represents a unique identifier in a domain T.
    * IDs are strongly typed to avoid mixing between domains.
    *
    * Features:
    * - explicit invalid state
    * - min/max bounds
    * - arithmetic (+, -) and increment/decrement operators
    * - automatic ID generation via generate( )
    * - cached std::hash for fast lookups in containers
    *
    * Example usage:
    * @code
    * using MyID = carpc::TID<MyClass>;
    *
    * MyID id1;             // invalid by default
    * MyID id2 = MyID::generate( );
    * ++id2;
    * MyID id3 = id2 + 5;
    *
    * std::unordered_set<MyID> ids;
    * ids.insert(id2);
    * @endcode
    */

   template< typename T, IntegralType V = std::uint64_t >
   class TID : public TStrongIntegral< TID< T, V >, V >
   {
      public:
         using tID = TID< T, V >;
         using BASE = TStrongIntegral< tID,V >;
         using typename BASE::VALUE_TYPE;
         using BASE::min_value;
         using BASE::max_value;
         using BASE::invalid_value;
         using BASE::zero_value;

      public:
         TID( VALUE_TYPE value = invalid_value )
            : BASE( value )
         { }

         TID( const tID& ) = default;
         ~TID( ) = default;
         tID& operator=( const tID& ) = default;

      public:
         /**
          * @brief Predefined minimum ID.
          */
         static const tID min;

         /**
          * @brief Predefined maximum ID.
          */
         static const tID max;

         /**
          * @brief Predefined invalid ID.
          */
         static const tID invalid;

         /**
          * @brief Predefined zero ID.
          */
         static const tID zero;

         /**
          * @brief Generates a new ID.
          *
          * Each call returns a new ID incremented from previous.
          */
         static tID generate( )
         {
            static std::atomic< VALUE_TYPE > counter{ zero_value };
            return tID{ ++counter };
         }
   };

   // Define static constants outside
   template< typename T, IntegralType V >
   const typename TID< T, V >::tID TID< T, V >::min{ min_value };

   template< typename T, IntegralType V >
   const typename TID< T, V >::tID TID< T, V >::max{ max_value };

   template< typename T, IntegralType V >
   const typename TID< T, V >::tID TID< T, V >::invalid{ invalid_value };

   template< typename T, IntegralType V >
   const typename TID< T, V >::tID TID< T, V >::zero{ zero_value };

   /**
    * @brief Stream output operator for TID.
    *
    * Enabled only if VALUE_TYPE supports operator<<.
    */
   template< typename T, IntegralType V >
      requires requires( std::ostream& os, const V& v )
      {
         os << v;
      }
   inline std::ostream& operator<<( std::ostream& os, const TID< T, V >& id )
   {
      return os << id.value( );
   }

} // namespace carpc



namespace std {

   template< typename T, carpc::IntegralType V >
   struct hash< carpc::TID< T, V > >
   {
      std::size_t operator( )( const carpc::TID< T, V >& id ) const noexcept
      {
         return id.hash( );
      }
   };

}
