/// \file CommonStuff.h
/// \brief Contains some useful, misc functions such as the ABORT
/// macro and some conversion methods.

//
// 3D Math Primer for Games and Graphics Development
//
// CommonStuff.h - Miscelaneous declarations that don't belong elsewhere.
//
// Visit gamemath.com for the latest version of this file.
//

#ifndef __COMMONSTUFF_H_INCLUDED__
#define __COMMONSTUFF_H_INCLUDED__

#include <string>
#include "EulerAngles.h"
#include "MathUtil.h"
#include "Vector3.h"

// Debugger stuff

bool areWeBeingDebugged();  ///< Detects whether the Windows debugger is running.

// Declare a function to terminate the program with a printf-like
// formatted error message
void reallyAbort(const char *fmt, ...);  ///< Aborts the program.  Called by ABORT()

extern const char *abortSourceFile;  ///< Contains the name of the source file that called ABORT()
extern int abortSourceLine;  ///< Contains the source line number of a call to ABORT()

/// \brief Aborts the program, automatically writing source file and line information
///     to the debugger.
///
/// This nasty looking macro
/// is a handy little trick to allow our macro appear to to take a variable
/// number of argmuents
#define ABORT (abortSourceFile = __FILE__, abortSourceLine = __LINE__, reallyAbort)

// Common utility functions

/// \brief Converts a string into a Vector3
/// The string must be in the form "x,y,z", where x, y, and z are floats.
/// If the string is not valid, the zero vector is returned.
/// \param strvec Source string to convert to a Vector3 value
/// \return The converted Vector3 value if successful, zero vector otherwise
Vector3 atovec3(const char* strvec);

/// \brief Converts a string into Euler angles
/// The string must be in the form "h,p,b", where h, p, and b are floats.
/// \param strvec Source string to convert to a Vector3 value
/// \param useDegrees Specifies whether to interpret angle values in degrees.
///     If false, interpets angles as radians.  If true, interprets angles
///     as degrees and automatically converts them to radians.
/// \return The converted Vector3 value if successful, identity angles otherwise
EulerAngles atoeuler(const char* strvec, bool useDegrees = false);

/// \brief Converts a string into a color value
/// Converts a string to a color value. The string must in the format
/// \"<b>r,g,b</b>\" or \"<b>a,r,g,b</b>\" where \b a, \b r, \b g, and
/// \b b are integers between 0 and 255. Each channel (a,r,g,b) will be
/// represented by 8 bits in the 32 bit return value (0xAARRGGBB)
/// \remark If only three channels are found, the function will assume that
/// the given channels represent the rgb value of the color, and will assign
/// 255 to the alpha channel.
/// \param strcolor String color value
/// \return Color value
unsigned int atocolor(const char* strcolor);

/// \brief Returns (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)
/// \return The distance squared of points (x1,y1) and (x2,y2)
int distSquared(int x1, int y1, int x2, int y2);

// Standard min and max functions (note: not needed thanks to std::min(), std::max())
template <class Type>
inline const Type &(min)(const Type &a, const Type &b) {
	return (a < b) ? a : b;
}

template <class Type>
inline const Type &(max)(const Type &a, const Type &b) {
	return (a > b) ? a : b;
}

// Standard template swap routine (note: not needed thanks to std::swap())
template <class Type>
inline void swap(Type &a, Type &b) {
	Type tmp(a);
	a = b;
	b = tmp;
}

#endif // #ifndef __COMMONSTUFF_H_INCLUDED__
