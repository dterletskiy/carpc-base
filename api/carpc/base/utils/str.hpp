#include <cstddef>
#include <cstdint>
#include <string>



namespace carpc::utils::cstring {

   constexpr const char* WHITESPACE = " \t\n\r\f\v";

   /**
    * @brief Safely copies a C-string into a destination buffer with truncation.
    *
    * Copies the null-terminated string from src to dst, ensuring no 
    * buffer overflow.
    * If src is longer than dst_size, it is truncated and dst is always 
    * null-terminated.
    *
    * @param dst Pointer to the destination buffer.
    * @param dst_size Size of the destination buffer in bytes.
    * @param src Pointer to the null-terminated source string.
    * @return The length of src (excluding null terminator). 
    *    If >= dst_size, src was truncated.
    *
    * @note If dst_size is 0, nothing is written to dst and the length of 
    * src is returned.
    * @warning dst and src must be valid non-null pointers.
    */
   std::size_t strcpy( char *dst, std::size_t dst_size, const char *src );

   /**
    * @brief Optimized in-place removal of leading whitespace using pointer arithmetic.
    * @param s Input C-string to modify.
    * @details
    * Scans from the start of the string to find the first non-whitespace character,
    * then moves the remaining characters to the beginning using memmove.
    * Fully whitespace strings become empty.
    */
   void ltrim_inplace( char* s );

   /**
    * @brief Optimized in-place removal of trailing whitespace using pointer arithmetic.
    * @param s Input C-string to modify.
    * @details
    * Scans from the end of the string to find the last non-whitespace character,
    * then moves the null terminator '\0' just after it.
    * Fully whitespace strings become empty.
    */
   void rtrim_inplace( char* s );

   /**
    * @brief Optimized in-place removal of leading and trailing whitespace using pointer arithmetic.
    * @param s Input C-string to modify.
    * @details
    * Scans from both ends to find the first and last non-whitespace characters,
    * then moves the substring to the beginning and adds null terminator.
    * Fully whitespace strings become empty.
    */
   void trim_inplace( char* s );

} // namespace carpc::utils::cstring



#include <sstream>
#include <string>

namespace carpc::utils::string {

   /**
    * @brief Concatenates any number of arguments into a single string.
    *
    * This function accepts a variable number of arguments of any types
    * that support the stream insertion operator (`<<`) and concatenates
    * them into a single `std::string`.
    *
    * @tparam TYPES Variadic template parameter pack for the types of the arguments.
    * @param args The arguments to concatenate.
    * @return A single `std::string` containing all arguments concatenated in order.
    *
    * @note This version does not insert any separators between the arguments.
    *
    * @example
    * std::string s = format_string(1, " + ", 2, " = ", 3);
    * // s == "1 + 2 = 3"
    */
   template< typename ... TYPES >
   std::string format_string( const TYPES& ... args )
   {
      std::ostringstream ss;
      (ss << ... << args);
      return ss.str( );
   }

   /**
    * @brief Concatenates any number of arguments into a single string 
    * with a separator.
    *
    * This function accepts a variable number of arguments of any types
    * that support the stream insertion operator (`<<`) and concatenates
    * them into a single `std::string`, inserting the given separator
    * between each argument.
    *
    * @tparam TYPES Variadic template parameter pack for the types of the 
    *    arguments.
    * @param sep The separator string to insert between each argument.
    * @param args The arguments to concatenate.
    * @return A single `std::string` containing all arguments concatenated 
    *    in order, with `sep` inserted between each argument.
    *
    * @example
    * std::string s = format_string_sep(", ", "apple", "banana", "cherry");
    * // s == "apple, banana, cherry"
    */
   template< typename ... TYPES >
   std::string format_string_sep( const std::string& sep, const TYPES& ... args )
   {
      std::ostringstream ss;
      bool first = true;
      auto append = [&]( const auto& arg )
      {
         if( !first ) ss << sep;
         ss << arg;
         first = false;
      };

      (append(args), ...); // fold expression calls append for each argument
      return ss.str( );
   }

   constexpr const char* WHITESPACE = " \t\n\r\f\v";

   /**
    * @brief Removes leading whitespace characters from a string.
    * 
    * @param s The input string.
    * @return A new string with leading whitespace removed.
    * 
    * @details
    * The function searches for the first character that is not in `WHITESPACE`
    * and returns the substring starting from that character.
    * If the input string is empty or contains only whitespace, an empty string is returned.
    * 
    * @code
    * std::string s = "   hello";
    * auto trimmed = carpc::utils::string::ltrim(s); // "hello"
    * @endcode
    */
   std::string ltrim( const std::string& s );

