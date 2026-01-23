#pragma once



namespace carpc::utils::generate {

   /**
    * @brief Generates a random UUID (version 4) as a string.
    *
    * This function generates a universally unique identifier (UUID) conforming
    * to RFC 4122, version 4. The generated UUID is represented as a 36-character
    * string in the standard format:
    * "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx", where each 'x' is a hexadecimal
    * digit, and 'y' is one of 8, 9, a, or b, representing the variant.
    *
    * The function uses a secure source of randomness provided by the operating
    * system (e.g., /dev/urandom on Unix-like systems) to ensure unpredictability.
    *
    * @return A std::string containing the generated UUID.
    *
    * @note The function is thread-safe and can be called concurrently from multiple threads.
    *
    * @example
    * std::string id = uuid();
    * // Example output: "3f2504e0-4f89-41d3-9a0c-0305e82c3301"
    */
   std::string uuid( );

}
