/// \file Renderer.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Renderer.h - Very simple low-level 3D renderer interface
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __RENDERER_H_INCLUDED__
#define __RENDERER_H_INCLUDED__

#include <d3d9.h>
#include "vector3.h"
#include "EulerAngles.h"
#include "Matrix4x3.h"
#include "Rectangle.h"
#include "Plane.h"

class AABB3;
class VertexBufferBase;
class IndexBuffer;

/////////////////////////////////////////////////////////////////////////////
//
// Simple constants, enums, macros
//
/////////////////////////////////////////////////////////////////////////////

/// Maximum number of chars in a texture name (including the null)
const int kMaxTextureNameChars = 64;

/// Types of ways you can align the text
enum ETextAlignMode
{
  eTextAlignModeLeft,   ///< Align text to the left
  eTextAlignModeRight,  ///< Align text to the right
  eTextAlignModeCenter,  ///< Center text
  eTextAlignModeBottom  ///< Align text to bottom of specified rectangle
};

/// Source blend functions
enum ESourceBlendMode
{
  eSourceBlendModeSrcAlpha, ///< Normal source blending (default)
  eSourceBlendModeOne,		  ///< Use source color unmodified
  eSourceBlendModeZero,		  ///< No source contribution
};

/// Destination blend functions
enum EDestBlendMode
{
  eDestBlendModeInvSrcAlpha, ///< Inverse source alpha blend (default)
  eDestBlendModeOne,         ///< Additive blending
  eDestBlendModeZero,        ///< Discard current frame buffer pixel, blend with "black"
  eDestBlendModeSrcColor,    ///< Multiplicative blend (often used for lightmapping)
};

/// Culling modes
enum EBackfaceMode
{
  eBackfaceModeCCW,     ///< Cull faces with counterclockwise screen-space order (default)
  eBackfaceModeCW,      ///< Cull faces with clockwise screen-space order
  eBackfaceModeDisable  ///< Render all faces, regardless of screenspace vertex order
};

//---------------------------------------------------------------------------
/// \name Clear function options
/// Bitfield of options to the clear() function.
//@{
const int kClearFrameBuffer = 1;      ///< Clear the frame buffer
const int kClearDepthBuffer = 2;      ///< Clear the zbuffer
const int kClearToConstantColor = 4;  ///< Clear frame buffer to constant color. By default, we clear to black
const int kClearToFogColor = 8;       ///< Clear frame buffer to fog color. By default, we clear to black
//@}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// \name Vertex outcodes
/// Bitfield of vertex outcodes. See the computeOutCode() function
//@{
const int kOutCodeLeft = 0x01;          ///< To the left of the screen
const int kOutCodeRight  = 0x02;        ///< To the right of the screen
const int kOutCodeBottom = 0x04;        ///< Below the screen
const int kOutCodeTop = 0x08;           ///< Above the screen
const int kOutCodeNear = 0x10;          ///< Between the near plane and camera
const int kOutCodeFar = 0x20;           ///< Beyond the far plane
const int kOutCodeFog = 0x40;           ///< Completely hidden by fog
const int kOutCodeFrustumMask = 0x3f;   ///< Outside the frustum - don't worry about fog
const int kOutCodeOffScreenMask = 0x1f; ///< Off screen - far or fog don't matter
//@}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// \name Refresh rates
/// Symbolic refresh rates that can be used when setting the video mode
//@{
const int kRefreshRateDefault = -1; ///< Refresh rate equal to the monitor's
const int kRefreshRateFastest = -2; ///< Refresh as fast as possible (don't wait for monitor)
//@}
//---------------------------------------------------------------------------

/// \brief Reserved texture handle
//
/// Special texture handle that is always reserved for the "white texture,"
/// white is a texture that is solid white. This important texture is useful
/// in a wide variety of circumstances
const int kWhiteTexture = 0;

//---------------------------------------------------------------------------
/// \name Color macros
/// Macros to construct a color in 3D-form.
/// \remark This color value is NOT corrected for different pixel formats
/// on different hardware formats.  It is for interface purposes
/// only, NOT for frame buffer access.  The values are packed as follows:\n
/// bits  0 - 7  blue \n
/// bits  8 - 15 green \n
/// bits 16 - 23 red \n
/// bits 24 - 31 alpha \n
/// It is also legal to pass a color like this: 0xAARRGGBB
/// \warning The above rules apply to accessing a color via an integer
/// value only, and have NOTHING to do with accessing the bytes in memory.
//@{
/// \brief Form a color value from red, green, and blue (alpha is undefined)
/// \param r Red value
/// \param g Green value
/// \param b Blue value
#define MAKE_RGB(r, g, b) ((unsigned int)(((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | ((unsigned int)(b))))

