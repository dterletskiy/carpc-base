#pragma once

#include <cstdlib>



namespace carpc {

   inline
   bool is_power_of_two( uint32_t x )
   {
      return x != 0 && ( x & ( x - 1 ) ) == 0;
   }

   inline
   size_t align_size( size_t value, size_t alignment )
   {
      size_t result;  

      if( 0u == ( value % alignment ) )
      {
         result = value;
      }
      else
      {
         result = ( value + alignment ) - ( value % alignment );
      }
      
      return result;
   }

   // alignment must be power of 2
   inline
   size_t align_to_power2( size_t value, size_t alignment )
   {
      return (value + alignment - 1) & ~(alignment - 1);
   }

   inline
   size_t align_to( size_t value, size_t alignment )
   {
      return ( ( value + alignment - 1 ) / alignment ) * alignment;
   }

   inline
   size_t align_value( size_t value, size_t alignment )
   {
      return is_power_of_two( alignment ) ? 
         align_to_power2( value, alignment ) : align_to( value, alignment );
   }

} // namespace carpc



#if 0 // Examples with testing value alignment performance

#include <utility>
#include <random>

#include "carpc/base/helpers/functions/math.hpp"
#include "carpc/tools/Performance.hpp"



void test_align_performance(
      size_t (*align)( size_t, size_t ),
      const std::vector< std::pair< size_t, size_t > >& values,
      const std::string& message
   )
{
   carpc::tools::Performance performance( message );
   performance.start( );
   for( size_t i{ 0 }; i < 1; ++i )
   {
      for( auto& item: values )
      {
         auto result = align( item.first, item.second );
         asm volatile( "" : : "r"(result) : "memory" );
         // MSG_DBG( "%zu / %zu -> %zu", values[i].first, values[i].second, v );
      }
   }
   performance.stop( );
}



bool test( int argc, char** argv, char** envp )
{
   MSG_MARKER( "TEST" );

   carpc::tools::parameters::Params params( argc, argv, envp );
   params.print( );

   const size_t count = static_cast< std::size_t >( std::stoll( params.value( "count" ) ) );
   std::vector< std::pair< size_t, size_t > > values;
   values.reserve( count );

   std::mt19937 rng { std::random_device{ }( ) };
   std::uniform_int_distribution< size_t > dist{ 0, 999999 };
   std::uniform_int_distribution< size_t > dist_power { 0, 15 }; // 2^0 to 2^13 (1 to 8192)

   for( size_t i{ 0 }; i < count; ++i )
   {
      values.push_back( {
            dist( rng ),
            static_cast< size_t >( 1 ) << dist_power( rng )
         } );
   }

   for( size_t i{ 0 }; i < 10; ++i )
   {
      std::cout << "{ " << values[ i ].first << ", " << values[ i ].second << " }" << std::endl;
   }

   test_align_performance( carpc::align_size,      values, "align_size" );
   test_align_performance( carpc::align_to_power2, values, "align_to_power2" );
   test_align_performance( carpc::align_to,        values, "align_to" );
   test_align_performance( carpc::align_value,     values, "align_value" );

   MSG_MARKER( "TEST" );

   return false;
}

#endif
