/// \file CommonStuff.cpp
/// \brief Contains definitions for some of the useful functions
/// found in CommonStuff.h.

//
// 3D Math Primer for Games and Graphics Development
//
// CommonStuff.cpp - Miscelaneous code that doesn't belong elsewhere.
//
// Visit gamemath.com for the latest version of this file.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "CommonStuff.h"
#include "WindowsWrapper/WindowsWrapper.h"
#include "common/renderer.h"

#ifdef WIN32
	#include <windows.h>
#endif

const char	*abortSourceFile = "(unknown)";
int		abortSourceLine;

// globals for handling abort error message reporting
char g_errMsg[1024]; //error message if we had to abort
bool g_errorExit=false; // true if we had to abort

// global code

/// \return true iff the Windows debugger is running.
bool	areWeBeingDebugged() {
	#ifdef WIN32

		// This routine is in kernel32.  Acquire access to the
		// DLL

		HINSTANCE h = LoadLibrary("kernel32.dll");
		if (h == NULL) {
			return false;
		}

		// Fetch the code

		typedef BOOL (*PFunc)(VOID);

		PFunc func = (PFunc)GetProcAddress(h, "IsDebuggerPresent");
		if (func == NULL) {
			return false;
		}

		// Actually call the routine

		bool result = (func() != FALSE);

		// Don't hang on to any resources we don't need

		FreeLibrary(h);

		// Return the result

		return result;
	#else

		// For now, let's assume we are being debugged
		// in a debug build, and otherwise we're not being
		// debugged

		#ifdef _DEBUG
			return true;
		#else
			return false;
		#endif

	#endif
}

/// \param fmt Contains a <tt>printf</tt>-style format string to write to the debugger.
/// \note This function is not meant to be called manually.  Instead, use the
///     macro ABORT().
/// \note This function never returns; it calls ExitProcess() or exit in Windows
///     and non-Windows environments, respectively.
void	reallyAbort(const char *fmt, ...) {

	// Format the error message into our buffer

	va_list ap;
	va_start(ap, fmt);
	//SECURITY-UPDATE:2/3/07
	//vsprintf(g_errMsg, fmt, ap);
	vsprintf_s(g_errMsg,sizeof(g_errMsg), fmt, ap);
	va_end(ap);

	// Tack on the source file and line number

	//SECURITY-UPDATE:2/3/07
	//sprintf(strchr(g_errMsg, '\0'), "\n%s line %d", abortSourceFile, abortSourceLine);
	sprintf_s(strchr(g_errMsg, '\0'),sizeof(g_errMsg)-strlen(g_errMsg), "\n%s line %d", abortSourceFile, abortSourceLine);

	// Shutdown renderer so we can see the error message

	gRenderer.shutdown();

	// Windows?  Dump message box

	#ifdef WIN32

		// Check if we're under the debugger

		if (areWeBeingDebugged()) {

			// Dump error message to the debug console

			OutputDebugString("FATAL ERROR: ");
			OutputDebugString(g_errMsg);
			OutputDebugString("\n");

			// Break the program, so we can check out what was going on.

			_asm {

				// HELLO! If you hit this breakpoint, then look at the debug
				// console, and go up the call stack to see what went wrong!

				int 3;
			}

			// You can try to keep running, if you want...

		} else {

			// Just dump a message box and terminate the app
			g_errorExit=true; // flag that there's been an error
			DestroyWindow(gWindowsWrapper.getHandle()); // post quit message so we can show dialog box last
			ExitProcess(1); // so we don't proceed with whatever caused this abort
		}
	#else

		// Just dump it to printf and use exit.  On most OSs,
		// this is basically useless for debugging, so you'd
		// want to do better, especially under the debugger

		printf("FATAL ERROR: %s\n", errMsg);
		exit(1);

	#endif
}

Vector3 atovec3(const char* strvec)
{
  float x = 0.0f, y = 0.0f, z = 0.0f;

  //SECURITY-UPDATE:2/3/07
  //if(sscanf(strvec, "%f,%f,%f", &x, &y, &z) != 3)
  if(sscanf_s(strvec, "%f,%f,%f", &x, &y, &z) != 3)
  {
    // string wasn't in proper format, so return zero vector
    return Vector3::kZeroVector;
  }

  return Vector3(x, y, z);
}

EulerAngles atoeuler(const char* strvec, bool useDegrees)
{
  float h = 0.0f, p = 0.0f, b = 0.0f;

  //SECURITY-UPDATE:2/3/07
  //if(sscanf(strvec, "%f,%f,%f", &h, &p, &b) != 3)
  if(sscanf_s(strvec, "%f,%f,%f", &h, &p, &b) != 3)
  {
    // string wasn't in proper format, so return zero vector
    return EulerAngles::kEulerAnglesIdentity;
  }
  return EulerAngles(degToRad(h), degToRad(p), degToRad(b));
}

unsigned int atocolor(const char* strcolor)
{
  int
    a = 0,
    r = 0,
    g = 0,
    b = 0;

  //SECURITY-UPDATE:2/3/07
  //int nInputs = sscanf(strcolor, "%i,%i,%i,%i", &a, &r, &g, &b);
  int nInputs = sscanf_s(strcolor, "%i,%i,%i,%i", &a, &r, &g, &b);

  if(nInputs == 3) // assume three values to mean rgb given
    return ((unsigned int)
      (((unsigned int)(255) << 24) |
        ((unsigned int)(a) << 16)   |
        ((unsigned int)(r) << 8)    |
        ((unsigned int)(g))));
  else if (nInputs == 4) // argb given
    return ((unsigned int)
      (((unsigned int)(a) << 24) |
        ((unsigned int)(r) << 16) |
        ((unsigned int)(g) << 8)  |
        ((unsigned int)(b))));
  else
    return 0;
}

int distSquared(int x1, int y1, int x2, int y2)
{
  int dx = x1-x2, dy = y1-y2;
  return dx*dx + dy*dy;
}
