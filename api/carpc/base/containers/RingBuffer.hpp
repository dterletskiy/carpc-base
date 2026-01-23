#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <cassert>



namespace carpc {

   /**
    * @brief High-performance lock-free ring buffer with zero-copy API.
    *
    * RingBuffer is a byte-oriented circular buffer optimized for:
    *  - minimal branching
    *  - power-of-two capacity
    *  - zero-copy producer / consumer access
    *
    * Internally uses monotonically increasing head/tail counters and
    * bit masking instead of modulo operations.
    *
    * Thread-safety:
    *  - This implementation is NOT thread-safe by default.
    *  - Can be safely used in single-threaded contexts.
    *  - Can be extended to SPSC with atomic head/tail.
    */
   class RingBuffer
   {
      public:
         /**
          * @brief Represents a single contiguous memory segment.
          *
          * Used by zero-copy API to expose readable or writable regions
          * of the internal buffer.
          *
          * A segment is considered:
          *  - valid   : ptr != nullptr && size > 0
          *  - invalid : ptr == nullptr || size == 0
          */
         struct Segment
         {
            /**
             * @brief Construct a segment.
             *
             * @param p Pointer to the memory region.
             * @param s Size of the region in bytes.
             */
            constexpr Segment( uint8_t* p, std::size_t s ) noexcept
               : ptr(p)
               , size(s)
            {
            }

            /**
             * @brief Equality comparison.
             */
            constexpr bool operator==( const Segment& rhs ) const noexcept
            {
               return ptr == rhs.ptr && size == rhs.size;
            }

            /**
             * @brief Returns an invalid (empty) segment.
             */
            static constexpr Segment invalid( ) noexcept
            {
               return Segment{ nullptr, 0 };
            }

            /**
             * @brief Check whether the segment is valid.
             */
            bool is_valid( ) const noexcept;

            /**
             * @brief Check whether the segment is invalid.
             */
            bool is_invalid( ) const noexcept;

            /**
             * @brief Check whether the segment is empty.
             *
             * Note: empty does not necessarily mean invalid.
             */
            bool is_empty( ) const noexcept;

            uint8_t*    ptr  = nullptr; /**< Pointer to memory */
            std::size_t size = 0;       /**< Size in bytes */
         };

         /**
          * @brief Represents up to two contiguous memory segments.
          *
          * Used when the buffer wraps around the end and data is split
          * into two linear regions.
          *
          * Layout:
          *  - first  : segment starting at head/tail
          *  - second : optional wrapped segment at buffer start
          */
         struct Segments
         {
            /**
             * @brief Construct segments from raw pointers and sizes.
             */
            constexpr Segments(
                     uint8_t* p1, std::size_t s1,
                     uint8_t* p2, std::size_t s2
                  ) noexcept
               : first( p1, s1 )
               , second( p2, s2 )
            {
            }

            /**
             * @brief Construct segments from Segment objects.
             */
            constexpr Segments( Segment s1, Segment s2 ) noexcept
               : first(s1)
               , second(s2)
            {
            }

            /**
             * @brief Equality comparison.
             */
            constexpr bool operator==( const Segments& rhs ) const noexcept
            {
               return first == rhs.first && second == rhs.second;
            }

            /**
             * @brief Returns invalid segments.
             */
            static constexpr Segments invalid( ) noexcept
            {
               return Segments{ Segment::invalid( ), Segment::invalid( ) };
            }

            /**
             * @brief Check whether the segments are valid.
             *
             * Validity is determined by the first segment.
             */
            bool is_valid( ) const noexcept;

            /**
             * @brief Check whether the segments are invalid.
             */
            bool is_invalid( ) const noexcept;

            /**
             * @brief Check whether both segments are empty.
             */
            bool is_empty( ) const;

            /**
             * @brief Total size of both segments.
             */
            std::size_t total_size( ) const;

            Segment first  = Segment::invalid( );  /**< First linear segment */
            Segment second = Segment::invalid( );  /**< Optional wrapped segment */
         };

