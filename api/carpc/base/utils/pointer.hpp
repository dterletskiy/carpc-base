#pragma once

#include <cstddef>
#include <type_traits>



namespace carpc::pointer {

   /**
    * @brief Increment a pointer by a given number of bytes.
    *
    * This function returns a pointer of the same type as @p ptr, moved forward
    * by @p value bytes. Works with object and void pointers.
    *
    * @tparam T Type of the pointer.
    * @param ptr Pointer to increment.
    * @param value Number of bytes to move forward.
    * @return Pointer of type T* offset by value bytes.
    */
   template< typename T >
   constexpr T* inc( T* ptr, std::size_t value )
   {
      static_assert(
            std::is_object_v< T > || std::is_void_v< T >,
            "inc can only be used with object or void pointers"
         );
      return reinterpret_cast< T* >(
            reinterpret_cast< std::byte* >( ptr ) + value
         );
   }

   /**
    * @brief Decrement a pointer by a given number of bytes.
    *
    * This function returns a pointer of the same type as @p ptr, moved backward
    * by @p value bytes. Works with object and void pointers.
    *
    * @tparam T Type of the pointer.
    * @param ptr Pointer to decrement.
    * @param value Number of bytes to move backward.
    * @return Pointer of type T* offset by -value bytes.
    */
   template< typename T >
   constexpr T* dec( T* ptr, std::size_t value )
   {
      static_assert(
            std::is_object_v< T > || std::is_void_v< T >,
            "dec can only be used with object or void pointers"
         );
      return reinterpret_cast< T* >(
            reinterpret_cast< std::byte* >( ptr ) - value
         );
   }

   /**
    * @brief Compute the difference in bytes between two pointers.
    *
    * Returns the number of bytes between @p ptr1 and @p ptr2. Works with
    * object and void pointers of potentially different types.
    *
    * @tparam T1 Type of the first pointer.
    * @tparam T2 Type of the second pointer.
    * @param ptr1 First pointer.
    * @param ptr2 Second pointer.
    * @return Difference in bytes (ptr1 - ptr2).
    */
   template< typename T1, typename T2 >
   constexpr std::ptrdiff_t diff( T1* ptr1, T2* ptr2 )
   {
      static_assert(
            ( std::is_object_v<T1> || std::is_void_v<T1> ) &&
            ( std::is_object_v<T2> || std::is_void_v<T2> ),
            "diff can only be used with object or void pointers"
         );
      return reinterpret_cast< const std::byte* >( ptr1 ) - 
         reinterpret_cast< const std::byte* >( ptr2 );
   }

} // namespace carpc::pointer
