#pragma once

#include <cstdint>
#include <limits>
#include <compare>



namespace carpc {

   /**
    * @ingroup version_api
    *
    * @brief Concept describing valid version value types.
    *
    * Version components must be integral types.
    */
   template< typename V >
   concept VersionValue = std::integral< V >;

   /**
    * @ingroup version_api
    *
    * @brief Strong-typed semantic version.
    *
    * TVersion represents a semantic version bound to
    * a specific object domain.
    *
    * @tparam T Object domain type.
    * @tparam V Integral type used for version components.
    */
   template< typename T, VersionValue V = std::uint8_t >
   class TVersion
   {
      public:
         /**
          * @brief Alias for this TVersion specialization.
          */
         using tVersion = TVersion< T, V >;

         /**
          * @brief Object domain type.
          */
         using OBJECT_TYPE = T;

         /**
          * @brief Underlying value type for version components.
          */
         using VALUE_TYPE = V;

         /**
          * @brief Minimum allowed value for a version component.
          */
         static constexpr VALUE_TYPE min_value
            = std::numeric_limits< VALUE_TYPE >::min( );

         /**
          * @brief Maximum allowed value for a version component.
          */
         static constexpr VALUE_TYPE max_value
            = std::numeric_limits< VALUE_TYPE >::max( );

         /**
          * @brief Invalid version component value.
          */
         static constexpr VALUE_TYPE invalid_value = max_value;

         /**
          * @brief Zero version component value.
          */
         static constexpr VALUE_TYPE zero_value = 0;

         /**
          * @brief Predefined zero version (0.0.0).
          */
         static const inline tVersion zero{ zero_value, zero_value, zero_value };

         /**
          * @brief Predefined invalid version.
          */
         static const inline tVersion invalid{
            invalid_value,
            invalid_value,
            invalid_value
         };

      public:
         /**
          * @brief Default constructor.
          *
          * Constructs version 0.0.0.
          */
         TVersion( )
            : m_major( zero_value )
            , m_minor( zero_value )
            , m_patch( zero_value )
         { }

         /**
          * @brief Construct version from components.
          *
          * @param major Major version.
          * @param minor Minor version.
          * @param patch Patch version.
          */
         TVersion(
            const VALUE_TYPE& major,
            const VALUE_TYPE& minor,
            const VALUE_TYPE& patch
         )
            : m_major( major )
            , m_minor( minor )
            , m_patch( patch )
         { }

         /**
          * @brief Copy constructor.
          */
         TVersion( const tVersion& ) = default;

         /**
          * @brief Destructor.
          */
         ~TVersion( ) = default;

      public:
         /**
          * @brief Three-way comparison operator.
          *
          * Performs lexicographical comparison:
          * major, then minor, then patch.
          */
         auto operator<=>( const tVersion& ) const = default;

         /**
          * @brief Equality comparison.
          */
         bool operator==( const tVersion& ) const = default;

      public:
         /**
          * @brief Get major version.
          */
         const VALUE_TYPE& major( ) const
         {
            return m_major;
         }

         /**
          * @brief Get minor version.
          */
         const VALUE_TYPE& minor( ) const
         {
            return m_minor;
         }

         /**
          * @brief Get patch version.
          */
         const VALUE_TYPE& patch( ) const
         {
            return m_patch;
         }

      public:
         /**
          * @brief Increment major version.
          *
          * Resets minor and patch to zero.
          *
          * @return Reference to this version.
          */
         tVersion& inc_major( )
         {
            ++m_major;
            m_minor = zero_value;
            m_patch = zero_value;
            return *this;
         }

         /**
          * @brief Increment minor version.
          *
          * Resets patch to zero.
          *
          * @return Reference to this version.
          */
         tVersion& inc_minor( )
         {
            ++m_minor;
            m_patch = zero_value;
            return *this;
         }

         /**
          * @brief Increment patch version.
          *
          * @return Reference to this version.
          */
         tVersion& inc_patch( )
         {
            ++m_patch;
            return *this;
         }

      public:
         /**
          * @brief Check whether version is valid.
          *
          * @return True if none of the components is invalid.
          */
         [[nodiscard]] bool is_valid( ) const
         {
            return
               m_major != invalid_value &&
               m_minor != invalid_value &&
               m_patch != invalid_value;
         }

         /**
          * @brief Check whether version is invalid.
          */
         [[nodiscard]] bool is_invalid( ) const
         {
            return !is_valid( );
         }

      public:
         /**
          * @brief Compute hash for this version.
          *
          * Combines major, minor, and patch into a single size_t hash.
          * Can be used for unordered containers without storing a cached hash.
          *
          * @return Hash value of this version.
          */
         std::size_t hash( ) const
         {
            std::size_t h = std::hash< V >{ }( m_major );
            h ^= std::hash< V >{ }( m_minor ) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash< V >{ }( m_patch ) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
         }

      private:
         /**
          * @note Field order is significant.
          *
          * The order of these data members defines the
          * lexicographical comparison used by the
          * defaulted comparison operators
          * (operator<=> and operator==).
          *
          * The fields must remain declared in the
          * following order:
          *   1. m_major
          *   2. m_minor
          *   3. m_patch
          *
          * Changing this order will change the semantic
          * meaning of version comparisons and will
          * break the intended version ordering.
          */
         VALUE_TYPE m_major{ zero_value };
         VALUE_TYPE m_minor{ zero_value };
         VALUE_TYPE m_patch{ zero_value };
   };

   /**
    * @brief Stream output operator for TVersion.
    *
    * Enabled only if VALUE_TYPE supports operator<<.
    *
    * Outputs version as "major.minor.patch".
    *
    * @param os Output stream.
    * @param version Version object to output.
    * @return Reference to output stream.
    */
   template< typename T, typename V >
      requires requires( std::ostream& os, const V& v )
      {
         os << v;
      }
   inline std::ostream& operator<<( std::ostream& os, const TVersion< T, V >& version )
   {
      using DisplayType = 
         std::conditional_t< sizeof( V ) < sizeof( int ), int, V >;

      return os << static_cast<DisplayType>( version.major( ) ) << '.'
                << static_cast<DisplayType>( version.minor( ) ) << '.'
                << static_cast<DisplayType>( version.patch( ) );
   }

} // namespace carpc



#include <functional>

namespace std {

   template< typename T, typename V >
   struct hash< carpc::TVersion< T, V > >
   {
      /**
       * @brief Compute hash of a TVersion.
       *
       * Delegates to TVersion::hash(), which combines
       * major, minor, and patch into a single size_t value.
       *
       * @param version Version to hash.
       * @return Hash value.
       */
      std::size_t operator( )( const carpc::TVersion< T, V >& version ) const noexcept
      {
         return version.hash( );
      }
   };

} // namespace std
