/*
 * ThorLog ESP-IDF Adapter
 *
 * This header provides an ESP-IDF compatible implementation of the ThorPrint
 * interface, allowing ThorLog to work with ESP-IDF's standard output (printf/stdout).
 *
 * This serves as both a working adapter AND documentation for how users can
 * create their own print adapters for different platforms.
 *
 * ============================================================================
 * USAGE EXAMPLE:
 * ============================================================================
 *
 * #include "thorlog.h"
 * #include "thorlog_espidf.h"
 *
 * void app_main() {
 *     // Option 1: Use the global instance
 *     ThorLog.begin(THORLOG_LEVEL_VERBOSE, &EspIdfOutput);
 *     ThorLog.infoln("Hello from ThorLog! Value: %d", 42);
 *
 *     // Option 2: Create your own instance
 *     EspIdfPrint myOutput;
 *     ThorLog.begin(THORLOG_LEVEL_VERBOSE, &myOutput);
 *     ThorLog.warningln("Custom instance works too!");
 *
 *     // Log at different levels
 *     ThorLog.fatalln("Fatal error: %s", "something bad");
 *     ThorLog.errorln("Error code: %x", 0xDEAD);
 *     ThorLog.warningln("Warning: temperature is %d degrees", 85);
 *     ThorLog.infoln("Info: system started");
 *     ThorLog.traceln("Trace: entering function");
 *     ThorLog.verboseln("Verbose: detailed debug info");
 * }
 *
 * ============================================================================
 * CREATING YOUR OWN ADAPTER:
 * ============================================================================
 *
 * To create an adapter for a different platform, implement the ThorPrint
 * interface with your platform's output mechanism. For example:
 *
 * class MyCustomPrint : public ThorPrint {
 * public:
 *     size_t print(char c) override {
 *         // Your implementation here
 *         return my_putchar(c) != EOF ? 1 : 0;
 *     }
 *
 *     size_t print(const char* str) override {
 *         // Your implementation here
 *         return my_puts(str);
 *     }
 *
 *     size_t print(int num, int base = 10) override {
 *         // Handle decimal, hex, binary
 *         if (base == 10) return my_print_decimal(num);
 *         if (base == 16) return my_print_hex(num);
 *         if (base == 2) return my_print_binary(num);
 *         return 0;
 *     }
 *
 *     // ... implement remaining virtual methods:
 *     // print(unsigned int, int base)
 *     // print(long, int base)
 *     // print(unsigned long, int base)
 *     // print(double)
 * };
 *
 * ============================================================================
 * SUPPORTED FORMAT SPECIFIERS (in ThorLog messages):
 * ============================================================================
 *
 * %s   - string (char*)
 * %c   - single character
 * %C   - character or hex if non-printable
 * %d   - integer (decimal)
 * %l   - long (decimal)
 * %u   - unsigned long (decimal)
 * %x   - hexadecimal (lowercase)
 * %X   - hexadecimal with 0x prefix
 * %b   - binary
 * %B   - binary with 0b prefix
 * %t   - boolean "t"/"f"
 * %T   - boolean "true"/"false"
 * %D,%F - double/float
 * %p   - pointer address
 *
 * ============================================================================
 */

#pragma once

#include "thorlog.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

/**
 * @class EspIdfPrint
 * @brief ESP-IDF implementation of ThorPrint using printf/stdout
 *
 * This class implements the ThorPrint interface using ESP-IDF's standard
 * output functions (printf). All output goes to the default UART console.
 *
 * Features:
 * - Support for all numeric bases (decimal, hexadecimal, binary)
 * - Proper handling of signed and unsigned integers
 * - Float/double support
 * - Efficient implementation using printf where possible
 *
 * Base Values:
 * - THORLOG_DEC (10): Decimal output using %d, %ld, %u, %lu
 * - THORLOG_HEX (16): Hexadecimal output using %x
 * - THORLOG_BIN (2):  Binary output via manual bit conversion
 */
class EspIdfPrint : public ThorPrint {
public:
    /**
     * @brief Default constructor
     */
    EspIdfPrint() = default;

    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~EspIdfPrint() = default;

    // ========================================================================
    // Character and String Output
    // ========================================================================

    /**
     * @brief Print a single character
     * @param c The character to print
     * @return Number of bytes written (1 on success, 0 on failure)
     */
    size_t print(char c) override {
        int result = printf("%c", c);
        return (result > 0) ? static_cast<size_t>(result) : 0;
    }

    /**
     * @brief Print a null-terminated string
     * @param str The string to print
     * @return Number of bytes written
     */
    size_t print(const char* str) override {
        if (str == nullptr) {
            return 0;
        }
        int result = printf("%s", str);
        return (result > 0) ? static_cast<size_t>(result) : 0;
    }

    // ========================================================================
    // Integer Output (signed)
    // ========================================================================

    /**
     * @brief Print a signed integer with specified base
     * @param num The number to print
     * @param base The numeric base (THORLOG_BIN=2, THORLOG_DEC=10, THORLOG_HEX=16)
     * @return Number of bytes written
     *
     * Supported bases:
     * - THORLOG_BIN (2):  Binary (e.g., "10110")
     * - THORLOG_DEC (10): Decimal (e.g., "-42")
     * - THORLOG_HEX (16): Hexadecimal (e.g., "2a")
     */
    size_t print(int num, int base = THORLOG_DEC) override {
        return printSignedLong(static_cast<long>(num), base);
    }

