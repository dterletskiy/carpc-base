#pragma once

#include <limits>
#include <functional>
#include <compare>



namespace carpc {

   template< typename V >
   concept IntegralType =
      std::integral< V > &&
      requires
      {
         std::numeric_limits< V >::min( );
         std::numeric_limits< V >::max( );
      };



   template< typename Derived, IntegralType V >
   class TStrongIntegral
   {
      public:
         using VALUE_TYPE        = V;
         using tDerived          = Derived;
         using tStrongIntegral   = TStrongIntegral< Derived, V >;

      public:
         static constexpr VALUE_TYPE min_value
            = std::numeric_limits< VALUE_TYPE >::min( );

         static constexpr VALUE_TYPE max_value
            = std::numeric_limits< VALUE_TYPE >::max( );

         static constexpr VALUE_TYPE invalid_value = max_value;
         static constexpr VALUE_TYPE zero_value    = 0;

      protected:
         explicit TStrongIntegral( VALUE_TYPE value = invalid_value )
            : m_value( clamp( value ) )
            , m_hash( std::hash< VALUE_TYPE >{ }( m_value ) )
         { }

      public:
         TStrongIntegral( const tStrongIntegral& ) = default;
         ~TStrongIntegral( ) = default;

         TStrongIntegral& operator=( const tStrongIntegral& ) = default;

      public:
         auto operator<=>( const tStrongIntegral& ) const = default;
         bool operator==( const tStrongIntegral& ) const = default;

      public:
         tDerived& operator++( )
         {
            if( is_valid( ) && m_value < max_value )
            {
               ++m_value;
               m_hash = std::hash< VALUE_TYPE >{ }( m_value );
            }
            return derived( );
         }

         tDerived operator++( int )
         {
            tDerived tmp{ derived( ) };
            ++( *this );
            return tmp;
         }

         tDerived& operator--( )
         {
            if( is_valid( ) && m_value > min_value )
            {
               --m_value;
               m_hash = std::hash< VALUE_TYPE >{ }( m_value );
            }
            return derived( );
         }

         tDerived operator--( int )
         {
            tDerived tmp{ derived( ) };
            --( *this );
            return tmp;
         }

         tDerived operator+( VALUE_TYPE value ) const
         {
            if( is_valid( ) && m_value <= ( max_value - value ) )
               return tDerived( m_value + value );
            return derived( );
         }

         tDerived operator-( VALUE_TYPE value ) const
         {
            if( is_valid( ) && m_value >= ( min_value + value ) )
               return tDerived( m_value - value );
            return derived( );
         }

      public:
         const VALUE_TYPE& value( ) const
         {
            return m_value;
         }

         [[nodiscard]] bool is_valid( ) const
         {
            return m_value != invalid_value;
         }

         [[nodiscard]] bool is_invalid( ) const
         {
            return m_value == invalid_value;
         }

         std::size_t hash( ) const
         {
            return m_hash;
         }

      protected:
         static constexpr VALUE_TYPE clamp( VALUE_TYPE value )
         {
            if( value < min_value )
               return min_value;
            if( value > max_value )
               return max_value;
            return value;
         }

         tDerived& derived( )
         {
            return static_cast< tDerived& >( *this );
         }

         const tDerived& derived( ) const
         {
            return static_cast< const tDerived& >( *this );
         }

      protected:
         VALUE_TYPE m_value{ invalid_value };
         std::size_t m_hash{ std::hash< VALUE_TYPE >{ }( invalid_value ) };
   };

} // namespace carpc
