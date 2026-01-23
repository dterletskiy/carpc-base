#pragma once

#include <string>
#include <string_view>
#include <compare>
#include <ostream>
#include <functional>



namespace carpc {

   /**
    * @brief Traits class for TName value types.
    *
    * TNameTraits defines properties associated with a particular
    * VALUE_TYPE used inside TName.
    *
    * At minimum, a specialization must define an invalid_value
    * which represents an invalid or uninitialized name.
    *
    * @tparam V Underlying value type.
    */
   template< typename V >
   struct TNameTraits;

   template< >
   struct TNameTraits< std::string >
   {
      static const std::string invalid_value;
   };

   inline const std::string
      TNameTraits< std::string >::invalid_value{ "NoName" };

   template< >
   struct TNameTraits< std::string_view >
   {
      static constexpr std::string_view invalid_value{ "" };
   };



   template< typename V >
   concept NameValue =
      std::regular< V > &&
      requires( const V& v )
      {
         { v < v } -> std::convertible_to< bool >;
         { std::hash< V >{ }( v ) } -> std::convertible_to< std::size_t >;
      };



   /**
    * @brief Strong-typed wrapper for object names.
    *
    * TName is a lightweight value-type wrapper around a name value
    * (usually std::string or std::string_view) that represents
    * a name of some domain object.
    *
    * The template parameter @p T defines the object type for which
    * the name is intended. This allows creating distinct name types
    * for different domains while using the same underlying value
    * representation.
    *
    * Example:
    * @code
    * struct Service {};
    *
    * using ServiceName = TName< Service >;
    *
    * ServiceName audio{ "Audio" };
    * ServiceName video{ "Video" };
    *
    * if( audio.is_valid( ) )
    * {
    *    std::cout << audio << std::endl;
    * }
    * @endcode
    *
    * Key properties:
    * - Provides strong typing for object names.
    * - Prevents accidental mixing of names of different object types.
    * - Supports comparison and ordering.
    * - Supports efficient usage as a key in unordered containers.
    *
    * Invalid value:
    * Each VALUE_TYPE must define an invalid value via TNameTraits.
    * The invalid value represents an uninitialized or invalid name.
    *
    * Hashing:
    * TName precomputes and stores a hash value on construction.
    * This makes it suitable for real-time and embedded systems
    * where predictable performance is required.
    *
    * Stream output:
    * If VALUE_TYPE supports operator<< for std::ostream, TName
    * also supports stream output.
    *
    * @tparam T  Object type the name belongs to.
    * @tparam V  Underlying value type used to store the name.
    */
   template< typename T, NameValue V = std::string >
   class TName
   {
      public:
         /**
          * @brief Alias for this TName specialization.
          */
         using tName = TName< T, V >;

         /**
          * @brief Type of the object this name belongs to.
          */
         using OBJECT_TYPE = T;

         /**
          * @brief Underlying value type used to store the name.
          */
         using VALUE_TYPE = V;

         /**
          * @brief Reference to invalid value for this VALUE_TYPE.
          *
          * The value is provided by TNameTraits<V>.
          */
         static const VALUE_TYPE& invalid_value;


         /**
          * @brief Predefined invalid name instance.
          */
         static inline const tName invalid{ invalid_value };

      public:
         /**
          * @brief Construct name from value.
          *
          * If no value is provided, the name is initialized
          * with invalid_value.
          *
          * @param value Name value.
          */
         explicit TName( const VALUE_TYPE& value = invalid_value )
            : m_value( value )
            , m_hash( std::hash< VALUE_TYPE >{ }( value ) )
         { }

         /**
          * @brief Copy constructor.
          */
         TName( const tName& ) = default;

         /**
          * @brief Destructor.
          */
         ~TName( ) = default;

      public:
         /**
          * @brief Three-way comparison operator.
          *
          * Provides ordering based on the underlying value.
          */
         auto operator<=>( const tName& ) const = default;

         /**
          * @brief Equality comparison.
          *
          * Compares underlying values.
          */
         bool operator==( const tName& ) const = default;


         /**
          * @brief Compare with raw value.
          *
          * @param value Value to compare with.
          * @return True if values are equal.
          */
         bool operator==( const VALUE_TYPE& value ) const
         {
            return m_value == value;
         }

      public:
         /**
          * @brief Get underlying name value.
          *
          * @return Const reference to stored value.
          */
         const VALUE_TYPE& value( ) const
         {
            return m_value;
         }

         /**
          * @brief Check whether name is valid.
          *
          * @return True if value is not equal to invalid_value.
          */
         [[nodiscard]] bool is_valid( ) const
         {
            return m_value != invalid_value;
         }

         /**
          * @brief Check whether name is invalid.
          *
          * @return True if value is equal to invalid_value.
          */
         [[nodiscard]] bool is_invalid( ) const
         {
            return m_value == invalid_value;
         }

      private:
         VALUE_TYPE m_value{ invalid_value };

      public:
         /**
          * @brief Get precomputed hash value.
          *
          * The hash is computed during construction and cached.
          * This allows fast usage in unordered containers.
          *
          * @return Hash value.
          */
         std::size_t hash( ) const
         {
            return m_hash;
         }
      private:
         std::size_t m_hash{ std::hash< VALUE_TYPE >{ }( value ) };
   };



   template< typename T, NameValue V >
   const typename TName< T, V >::VALUE_TYPE&
      TName< T, V >::invalid_value =
         TNameTraits< V >::invalid_value;



   /**
    * @brief Stream output operator for TName.
    *
    * Enabled only if VALUE_TYPE supports operator<<.
    *
    * Outputs the underlying value.
    *
    * @param os Output stream.
    * @param name Name to output.
    * @return Reference to output stream.
    */
   template< typename T, typename V >
      requires requires( std::ostream& os, const V& v )
      {
         os << v;
      }
   std::ostream& operator<<( std::ostream& os, const TName< T, V >& name )
   {
      return os << name.value( );
   }

} // namespace carpc



namespace std {

   /**
    * @brief Hash specialization for TName.
    *
    * Allows using TName as a key in unordered containers.
    *
    * Hash is derived from the underlying VALUE_TYPE.
    */
   template< typename T, typename V >
   struct hash< carpc::TName< T, V > >
   {
      /**
       * @brief Compute hash of a TName.
       *
       * @param name Name to hash.
       * @return Hash value.
       */
      std::size_t operator( )( const carpc::TName< T, V >& name ) const noexcept
      {
         // return std::hash< V >{ }( name.value( ) );
         return name.hash( );
      }
   };

} // namespace std

