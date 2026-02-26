#pragma once

#include <cstddef>
#include <new>
#include <utility>
#include <type_traits>
#include <cstring>



namespace carpc::callable {

   /**
    * @brief Small, fixed-size, type-erased callable storage without heap usage.
    *
    * CallableStorage provides a minimal type-erasure mechanism for storing
    * and invoking arbitrary callable objects (lambdas, function objects,
    * function pointers) inside a fixed-size inline buffer.
    *
    * The callable object is constructed in-place using placement new and
    * must fit into the specified buffer size. No dynamic memory allocation
    * is performed.
    *
    * The stored callable is invoked through a single indirect function
    * pointer (trampoline). Compared to std::function, this implementation:
    *
    * - Does not allocate memory on the heap
    * - Has deterministic and fixed memory footprint
    * - Avoids manager/allocator machinery
    * - Provides predictable performance characteristics
    *
    * @tparam Size
    * Maximum size in bytes available for storing the callable object.
    *
    * @note
    * - The callable type must satisfy sizeof( T ) <= Size.
    * - The callable must be MoveConstructible.
    * - Invocation is non-virtual and performed via a single function pointer.
    *
    * @warning
    * This class does not perform automatic destruction of the stored object
    * unless explicitly implemented. If the callable has a non-trivial
    * destructor, proper destroy() handling must be provided.
    *
    * @par Memory Layout
    * Internally the class contains:
    * - Inline aligned byte buffer of Size bytes
    * - Function pointer used as invocation trampoline
    *
    * The callable object itself is stored directly inside the buffer.
    *
    * @par Performance Characteristics
    * - No heap allocations
    * - One indirect call on invocation
    * - No RTTI usage
    * - Cache-friendly contiguous storage
    *
    * @code
    * CallableStorage< 64 > storage;
    *
    * storage.set(
    *    [ value = 42 ]()
    *    {
    *       std::printf( "%d\n", value );
    *    }
    * );
    *
    * storage.invoke();
    * @endcode
    */
   template< std::size_t Size = 64 >
   class CallableStorage
   {
      public:
         /**
          * @brief Constructs empty storage.
          *
          * No callable is stored after construction.
          */
         CallableStorage( ) noexcept = default;

         /**
          * @brief Destroys the storage.
          *
          * If destruction support is implemented, destroys the stored callable.
          */
         ~CallableStorage( );

         CallableStorage( const CallableStorage& ) = delete;
         CallableStorage& operator=( const CallableStorage& ) = delete;

         /**
          * @brief Move constructor.
          *
          * Transfers ownership of the stored callable from another instance.
          * The source object becomes empty after the move.
          *
          * @param other Source storage to move from.
          */
         CallableStorage( CallableStorage&& other ) noexcept;

         /**
          * @brief Move assignment operator.
          *
          * Destroys the currently stored callable (if any) and transfers
          * ownership from another instance.
          *
          * @param other Source storage to move from.
          * @return Reference to this object.
          */
         CallableStorage& operator=( CallableStorage&& other ) noexcept;

      public:
         /**
          * @brief Stores a callable object inside the inline buffer.
          *
          * The callable is constructed in-place using placement new.
          * Any previously stored callable is destroyed.
          *
          * @tparam Fn Callable type.
          * @param fn Callable object to store.
          *
          * @pre sizeof( Fn ) <= Size
          * @post Storage contains the new callable.
          */
         template< typename Fn >
         void set( Fn&& fn ) noexcept;

         /**
          * @brief Invokes the stored callable.
          *
          * Performs a single indirect call through the internal trampoline.
          *
          * @pre A callable must be stored.
          */
         void operator()( ) noexcept;

         /**
          * @brief Destroys the currently stored callable and resets storage.
          *
          * After reset(), valid() returns false.
          */
         void reset( ) noexcept;

         /**
          * @brief Checks whether a callable is currently stored.
          *
          * @return true if a callable is present.
          * @return false otherwise.
          */
         bool valid( ) const noexcept;

      private:
         /**
          * @brief Moves internal state from another instance.
          *
          * Transfers storage contents and trampoline pointers.
          * The source object is left in empty state.
          *
          * @param other Source storage.
          */
         void move_from( CallableStorage&& other ) noexcept;

      private:
         /**
          * @brief Inline storage buffer for the callable object.
          *
          * The callable is placed here using placement new.
          */
         alignas( std::max_align_t ) unsigned char m_storage[ Size ];

         /**
          * @brief Invocation trampoline.
          *
          * Points to a function capable of invoking the stored callable.
          */
         void( *m_invoke )( void* )  = nullptr;

         /**
          * @brief Destruction trampoline.
          *
          * Points to a function capable of destroying the stored callable.
          */
         void( *m_destroy )( void* ) = nullptr;
   };



   template< std::size_t Size >
   CallableStorage< Size >::~CallableStorage( )
   {
      reset( );
   }

   template< std::size_t Size >
   CallableStorage< Size >::CallableStorage( CallableStorage&& other ) noexcept
   {
      move_from( std::move( other ) );
   }

   template< std::size_t Size >
   CallableStorage< Size >& CallableStorage< Size >::operator=( CallableStorage&& other ) noexcept
   {
      if( this != &other )
      {
         reset( );
         move_from( std::move( other ) );
      }
      return *this;
   }


   template< std::size_t Size >
   template< typename Fn >
   void CallableStorage< Size >::set( Fn&& fn ) noexcept
   {
      using T = std::decay_t< Fn >;

      static_assert(
            sizeof( T ) <= Size,
            "Callable too large for storage"
         );

      static_assert(
            std::is_invocable_v< T& >,
            "Callable must be void() invocable"
         );

      new( m_storage ) T( std::forward< Fn >( fn ) );

      m_invoke = [ ]( void* ptr )
      {
         ( *reinterpret_cast< T* >( ptr ) )( );
      };

      m_destroy = [ ]( void* ptr )
      {
         reinterpret_cast< T* >( ptr )->~T( );
      };
   }

   template< std::size_t Size >
   void CallableStorage< Size >::operator()( ) noexcept
   {
      if( m_invoke )
         m_invoke( m_storage );
   }

   template< std::size_t Size >
   void CallableStorage< Size >::reset( ) noexcept
   {
      if( m_destroy )
      {
         m_destroy( m_storage );
         m_destroy = nullptr;
         m_invoke  = nullptr;
      }
   }

   template< std::size_t Size >
   bool CallableStorage< Size >::valid( ) const noexcept
   {
      return m_invoke != nullptr;
   }

   template< std::size_t Size >
   void CallableStorage< Size >::move_from( CallableStorage&& other ) noexcept
   {
      std::memcpy( m_storage, other.m_storage, Size );

      m_invoke  = other.m_invoke;
      m_destroy = other.m_destroy;

      other.m_invoke  = nullptr;
      other.m_destroy = nullptr;
   }

} // namespace carpc::callable