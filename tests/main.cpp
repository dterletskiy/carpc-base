#include "carpc/trace/trace.h"



#include <type_traits>
#include <string>
#include <string_view>
#include <cassert>
#include <sstream>

#include "carpc/base/types/Name.hpp"

namespace test::name {

   struct Service { };

   using NameString = carpc::TName< Service, std::string >;
   using NameView   = carpc::TName< Service, std::string_view >;

   static_assert( std::is_copy_constructible_v< NameString > );
   static_assert( std::is_copy_assignable_v< NameString > );

   static_assert( std::is_same_v<
         NameString::VALUE_TYPE,
         std::string
      > );

   static_assert( std::totally_ordered< NameString > );
   static_assert( std::equality_comparable< NameString > );

   void basic_string_name( )
   {
      using Name = carpc::TName< Service, std::string >;

      Name invalid;
      assert( invalid.is_invalid( ) );
      assert( !invalid.is_valid( ) );
      assert( invalid == Name::invalid_value );

      Name audio{ "Audio" };
      assert( audio.is_valid( ) );
      assert( audio != Name::invalid_value );
      assert( audio == std::string{ "Audio" } );

      Name audio2{ "Audio" };
      Name video{ "Video" };

      assert( audio == audio2 );
      assert( audio != video );
      assert( audio < video );
   }

   void string_view_name( )
   {
      using Name = carpc::TName< Service, std::string_view >;

      Name invalid;
      assert( invalid.is_invalid( ) );

      Name a{ "A" };
      Name b{ "B" };

      assert( a.is_valid( ) );
      assert( a < b );
   }

   void ostream_output( )
   {
      using Name = carpc::TName< Service, std::string >;

      Name audio{ "Audio" };

      std::ostringstream oss;
      oss << audio;

      assert( oss.str( ) == "Audio" );
   }

   void invalid_singleton( )
   {
      using Name = carpc::TName< Service, std::string >;

      assert( Name::invalid.is_invalid( ) );
      assert( Name::invalid == Name{ } );
   }

   void run_all( )
   {
      basic_string_name( );
      string_view_name( );
      ostream_output( );
      invalid_singleton( );
      CARPC_TRACE_LOG_TRACE( "[TName] All tests passed." );
   }

}



#include <cassert>
#include <iostream>
#include <unordered_set>

#include "carpc/base/types/ID.hpp"

namespace test::id {

   class Service { };
   using ID_S = carpc::TID< Service >;

   class Thread { };
   using ID_T = carpc::TID< Thread >;


   void basic( )
   {
      // --- Конструкторы и копирование ---
      ID_S id1;
      assert( id1.is_invalid( ) );
      assert( id1.value( ) == ID_S::invalid_value );

      ID_S id2{ 42 };
      assert( id2.value( ) == 42 );
      ID_S id3{ id2 };
      assert( id3 == id2 );

      // --- Константы ---
      assert( ID_S::min.value( ) == ID_S::min_value );
      assert( ID_S::max.value( ) == ID_S::max_value );
      assert( ID_S::invalid.value( ) == ID_S::invalid_value );
      assert( ID_S::zero.value( ) == ID_S::zero_value );

      // --- Операторы сравнения ---
      assert( id2 == id3 );
      assert( id2 != ID_S::min );
      assert( ID_S::min < ID_S::max );
      assert( ID_S::max > ID_S::min );
      assert( ID_S::min <= ID_S::min );
      assert( ID_S::max >= ID_S::min );

      // --- Инкремент / Декремент ---
      ID_S id4{ 10 };
      ++id4;
      assert( id4.value( ) == 11 );
      id4++;
      assert( id4.value( ) == 12 );
      --id4;
      assert( id4.value( ) == 11 );
      id4--;
      assert( id4.value( ) == 10 );

      // --- Арифметика + / - ---
      ID_S id5{ 100 };
      ID_S id6 = id5 + 5;
      assert( id6.value( ) == 105 );
      ID_S id7 = id6 - 10;
      assert( id7.value( ) == 95 );

      // --- Проверка std::hash ---
      std::unordered_set< ID_S > id_set;
      id_set.insert(id5);
      id_set.insert(id6);
      assert( id_set.find(id5) != id_set.end( ) );
      assert( id_set.find(id6) != id_set.end( ) );
      assert( id_set.find(id7) == id_set.end( ) );

      ID_S id0;
      assert( id1 == id0 );
   }