/// \brief Form a color value from alpha, red, green, and blue
/// \param a Alpha value
/// \param r Red value
/// \param g Green value
/// \param b Blue value
#define MAKE_ARGB(a, r, g, b) ((unsigned int)(((unsigned int)(a) << 24) | ((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | ((unsigned int)(b))))

/// \brief Extract the alpha value from a color value
/// \param argb Color value
#define GET_A(argb) ((int)(((unsigned int)(argb) >> 24U) & (unsigned int)0xff))

/// \brief Extract the red value from a color value
/// \param argb Color value
#define GET_R(argb) ((int)(((unsigned int)(argb) >> 16U) & (unsigned int)0xff))

/// \brief Extract the green value from a color value
/// \param argb Color value
#define GET_G(argb) ((int)(((unsigned int)(argb) >> 8U) & (unsigned int)0xff))

/// \brief Extract the blue value from a color value
/// \param argb Color value
#define GET_B(argb) ((int)((unsigned int)(argb) & (unsigned int)0xff))
//@}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// \struct VideoMode
/// \brief Defines a video mode
//
/// Defines a video mode - the resolution, color bit depth, and refresh rate.
/// This struct is used when querying for a list of available modes, and
/// also when setting the video mode.
struct VideoMode
{
  int xRes;         ///< Horizontal resolution, in pixels
  int yRes;         ///< Vertical resolution, in pixels
  int bitsPerPixel; ///< Currently only 16, 24, or 32 supported
  int refreshHz;    ///< You can use one of kRefreshRateXxx constants when setting the video mode
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// \name Vertex formats
///
/// These structures are used to pass vertex data to the renderer.
/// Depending on what you want the renderer to do for you, you use a different
/// structure.  For example, if the vertices are in modeling space and
/// need to be transformed and projected to screen space, then you would
/// use an untransformed vertex type.  If you want the renderer to apply
/// lighting calculations, then you would use an unlit vertex type.  If you  
/// want to specify the vertex color manually, then use a pre-lit vertex type.
//@{

/// \struct RenderVertex
/// \brief Untransformed, unlit vertex
struct RenderVertex
{
  Vector3	p;  ///< Position
  Vector3	n;  ///< Normal
  float	u;    ///< Texture mapping coordinate
  float v;    ///< Texture mapping coordinate
  static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

/// \struct RenderVertexL
/// \brief Untransformed, lit vertex
struct RenderVertexL
{
  Vector3 p;      ///< Position
  unsigned argb;  ///< Prelit diffuse color
  float u;        ///< Texture mapping coordinate
  float v;        ///< Texture mapping coordinate
  static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
};

/// \struct RenderVertexTL
/// \brief Transformed and lit vertex
struct RenderVertexTL
{
  Vector3 p;      ///< Screen space position and z value
  float oow;      ///< One Over W. This is used for perspective projection. Usually, you can just use 1/z.
  unsigned argb;  ///< Prelit diffuse color (8 bits per component - 0xAARRGGBB)
  float u;        ///< Texture mapping coordinate
  float v;        ///< Texture mapping coordinate
  static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
};

/// \struct RenderVertex0L
/// \brief Lit vertex with zero textures
struct RenderVertex0L
{
  Vector3 p;
  unsigned argb;
};

//@}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// \struct RenderTri
/// \brief A single triangle for rendering.
//
/// Notice that the indices are unsigned shorts.  This is for two
/// reasons.
/// <ol>
/// <li>Using 16-bit rather than 32-bit indices effectively doubles
/// the memory throughput for the index data.</li>
/// <li>Some graphics cards do not natively support 32-bit index data.</li>
/// </ol>
/// This does put a limit on the max number of vertices in a single
/// mesh at 65536. This is usually not a problem, since most
/// large objects can easily be broken down into multiple meshes - in fact,
/// you probably want to divide things up for visibility, etc, anyway.
struct RenderTri
{
  unsigned short index[3]; ///< Array of vertex indices forming the triangle
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// \struct TextureReference
/// \brief Handy class for keeping track of a texture's name and handle.
struct TextureReference
{
  /// Name of the texture. Usually this is a filename
  char name[kMaxTextureNameChars];