   /**
    * @brief Removes trailing whitespace characters from a string.
    * 
    * @param s The input string.
    * @return A new string with trailing whitespace removed.
    * 
    * @details
    * The function searches for the last character that is not in `WHITESPACE`
    * and returns the substring up to that character (inclusive).
    * If the input string is empty or contains only whitespace, an empty string is returned.
    * 
    * @code
    * std::string s = "hello   ";
    * auto trimmed = carpc::utils::string::rtrim(s); // "hello"
    * @endcode
    */
   std::string rtrim( const std::string& s );

   /**
    * @brief Removes both leading and trailing whitespace from a string.
    * 
    * @param s The input string.
    * @return A new string with leading and trailing whitespace removed.
    * 
    * @details
    * This function combines `ltrim` and `rtrim` into a single call.
    * If the input string is empty or contains only whitespace, an empty string is returned.
    * 
    * @code
    * std::string s = "  hello world  ";
    * auto trimmed = carpc::utils::string::trim(s); // "hello world"
    * @endcode
    */
   std::string trim( const std::string& s );

   /**
    * @brief Removes leading whitespace from a string in-place.
    * 
    * @param s The string to modify.
    * 
    * @details
    * This function directly modifies the input string.
    * Leading whitespace is removed using `std::string::erase`.
    * If the string is empty or contains only whitespace, it becomes empty.
    * 
    * @code
    * std::string s = "   hello";
    * carpc::utils::string::ltrim_inplace(s);
    * // s == "hello"
    * @endcode
    */
   void ltrim_inplace( std::string& s );

   /**
    * @brief Removes trailing whitespace from a string in-place.
    * 
    * @param s The string to modify.
    * 
    * @details
    * This function directly modifies the input string.
    * Trailing whitespace is removed using `std::string::erase`.
    * If the string is empty or contains only whitespace, it becomes empty.
    * 
    * @code
    * std::string s = "hello   ";
    * carpc::utils::string::rtrim_inplace(s);
    * // s == "hello"
    * @endcode
    */
   void rtrim_inplace( std::string& s );

   /**
    * @brief Removes leading and trailing whitespace from a string in-place.
    * 
    * @param s The string to modify.
    * 
    * @details
    * This function combines `ltrim_inplace` and `rtrim_inplace`.
    * Two `erase` calls are made: first for trailing whitespace, then for leading 
    * whitespace.
    * The input string is modified directly.
    * If the string is empty or contains only whitespace, it becomes empty.
    * 
    * @code
    * std::string s = "  hello world  ";
    * carpc::utils::string::trim_inplace(s);
    * // s == "hello world"
    * @endcode
    */
   void trim_inplace( std::string& s );

   namespace optimized {

      /**
       * @brief Optimized in-place removal of leading whitespace using 
       * pointer arithmetic.
       * 
       * @param s The string to modify.
       * 
       * @details
       * Designed for long strings, this function:
       * - Scans from the beginning of the string with a pointer until 
       * the first non-whitespace character.
       * - Moves the non-whitespace part to the beginning of the string 
       * using `memmove`.
       * - Resizes the string to remove trailing characters.
       * Fully whitespace strings become empty.
       * 
       * @code
       * std::string s = "   hello";
       * carpc::utils::string::optimized::ltrim_inplace(s);
       * // s == "hello"
       * @endcode
       */
      void ltrim_inplace( std::string& s );

      /**
       * @brief Optimized in-place removal of trailing whitespace using pointer arithmetic.
       * 
       * @param s The string to modify.
       * 
       * @details
       * Designed for long strings, this function:
       * - Scans from the end of the string with a pointer until the last 
       * non-whitespace character.
       * - Resizes the string to remove trailing characters.
       * Fully whitespace strings become empty.
       * 
       * @code
       * std::string s = "hello   ";
       * carpc::utils::string::optimized::rtrim_inplace(s);
       * // s == "hello"
       * @endcode
       */
      void rtrim_inplace( std::string& s );

      /**
       * @brief Optimized in-place removal of leading and trailing whitespace 
       * using pointer arithmetic.
       * 
       * @param s The string to modify.
       * 
       * @details
       * Designed for long strings, this function:
       * - Scans from both ends with pointers to find the first and last 
       * non-whitespace characters.
       * - Moves the non-whitespace substring to the beginning using `memmove`.
       * - Resizes the string accordingly.
       * Fully whitespace strings become empty.
       * This version minimizes memory operations and avoids multiple calls to `erase`.
       * 
       * @code
       * std::string s = "  hello world  ";
       * carpc::utils::string::optimized::trim_inplace(s);
       * // s == "hello world"
       * @endcode
       */
      void trim_inplace( std::string& s );

   } // namespace optimized

} // namespace carpc::utils::string
