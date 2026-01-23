#include <fstream>
#include <string>
#include <array>

#include "carpc/base/utils/generate/uuid.hpp"



namespace carpc::utils::generate {

   std::string uuid( )
   {
      std::array< unsigned char, 16 > bytes{ };

      // read 16 random bytes
      {
         std::ifstream urandom(
               "/dev/urandom", std::ios::in | std::ios::binary
            );
         urandom.read(
               reinterpret_cast<char*>( bytes.data( ) ), bytes.size( )
            );
      }

      // set version (4) and variant (RFC4122)
      bytes[ 6 ] = ( bytes[ 6 ] & 0x0F ) | 0x40; // version 4
      bytes[ 8 ] = ( bytes[ 8 ] & 0x3F ) | 0x80; // variant

      static const char* hex = "0123456789abcdef";
      std::string out;
      out.reserve( 36 );

      auto put = [&]( unsigned char b )
      {
         out.push_back( hex[ b >> 4 ] );
         out.push_back( hex[ b & 0x0F ] );
      };

      // format xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
      put( bytes[ 0 ] );
      put( bytes[ 1 ] );
      put( bytes[ 2 ] );
      put( bytes[ 3 ] );
      out.push_back( '-' );
      put( bytes[ 4 ] );
      put( bytes[ 5 ] );
      out.push_back( '-' );
      put( bytes[ 6 ] );
      put( bytes[ 7 ] );
      out.push_back( '-' );
      put( bytes[ 8 ] );
      put( bytes[ 9 ] );
      out.push_back( '-' );
      put( bytes[ 10 ] );
      put( bytes[ 11 ] );
      put( bytes[ 12 ] );
      put( bytes[ 13 ] );
      put( bytes[ 14 ] );
      put( bytes[ 15 ] );

      return out;
   }

}