  /// Texture handle, within the graphics system
  int	handle;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// \class Renderer
/// \brief Low-level renderer abstraction layer.
/// \remark See the .cpp file for more comments and opinions.
class Renderer
{
public:
  Renderer(); ///< Basic constructor

  

  /// \name Maintenance
  //@{

  /// \brief Initializes the engine. Must be called once at program startup
  void init(const VideoMode &mode, bool shaderDebug, bool windowed);
  
  /// \brief Shuts down the engine. Must be called once at program shutdown
  void shutdown();

  void beginScene(); ///< Begin the scene

  void endScene();   ///< End the scene

  void flipPages(); ///< Flip video pages and display the back buffer

  void validateDevice(); ///< Restores device if it is lost

  void restoreRenderStates(); ///< Restores render states
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Video mode
  //@{
  
  /// \brief Returns the number of video modes found
  int getVideoModeCount();

  const VideoMode &getVideoMode(int index);
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Save/restore video mode
  /// \brief These are called when our app moves in and out of the foreground,
  /// and should not be called directly
  //@{
  void videoSave();     ///< Save the video mode
  void videoRestore();  ///< Restore the video mode  
  //@}
  //-------------------------------------------------------------------------

  /// \brief Gets number of triangles rendered
  int GetTrianglesRendered();

  /// \brief Gets number of triangles rendered so far this frame
  int GetTrianglesRenderedLastScene();


  //-------------------------------------------------------------------------
  /// \name Camera specifications
  /// \brief Set and access camera data
  //@{

  /// \brief Set the camera's position and orientation
  void setCamera(const Vector3 &pos, const EulerAngles &orient);
  
  /// \brief Get the camera's postion
  /// \return The camera's position
  const Vector3 &getCameraPos() const { return cameraPos; }

  /// \brief Get the camera's orientation
  /// \return The camera's orientation
  const EulerAngles &getCameraOrient() const { return cameraOrient; }

  /// \brief Set the zoom.  A zero zoom value means "compute it for me"
  void setZoom(float xZoom, float yZoom = 0.0f);
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Near/far clipping planes
  /// Set and access near and far clipping plane distances
  //@{

  /// \brief Set the near and far clipping plane distances
  void setNearFarClippingPlanes(float n, float f);

  /// \brief Get the near clipping plane distance
  /// \return The near clipping plane distance
  float getNearClippingPlane() const { return nearClipPlane; }

  /// \brief Get the far clipping plane distance
  /// \return The far clipping plane distance
  float getFarClippingPlane() const { return farClipPlane; }
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Window definition
  /// Set and access the app's window data
  //@{
	
  /// \brief Set the window's location and size
  void	setWindow(int x1, int y1, int xSize, int ySize);

  /// \brief Set window to fullscreen position and size
  void	setFullScreenWindow();

  /// \brief Get the window position and size
  void	getWindow(int *x1, int *y1, int *xSize, int *ySize);

  /// \brief Get the screen width
  /// \return The width of the screen in pixels
  int getScreenX() const { return screenX; }

  /// \brief Get the screen height
  /// \return The height of the screen in pixels
  int getScreenY() const { return screenY; }
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Reference frame
  //@{

  /// \brief Push a reference frame onto the stack 
  void instance(const Vector3 &pos, const EulerAngles &orient);

  /// \brief Push a reference frame onto the stack
  void instance(const Matrix4x3 &m);

  /// \brief Pop the last reference frame off of the stack  
  void instancePop();
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Low-level texture cache maintenance functions
  /// Manages textures
  //@{

  /// \brief Clears the texture cache
  void resetTextureCache();

  /// \brief Finds a texture by name
  int findTexture(const char *name);

  /// \brief Creates a texture  
  int allocTexture(const char *name, int xSize, int ySize, bool renderTarget = false, bool createDepthStencil = false);

  /// \brief Remove the texture from cache  
  void freeTexture(int handle);

  /// \brief Set a texture's image data  
  void setTextureImage(int handle, const unsigned *image);

  /// \brief Cache a texture  
  int cacheTexture(const char *filename, bool defaultDirectory = true);
  
  /// \brief Cache a texture
  int cacheTextureDX(const char *filename, bool defaultDirectory = true);

  /// \brief Slightly simpler texture cache access through the TextureReference class.  
  void	cacheTexture(TextureReference &texture);
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Rendering context management functions
  //@{

