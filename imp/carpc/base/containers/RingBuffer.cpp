#include "carpc/base/utils/math.hpp"
#include "carpc/base/containers/RingBuffer.hpp"



using namespace carpc;



bool RingBuffer::Segment::is_valid( ) const noexcept
{
   return size != 0 && ptr != nullptr;
}

bool RingBuffer::Segment::is_invalid( ) const noexcept
{
   return !is_valid( );
}

bool RingBuffer::Segment::is_empty( ) const noexcept
{
   return size == 0;
}




bool RingBuffer::Segments::is_valid( ) const noexcept
{
   return first.is_valid( );
}

bool RingBuffer::Segments::is_invalid( ) const noexcept
{
   return !is_valid( );
}

bool RingBuffer::Segments::is_empty( ) const
{
   return first.is_empty( ) && second.is_empty( );
}

std::size_t RingBuffer::Segments::total_size( ) const
{
   return first.size + second.size;
}



RingBuffer::RingBuffer( std::size_t capacity )
{
   capacity = math::round_pow2( capacity );
   assert( math::is_pow2( capacity ) );

   m_capacity = capacity;
   m_mask     = capacity - 1;
   mp_buffer  = static_cast< uint8_t* >( std::malloc( capacity ) );
}

RingBuffer::~RingBuffer( )
{
   std::free( mp_buffer );
}

RingBuffer::RingBuffer( RingBuffer&& other )
{
   *this = std::move( other );
}

RingBuffer& RingBuffer::operator=( RingBuffer&& other )
{
   if( this == &other )
      return *this;

   std::free( mp_buffer );

   mp_buffer  = other.mp_buffer;
   m_capacity = other.m_capacity;
   m_mask     = other.m_mask;
   m_head     = other.m_head;
   m_tail     = other.m_tail;

   other.mp_buffer  = nullptr;
   other.m_capacity = 0;
   other.m_mask     = 0;
   other.m_head     = 0;
   other.m_tail     = 0;

   return *this;
}



/* ================= BASIC STATE ================= */

std::size_t RingBuffer::size( ) const
{
   return m_tail - m_head;
}

std::size_t RingBuffer::capacity( ) const
{
   return m_capacity;
}

std::size_t RingBuffer::free_space( ) const
{
   return m_capacity - size( );
}

bool RingBuffer::empty( ) const
{
   return m_head == m_tail;
}

bool RingBuffer::full( ) const
{
   return size( ) == m_capacity;
}

void RingBuffer::reset( )
{
   m_head = 0;
   m_tail = 0;
}



/* ================= ZERO-COPY WRITE ================= */

RingBuffer::Segment RingBuffer::reserve_write( std::size_t max_size )
{
   const std::size_t free = free_space( );
   if( free == 0 )
      return Segment::invalid( );

   const std::size_t n = ( max_size < free ? max_size : free );

   const std::size_t tail     = m_tail & m_mask;
   const std::size_t to_end   = m_capacity - tail;
   const std::size_t linear   = ( n < to_end ? n : to_end );

   return Segment{ mp_buffer + tail, linear };
}

RingBuffer::Segments RingBuffer::reserve_writev( std::size_t max_size )
{
   const std::size_t free = free_space( );
   if( free == 0 )
      return Segments::invalid( );

   const std::size_t n = ( max_size < free ? max_size : free );

   const std::size_t tail   = m_tail & m_mask;
   const std::size_t to_end = m_capacity - tail;

   if( n <= to_end )
   {
      return Segments{
         mp_buffer + tail, n,
         nullptr,          0
      };
   }

   return Segments{
      mp_buffer + tail, to_end,
      mp_buffer,        n - to_end
   };
}

void RingBuffer::commit_write( std::size_t size )
{
   assert( size <= free_space( ) );
   m_tail += size;
}



/* ================= ZERO-COPY READ ================= */

RingBuffer::Segment RingBuffer::reserve_read( std::size_t max_size ) const
{
   const std::size_t used = size( );
   if( used == 0 )
      return Segment::invalid( );

   const std::size_t n = ( max_size < used ? max_size : used );

   const std::size_t head   = m_head & m_mask;
   const std::size_t to_end = m_capacity - head;
   const std::size_t linear = ( n < to_end ? n : to_end );

   return Segment{ mp_buffer + head, linear };
}

RingBuffer::Segments RingBuffer::reserve_readv( std::size_t max_size ) const
{
   const std::size_t used = size( );
   if( used == 0 )
      return Segments::invalid( );

   const std::size_t n = ( max_size < used ? max_size : used );

   const std::size_t head   = m_head & m_mask;
   const std::size_t to_end = m_capacity - head;

   if( n <= to_end )
   {
      return Segments{
         mp_buffer + head, n,
         nullptr,          0
      };
   }

   return Segments{
      mp_buffer + head, to_end,
      mp_buffer,        n - to_end
   };
}

void RingBuffer::commit_read( std::size_t size )
{
   assert( size <= this->size( ) );
   m_head += size;
}



/* ================= COMPATIBILITY API ================= */

bool RingBuffer::push_back( const void* data, std::size_t size )
{
   if( not data )
      return false;

   auto segs = reserve_writev( size );

   if( segs.total_size( ) < size )
      return false;

   std::memcpy( segs.first.ptr, data, segs.first.size );

   if( segs.second.is_valid( ) )
   {
      std::memcpy(
         segs.second.ptr,
         static_cast< const uint8_t* >( data ) + segs.first.size,
         segs.second.size
      );
   }

   commit_write( size );
   return true;
}



std::size_t RingBuffer::peek( void* out, std::size_t size ) const
{
   if( not out )
      return 0;

   auto segs = reserve_readv( size );

   if( segs.is_empty( ) )
      return 0;

   std::memcpy( out, segs.first.ptr, segs.first.size );

   if( segs.second.is_valid( ) )
   {
      std::memcpy(
         static_cast< uint8_t* >( out ) + segs.first.size,
         segs.second.ptr,
         segs.second.size
      );
   }

   return segs.total_size( );
}



std::size_t RingBuffer::pop_front( void* out, std::size_t size )
{
   const std::size_t n = peek( out, size );
   commit_read( n );
   return n;
}
