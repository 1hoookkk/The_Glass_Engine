#pragma once

// This header must be included before any <string>/<format>/<chrono> etc.
// It neutralises accidental project-wide macros that collide with the STL.

#ifdef string
  #undef string
#endif

#ifdef format
  #undef format
#endif

#ifdef min
  #undef min
#endif

#ifdef max
  #undef max
#endif

// If Windows headers are used anywhere, prefer the lean version.
#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif

// Ensure we compile as C++20 (CMake should already set this).
#if __cplusplus < 202002L && !defined(_MSVC_LANG)
  #error "C++20 is required."
#endif