  /// \brief Returns true if shader debugging is on
  /// \return True if shader debugging is on
  bool getShaderDebug() {return m_shaderDebug;}

  /// \brief Returns true if the device was created in reference
  bool getDeviceReference() {return m_deviceReference;}

  /// \brief Set the depth buffer mode  
  void setDepthBufferMode(bool readEnabled, bool writeEnabled);

  /// \brief Set the depth buffer mode to default values
  void setDepthBufferMode() { setDepthBufferMode(true, true); }

  /// \brief Get the depth buffer mode for reading
  /// \return True if depth buffer reading is enabled, false otherwise
  bool getDepthBufferRead() const { return depthBufferRead; }

  /// \brief Get the depth buffer mode for writing
  /// \return True if depth buffer writing is enabled, false otherwise
  bool getDepthBufferWrite() const { return depthBufferWrite; }

  /// \brief Set blending enabled  
  void setBlendEnable(bool blend = false);

  /// \brief Get blending enabled
  /// \return True if blending is enabled
  bool getBlendEnable() const { return blendEnable; }

  /// \brief Set the source blend mode  
  void setSourceBlendMode(ESourceBlendMode mode = eSourceBlendModeSrcAlpha);

  /// \brief Get the source blend mode
  /// \return The current mode being used to blend from the source
  ESourceBlendMode getSourceBlendMode() const { return sourceBlendMode; }

  /// \brief Set the destination blend mode  
  void setDestBlendMode(EDestBlendMode mode = eDestBlendModeInvSrcAlpha);

  /// \brief Get the destination blend mode
  /// \return The current mode being used to blend to the destination
  EDestBlendMode getDestBlendMode() const { return destBlendMode; }

  /// \brief Set the color constant (used for 2D primitives)  
  void setRGB(unsigned int rgb);

  /// \brief Get the color constant and global opacity
  /// \return The color constant with the global opacity in the alpha channel
  unsigned getARGB() { return constantARGB; }

  /// \brief Set the color and opacity in one call  
  void setARGB(unsigned argb);

  /// \brief Set the global opacity  
  void setOpacity(float a = 1.0F);

  /// \brief Get the global opacity
  /// \return The opacity
  float getOpacity() const { return constantOpacity; }

  /// \brief Set fog enable  
  void setFogEnable(bool flag = false);

  /// \brief Get fog enabled
  /// \return True if fog is enabled, false otherwise
  bool getFogEnable() { return fogEnable; }

  /// \brief Set the fog color  
  void setFogColor(unsigned rgb);

  /// \brief Get the fog color
  /// \return Color value of the fog
  unsigned getFogColor() const { return fogColor; }

  /// \brief Set the fog's near and far distance
  void setFogDistance(float nearFog, float farFog);

  /// \brief Get the fog's near distance
  /// \return The fog's near distance
  float getFogNear() const { return fogNear; }

  /// \brief Get the fog's far distance
  /// \return The fog's far distance
  float getFogFar() const { return fogFar; }

  /// \brief Set wireframe enabled  
  void setWireframe(bool On);
  
  /// \brief Get wireframe enabled
  /// \return true if wireframe is on
  bool getWireframe() const { return wireframeOn; }

  // Lighting context.  We will assume one single directional light, with ambient

  /// \brief Set the ambient light color  
  void setAmbientLightColor(unsigned rgb);

  /// \brief Get the current ambient light color value
  /// \return The current ambient light color value
  unsigned getAmbientLightColor() const { return ambientLightColor; }

  /// \brief Set the directional light vector
  
  void setDirectionalLightVector(const Vector3 &v);

  /// \brief Get the directional light vector
  /// \return The direction of the directional light
  const Vector3 &getDirectionalLightVector() const { return directionalLightVector; }

  /// \brief Set the color of the directional light  
  void setDirectionalLightColor(unsigned rgb);
  
  /// \brief Get the color of the directional light
  /// \return The color value of the directional light
  unsigned getDirectionalLightColor() const { return directionalLightColor; }

  /// \brief Set lighting enabled  
  void setLightEnable(bool flag = true);

  /// \brief Get lighting enabled
  /// \return True if lighting is enabled, false otherwise
  bool getLightEnable() const { return lightEnable; }

  /// \brief Set z-buffer enabled
  void setZBufferEnable(bool flag = true);

