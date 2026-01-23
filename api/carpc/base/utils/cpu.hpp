#pragma once



namespace carpc::utils {

   /**
    * @brief Read raw CPU timestamp counter or equivalent hardware counter.
    *
    * This function returns a fast, low-level hardware counter value that can be
    * used for high-resolution timing and profiling.
    *
    * Architecture-specific behavior:
    * - x86 / x86_64:
    *   Returns the value of the Time Stamp Counter (TSC) using RDTSC or RDTSCP.
    *   The value is a raw counter in CPU cycles and has no defined epoch.
    *
    * - ARMv8 (AArch64):
    *   Reads CNTVCT_EL0 and converts it to nanoseconds using CNTFRQ_EL0.
    *   The returned value is already expressed in nanoseconds.
    *
    * - ARMv7:
    *   Reads the 32-bit PMCCNTR register and extends it to 64 bits in software.
    *   The value represents raw counter ticks, not nanoseconds.
    *
    * - Other architectures:
    *   Returns 0.
    *
    * Notes:
    * - The returned value is monotonic but not synchronized to any absolute
    *   time reference.
    * - The meaning and units of the return value depend on the architecture.
    * - Intended for internal use or low-level profiling code.
    *
    * @return Raw hardware counter value or nanoseconds on ARMv8.
    */
   unsigned long long read_tsc( );

   /**
    * @brief Read high-resolution monotonic time in nanoseconds.
    *
    * This function returns a monotonic time value expressed in nanoseconds,
    * suitable for profiling and measuring elapsed time intervals.
    *
    * The implementation uses the fastest available hardware counter on each
    * architecture and performs automatic frequency detection when required.
    *
    * Architecture-specific behavior:
    * - x86 / x86_64:
    *   Uses the Time Stamp Counter (TSC) and converts it to nanoseconds.
    *   The TSC frequency is detected once using CPUID leaf 0x15 or 0x16.
    *
    * - ARMv8 (AArch64):
    *   Uses CNTVCT_EL0 as the counter and CNTFRQ_EL0 for frequency conversion.
    *   The returned value is derived directly from architectural timers.
    *
    * - ARMv7:
    *   Uses the PMCCNTR performance counter extended to 64 bits.
    *   The counter frequency is read from system registers or a fallback
    *   value is used if unavailable.
    *
    * - Other architectures:
    *   Returns 0.
    *
    * Important notes:
    * - The returned value is monotonic but does not represent wall-clock time.
    * - There is no defined epoch; values should only be compared by difference.
    * - Suitable for high-resolution profiling and benchmarking.
    * - Not intended as a replacement for CLOCK_REALTIME or system time APIs.
    *
    * @return Monotonic time value in nanoseconds.
    */
   unsigned long long read_time_ns( );

}