    /**
     * @brief Print a signed long integer with specified base
     * @param num The number to print
     * @param base The numeric base (THORLOG_BIN=2, THORLOG_DEC=10, THORLOG_HEX=16)
     * @return Number of bytes written
     */
    size_t print(long num, int base = THORLOG_DEC) override {
        return printSignedLong(num, base);
    }

    // ========================================================================
    // Integer Output (unsigned)
    // ========================================================================

    /**
     * @brief Print an unsigned integer with specified base
     * @param num The number to print
     * @param base The numeric base (THORLOG_BIN=2, THORLOG_DEC=10, THORLOG_HEX=16)
     * @return Number of bytes written
     */
    size_t print(unsigned int num, int base = THORLOG_DEC) override {
        return printUnsignedLong(static_cast<unsigned long>(num), base);
    }

    /**
     * @brief Print an unsigned long integer with specified base
     * @param num The number to print
     * @param base The numeric base (THORLOG_BIN=2, THORLOG_DEC=10, THORLOG_HEX=16)
     * @return Number of bytes written
     */
    size_t print(unsigned long num, int base = THORLOG_DEC) override {
        return printUnsignedLong(num, base);
    }

    // ========================================================================
    // Floating Point Output
    // ========================================================================

    /**
     * @brief Print a double value
     * @param num The number to print
     * @return Number of bytes written
     *
     * Uses default precision of 2 decimal places.
     */
    size_t print(double num) override {
        int result = printf("%.2f", num);
        return (result > 0) ? static_cast<size_t>(result) : 0;
    }

private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================

    /**
     * @brief Print a signed long with specified base
     * @param num The number to print
     * @param base The numeric base
     * @return Number of bytes written
     *
     * Implementation details:
     * - base 10: Uses printf with %ld format
     * - base 16: Uses printf with %lx format (handles negative as unsigned)
     * - base 2:  Manual bit-by-bit conversion (loops through bits)
     */
    size_t printSignedLong(long num, int base) {
        int result;

        switch (base) {
            case THORLOG_DEC:  // base 10: decimal
                result = printf("%ld", num);
                return (result > 0) ? static_cast<size_t>(result) : 0;

            case THORLOG_HEX:  // base 16: hexadecimal
                // For negative numbers in hex, print the two's complement representation
                if (num < 0) {
                    result = printf("%lx", static_cast<unsigned long>(num));
                } else {
                    result = printf("%lx", num);
                }
                return (result > 0) ? static_cast<size_t>(result) : 0;

            case THORLOG_BIN:  // base 2: binary
                // Manual conversion - loop through bits
                return printBinary(static_cast<unsigned long>(num));

            default:
                // Unsupported base, fall back to decimal
                result = printf("%ld", num);
                return (result > 0) ? static_cast<size_t>(result) : 0;
        }
    }

    /**
     * @brief Print an unsigned long with specified base
     * @param num The number to print
     * @param base The numeric base
     * @return Number of bytes written
     *
     * Implementation details:
     * - base 10: Uses printf with %lu format
     * - base 16: Uses printf with %lx format
     * - base 2:  Manual bit-by-bit conversion (loops through bits)
     */
    size_t printUnsignedLong(unsigned long num, int base) {
        int result;

        switch (base) {
            case THORLOG_DEC:  // base 10: decimal
                result = printf("%lu", num);
                return (result > 0) ? static_cast<size_t>(result) : 0;

            case THORLOG_HEX:  // base 16: hexadecimal
                result = printf("%lx", num);
                return (result > 0) ? static_cast<size_t>(result) : 0;

            case THORLOG_BIN:  // base 2: binary
                // Manual conversion - loop through bits
                return printBinary(num);

            default:
                // Unsupported base, fall back to decimal
                result = printf("%lu", num);
                return (result > 0) ? static_cast<size_t>(result) : 0;
        }
    }

    /**
     * @brief Print a number in binary format
     * @param num The number to print (treated as unsigned)
     * @return Number of bytes written
     *
     * This method manually converts the number to binary by iterating through
     * each bit from MSB to LSB. It skips leading zeros for cleaner output.
     *
     * Algorithm:
     * 1. Handle zero as a special case
     * 2. Build binary string by extracting bits (num & 1)
     * 3. Right-shift to process next bit (num >>= 1)
     * 4. Print result without leading zeros
     */
    size_t printBinary(unsigned long num) {
        // Handle zero specially - just print "0"
        if (num == 0) {
            return print('0');
        }

        // Buffer to hold binary digits
        // Max 64 bits for 64-bit platforms, +1 for null terminator
        char buffer[65];
        int pos = 64;
        buffer[pos] = '\0';

        // Convert to binary by extracting bits from LSB to MSB
        // Each iteration: get lowest bit, then shift right
        while (num > 0 && pos > 0) {
            pos--;
            buffer[pos] = (num & 1) ? '1' : '0';  // Extract LSB
            num >>= 1;                             // Shift to next bit
        }

        // Print from the first significant digit (skip leading zeros)
        return print(&buffer[pos]);
    }
};

// ============================================================================
// Global Instance
// ============================================================================

/**
 * @brief Global EspIdfPrint instance for convenience
 *
 * This provides a ready-to-use print adapter that can be passed to ThorLog.begin().
 * It is declared inline to avoid multiple definition errors when this header
 * is included in multiple translation units.
 *
 * Usage:
 *   ThorLog.begin(THORLOG_LEVEL_VERBOSE, &EspIdfOutput);
 *
 * You can also create your own instances if needed:
 *   EspIdfPrint myOutput;
 *   ThorLog.begin(THORLOG_LEVEL_INFO, &myOutput);
 */
inline EspIdfPrint EspIdfOutput;
