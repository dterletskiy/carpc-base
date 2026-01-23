#pragma once

#include <type_traits>
#include <utility>
#include <cstddef>



namespace carpc::memory {

   struct AdoptTag
   {
   };

   inline constexpr AdoptTag Adopt { };



   template< typename T >
   class IntrusivePtr
   {
      public:
         using element_type = T;

      public:
         IntrusivePtr( ) noexcept
            : m_ptr( nullptr )
         {
         }

         explicit IntrusivePtr( T* ptr )
            : m_ptr( ptr )
         {
            if( nullptr != m_ptr )
            {
               m_ptr->add_ref( );
            }
         }

         IntrusivePtr( T* ptr, AdoptTag ) noexcept
            : m_ptr( ptr )
         {
         }

         IntrusivePtr( const IntrusivePtr& other )
            : m_ptr( other.m_ptr )
         {
            if( nullptr != m_ptr )
            {
               m_ptr->add_ref( );
            }
         }

         template< typename U >
         IntrusivePtr( const IntrusivePtr< U >& other )
            requires std::is_convertible_v< U*, T* >
            : m_ptr( other.get( ) )
         {
            if( nullptr != m_ptr )
            {
               m_ptr->add_ref( );
            }
         }

         IntrusivePtr( IntrusivePtr&& other ) noexcept
            : m_ptr( other.m_ptr )
         {
            other.m_ptr = nullptr;
         }

         template< typename U >
         IntrusivePtr( IntrusivePtr< U >&& other ) noexcept
            requires std::is_convertible_v< U*, T* >
            : m_ptr( other.get( ) )
         {
            other.reset( );
         }

         ~IntrusivePtr( )
         {
            if( nullptr != m_ptr )
            {
               m_ptr->release( );
            }
         }

      public:
         IntrusivePtr& operator=( const IntrusivePtr& other )
         {
            if( this != &other )
            {
               reset( other.m_ptr );
            }
            return *this;
         }

         template< typename U >
         IntrusivePtr& operator=( const IntrusivePtr< U >& other )
            requires std::is_convertible_v< U*, T* >
         {
            reset( other.get( ) );
            return *this;
         }

         IntrusivePtr& operator=( IntrusivePtr&& other ) noexcept
         {
            if( this != &other )
            {
               reset( );
               m_ptr = other.m_ptr;
               other.m_ptr = nullptr;
            }
            return *this;
         }

         template< typename U >
         IntrusivePtr& operator=( IntrusivePtr< U >&& other ) noexcept
            requires std::is_convertible_v< U*, T* >
         {
            reset( );
            m_ptr = other.get( );
            other.reset( );
            return *this;
         }

      public:
         void reset( T* ptr = nullptr )
         {
            if( ptr == m_ptr )
            {
               return;
            }

            if( nullptr != m_ptr )
            {
               m_ptr->release( );
            }

            m_ptr = ptr;

            if( nullptr != m_ptr )
            {
               m_ptr->add_ref( );
            }
         }

         T* release( ) noexcept
         {
            T* tmp = m_ptr;
            m_ptr = nullptr;
            return tmp;
         }

         void swap( IntrusivePtr& other ) noexcept
         {
            std::swap( m_ptr, other.m_ptr );
         }

      public:
         T* get( ) const noexcept
         {
            return m_ptr;
         }

         T& operator*( ) const
         {
            return *m_ptr;
         }

         T* operator->( ) const noexcept
         {
            return m_ptr;
         }

         explicit operator bool( ) const noexcept
         {
            return nullptr != m_ptr;
         }

      public:
         bool operator==( const IntrusivePtr& other ) const noexcept
         {
            return m_ptr == other.m_ptr;
         }

         bool operator!=( const IntrusivePtr& other ) const noexcept
         {
            return m_ptr != other.m_ptr;
         }

         bool operator==( std::nullptr_t ) const noexcept
         {
            return nullptr == m_ptr;
         }

         bool operator!=( std::nullptr_t ) const noexcept
         {
            return nullptr != m_ptr;
         }

      private:
         T* m_ptr;
   };



   template< typename T >
   void swap( IntrusivePtr< T >& a, IntrusivePtr< T >& b ) noexcept
   {
      a.swap( b );
   }

} // namespace carpc::memory