      public:
         /**
          * @brief Construct a ring buffer with given capacity.
          *
          * Capacity will be rounded up to the nearest power of two.
          *
          * @param capacity Requested capacity in bytes.
          */
         explicit RingBuffer( std::size_t capacity );

         /**
          * @brief Destructor.
          */
         ~RingBuffer( );

         /**
          * @brief Move constructor.
          */
         RingBuffer( RingBuffer&& other );

         /**
          * @brief Move assignment.
          */
         RingBuffer& operator=( RingBuffer&& other );

         RingBuffer( const RingBuffer& ) = delete;
         RingBuffer& operator=( const RingBuffer& ) = delete;

      public:
         /* ================= ZERO-COPY WRITE API ================= */

         /**
          * @brief Reserve a contiguous writable region.
          *
          * Returns a single linear segment which can be written to directly.
          *
          * @param max_size Maximum requested size.
          * @return Writable segment (may be smaller than requested).
          *
          * @note Must be followed by commit_write().
          */
         Segment reserve_write( std::size_t max_size );

         /**
          * @brief Reserve writable regions (possibly wrapped).
          *
          * Returns up to two segments covering the writable space.
          *
          * @param max_size Maximum requested size.
          * @return Writable segments.
          *
          * @note Must be followed by commit_write().
          */
         Segments reserve_writev( std::size_t max_size );

         /**
          * @brief Commit previously written bytes.
          *
          * @param size Number of bytes written.
          *
          * @pre size <= free_space()
          */
         void commit_write( std::size_t size );

      public:
         /* ================= ZERO-COPY READ API ================= */

         /**
          * @brief Reserve a contiguous readable region.
          *
          * @param max_size Maximum requested size.
          * @return Readable segment.
          *
          * @note Must be followed by commit_read().
          */
         Segment reserve_read( std::size_t max_size ) const;

         /**
          * @brief Reserve readable regions (possibly wrapped).
          *
          * @param max_size Maximum requested size.
          * @return Readable segments.
          *
          * @note Must be followed by commit_read().
          */
         Segments reserve_readv ( std::size_t max_size ) const;

         /**
          * @brief Commit previously read bytes.
          *
          * @param size Number of bytes consumed.
          *
          * @pre size <= size()
          */
         void commit_read( std::size_t size );

      public:
         /* ================= COMPATIBILITY API ================= */

         /**
          * @brief Push data into the buffer.
          *
          * Copies data into the buffer.
          *
          * @param data Source buffer.
          * @param size Number of bytes.
          * @return true on success, false if insufficient space.
          */
         bool push_back( const void* data, std::size_t size );

         /**
          * @brief Pop data from the buffer.
          *
          * Copies data out and consumes it.
          *
          * @param out Destination buffer (must not be nullptr).
          * @param size Maximum number of bytes to read.
          * @return Number of bytes read.
          */
         std::size_t pop_front( void* out, std::size_t size );

         /**
          * @brief Peek data without consuming it.
          *
          * @param out Destination buffer (must not be nullptr).
          * @param size Maximum number of bytes to read.
          * @return Number of bytes read.
          */
         std::size_t peek( void* out, std::size_t size ) const;

      public:
         /**
          * @brief Reset buffer state.
          *
          * Discards all stored data.
          */
         void reset( );

         /**
          * @brief Current number of stored bytes.
          */
         std::size_t size( ) const;

         /**
          * @brief Buffer capacity in bytes.
          */
         std::size_t capacity( ) const;

         /**
          * @brief Remaining free space.
          */
         std::size_t free_space( ) const;

         /**
          * @brief Check if buffer is empty.
          */
         bool empty( ) const;

         /**
          * @brief Check if buffer is full.
          */
         bool full( ) const;

      private:
         uint8_t*    mp_buffer = nullptr; /**< Storage */
         std::size_t m_capacity = 0;      /**< Capacity (power of two) */
         std::size_t m_mask = 0;          /**< capacity - 1 */

         std::size_t m_head = 0;          /**< Read position (monotonic) */
         std::size_t m_tail = 0;          /**< Write position (monotonic) */
   };

} // namespace carpc