   void run_all( )
   {
      basic( );
      CARPC_TRACE_LOG_TRACE( "[TID] All tests passed." );
   }

}



#include <cassert>
#include <unordered_set>
#include <iostream>

#include "carpc/base/types/Version.hpp"

namespace test::version {

   struct Service { };
   using Version = carpc::TVersion< Service >;

   void basic( )
   {
      Version v1; // default 0.0.0
      Version v2(1, 2, 3);
      Version v3(1, 2, 3);
      Version v_invalid = Version::invalid;
      Version v_zero = Version::zero;

      // Проверка default ctor
      assert(v1.major( ) == 0 && v1.minor( ) == 0 && v1.patch( ) == 0);

      // Проверка равенства и сравнения
      assert(v2 == v3);
      assert(v1 < v2);
      assert(v2 > v1);
      assert(v1 <= v_zero);
      assert(v_zero >= v1);

      // Проверка is_valid/is_invalid
      assert(v2.is_valid( ));
      assert(v_invalid.is_invalid( ));
      assert(!v_invalid.is_valid( ));

      // Проверка инкрементов
      Version v = v1;
      v.inc_patch( );
      assert(v.patch( ) == 1 && v.minor( ) == 0 && v.major( ) == 0);

      v.inc_minor( );
      assert(v.minor( ) == 1 && v.patch( ) == 0);

      v.inc_major( );
      assert(v.major( ) == 1 && v.minor( ) == 0 && v.patch( ) == 0);

      // Проверка ostream
      std::ostringstream oss;
      oss << v;
      assert(oss.str( ) == "1.0.0");

      // Проверка hash и использование в unordered_set
      std::unordered_set<Version> set;
      set.insert(v1);
      set.insert(v2);
      set.insert(v_invalid);

      assert(set.find(v1) != set.end( ));
      assert(set.find(v2) != set.end( ));
      assert(set.find(v_invalid) != set.end( ));
      assert(set.find(v3) != set.end( )); // v3 == v2
   }

   void run_all( )
   {
      basic( );
      CARPC_TRACE_LOG_TRACE( "[TVersion] All tests passed." );
   }

}



#include <iostream>

#include "carpc/base/callback/SharedCallback.hpp"

namespace test::shared_callback {

   struct MyData
   {
      int x;
      std::string name;
   };

   void basic( )
   {
      using namespace carpc::callback;

      CARPC_TRACE_LOG_TRACE( "Example void callback" );
      {
         auto cb_void = tBlockerRoot::create(
            [ ]( )
            {
               CARPC_TRACE_LOG_TRACE( "[void] Callback executed!" );
            }
         );
         tBlockerLink wp_void = cb_void;
         CARPC_TRACE_LOG_TRACE( "Is void callback alive? %s",
               SharedCallback::is_locked( wp_void ) ? "YES" : "NO"
            );
         // cb_void->reset_finalizer( );
      }

      CARPC_TRACE_LOG_TRACE( "Example callback with data" );
      {
         auto cb_data = TSharedCallback< MyData >::create(
            [ ]( const MyData& d )
            {
               CARPC_TRACE_LOG_TRACE( "[data] x=%d, name=%s", d.x, d.name.c_str( ) );
            },
            MyData{ 42, "example" }
         );
         TSharedCallback< MyData >::tWptr wp_data = cb_data;
         CARPC_TRACE_LOG_TRACE( "Is data callback alive? %s",
               SharedCallback::is_locked( wp_data ) ? "YES" : "NO"
            );
      }

      CARPC_TRACE_LOG_TRACE( "An example of a safe cyclic dependency" );
      {
         TSharedCallback< MyData >::tWptr wp;
         auto cb_cycle = TSharedCallback< MyData >::create(
            [ &wp ]( const MyData& d )
            {
               CARPC_TRACE_LOG_TRACE( "[cycle] x=%d, name=%s",
                     d.x, d.name.c_str( )
                  );
               if( auto sp = wp.lock( ) )
                  CARPC_TRACE_LOG_TRACE(
                        "[cycle] Object still alive (use_count=%zu)",
                        sp.use_count( )
                     );
               else
                  CARPC_TRACE_LOG_TRACE( "[cycle] Object destroyed" );
            },
            MyData{ 99, "cycle_test" }
         );
         wp = cb_cycle; // weak_ptr for safe access
      }
   }

