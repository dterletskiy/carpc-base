#include <random>
#include <string>
#include <cstddef>

#include "carpc/base/utils/generate/random.hpp"



namespace carpc::utils::generate::random
{

   std::string text( const std::size_t length )
   {
      static const char alphanum[ ] =
         "0123456789"
         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
         "abcdefghijklmnopqrstuvwxyz";

      std::string result;
      result.reserve( length );

      std::uniform_int_distribution< std::size_t > dist
      (
         0,
         sizeof( alphanum ) - 2
      );

      auto& eng = engine( );

      for( std::size_t i = 0; i < length; ++i )
      {
         result += alphanum[ dist( eng ) ];
      }

      return result;
   }

}