  /// \brief Get z-buffer enabled
  /// \return True if z-buffering is enabled, false otherwise
  bool getZBufferEnable() const { return zEnable; }

  /// \brief Set the backface culling mode
  void setBackfaceMode(EBackfaceMode mode = eBackfaceModeCCW);

  /// \brief Get the backface culling mode
  /// \return The current backface culling mode
  EBackfaceMode getBackfaceMode() const { return backfaceMode; }

  /// \brief Set current diffuse texture using handle  
  void selectTexture(int handle, int stage = 0);

  /// \brief Get the handle of the current diffuse texture
  /// \return Handle to the current diffuse texture
  int getCurrentTexture() const { return currentTextureHandle; }

  /// \brief Set current diffuse texture using a texture reference    
  void selectTexture(TextureReference &texture);

  /// \brief Set texture clamping enabled
  /// \param flag Whether to enable texture clamping
  void setTextureClamp(bool flag = false);

  /// \brief Get texture clamping enabled
  /// \return True if texture clamping is enabled, false otherwise
  bool getTextureClamp() const { return textureClamp; }

  /// \brief Set the rendering target texture
  void setRenderTarget(int textureHandle = -1);
  
  /// \brief Get the current render target texture handle
  /// \return The handle to the current render target texture
	int getRenderTarget() {return renderTargetHandle;}


  /// \brief Sets the clipping plane for the rendering pipeline  
  void setClipPlane(const Plane& plane);

  /// \brief Gets the current clipping plane
  void getClipPlane(Plane* plane);
  
  /// \brief Returns true if the clipping plane is enabled
  /// \return True if the clipping plane is enabled
  bool getClipPlaneEnable() {return clipPlaneEnable;}

  /// \brief Diables the clipping plane
  void disableClipPlane();

  


  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Rendering primitives
  //@{

  /// \brief Clear the frame buffer and/or depth buffer.
  void clear(int options = kClearFrameBuffer | kClearDepthBuffer);

  /// \brief Render a triangle mesh of untransformed, unlit vertices
  void renderTriMesh(const RenderVertex *vertexList, int vertexCount, const RenderTri *triList, int triCount);

  /// \brief Render a triangle mesh of untransformed, lit vertices
  void renderTriMesh(const RenderVertexL *vertexList, int vertexCount, const RenderTri *triList, int triCount);
  
  /// \brief Render a triangle mesh of transformed, lit vertices
  void renderTriMesh(const RenderVertexTL *vertexList, int vertexCount, const RenderTri *triList, int triCount);	

  /// \brief Render a bounding box's edges using the current color.
  void renderBoundingBox(const AABB3 &box);
  
  /// \brief Render indexed geometry from an index buffer and a vertex buffer
  void render(VertexBufferBase *vb, IndexBuffer *ib);

  /// \brief Render indexed geometry from an index buffer and a vertex buffer
  void render(VertexBufferBase *vb, int vertCount, IndexBuffer *ib, int triCount);

  /// \brief Render indexed geometry from an index buffer and a vertex buffer
  void render(VertexBufferBase *vb, int vertStart, int vertCount, IndexBuffer *ib, int triStart, int triCount);
  
  /// \brief Render non-indexed geometry from a vertex buffer
  void render(VertexBufferBase *vb);

  /// \brief Render non-indexed geometry from a vertex buffer
  void render(VertexBufferBase *vb, int vertCount);

  /// \brief Render non-indexed geometry from a vertex buffer
  void render(VertexBufferBase *vb, int vertStart, int vertCount);

  /// \brief Render a sprite in 2D
  void renderSprite(float width, float height);

  /// \brief Render a sprite in 2D
  void renderSprite(RenderVertexTL *VertexList);

  /// \brief Renders a quad accross the entire screen.
  void renderTextureOverScreen();
  
  /// \brief Render a quad in 3D
  void renderQuad(RenderVertexL *quad);

  /// \brief Draw a dot on the screen using the current constant color
  void dot(int x, int y);

  /// \brief Draw a line on the screen using the current constant color
  void line(int x1, int y1, int x2, int y2);

  /// \brief Draw a solid color box using the current constant color
  void boxFill(int x1, int y1, int x2, int y2);
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Text drawing
  //@{
  
  /// \brief Draw text to the render target
  void drawText(const char* text, IRectangle* boundary, ETextAlignMode alignment, bool wrapText, int fontHandle = 0);
  void drawText(const char* text, int x, int y, int fontHandle = 0);

