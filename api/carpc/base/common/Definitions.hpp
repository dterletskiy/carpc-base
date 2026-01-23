#pragma once



#include <bit>
#include <cstdint>
#include <climits>



#if CHAR_BIT != 8
   #error "unsupported char size"
#endif



//======================================================================
//  Architecture detection
//======================================================================

namespace carpc {

   enum class eCpuArch : std::uint32_t
   {
      unknown = 0,

      x86_32,
      x86_64,

      arm_v2,
      arm_v3,
      arm_v4t,
      arm_v5,
      arm_v6,
      arm_v6t2,
      arm_v7,
      arm_v7a,
      arm_v7r,
      arm_v7m,
      arm_v7s,
      arm64,

      mips,
      powerpc,
      powerpc64,
      sparc,
      superh,
      m68k
   };

}



//--------------------------------------------------------------------
//  Compiler-provided architecture macros
//--------------------------------------------------------------------

#if defined(__x86_64__) || defined(_M_X64)

   #define CARPC_ARCH_X86_64
   #define CARPC_ARCH_VALUE carpc::eCpuArch::x86_64

#elif defined(__i386__) || defined(_M_IX86)

   #define CARPC_ARCH_X86_32
   #define CARPC_ARCH_VALUE carpc::eCpuArch::x86_32


#elif defined(__aarch64__) || defined(_M_ARM64)

   #define CARPC_ARCH_ARM64
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm64


#elif defined(__ARM_ARCH_7M__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V7
   #define CARPC_ARCH_ARM_V7M
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v7m

#elif defined(__ARM_ARCH_7R__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V7
   #define CARPC_ARCH_ARM_V7R
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v7r

#elif defined(__ARM_ARCH_7A__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V7
   #define CARPC_ARCH_ARM_V7A
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v7a

#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7S__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V7
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v7


#elif defined(__ARM_ARCH_6T2__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V6T2
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v6t2

#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || \
      defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V6
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v6


#elif defined(__ARM_ARCH_5__) || defined(__ARM_ARCH_5E__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V5
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v5


#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V4T
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v4t


#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V3
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v3


#elif defined(__ARM_ARCH_2__)

   #define CARPC_ARCH_ARM
   #define CARPC_ARCH_ARM_V2
   #define CARPC_ARCH_VALUE carpc::eCpuArch::arm_v2


#elif defined(__mips__) || defined(__mips)

   #define CARPC_ARCH_MIPS
   #define CARPC_ARCH_VALUE carpc::eCpuArch::mips


#elif defined(__ppc64__) || defined(__powerpc64__)

   #define CARPC_ARCH_POWERPC64
   #define CARPC_ARCH_VALUE carpc::eCpuArch::powerpc64

#elif defined(__ppc__) || defined(__powerpc__) || defined(_ARCH_PPC)

   #define CARPC_ARCH_POWERPC
   #define CARPC_ARCH_VALUE carpc::eCpuArch::powerpc


#elif defined(__sparc__)

   #define CARPC_ARCH_SPARC
   #define CARPC_ARCH_VALUE carpc::eCpuArch::sparc


#elif defined(__sh__)

   #define CARPC_ARCH_SUPERH
   #define CARPC_ARCH_VALUE carpc::eCpuArch::superh


#elif defined(__m68k__)

   #define CARPC_ARCH_M68K
   #define CARPC_ARCH_VALUE carpc::eCpuArch::m68k


#else

   #define CARPC_ARCH_UNKNOWN
   #define CARPC_ARCH_VALUE carpc::eCpuArch::unknown

#endif



namespace carpc {

   inline constexpr eCpuArch cpu_arch = CARPC_ARCH_VALUE;

}



//======================================================================
//  RTTI detection
//======================================================================

#if defined(__clang__)

   #if __has_feature(cxx_rtti)
      #define CARPC_RTTI_ENABLED
   #endif

#elif defined(__GNUG__)

   #if defined(__GXX_RTTI)
      #define CARPC_RTTI_ENABLED
   #endif

#elif defined(_MSC_VER)

   #if defined(_CPPRTTI)
      #define CARPC_RTTI_ENABLED
   #endif

#endif



//======================================================================
//  Endianness (C++20)
//======================================================================

namespace carpc {

   enum class eByteOrder : std::uint32_t
   {
      little = 0,
      big    = 1,
      mixed  = 2
   };

   inline constexpr eByteOrder byte_order =
      ( std::endian::native == std::endian::little ) ? eByteOrder::little :
      ( std::endian::native == std::endian::big    ) ? eByteOrder::big    :
                                                        eByteOrder::mixed;

   inline constexpr const char* to_str( const eByteOrder value )
   {
      switch( value )
      {
         case eByteOrder::little: return "little-endian";
         case eByteOrder::big:    return "big-endian";
         default:                 return "mixed-endian";
      }
   }

}



//======================================================================
//  Debug helpers
//======================================================================

#if 0

#include <cstdio>

int main( )
{
   std::printf( "arch = %u\n", static_cast< unsigned >( carpc::cpu_arch ) );
   std::printf( "endian = %s\n", carpc::to_str( carpc::byte_order ) );

   return 0;
}

#endif
