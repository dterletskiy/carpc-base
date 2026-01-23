#pragma once

#include <atomic>
#include <cstdint>



namespace carpc::memory {

   class RefCounted
   {
      protected:
         RefCounted( ) = default;
         RefCounted( const RefCounted& ) = delete;
         RefCounted& operator=( const RefCounted& ) = delete;
         virtual ~RefCounted( ) = default;

      public:
         void add_ref( ) noexcept;

         void release( ) noexcept;

         std::uint32_t ref_count( ) const noexcept;

      protected:
         virtual void on_zero_ref( ) noexcept;

      private:
         std::atomic< std::uint32_t > m_ref_count{ 0 };
   };



   inline
   void RefCounted::add_ref( ) noexcept
   {
      m_ref_count.fetch_add( 1, std::memory_order_relaxed );
   }

   inline
   void RefCounted::release( ) noexcept
   {
      if( m_ref_count.fetch_sub( 1, std::memory_order_acq_rel ) == 1 )
      {
         on_zero_ref( );
      }
   }

   inline
   std::uint32_t RefCounted::ref_count( ) const noexcept
   {
      return m_ref_count.load( std::memory_order_relaxed );
   }

   inline
   void RefCounted::on_zero_ref( ) noexcept
   {
      delete this;
   }

} // namespace carpc::memory
