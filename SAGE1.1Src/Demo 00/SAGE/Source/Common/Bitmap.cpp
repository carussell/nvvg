#pragma warning(disable : 4996)
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Bitmap.cpp - Simple bitmap loader
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Bitmap.h"
#include "CommonStuff.h"
#include "common/renderer.h"

/////////////////////////////////////////////////////////////////////////////
//
// Local stuff
//
/////////////////////////////////////////////////////////////////////////////

// File header for .TGA file

#pragma pack(1)

struct TGAHeader {
	unsigned char	imageIDLength;
	unsigned char	colorMapType;
	unsigned char	imageType;
	unsigned short	colorMapFirstIndex;
	unsigned short	colorMapLength;
	unsigned char	colorMapBitsPerEntry;
	unsigned short	xOrigin, yOrigin;
	unsigned short	width, height;
	unsigned char	bitsPerPixel;
	unsigned char	imageDescriptor;
};

#pragma pack()

/////////////////////////////////////////////////////////////////////////////
//
// class Bitmap member functions
//
/////////////////////////////////////////////////////////////////////////////

/// \brief Constructs an empty bitmap.
Bitmap::Bitmap() {
	sizeX = 0;
	sizeY = 0;
	fmt = eFormat_None;
	data = NULL;
}

/// \brief Destructs the bitmap, freeing any allocated resources.
Bitmap::~Bitmap() {
	freeMemory();
}

/// \param xs Specifies the bitmap's width, in pixels
/// \param ys Specifies the bitmap's height, in pixels
/// \param format Specifies the desired format of the bitmap
void	Bitmap::allocateMemory(int xs, int ys, EFormat format) {

	// Sanity check

	assert(xs > 0);
	assert(ys > 0);

	// First, free up anything already allocated

	freeMemory();

	// Figure out how big one row is, in bytes

	int	rowBytes;
	switch (format) {
		case eFormat_8888:
			rowBytes = xs*4;
			break;

		default:
			assert(false); // bogus pixel format
	}

	// Allocate memory

	data = malloc(ys * rowBytes);
	if (data == NULL) {
		ABORT("Out of memory for bitmap");
	}

	// Remember dimensions

	sizeX = xs;
	sizeY = ys;
	fmt = format;
}

void	Bitmap::freeMemory() {

	// Free memory, if some was allocated

	if (data != NULL) {
		free(data);
		data = NULL;
	}

	// Reset

	sizeX = 0;
	sizeY = 0;
	fmt = eFormat_None;
}

/// \param x Specifies the x-coordinate.
/// \param y Specifies the y-coordinate.
/// \return The color of the pixel in 0xAARRGGBB format.
unsigned	Bitmap::getPix(int x, int y) const {

	// Safety check

	if ((x < 0) || (y < 0) || (x >= sizeX) || (y >= sizeY) || (data == NULL)) {
		assert(false);
		return 0;
	}

	// Check format

	unsigned result;
	switch (fmt) {
		case eFormat_8888:
			result = ((unsigned *)data)[y*sizeX + x];
			break;

		default:
			assert(false);
			result = 0;
			break;
	}

	// Return it

	return result;
}

/// \param x Specifies the x-coordinate.
/// \param y Specifies the y-coordinate.
/// \param argb Specifies the color in 0xAARRGGBB format.  Use MAKE_ARGB
/// to construct colors of this type.
void	Bitmap::setPix(int x, int y, unsigned argb) {

	// Safety check

	if ((x < 0) || (y < 0) || (x >= sizeX) || (y >= sizeY) || (data == NULL)) {
		assert(false);
		return;
	}

	// Check format

	switch (fmt) {
		case eFormat_8888:
			((unsigned *)data)[y*sizeX + x] = argb;
			break;

		default:
			assert(false);
			break;
	}
}

/// \param filename Specifies the name of the file.
/// \param returnErrMsg Must point to a valid character buffer to be filled
///     with an error message in the event of failure.
/// \return true iff successful, else false.
/// \warning This function is currently unsafe, as there is no guarantee that
///     the given error message buffer will be large enough to accomodate the
///     error message.  Recommend changing to use string instead.
//SECURITY-UPDATE:2/4/07
//bool	Bitmap::load(const char *filename, char *returnErrMsg) {
bool	Bitmap::load(const char *filename, char *returnErrMsg,size_t errMsgSize) {

	// Free up anything already allocated

	freeMemory();

	// Fetch extension.  I wish I could use the _splitpath function,
	// but it's not cross-platform.  I'll parse the thing myself.

	const char *ext = strchr(filename, '.');
	if (ext == NULL) {
		//SECURITY-UPDATE:2/4/07
		//strcpy(returnErrMsg, "Filename has no extension");
		strcpy_s(returnErrMsg,errMsgSize, "Filename has no extension");
		return false;
	}
	for (;;) {
		const char *dot = strchr(ext+1, '.');
		if (dot == NULL) {
			break;
		}
		ext = dot;
	}

	// Check for known extensions

	if (stricmp(ext, ".tga") == 0) {
		//SECURITY-UPDATE:2/4/07
		//return loadTGA(filename, returnErrMsg);
		return loadTGA(filename, returnErrMsg,errMsgSize);
	}
	if (stricmp(ext, ".bmp") == 0) {
		//SECURITY-UPDATE:2/4/07
		//return loadBMP(filename, returnErrMsg);
		return loadBMP(filename, returnErrMsg,errMsgSize);
	}

	// Unknown extension
	//SECURITY-UPDATE:2/4/07
	//sprintf(returnErrMsg, "Unknown/unsupported file extension '%s'", ext);
	sprintf_s(returnErrMsg,errMsgSize, "Unknown/unsupported file extension '%s'", ext);
	return false;
}

