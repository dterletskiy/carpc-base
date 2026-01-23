#include "carpc/base/utils/str.hpp"



#include <cstring>

namespace carpc::utils::cstring {

   std::size_t strcpy( char *dst, std::size_t dst_size, const char *src )
   {
      std::size_t src_len;

      if( 0 == dst_size )
      {
         return strlen( src );
      }

      src_len = strlen( src );

      if( src_len >= dst_size )
      {
         memcpy( dst, src, dst_size - 1 );
         dst[ dst_size - 1 ] = '\0';
      }
      else
      {
         memcpy( dst, src, src_len + 1 );
      }

      return src_len;
   }

   void ltrim_inplace( char* s )
   {
      if( !s || *s == '\0' ) return;

      char* begin = s;
      char* end = s + std::strlen(s) - 1;

      while( begin <= end && std::strchr( WHITESPACE, *begin ) )
         ++begin;

      if( begin > end )
      {
         *s = '\0';
         return;
      }

      size_t new_len = end - begin + 1;
      if( begin != s )
         std::memmove( s, begin, new_len );

      s[ new_len ] = '\0';
   }

   void rtrim_inplace( char* s )
   {
      if( !s || *s == '\0' ) return;

      char* begin = s;
      char* end = s + std::strlen(s) - 1;

      while( end >= begin && std::strchr( WHITESPACE, *end ) )
         --end;

      *( end + 1 ) = '\0';
   }

   void trim_inplace( char* s )
   {
      if( !s || *s == '\0' ) return;

      char* begin = s;
      char* end = s + std::strlen(s) - 1;

      while( begin <= end && std::strchr( WHITESPACE, *begin ) )
         ++begin;

      while( end >= begin && std::strchr( WHITESPACE, *end ) )
         --end;

      if( begin > end )
      {
         *s = '\0';
         return;
      }

      size_t new_len = end - begin + 1;
      if( begin != s )
         std::memmove( s, begin, new_len );

      s[ new_len ] = '\0';
   }

} // namespace::utils::cstring



namespace carpc::utils::string {

   std::string ltrim( const std::string& s )
   {
      std::size_t start = s.find_first_not_of( WHITESPACE );
      return ( start == std::string::npos ) ? "" : s.substr( start );
   }
    
   std::string rtrim( const std::string& s )
   {
      std::size_t end = s.find_last_not_of( WHITESPACE );
      return ( end == std::string::npos ) ? "" : s.substr( 0, end + 1 );
   }
    
   std::string trim( const std::string& s )
   {
      std::size_t start = s.find_first_not_of( WHITESPACE );
      if( start == std::string::npos )
         return "";

      std::size_t end = s.find_last_not_of( WHITESPACE );
      return s.substr( start, end - start + 1 );
   }



   void ltrim_inplace( std::string& s )
   {
      std::size_t start = s.find_first_not_of( WHITESPACE );
      if( start == std::string::npos )
         s.clear( );
      else
         s.erase( 0, start );
   }

   void rtrim_inplace( std::string& s )
   {
      std::size_t end = s.find_last_not_of( WHITESPACE );
      if( end == std::string::npos )
         s.clear( );
      else
         s.erase( end + 1 );
   }

   void trim_inplace( std::string& s )
   {
      std::size_t start = s.find_first_not_of( WHITESPACE );
      if( start == std::string::npos )
      {
         s.clear( );
         return;
      }

      std::size_t end = s.find_last_not_of( WHITESPACE );
      s.erase( end + 1 );
      s.erase( 0, start );
   }

   namespace optimized {

      void ltrim_inplace( std::string& s )
      {
         if( s.empty( ) )
            return;

         const char* whitespace = WHITESPACE;
         char* begin = &s[0];
         char* end   = &s[s.size( ) - 1];

         while( begin <= end && std::strchr( whitespace, *begin ) )
            ++begin;

         if( begin > end )
         {
            s.clear( );
            return;
         }

         std::size_t new_len = end - begin + 1;
         if( begin != &s[0] )
            std::memmove( &s[0], begin, new_len );

         s.resize( new_len );
      }

      void rtrim_inplace( std::string& s )
      {
         if( s.empty( ) )
            return;

         const char* whitespace = WHITESPACE;
         char* begin = &s[0];
         char* end   = &s[s.size( ) - 1];

         while( end >= begin && std::strchr( whitespace, *end ) )
            --end;

         if( end < begin )
         {
            s.clear( );
            return;
         }

         s.resize( end - begin + 1 );
      }

      void trim_inplace( std::string& s )
      {
         if( s.empty( ) )
            return;

         const char* whitespace = WHITESPACE;
         char* begin = &s[0];
         char* end = begin + s.size( ) - 1;

         // Find no space start symbols
         while( begin <= end && std::strchr( whitespace, *begin ) )
            ++begin;

         // Find no space end symbols
         while( end >= begin && std::strchr( whitespace, *end ) )
            --end;

         // If string contains only whitespaces
         if( begin > end )
         {
            s.clear( );
            return;
         }

         // Calculate new string length
         std::size_t new_len = end - begin + 1;

         // Move non empty string part to the beginning
         if( begin != &s[0] )
            std::memmove( &s[0], begin, new_len );

         s.resize( new_len );
      }

   }
}
