#include <atomic>
#include <stdint.h>
#include <thread>

#include "carpc/base/utils/cpu.hpp"



#if defined(__x86_64__) || defined(__i386__)
   #include <x86intrin.h>   // __rdtsc(), __rdtscp()
#elif defined(__aarch64__) || defined(__arm__)
   #include <unistd.h>
#endif



namespace carpc::utils {

// Returns CPU timestamp counter (TSC) or equivalent for ARM/x86
// On ARMv8 returns nanoseconds using CNTVCT_EL0 and CNTFRQ_EL0
unsigned long long read_tsc( )
{
   unsigned long long tsc = 0;

#if defined(__x86_64__) || defined(__i386__)
   #if defined(__RDTSCP__)
      unsigned int aux;
      tsc = __rdtscp(&aux); // serialized
   #else
      unsigned int hi, lo;
      asm volatile("lfence\nrdtsc" : "=a"(lo), "=d"(hi));
      tsc = ((unsigned long long)hi << 32) | lo;
   #endif

#elif defined(__aarch64__)
   // ARMv8: CNTVCT_EL0 is 64-bit virtual counter, CNTFRQ_EL0 = frequency
   uint64_t cnt, freq;
   asm volatile("mrs %0, cntvct_el0" : "=r"(cnt));
   asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
   // Convert ticks to nanoseconds
   tsc = (cnt * 1000000000ULL) / freq;

#elif defined(__arm__)
   // ARMv7: 32-bit PMCCNTR, extend to 64-bit if needed
   uint32_t cc;
   asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(cc));
   static uint64_t high = 0;
   static uint32_t last = 0;

   // Extend 32-bit counter to 64-bit manually
   if(cc < last) high++; // overflow
   last = cc;
   tsc = (high << 32) | cc;

#else
   // Fallback for unsupported arch
   std::atomic_signal_fence(std::memory_order_seq_cst);
   tsc = 0;
   std::atomic_signal_fence(std::memory_order_seq_cst);
#endif

   return tsc;
}



// Returns current time in nanoseconds, unified for x86 and ARM.
// Uses system registers for accurate frequency estimation.
unsigned long long read_time_ns( )
{
   static bool initialized = false;
   static uint64_t tsc_freq = 0;   // x86 TSC frequency
   static uint64_t pmu_freq = 0;   // ARMv7 PMCCNTR frequency
   unsigned long long tsc = 0;

#if defined(__x86_64__) || defined(__i386__)

   if( not initialized )
   {
      // Try CPUID leaf 0x15 (TSC/Crystal) or leaf 0x16 (base freq MHz)
      uint32_t eax=0, ebx=0, ecx=0, edx=0;
      eax = 0x15;
      __asm__ volatile( "cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(eax)
         );
      if( eax != 0 && ebx != 0 )
      {
         tsc_freq = ((uint64_t)ecx * (uint64_t)ebx) / (uint64_t)eax;
      }
      else
      {
         eax = 0x16;
         __asm__ volatile( "cpuid"
               : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
               : "a"(eax)
            );
         tsc_freq = (eax != 0 ? (uint64_t)eax * 1000000ULL : 3000000000ULL);
      }
      initialized = true;
    }

   #if defined(__RDTSCP__)
      unsigned int aux;
      tsc = __rdtscp(&aux);
   #else
      unsigned int hi, lo;
      asm volatile( "lfence\nrdtsc" : "=a"(lo), "=d"(hi) );
      tsc = ((unsigned long long)hi << 32) | lo;
   #endif
   tsc = (tsc * 1000000000ULL) / tsc_freq;

#elif defined(__aarch64__)

   uint64_t cnt, freq;
   asm volatile( "mrs %0, cntvct_el0" : "=r"(cnt) );
   asm volatile( "mrs %0, cntfrq_el0" : "=r"(freq) );
   tsc = (cnt * 1000000000ULL) / freq;

#elif defined(__arm__)

   if( not initialized )
   {
      // Attempt to read CNTFRQ_EL0 if available
      // (ARMv7 Performance counter freq)
      uint32_t frq = 0;
      asm volatile( "mrc p15, 0, %0, c9, c14, 0" : "=r"(frq) );
      pmu_freq = (frq != 0 ? (uint64_t)frq : 100000000ULL); // fallback 100MHz
      initialized = true;
   }

   uint32_t cc;
   asm volatile( "mrc p15, 0, %0, c9, c13, 0" : "=r"(cc) );
   static uint64_t high = 0;
   static uint32_t last = 0;
   if( cc < last ) high++; // handle 32-bit overflow
   last = cc;
   uint64_t counter64 = (high << 32) | cc;
   tsc = (counter64 * 1000000000ULL) / pmu_freq;

#else

   // fallback for unsupported architectures
   std::atomic_signal_fence( std::memory_order_seq_cst );
   tsc = 0;
   std::atomic_signal_fence( std::memory_order_seq_cst );

#endif

   return tsc;
}



}