   void run_all( )
   {
      basic( );
      CARPC_TRACE_LOG_TRACE( "[TSharedCallback] All tests passed." );
   }

}



#include "carpc/base/types/meta/TypeInfo.hpp"

namespace test::type_info {

   void basic( )
   {
      CARPC_TRACE_LOG_TRACE( "%s",
            carpc::types::get_type_name< int >( )
         );

      CARPC_TRACE_LOG_TRACE( "%s",
            carpc::types::get_type_name< std::vector< int > >( )
         );

      CARPC_TRACE_LOG_TRACE( "%s",
            carpc::types::get_type_name< std::vector< std::set< int > > >( )
         );
   }


   void run_all( )
   {
      basic( );
      CARPC_TRACE_LOG_TRACE( "[TSharedCallback] All tests passed." );
   }

}



#include "carpc/base/containers/RingBuffer.hpp"

namespace test::ring_buffer {

   using namespace carpc;

   static void basic( )
   {
      RingBuffer rb( 8 );

      assert( rb.capacity( ) == 8 );
      assert( rb.size( ) == 0 );
      assert( rb.empty( ) );
      assert( !rb.full( ) );
   }

   static void push_pop_linear( )
   {
      RingBuffer rb( 8 );

      uint8_t in[4]  = { 1, 2, 3, 4 };
      uint8_t out[4] = { 0 };

      assert( rb.push_back( in, 4 ) );
      assert( rb.size( ) == 4 );
      assert( !rb.empty( ) );

      std::size_t n = rb.pop_front( out, 4 );
      assert( n == 4 );
      assert( rb.empty( ) );

      assert( std::memcmp( in, out, 4 ) == 0 );
   }

   static void push_pop_wrap_around( )
   {
      RingBuffer rb( 8 );

      uint8_t a[6] = { 1, 2, 3, 4, 5, 6 };
      uint8_t b[4] = { 7, 8, 9, 10 };

      uint8_t out[10] = { 0 };

      assert( rb.push_back( a, 6 ) );

      std::size_t n1 = rb.pop_front( out, 4 );
      assert( n1 == 4 );

      assert( rb.push_back( b, 4 ) );
      assert( rb.size( ) == 6 );

      std::size_t n2 = rb.pop_front( out, 6 );
      assert( n2 == 6 );

      uint8_t expected[6] = { 5, 6, 7, 8, 9, 10 };
      assert( std::memcmp( out, expected, 6 ) == 0 );
   }

   static void peek_does_not_consume( )
   {
      RingBuffer rb( 8 );

      uint8_t in[4]  = { 11, 12, 13, 14 };
      uint8_t out[4] = { 0 };

      rb.push_back( in, 4 );

      std::size_t n = rb.peek( out, 4 );
      assert( n == 4 );
      assert( rb.size( ) == 4 );

      assert( std::memcmp( in, out, 4 ) == 0 );
   }

   static void zero_copy_write_read( )
   {
      RingBuffer rb( 8 );

      auto w = rb.reserve_write( 4 );
      assert( w.is_valid( ) );
      assert( w.size == 4 );

      for( std::size_t i = 0; i < w.size; ++i )
         w.ptr[i] = static_cast< uint8_t >( i + 1 );

      rb.commit_write( w.size );
      assert( rb.size( ) == 4 );

      auto r = rb.reserve_read( 4 );
      assert( r.is_valid( ) );
      assert( r.size == 4 );

      for( std::size_t i = 0; i < r.size; ++i )
         assert( r.ptr[i] == i + 1 );

      rb.commit_read( r.size );
      assert( rb.empty( ) );
   }

