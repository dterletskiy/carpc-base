#pragma once

#include <ostream>
#include "carpc/base/types/core/StrongIntegral.hpp"



namespace carpc {

   /**
    * @defgroup id_api Priority API
    *
    * @brief Strongly typed unique identifiers based on TStrongIntegral.
    */

   /**
    * @page id_concept TPriority concept
    *
    * @brief Strongly typed Priority concept.
    *
    * TPriority represents a unique identifier in a domain T.
    * Priorities are strongly typed to avoid mixing between domains.
    *
    * Features:
    * - explicit invalid state
    * - min/max bounds
    * - arithmetic (+, -) and increment/decrement operators
    * - cached std::hash for fast lookups in containers
    *
    * Example usage:
    * @code
    * using MyPriority = carpc::TPriority<MyClass>;
    *
    * MyPriority id1;             // invalid by default
    * MyPriority id2{9};
    * ++id2;
    * MyPriority id3 = id2 + 5;
    *
    * std::unordered_set<MyPriority> ids;
    * ids.insert(id2);
    * @endcode
    */

   template< typename T, IntegralType V = std::uint64_t >
   class TPriority : public TStrongIntegral< TPriority< T, V >, V >
   {
      public:
         using tPriority = TPriority< T, V >;
         using BASE = TStrongIntegral< tPriority,V >;
         using typename BASE::VALUE_TYPE;
         using BASE::min_value;
         using BASE::max_value;
         using BASE::invalid_value;
         using BASE::zero_value;

      public:
         TPriority( VALUE_TYPE value = invalid_value )
            : BASE( value )
         { }

         TPriority( const tPriority& ) = default;
         ~TPriority( ) = default;
         tPriority& operator=( const tPriority& ) = default;

      public:
         /**
          * @brief Predefined minimum Priority.
          */
         static const tPriority min;

         /**
          * @brief Predefined maximum Priority.
          */
         static const tPriority max;

         /**
          * @brief Predefined invalid Priority.
          */
         static const tPriority invalid;

         /**
          * @brief Predefined zero Priority.
          */
         static const tPriority zero;
   };

   // Define static constants outside
   template< typename T, IntegralType V >
   const typename TPriority< T, V >::tPriority TPriority< T, V >::min{ min_value };

   template< typename T, IntegralType V >
   const typename TPriority< T, V >::tPriority TPriority< T, V >::max{ max_value };

   template< typename T, IntegralType V >
   const typename TPriority< T, V >::tPriority TPriority< T, V >::invalid{ invalid_value };

   template< typename T, IntegralType V >
   const typename TPriority< T, V >::tPriority TPriority< T, V >::zero{ zero_value };

   /**
    * @brief Stream output operator for TPriority.
    *
    * Enabled only if VALUE_TYPE supports operator<<.
    */
   template< typename T, IntegralType V >
      requires requires( std::ostream& os, const V& v )
      {
         os << v;
      }
   inline std::ostream& operator<<( std::ostream& os, const TPriority< T, V >& id )
   {
      return os << id.value( );
   }

} // namespace carpc



namespace std {

   template< typename T, carpc::IntegralType V >
   struct hash< carpc::TPriority< T, V > >
   {
      std::size_t operator( )( const carpc::TPriority< T, V >& id ) const noexcept
      {
         return id.hash( );
      }
   };

}
