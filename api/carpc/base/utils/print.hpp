#pragma once



#include <cstdio>
#include <unistd.h>
#include <algorithm>
#include <array>

   /******************************************************************************
    * @file fast_write_stack.h
    * @brief Safe, fully stack-only formatted output API for 
    * embedded/real-time systems.
    *
    * Features:
    *   - Thread-safe (stack-based)
    *   - No dynamic allocation
    *   - Uses small stack buffer for most messages
    *   - Uses large stack buffer for oversized messages up to 16 KB
    *   - Directly writes via write() syscall
    ******************************************************************************/

namespace carpc::utils {

   /**
    * @brief Print formatted message to a file descriptor using stack buffers.
    *
    * Uses a 1KB stack buffer for most messages. For messages >1KB, uses a
    * stack-allocated buffer up to 16 KB to avoid heap allocation. Thread-safe.
    * If fd < 0, defaults to STDOUT_FILENO.
    *
    * @tparam Args Variadic template for format arguments
    * @param fd File descriptor for output
    * @param format C-style format string
    * @param args Arguments for format specifiers
    */
   template< typename... Args >
   void write( int fd, const char* const format, Args&&... args )
   {
      if( fd < 0 ) fd = STDOUT_FILENO;

      constexpr size_t small_size = 1024;
      constexpr size_t max_size   = 16 * 1024;

      char small_buffer[small_size];
      int size = ::snprintf( small_buffer, small_size, format, args... );

      if( size < 0 ) return;
      if( size < (int)small_size )
      {
         ::write( fd, small_buffer, size );
         return;
      }

      size = std::min( size, (int)max_size );
      std::array< char, max_size > large_buffer;
      ::snprintf( large_buffer.data( ), large_buffer.size( ), format, args... );
      ::write( fd, large_buffer.data( ), size );
   }

   /**
    * @brief Print formatted message to stdout using stack buffers.
    *
    * Uses a 1KB stack buffer for most messages. For messages >1KB, uses a
    * stack-allocated buffer up to 16 KB to avoid heap allocation. Thread-safe
    * as each call uses its own stack memory.
    *
    * @tparam Args Variadic template for format arguments
    * @param format C-style format string
    * @param args Arguments for format specifiers
    */
   template< typename... Args >
   void write( const char* const format, Args&&... args )
   {
      return write( -1, format, args... );
   }

}



#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <tuple>
#include <optional>
#include <memory>
#include <unistd.h>

namespace carpc::utils {


   /******************************************************************************
    * 
    * Print byte buffer
    * 
    ******************************************************************************/
   void print_hex( const void* p_buffer, const std::size_t size, const bool is_new_line = false );
   void print_bin( const void* p_buffer, const std::size_t size, const bool is_new_line = false );

   void print_hex_ex(
         const void* p_buffer, const std::size_t size, const std::size_t items_in_line = 16
      );
   void print_bin_ex(
         const void* p_buffer, const std::size_t size, const std::size_t items_in_line = 16
      );

   template< typename TYPE >
   void print( const TYPE& value, const bool is_new_line = false )
   {
      std::cout << value;

      if( is_new_line ) printf( "\n" );
   }

   // template< typename TYPE >
   // void print( const TYPE* pointer, const bool is_new_line = false )
   // {
   //    printf( "%#x", pointer );

   //    if( is_new_line ) printf( "\n" );
   // }



   /****************************************************************************************************
    *
    * Forward declarations for "print" functions
    *
    ***************************************************************************************************/
   template< typename TYPE >
   void print( const TYPE* array, const std::size_t size, const bool is_new_line = false );

   template < typename T, std::size_t N >
   void print( T (&array)[N], const bool is_new_line = false );

   template< typename TYPE >
   void print( const std::shared_ptr< TYPE >& shared_ptr, const bool is_new_line = false );

   template< typename TYPE >
   void print( const std::optional< TYPE >& optional, const bool is_new_line = false );

   template< typename TYPE >
   void print( const std::basic_string< TYPE >& string, const bool is_new_line = false );

   template< >
   void print( const std::basic_string< char >& string, const bool is_new_line );

   template< >
   void print( const std::basic_string< wchar_t >& string, const bool is_new_line );

   template< typename TYPE >
   void print( const std::list< TYPE >& list, const bool is_new_line = false );

   template< typename TYPE >
   void print( const std::vector< TYPE >& vector, const bool is_new_line = false );

   template< typename TYPE >
   void print( const std::set< TYPE >& set, const bool is_new_line = false );

   template< typename TYPE_KEY, typename TYPE_VALUE >
   void print( const std::pair< TYPE_KEY, TYPE_VALUE >& pair, const bool is_new_line = false );

   template< typename TYPE_KEY, typename TYPE_VALUE >
   void print( const std::map< TYPE_KEY, TYPE_VALUE >& map, const bool is_new_line = false );

   template< typename... TYPES >
   void print( const std::tuple< TYPES... >& tuple, const bool is_new_line = false );

   template< typename T, typename...Ts >
   void print_pack( T&& first, Ts&&... rest );



   /****************************************************************************************************
    *
    * This function prints array declared on the stack or heap:
    *    std::size_t array[ ] = { 4, 5, 55, 4, 457, 45, 68, 1, 87, 41 };
    *    carpc::utils::print( array, 10, true );
    *
    ***************************************************************************************************/
   template< typename TYPE >
   void print( const TYPE* array, const std::size_t size, const bool is_new_line )
   {
      printf( "{ " );
      for ( std::size_t index = 0; index < size; ++index )
      {
         print( array[ index ] );
         printf( " " );
      }
      printf( "}" );

      if( is_new_line ) printf( "\n" );
   }