   static void writev_readv_wrap( )
   {
      RingBuffer rb( 8 );

      uint8_t a[6] = { 1, 2, 3, 4, 5, 6 };
      uint8_t b[6] = { 7, 8, 9, 10, 11, 12 };

      rb.push_back( a, 6 );
      uint8_t tmp[4];
      rb.pop_front( tmp, 4 );   // consume partially

      auto w = rb.reserve_writev( 6 );
      assert( w.first.is_valid( ) );
      assert( w.second.is_valid( ) );

      std::memcpy( w.first.ptr, b, w.first.size );
      std::memcpy( w.second.ptr, b + w.first.size, w.second.size );

      rb.commit_write( w.total_size( ) );
      assert( rb.size( ) == 8 );

      uint8_t out[8] = { 0 };
      auto r = rb.reserve_readv( 8 );

      std::memcpy( out, r.first.ptr, r.first.size );
      std::memcpy( out + r.first.size, r.second.ptr, r.second.size );

      uint8_t expected[8] = { 5, 6, 7, 8, 9, 10, 11, 12 };
      assert( std::memcmp( out, expected, 8 ) == 0 );

      rb.commit_read( 8 );
      assert( rb.empty( ) );
   }

   static void full_condition( )
   {
      RingBuffer rb( 8 );

      uint8_t data[8] = { 0 };

      assert( rb.push_back( data, 8 ) );
      assert( rb.full( ) );
      assert( !rb.push_back( data, 1 ) );
   }




   void run_all( )
   {
      basic( );
      push_pop_linear( );
      push_pop_wrap_around( );
      peek_does_not_consume( );
      zero_copy_write_read( );
      writev_readv_wrap( );
      full_condition( );
      CARPC_TRACE_LOG_TRACE( "[RingBuffer] All tests passed." );
   }

}



#include "carpc/base/utils/generate/type_id.hpp"
#include "carpc/base/utils/generate/random.hpp"
#include "carpc/base/utils/generate/uuid.hpp"

namespace test::utils::generate {

   struct MsgA { };
   struct MsgB { };

   void run_all( )
   {
      CARPC_TRACE_LOG_TRACE( "%s",
            carpc::utils::generate::type_id::name< MsgA >( ).c_str( )
         );
      CARPC_TRACE_LOG_TRACE( "%llu",
            carpc::utils::generate::type_id::hash< MsgA >( )
         );

      CARPC_TRACE_LOG_TRACE( "%s",
            carpc::utils::generate::random::text( 100 ).c_str( )
         );
      CARPC_TRACE_LOG_TRACE( "%zu",
            carpc::utils::generate::random::number( 100, 1000 )
         );
      CARPC_TRACE_LOG_TRACE( "%f",
            carpc::utils::generate::random::real( 100.0, 1000.0 )
         );

      CARPC_TRACE_LOG_TRACE( "%s",
            carpc::utils::generate::uuid( ).c_str( )
         );

      CARPC_TRACE_LOG_TRACE( "[utils] All tests passed." );
   }

}



#include "carpc/base/utils/type_traits.hpp"

namespace test::utils::type_traits {

   struct WithFoo
   {
      void foo( )
      {
         CARPC_TRACE_LOG_TRACE( "1" );
      }
      int foo( int a )
      {
         CARPC_TRACE_LOG_TRACE( "2" );
         return a + 1;
      }
      int foo( int a ) const
      {
         CARPC_TRACE_LOG_TRACE( "2 const" );
         return a + 1;
      }
   };

   struct WithoutFoo
   {
      void bar( )
      {
         CARPC_TRACE_LOG_TRACE( "3" );
      }
      int bar( int a )
      {
         CARPC_TRACE_LOG_TRACE( "4" );
         return a + 2;
      }
   };

   DEFINE_CALL_IF_EXISTS( foo );
   DEFINE_CALL_IF_EXISTS( bar );

   // DEFINE_CALL_IF_EXISTS_20( foo );
   // DEFINE_CALL_IF_EXISTS_20( bar );