  /// \brief Calculates the height in pixels that drawing the wrapped text requires
  int calculateTextHeight(const char* text, int width, int fontHandle = 0);

  /// \brief Add a font to the font cache array
  int addFont(const char* fontName, int fontWidth, int fontHeight, bool antialiased);

  /// \brief Release all fonts from the cache array
  void freeAllFonts();
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Software vertex transformation functions
  //
  /// These are primarily useful for performing visibility or other manual
  /// manipulations
  //@{

  /// \brief Get the matrix that transforms from world to camera space
  /// \return The matrix that transforms from world to camera space
  const Matrix4x3 &getWorldToCameraMatrix() { return worldToCameraMatrix; }

  /// \brief Get the matrix that transforms from model to camera space  
  const Matrix4x3 &getModelToCameraMatrix();

  /// \brief Get the matrix that transforms from model to world space  
  const Matrix4x3 &getModelToWorldMatrix();

  /// \brief Get a vertex outcode given a point in the current reference space
  int computeOutCode(const Vector3 &p);

  /// \brief Compute outcode and project point onto screen space, if possible  
  int projectPoint(const Vector3 &p, Vector3 *result);
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Timing
  //@{

  /// \brief Get the time elapsed between frames
  //
  /// This function returns the visual timestep, i.e. the amount of time, in
  /// seconds, it took between the last two page flips.
  /// \return Time elapsed between the last two page flips
  /// \remark This is the inverse of the frame rate.  You can use this to
  /// estimate the amount of time the current frame will take to render.
  float	getTimeStep() const { return timeStep; }

  /// \brief Returns the time in milliseconds since the system started
  /// \return Time in milliseconds since the system started.
  long	getTime();
  //@}
  //-------------------------------------------------------------------------

protected:

// Internal state variables

  //to count number of triangles rendered
  int nTriangleCount; 

  //to count number of triangles rendered per frame
  int nTriangleFrameCount;

	// Full screen resolution

	int	screenX;
	int	screenY;

  /// True means that shaders are created for debugging.  If this is true,
  /// m_deviceReference must also be true because you can only debug a shader
  /// that is created in software.
  bool m_shaderDebug;

  /// True means the device has been created in reference
  bool m_deviceReference;

	// Camera specification

	Vector3		cameraPos;
	EulerAngles	cameraOrient;
	float		zoomX;
	float		zoomY;

	// Near/far clipping planes

	float	nearClipPlane;
	float	farClipPlane;

	// The 2D output window

	int	windowX1;
	int	windowY1;
	int	windowX2;
	int	windowY2;
	int	windowSizeX;
	int	windowSizeY;

	// Zbuffer mode

	bool	depthBufferRead;
	bool	depthBufferWrite;

	// Alpha blending

	bool			blendEnable;
	ESourceBlendMode	sourceBlendMode;
	EDestBlendMode		destBlendMode;

	// Global constant color and opacity.

	unsigned	constantARGB;
	float		constantOpacity;

	// Fog

	bool		fogEnable;
	unsigned 	fogColor;
	float		fogNear;
	float		fogFar;

	// Wireframe
	bool		wireframeOn;

	// Z-Buffer
	bool zEnable;

	// Lighting context.

	bool		lightEnable;
	unsigned	ambientLightColor;
	Vector3		directionalLightVector;
	unsigned	directionalLightColor;

	// Culling

	EBackfaceMode	backfaceMode;

	// Currently selected texture

	int	currentTextureHandle;

	// Texture clamp

	bool	textureClamp;

	// Render Target
	// handle to the current rendering target.
	// -1 means the backbuffer
	int renderTargetHandle;
	
  // clipping plane
  bool clipPlaneEnable; ///< True if a clipping plane is enabled
  Plane clipPlane; ///< The clipping plane if there is one

	// Current visual timestep, in seconds

	float	timeStep;

	// Current world->camera matrix.  This will always be a rigid body
	// transform - it does not contain zoom or aspect ratio correction.

	Matrix4x3	worldToCameraMatrix;

// Implementation details

	void	updateModelToWorldMatrix();
	void	computeClipMatrix();
	void	getModelToClipMatrix();
	void	freeAllTextures();
};

// The global class object

extern Renderer	gRenderer;

// The global device, used to extend Renderer (defined in Renderer.cpp)

extern LPDIRECT3DDEVICE9 pD3DDevice;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __RENDERER_H_INCLUDED__