   /****************************************************************************************************
    *
    * This function prints array declared on the stack:
    *    std::size_t array[ ] = { 4, 5, 55, 4, 457, 45, 68, 1, 87, 41 };
    *    carpc::utils::print( *array, true );
    *
    ***************************************************************************************************/
   template < typename T, std::size_t N >
   void print( T (&array)[N], const bool is_new_line )
   {
      printf( "{ " );
      for ( std::size_t index = 0; index < N; ++index )
      {
         carpc::utils::print( array[ index ] );
         printf( " " );
      }
      printf( "}" );

      if( is_new_line ) printf( "\n" );
   }



   /****************************************************************************************************
    * 
    * Print std containers and types
    * 
    ***************************************************************************************************/

   template< typename TYPE >
   void print( const std::shared_ptr< TYPE >& shared_ptr, const bool is_new_line )
   {
      printf( "{ " );
      printf( "%p", shared_ptr.get( ) );
      printf( " }" );

      if( is_new_line ) printf( "\n" );
   }

   template< typename TYPE >
   void print( const std::optional< TYPE >& optional, const bool is_new_line )
   {
      printf( "{ " );
      if( optional.has_value( ) )
      {
         print( optional.value( ) );
      }
      else
      {
         printf( "std::nullopt" );
      }
      printf( " }" );

      if( is_new_line ) printf( "\n" );
   }

   template< typename TYPE >
   void print( const std::basic_string< TYPE >& string, const bool is_new_line )
   {
      printf( "\"" );
      for ( const auto& value : string )
      {
         print( value );
      }
      printf( "\"" );

      if( is_new_line ) printf( "\n" );
   }

   template< typename TYPE >
   void print( const std::list< TYPE >& list, const bool is_new_line )
   {
      printf( "{ " );
      for ( const auto& value : list )
      {
         print( value );
         printf( " " );
      }
      printf( "}" );

      if( is_new_line ) printf( "\n" );
   }

   template< typename TYPE >
   void print( const std::vector< TYPE >& vector, const bool is_new_line )
   {
      printf( "{ " );
      for ( const auto& value : vector )
      {
         print( value );
         printf( " " );
      }
      printf( "}" );

      if( is_new_line ) printf( "\n" );
   }

   template< typename TYPE >
   void print( const std::set< TYPE >& set, const bool is_new_line )
   {
      printf( "{ " );
      for ( const auto& value : set )
      {
         print( value );
         printf( " " );
      }
      printf( "}" );

      if( is_new_line ) printf( "\n" );
   }

   template< typename TYPE_KEY, typename TYPE_VALUE >
   void print( const std::pair< TYPE_KEY, TYPE_VALUE >& pair, const bool is_new_line )
   {
      printf( "{ " );
      print( pair.first );
      printf( " => " );
      print( pair.second );
      printf( " }" );

      if( is_new_line ) printf( "\n" );
   }

   template< typename TYPE_KEY, typename TYPE_VALUE >
   void print( const std::map< TYPE_KEY, TYPE_VALUE >& map, const bool is_new_line )
   {
      printf( "{ " );
      for ( const auto& pair : map )
      {
         print( pair );
         printf( " " );
      }
      printf( "}" );

      if( is_new_line ) printf( "\n" );
   }

   namespace _private_ {

      namespace _compare_v1_ {

         template< std::size_t I, typename... Tp >
         constexpr bool Tp_more( )
         {
            return I < sizeof...( Tp );
         }

         template< std::size_t I, typename... Tp >
         constexpr bool Tp_equal( )
         {
            return I == sizeof...( Tp );
         }

      }

      namespace _compare_v2_ {

         template< typename... Tp >
         inline
         constexpr bool Tp_more( const std::size_t& I )
         {
            return I < sizeof...( Tp );
         }

         template< typename... Tp >
         inline
         constexpr bool Tp_equal( const std::size_t& I )
         {
            return I == sizeof...( Tp );
         }

      }

      template< std::size_t I = 0, typename... Tp >
         inline typename std::enable_if< _compare_v2_::Tp_equal< Tp... >( I ), void >::type
            print_tuple( const std::tuple< Tp... >& t )
      {
      }

      template< std::size_t I = 0, typename... Tp >
         // inline typename std::enable_if< I < sizeof...(Tp), void >::type
         // inline typename std::enable_if< _compare_v1_::Tp_more< I, Tp... >( ), void >::type
         inline typename std::enable_if< _compare_v2_::Tp_more< Tp... >( I ), void >::type
            print_tuple( const std::tuple< Tp... >& t )
      {
         print( std::get< I >( t ) );
         printf( " " );
         print_tuple< I + 1 >( t );
      }

   }

   template< typename... TYPES >
   void print( const std::tuple< TYPES... >& tuple, const bool is_new_line )
   {
      printf( "{ " );
      _private_::print_tuple( tuple );
      printf( "}" );

      if( is_new_line ) printf( "\n" );
   }

   inline
   void print_pack( )
   {
      printf( "\n" );
   }

   template< typename T, typename...Ts >
   void print_pack( T&& first, Ts&&... rest )
   {
      print( std::forward< T >( first ) );
      printf( " " );
      print_pack( std::forward< Ts >( rest )... );
   }

} // namespace carpc::utils