   void run_all( )
   {
      WithFoo with_foo;
      WithoutFoo without_foo;

      {
         auto result = call_foo( with_foo );
      }
      {
         auto result = call_foo( with_foo, 45 );
         if( result )
         {
            CARPC_TRACE_LOG_TRACE( "result: %d", result.value( ) );
         }
      }
      {
         auto result = call_foo( with_foo, 45, 6 );
      }
      {
         auto result = call_foo( without_foo );
      }
      {
         auto result = call_foo( without_foo, 45 );
         if( result )
         {
            CARPC_TRACE_LOG_TRACE( "result: %d", result.value( ) );
         }
      }

      CARPC_TRACE_LOG_TRACE( "-----" );

      {
         auto result = call_bar( with_foo );
      }
      {
         auto result = call_bar( with_foo, 45 );
         if( result )
         {
            CARPC_TRACE_LOG_TRACE( "result: %d", result.value( ) );
         }
      }
      {
         auto result = call_bar( with_foo, 45, 6 );
      }
      {
         auto result = call_bar( without_foo );
      }
      {
         auto result = call_bar( without_foo, 45 );
         if( result )
         {
            CARPC_TRACE_LOG_TRACE( "result: %d", result.value( ) );
         }
      }

      CARPC_TRACE_LOG_TRACE( "-----" );


      // auto r1 = CARPC_METHOD(&WithFoo::foo)(with_foo, 10);
      // auto r2 = CARPC_METHOD(&WithFoo::foo)(without_foo, 10);


      {
         auto result = CARPC_METHOD(
               int (WithFoo::*)(int),
               WithFoo,
               foo
            )(with_foo, 10);
         if( result )
         {
            CARPC_TRACE_LOG_TRACE( "result: %d", result.value( ) );
         }
      }
      {
         auto result = CARPC_METHOD(
               int (WithFoo::*)(int),
               WithFoo,
               foo
            )(without_foo, 10);
         if( result )
         {
            CARPC_TRACE_LOG_TRACE( "result: %d", result.value( ) );
         }
      }

      CARPC_TRACE_LOG_TRACE( "[utils] All tests passed." );
   }

}



#include "carpc/base/utils/converter.hpp"

namespace test::utils::converter {

   void run_all( )
   {
      TYPE_INFO( int );

      {
         using TYPE = int;
         TYPE value = 0;
         const char* string = "12345";
         if( carpc::utils::from_string( string, value ) )
         {
            CARPC_TRACE_LOG_TRACE( "'%s' -> '%d'", string, value  );
         }
         else
         {
            CARPC_TRACE_LOG_ERROR( "'%s' convert error", string );
         }
      }

      {
         using TYPE = uint16_t;
         TYPE value = 0;
         const char* string = "12345789";
         if( carpc::utils::from_string( string, value ) )
         {
            CARPC_TRACE_LOG_TRACE( "'%s' -> '%u'", string, value  );
         }
         else
         {
            CARPC_TRACE_LOG_ERROR( "'%s' convert error", string );
         }
      }

      {
         using TYPE = uint16_t;
         TYPE value = 0;
         const char* string = "65535";
         if( carpc::utils::from_string( string, value ) )
         {
            CARPC_TRACE_LOG_TRACE( "'%s' -> '%u'", string, value  );
         }
         else
         {
            CARPC_TRACE_LOG_ERROR( "'%s' convert error", string );
         }
      }

      {
         using TYPE = float;
         TYPE value = 0;
         const char* string = "3.14159";
         if( carpc::utils::from_string( string, value ) )
         {
            CARPC_TRACE_LOG_TRACE( "'%s' -> '%f'", string, value  );
         }
         else
         {
            CARPC_TRACE_LOG_ERROR( "'%s' convert error", string );
         }
      }

      {
         using TYPE = double;
         TYPE value = 0;
         const char* string = "2.718281828459045";
         if( carpc::utils::from_string( string, value ) )
         {
            CARPC_TRACE_LOG_TRACE( "'%s' -> '%.10f'", string, value  );
         }
         else
         {
            CARPC_TRACE_LOG_ERROR( "'%s' convert error", string );
         }
      }


      {
         using TYPE = long double;
         TYPE value = 0;
         const char* string = "1.6180339887";
         if( carpc::utils::from_string( string, value ) )
         {
            CARPC_TRACE_LOG_TRACE( "'%s' -> '%.10Lf'", string, value  );
         }
         else
         {
            CARPC_TRACE_LOG_ERROR( "'%s' convert error", string );
         }
      }

      CARPC_TRACE_LOG_TRACE( "[utils::converter] All tests passed." );
   }

}



