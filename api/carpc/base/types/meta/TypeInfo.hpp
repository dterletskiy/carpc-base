#pragma once

#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <string>
#include <vector>
#include <list>
#include <forward_list>
#include <queue>
#include <deque>
#include <stack>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <tuple>
#include <optional>



namespace carpc::types {

   using tID = std::uint16_t;

   constexpr tID invalid_id = std::numeric_limits< tID >::max( );



   enum class ETypeCategory : std::uint8_t
   {
      Invalid   = 0,
      Scalar,
      Container,
      Tuple
   };



   enum class ETypeID : tID
   {
      Invalid = invalid_id,

      UInt8  = 1,
      UInt16,
      UInt32,
      UInt64,

      Int8,
      Int16,
      Int32,
      Int64,

      Char,
      Bool,

      Float,
      Double,
      LongDouble,

      String,

      Vector,
      List,
      ForwardList,
      Queue,
      Deque,
      Stack,

      Set,
      MultiSet,
      UnorderedSet,
      UnorderedMultiSet,

      Pair,
      Map,
      MultiMap,
      UnorderedMap,
      UnorderedMultiMap,

      Tuple,
      Optional
   };



   template< typename T >
   struct TTypeInfo
   {
      using TYPE = T;

      static constexpr const char* const name = "undefined";
      static constexpr ETypeID id = ETypeID::Invalid;
      static constexpr ETypeCategory category = ETypeCategory::Invalid;

      static constexpr bool is_registered = false;

      const char* get_name( ) const { return name; }
      ETypeID get_id( ) const       { return id;   }
   };



   template< typename T >
   inline constexpr bool is_registered_type_v =
      TTypeInfo< T >::is_registered;



   #define CARPC_DEFINE_TYPE_INFO_SIMPLE( ID, CATEGORY, T )       \
      template< >                                                 \
      struct TTypeInfo< T >                                       \
      {                                                           \
         using TYPE = T;                                          \
                                                                  \
         static constexpr const char* const name = #T;            \
         static constexpr ETypeID id = ID;                        \
         static constexpr ETypeCategory category = CATEGORY;      \
                                                                  \
         static constexpr bool is_registered = true;              \
                                                                  \
         const char* get_name( ) const { return name; }           \
         ETypeID get_id( ) const       { return id;   }           \
      };



   #define CARPC_DEFINE_TYPE_INFO_TEMPLATE( ID, CATEGORY, T )     \
      template< typename ... TYPES >                              \
      struct TTypeInfo< T< TYPES... > >                           \
      {                                                           \
         using TYPE = T< TYPES... >;                              \
                                                                  \
         static constexpr const char* const name = #T;            \
         static constexpr ETypeID id = ID;                        \
         static constexpr ETypeCategory category = CATEGORY;      \
                                                                  \
         static constexpr bool is_registered = true;              \
                                                                  \
         const char* get_name( ) const { return name; }           \
         ETypeID get_id( ) const       { return id;   }           \
      };



   // ===== Scalars =====

   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::UInt8,       ETypeCategory::Scalar, std::uint8_t  )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::UInt16,      ETypeCategory::Scalar, std::uint16_t )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::UInt32,      ETypeCategory::Scalar, std::uint32_t )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::UInt64,      ETypeCategory::Scalar, std::uint64_t )

   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Int8,        ETypeCategory::Scalar, std::int8_t   )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Int16,       ETypeCategory::Scalar, std::int16_t  )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Int32,       ETypeCategory::Scalar, std::int32_t  )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Int64,       ETypeCategory::Scalar, std::int64_t  )

   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Char,        ETypeCategory::Scalar, char          )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Bool,        ETypeCategory::Scalar, bool          )

   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Float,       ETypeCategory::Scalar, float         )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::Double,      ETypeCategory::Scalar, double        )
   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::LongDouble,  ETypeCategory::Scalar, long double   )

   CARPC_DEFINE_TYPE_INFO_SIMPLE( ETypeID::String,      ETypeCategory::Container, std::string )



   // ===== Containers =====

   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Vector,              ETypeCategory::Container, std::vector )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::List,                ETypeCategory::Container, std::list )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::ForwardList,         ETypeCategory::Container, std::forward_list )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Queue,               ETypeCategory::Container, std::queue )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Deque,               ETypeCategory::Container, std::deque )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Stack,               ETypeCategory::Container, std::stack )

   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Set,                 ETypeCategory::Container, std::set )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::MultiSet,            ETypeCategory::Container, std::multiset )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::UnorderedSet,        ETypeCategory::Container, std::unordered_set )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::UnorderedMultiSet,   ETypeCategory::Container, std::unordered_multiset )

   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Map,                 ETypeCategory::Container, std::map )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::MultiMap,            ETypeCategory::Container, std::multimap )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::UnorderedMap,        ETypeCategory::Container, std::unordered_map )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::UnorderedMultiMap,   ETypeCategory::Container, std::unordered_multimap )

   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Pair,                ETypeCategory::Tuple, std::pair )
   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Tuple,               ETypeCategory::Tuple, std::tuple )

   CARPC_DEFINE_TYPE_INFO_TEMPLATE( ETypeID::Optional,            ETypeCategory::Container, std::optional )



   #undef CARPC_DEFINE_TYPE_INFO_SIMPLE
   #undef CARPC_DEFINE_TYPE_INFO_TEMPLATE



   // ===== Compile-time helpers for serialization =====

   template< typename T >
   constexpr ETypeCategory get_type_category( )
   {
      static_assert(
         is_registered_type_v< T >,
         "Type is not registered in TTypeInfo"
      );

      return TTypeInfo< T >::category;
   }

   template< typename T >
   constexpr ETypeID get_type_id( )
   {
      static_assert(
         is_registered_type_v< T >,
         "Type is not registered in TTypeInfo"
      );

      return TTypeInfo< T >::id;
   }

   template< typename T >
   constexpr const char* get_type_name( )
   {
      static_assert(
         is_registered_type_v< T >,
         "Type is not registered in TTypeInfo"
      );

      return TTypeInfo< T >::name;
   }

} // namespace carpc::types