/// \param filename Specifies the name of the file.
/// \param returnErrMsg Must point to a valid character buffer to be filled
///     with an error message in the event of failure.
/// \return true iff successful, else false.
/// \warning This function is currently unsafe, as there is no guarantee that
///     the given error message buffer will be large enough to accomodate the
///     error message.  Recommend changing to use string instead.
//SECURITY-UPDATE:2/4/07
//bool	Bitmap::loadTGA(const char *filename, char *returnErrMsg) {
bool	Bitmap::loadTGA(const char *filename, char *returnErrMsg,size_t errMsgSize) {

	// Open the file
	//SECURITY-UPDATE:2/4/07
	//FILE *f = fopen(filename, "rb");
	//if (f == NULL) {
	FILE *f=NULL;
	errno_t err;
	err=fopen_s(&f,filename,"rb");
	if(f==NULL || err!=0) {
		//SECURITY-UPDATE:2/4/07
		//strcpy(returnErrMsg, "Can't open file.");
		strcpy_s(returnErrMsg,errMsgSize, "Can't open file.");
failed:

		// Cleanup

		freeMemory();
		if (f != NULL) {
			fclose(f);
		}

		// Report failure

		return false;
	}

	// Read TGA header

	TGAHeader	head;
	if (fread(&head, sizeof(head), 1, f) != 1) {
ioError:
		//SECURITY-UPDATE:2/4/07
		//strcpy(returnErrMsg, "I/O error, or file is corrupt.");
		strcpy_s(returnErrMsg,errMsgSize, "I/O error, or file is corrupt.");
		goto failed;
	}

	// Check format

	if (head.imageType == 2) { // UNCOMPRESSED_TRUECOLOR
		if ((head.bitsPerPixel != 24) && (head.bitsPerPixel != 32)) {
			//SECURITY-UPDATE:2/4/07
			//sprintf(returnErrMsg, "%d-bit truecolor image not supported", (int)head.bitsPerPixel);
			sprintf_s(returnErrMsg,errMsgSize, "%d-bit truecolor image not supported", (int)head.bitsPerPixel);
			goto failed;
		}
		if (head.colorMapType != 0) {
			//SECURITY-UPDATE:2/4/07
			//strcpy(returnErrMsg, "Truecolor image with colormap not supported");
			strcpy_s(returnErrMsg,errMsgSize, "Truecolor image with colormap not supported");
			goto failed;
		}
	//} else if (head.imageType == 1) { // UNCOMPRESSED_COLORMAPPED
	//	if (
	//		(head.colorMapType != 1) ||
	//		(head.bitsPerPixel != 8) ||
	//		(head.colorMapFirstIndex != 0) ||
	//		(head.colorMapLength != 256) ||
	//		(head.colorMapBitsPerEntry != 24)
	//	) {
	//		strcpy(returnErrMsg, "Invalid colormapped image file format");
	//		return 0;
	//	}
	} else {
		//SECURITY-UPDATE:2/4/07
		//sprintf(returnErrMsg, ".TGA image type %d not supported", (int)head.imageType);
		sprintf_s(returnErrMsg,errMsgSize, ".TGA image type %d not supported", (int)head.imageType);
		goto failed;
	}

	// Check origin

	assert(!(head.imageDescriptor & 0x10)); // x origin at the right not supported

	// Allocate image of the correct size

	allocateMemory(head.width, head.height, eFormat_8888);

	// Read the image data, in file order

	int	rowSz = head.bitsPerPixel / 8 * sizeX;
	for (int y = 0 ; y < sizeY ; ++y) {

		// Figure out which row this is in the image.
		// TGA's can be stored "upside down"

		int	dy;
		if (head.imageDescriptor & 0x20) {
			dy = y;
		} else {
			dy = sizeY - y - 1;
		}

		// Get shortcut pointer to first pixel in the row

		unsigned *destPtr = (unsigned *)data + dy*sizeX;

		// Read in the data for this row

		for (int x = 0 ; x < sizeX ; ++x) {
			int b = fgetc(f);
			int g = fgetc(f);
			int r = fgetc(f);
			int a;
			if (head.bitsPerPixel == 24) {
				a = 255;
			} else {
				a = fgetc(f);
			}
			if (b < 0 || g < 0 || r < 0 || a < 0) {
				goto ioError;
			}
			*destPtr = MAKE_ARGB(a, r, g, b);
			++destPtr;
		}
	}

	// OK

	fclose(f);
	return true;
}

/// \param filename Specifies the name of the file.
/// \param returnErrMsg Must point to a valid character buffer to be filled
///     with an error message in the event of failure.
/// \return true iff successful, else false.
/// \warning This function is currently unsafe, as there is no guarantee that
///     the given error message buffer will be large enough to accomodate the
///     error message.  Recommend changing to use string instead.
bool	Bitmap::loadBMP(const char *filename, char *returnErrMsg,size_t errMsgSize) {

	// Free up anything already allocated

	freeMemory();

	// !FIXME!

	assert(false);
	//SECURITY-UPDATE:2/4/07
	//strcpy_s(returnErrMsg,errMsgSize, "BMP support not implemented yet");
	return false;
}