#include "carpc/base/utils/cpu.hpp"

namespace test::utils::cpu {

   void run_all( )
   {
      CARPC_TRACE_LOG_TRACE( "%llu", carpc::utils::read_tsc( ) );
      CARPC_TRACE_LOG_TRACE( "%llu", carpc::utils::read_time_ns( ) );

      CARPC_TRACE_LOG_TRACE( "[utils::cpu] All tests passed." );
   }

}



#include "carpc/base/utils/print.hpp"

namespace test::utils::print {

   void run_all( )
   {
      carpc::utils::write( -1, "write: %d\n", 1 );
      carpc::utils::write( -1, "write: %d / %s\n", 1, "lmcpsmdckmsdkvcndisubiudsbvcyisdcbisyavbisdycidyiasducbisdbcisdbcisdujciuvyoudncuoiwetrugfirbtknrbvjksbdcv" );

      CARPC_TRACE_LOG_TRACE( "[utils::print] All tests passed." );
   }

}







#if 0
#define DEFINE_ENUM_EXP( EnumName, UnderlyingType, DefFile )      \
   enum class EnumName : UnderlyingType                           \
   {                                                              \
      /* NOLINTBEGIN */                                           \
      /* clang-format off */                                      \
      #define X(name) name,                                       \
      #include DefFile                                            \
      #undef X                                                    \
      /* clang-format on */                                       \
      /* NOLINTEND */                                             \
      Count                                                       \
   };

#define DEFINE_ENUM_TO_STRING( EnumName, DefFile )                \
   constexpr std::string_view to_string( EnumName value )         \
   {                                                              \
      switch( value )                                             \
      {                                                           \
         #define X(name) case EnumName::name: return #name;       \
         #include DefFile                                         \
         #undef X                                                 \
         case EnumName::Count: break;                             \
      }                                                           \
      return "Unknown";                                           \
   }

#define DEFINE_ENUM_FROM_STRING( EnumName, DefFile )              \
   constexpr std::optional< EnumName >                            \
      from_string( std::string_view str )                         \
   {                                                              \
      #define X(name)                                             \
         if( str == #name )                                       \
         {                                                        \
            return EnumName::name;                                \
         }                                                        \
      #include DefFile                                            \
      #undef X                                                    \
      return std::nullopt;                                        \
   }

#define DEFINE_ENUM_ARRAY( EnumName, DefFile )                    \
   constexpr auto EnumName##Values = [ ]( )                       \
   {                                                              \
      return std::array<                                          \
            EnumName, static_cast< size_t >( EnumName::Count )    \
         >                                                        \
      {                                                           \
         #define X(name) EnumName::name,                          \
         #include DefFile                                         \
         #undef X                                                 \
      };                                                          \
   }( );

#define DEFINE_ENUM_STATIC_ASSERT( EnumName )                     \
   static_assert(                                                 \
         EnumName##Values.size() ==                               \
            static_cast< size_t >(EnumName::Count),               \
         "Enum Count mismatch"                                    \
      );

#define DEFINE_ENUM( EnumName, UnderlyingType, DefFile )          \
   DEFINE_ENUM_FROM_DEF( EnumName, UnderlyingType, DefFile )      \
   DEFINE_ENUM_TO_STRING( EnumName, DefFile )                     \
   DEFINE_ENUM_FROM_STRING( EnumName, DefFile )                   \
   DEFINE_ENUM_ARRAY( EnumName, DefFile )                         \
   DEFINE_ENUM_STATIC_ASSERT( EnumName )
#endif



int main( )
{
   carpc::trace::StdoutSink sink;
   carpc::trace::Runtime::start( &sink );

   // test::name::run_all( );
   // test::id::run_all( );
   // test::version::run_all( );
   // test::shared_callback::run_all( );
   // test::type_info::run_all( );
   // test::ring_buffer::run_all( );
   // test::utils::generate::run_all( );
   // test::utils::type_traits::run_all( );
   // test::utils::converter::run_all( );
   // test::utils::cpu::run_all( );
   // test::utils::print::run_all( );

   carpc::trace::Runtime::stop( );

   return 0;
}
