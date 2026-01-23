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
         IntrusivePtr( ) noexcept;

         explicit IntrusivePtr( T* ptr );

         IntrusivePtr( T* ptr, AdoptTag ) noexcept;

         IntrusivePtr( const IntrusivePtr& other );

         template< typename U >
            requires std::is_convertible_v< U*, T* >
         IntrusivePtr( const IntrusivePtr< U >& other );

         IntrusivePtr( IntrusivePtr&& other ) noexcept;

         template< typename U >
            requires std::is_convertible_v< U*, T* >
         IntrusivePtr( IntrusivePtr< U >&& other ) noexcept;

         ~IntrusivePtr( );

      public:
         IntrusivePtr& operator=( const IntrusivePtr& other );

         template< typename U >
            requires std::is_convertible_v< U*, T* >
         IntrusivePtr& operator=( const IntrusivePtr< U >& other );

         IntrusivePtr& operator=( IntrusivePtr&& other ) noexcept;

         template< typename U >
            requires std::is_convertible_v< U*, T* >
         IntrusivePtr& operator=( IntrusivePtr< U >&& other ) noexcept;

      public:
         void reset( T* ptr = nullptr );

         T* release( ) noexcept;

         void swap( IntrusivePtr& other ) noexcept;

      public:
         T* get( ) const noexcept;

         T& operator*( ) const;

         T* operator->( ) const noexcept;

         explicit operator bool( ) const noexcept;

      public:
         bool operator==( const IntrusivePtr& other ) const noexcept;

         bool operator!=( const IntrusivePtr& other ) const noexcept;

         bool operator==( std::nullptr_t ) const noexcept;

         bool operator!=( std::nullptr_t ) const noexcept;

      private:
         T* m_ptr;
   };



   template< typename T >
   IntrusivePtr< T >::IntrusivePtr( ) noexcept
      : m_ptr( nullptr )
   {
   }

   template< typename T >
   IntrusivePtr< T >::IntrusivePtr( T* ptr )
      : m_ptr( ptr )
   {
      if( nullptr != m_ptr )
      {
         m_ptr->add_ref( );
      }
   }

   template< typename T >
   IntrusivePtr< T >::IntrusivePtr( T* ptr, AdoptTag ) noexcept
      : m_ptr( ptr )
   {
   }

   template< typename T >
   IntrusivePtr< T >::IntrusivePtr( const IntrusivePtr& other )
      : m_ptr( other.m_ptr )
   {
      if( nullptr != m_ptr )
      {
         m_ptr->add_ref( );
      }
   }

   template< typename T >
   template< typename U >
      requires std::is_convertible_v< U*, T* >
   IntrusivePtr< T >::IntrusivePtr( const IntrusivePtr< U >& other )
      : m_ptr( other.get( ) )
   {
      if( nullptr != m_ptr )
      {
         m_ptr->add_ref( );
      }
   }

   template< typename T >
   IntrusivePtr< T >::IntrusivePtr( IntrusivePtr&& other ) noexcept
      : m_ptr( other.m_ptr )
   {
      other.m_ptr = nullptr;
   }

   template< typename T >
   template< typename U >
      requires std::is_convertible_v< U*, T* >
   IntrusivePtr< T >::IntrusivePtr( IntrusivePtr< U >&& other ) noexcept
      : m_ptr( other.get( ) )
   {
      other.reset( );
   }

   template< typename T >
   IntrusivePtr< T >::~IntrusivePtr( )
   {
      if( nullptr != m_ptr )
      {
         m_ptr->release( );
      }
   }

   template< typename T >
   IntrusivePtr< T >& IntrusivePtr< T >::operator=( const IntrusivePtr& other )
   {
      if( this != &other )
      {
         reset( other.m_ptr );
      }
      return *this;
   }

   template< typename T >
   template< typename U >
      requires std::is_convertible_v< U*, T* >
   IntrusivePtr< T >& IntrusivePtr< T >::operator=( const IntrusivePtr< U >& other )
   {
      reset( other.get( ) );
      return *this;
   }

   template< typename T >
   IntrusivePtr< T >& IntrusivePtr< T >::operator=( IntrusivePtr&& other ) noexcept
   {
      if( this != &other )
      {
         reset( );
         m_ptr = other.m_ptr;
         other.m_ptr = nullptr;
      }
      return *this;
   }

   template< typename T >
   template< typename U >
      requires std::is_convertible_v< U*, T* >
   IntrusivePtr< T >& IntrusivePtr< T >::operator=( IntrusivePtr< U >&& other ) noexcept
   {
      reset( );
      m_ptr = other.get( );
      other.reset( );
      return *this;
   }

   template< typename T >
   void IntrusivePtr< T >::reset( T* ptr )
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

   template< typename T >
   T* IntrusivePtr< T >::release( ) noexcept
   {
      T* tmp = m_ptr;
      m_ptr = nullptr;
      return tmp;
   }

   template< typename T >
   void IntrusivePtr< T >::swap( IntrusivePtr& other ) noexcept
   {
      std::swap( m_ptr, other.m_ptr );
   }

   template< typename T >
   T* IntrusivePtr< T >::get( ) const noexcept
   {
      return m_ptr;
   }

   template< typename T >
   T& IntrusivePtr< T >::operator*( ) const
   {
      return *m_ptr;
   }

   template< typename T >
   T* IntrusivePtr< T >::operator->( ) const noexcept
   {
      return m_ptr;
   }

   template< typename T >
   IntrusivePtr< T >::operator bool( ) const noexcept
   {
      return nullptr != m_ptr;
   }

   template< typename T >
   bool IntrusivePtr< T >::operator==( const IntrusivePtr& other ) const noexcept
   {
      return m_ptr == other.m_ptr;
   }

   template< typename T >
   bool IntrusivePtr< T >::operator!=( const IntrusivePtr& other ) const noexcept
   {
      return m_ptr != other.m_ptr;
   }

   template< typename T >
   bool IntrusivePtr< T >::operator==( std::nullptr_t ) const noexcept
   {
      return nullptr == m_ptr;
   }

   template< typename T >
   bool IntrusivePtr< T >::operator!=( std::nullptr_t ) const noexcept
   {
      return nullptr != m_ptr;
   }



   template< typename T >
   void swap( IntrusivePtr< T >& a, IntrusivePtr< T >& b ) noexcept
   {
      a.swap( b );
   }

} // namespace carpc::memory
