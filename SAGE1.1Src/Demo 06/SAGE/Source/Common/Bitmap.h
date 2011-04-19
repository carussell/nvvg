/// \file Bitmap.h
/// \brief Interface for the Bitmap class.

/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Bitmap.h - Simple bitmap loader
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __BITMAP_H_INCLUDED__
#define __BITMAP_H_INCLUDED__

/////////////////////////////////////////////////////////////////////////////
//
// class Bitmap - a simple class to hold a bitmap image
//
/////////////////////////////////////////////////////////////////////////////

/// \brief Represents a bitmap image
class Bitmap {
public:

	// An enum to describe the format of the image

	/// \brief Describes the format of the image
	/// \remarks For now, only 32-bit ARGB bitmaps are supported.
	enum EFormat {
		eFormat_None,	///< Dummy value
		eFormat_8888,	///< 32-bit ARGB

		// !KLUDGE! FOr now, this is all we'll support.

	};

	// Constructor/destructor

	Bitmap();
	~Bitmap();

	// Memory management

	void	allocateMemory(int xs, int ys, EFormat format); ///< Allocates the memory for a bitmap
	void	freeMemory(); ///< Frees the memory associated with the bitmap, invalidating it

	// Accessors

	/// \brief Queries the bitmap for its width.
	/// \return The bitmap's width
	int	xSize() const { return sizeX; }

	/// \brief Queries the bitmap for its height.
	/// \return The bitmap's height
	int	ySize() const { return sizeY; }

	/// \brief Queries the bitmap for its format.
	/// \return The bitmap's format
	EFormat	format() const { return fmt; }

	/// \brief Queries the bitmap for its raw data.
	/// \return A pointer to the bitmap's raw data.
	void	*rawData() const { return data; }

	// Access to the pixel data.  Colors are specified using
	// the standard 0xAARRGGBB format used by class Renderer

	unsigned	getPix(int x, int y) const; ///< Queries the bitmap for the color at a given set of coordinates.
	void		setPix(int x, int y, unsigned argb); ///<Sets the color of the pixel at the given set of coordinates.
	//SECURITY-UPDATE:2/3/07
	//bool	load(const char *filename, char *returnErrMsg); ///< \brief Loads a bitmap from an image file based on file extension.
	bool	load(const char *filename, char *returnErrMsg, size_t errMsgSize); ///< \brief Loads a bitmap from an image file based on file extension.
	//SECURITY-UPDATE:2/3/07
	//bool	loadTGA(const char *filename, char *returnErrMsg); ///< Loads a bitmap from an Targe image file (.TGA).
	bool	loadTGA(const char *filename, char *returnErrMsg, size_t errMsgSize); ///< Loads a bitmap from an Targe image file (.TGA).
	//SECURITY-UPDATE:2/3/07
	//bool	loadBMP(const char *filename, char *returnErrMsg); ///< Loads a bitmap from a Windows bitmap file (.BMP).
	bool	loadBMP(const char *filename, char *returnErrMsg,size_t errMsgSize); ///< Loads a bitmap from a Windows bitmap file (.BMP).

private:

	int	sizeX;   ///< Specifies the width of the bitmap.
	int	sizeY;   ///< Specifies the height of the bitmap.
	EFormat	fmt; ///< Specifies the format of the bitmap.
	void	*data; ///< Points to the bitmap's raw data.

};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __BITMAP_H_INCLUDED__

