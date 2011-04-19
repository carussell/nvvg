/// \file Rectangle.h
/// Simple declaration of an integer rectangle struct and a float rectangle

#ifndef __H_RECTANGLE_INCLUDED__
#define __H_RECTANGLE_INCLUDED__

/// \brief Defines a rectangle using floats.
struct FRectangle
{
  FRectangle() { left = top = bottom = right = 0.0f; }
  
  /// \brief Constructs a rectangle.
  /// \param left Specifies the x-coordinate of the left side of the rectangle.
  /// \param top Specifies the y-coordinate of the top of the rectangle.
  /// \param right Specifies the x-coordinate of the right side of the rectangle.
  /// \param bottom Specifies the y-coordinate of the bottom of the rectangle.
  FRectangle(float left, float top, float right, float bottom)
  {
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;

  }
	
	float left;	///< left of rectangle (x1)
	float top;	///< top of rectangle (y1)
	float right;	///< right of rectangle (x2)
	float bottom;	///< bottom of rectangle (y2)
};

/// \brief Defines a rectangle using integers.
struct IRectangle
{	
  IRectangle() { left = top = bottom = right = 0; }  ///< Constructs the identity rectangle (all zeros).

  /// \brief Constructs a rectangle.
  /// \param left Specifies the x-coordinate of the left side of the rectangle.
  /// \param top Specifies the y-coordinate of the top of the rectangle.
  /// \param right Specifies the x-coordinate of the right side of the rectangle.
  /// \param bottom Specifies the y-coordinate of the bottom of the rectangle.
  IRectangle(int left, int top, int right, int bottom)
  {
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;

  }
	int left;	///< left of rectangle (x1)
	int top;	///< top of rectangle (y1)
	int right;	///< right of rectangle (x2)
	int bottom;	///< bottom of rectangle (y2)
};

#endif