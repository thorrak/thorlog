/*
 * ThorLog ESP-IDF Adapter - Implementation File
 *
 * This file provides the definition of the global ThorLog instance for ESP-IDF.
 * Include this in your build, or define THORLOG_ESPIDF_HEADER_ONLY if you want
 * to use only the header file (requires C++17 or later for inline variables).
 *
 * For most ESP-IDF projects using C++17 or later, you can simply include
 * "thorlog_espidf.h" and the inline definitions will work. This .cpp file
 * is provided for compatibility with older compilers or if you prefer
 * separate compilation.
 *
 * ============================================================================
 * BUILD INTEGRATION:
 * ============================================================================
 *
 * Option 1: Header-only (C++17 or later - recommended for ESP-IDF)
 *   - Just include "thorlog_espidf.h" in your source files
 *   - The inline global instances (ThorLog, EspIdfOutput) will be handled
 *     automatically by the linker
 *
 * Option 2: Separate compilation (C++11/14 or explicit preference)
 *   - Add thorlog.cpp and thorlog_espidf.cpp to your build
 *   - This provides the global instance definitions
 *
 * CMakeLists.txt example for ESP-IDF component:
 *   idf_component_register(
 *       SRCS "main.cpp" "thorlog.cpp" "thorlog_espidf.cpp"
 *       INCLUDE_DIRS "."
 *   )
 *
 * ============================================================================
 */

#include "thorlog_espidf.h"

// The global instances are defined as inline in the headers (C++17),
// so no additional definitions are needed here for modern compilers.
//
// If targeting C++11/14, you would uncomment these lines and remove
// the 'inline' keyword from the header declarations:
//
// ThorLogging ThorLog;
// EspIdfPrint EspIdfOutput;

// Note: If you're using an older compiler without inline variable support,
// you need to:
// 1. Remove 'inline' from the declarations in thorlog.h and thorlog_espidf.h
// 2. Uncomment the definitions above
// 3. Ensure this .cpp file is compiled and linked exactly once
