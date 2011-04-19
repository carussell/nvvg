// Terrain.fx

/*
		  This effect file supports a vertex shader and a pixel shader.
		  The pixel shader blends 6 textures together using weights specfied in the vertex.		  
		  The vertex shader supports ambient lighting, directional lighting, and a
		  world to projection transformation.
		  
		  
*/

// Variables that can be set from the C#/C++/Coding side
// These are global variables and can be accessed in both the vertex 
// shader and the pixel shader

// Transformations
float4x4 WorldViewProj : WORLDVIEWPROJ;
float4x4 World;

// Camera position
float4 CameraPosition;

// Fog variables
float FogEnd;
float FogConstant;

// lighting variables
float4 NegativeLightDirection;
float4 LightDirectionColor;
float4 AmbientLight;

// texture stretching
float textureStretch1;
float textureStretch2;
float textureStretch3;
float textureStretch4;
float textureStretch5;
float textureStretch6;

// six textures used
texture texture1;
texture texture2;
texture texture3;
texture texture4;
texture texture5;
texture texture6;

//// Output from the vertex shader to a Pixel Shader ///
struct VS_OUTPUT
{
    float4 Pos  : POSITION;			// position of vertex
    float4 Diffuse : COLOR0;		// color vertex needs to be (after lighting is computed)
    float4 Weights : COLOR1;		// Texture weights 1 through 4
    float2 TexCoord1 : TEXCOORD0;	// texture coordinates after they have been stretched
    float2 TexCoord2 : TEXCOORD1;
    float2 TexCoord3 : TEXCOORD2;
    float2 TexCoord4 : TEXCOORD3;
    float2 TexCoord5 : TEXCOORD4;
    float2 TexCoord6 : TEXCOORD5;
    float2 TexCoord7 : TEXCOORD6;	// texture weights 5 and 6
    float1 Fog : FOG;
};

float calculateFog(float3 pos)
{
	float4 transformed = mul(float4( pos.x, pos.y, pos.z, 1), World);
	float dist = distance(transformed, CameraPosition);
	
	// FogConstant is:    1 / (FogEnd - FogStart)
	// FogConstant is used to avoid a division
	return (FogEnd - dist) * FogConstant;
}

VS_OUTPUT VS(
    float3 Pos  : POSITION,        // position of vertex
    float3 Norm : NORMAL,          // Normal of vertex (for lighting)
    float2 TexCoord : TEXCOORD0,   // Texture coordinates    
    float4 Weights1 : COLOR,	   // Texture weights 1 - 4
    float4 Weights2 : COLOR1	   // Texture weights 5 - 6 and 7 is the alpha channel
    )
{
	// create a structure for the output   
    VS_OUTPUT Out = (VS_OUTPUT)0; 

	Out.Fog = calculateFog(Pos);

	// scale and pass each texture coordinate to the pixel shader
	Out.TexCoord1 = TexCoord * textureStretch1;
	Out.TexCoord2 = TexCoord * textureStretch2;
	Out.TexCoord3 = TexCoord * textureStretch3;
	Out.TexCoord4 = TexCoord * textureStretch4;	
	Out.TexCoord5 = TexCoord * textureStretch5;
	Out.TexCoord6 = TexCoord * textureStretch6;	
	
	// pass texture blending weights
	Out.Weights = Weights1; 
	Out.TexCoord7.x = Weights2.a;	
	Out.TexCoord7.y = Weights2.r;	
   
    // Transform the position to projection space
    Out.Pos  = mul(float4( Pos.x, Pos.y, Pos.z, 1), WorldViewProj);
          
    // calculate direction lighting 
    Out.Diffuse = clamp ( dot(NegativeLightDirection, Norm),0,1 ) * LightDirectionColor;
            
    // Toss in ambient lighting
    Out.Diffuse += AmbientLight;
    Out.Diffuse.a = Weights2.g;
   
	// All information is filled in so return it all
    return Out;
}



// There are 6 different samplers, One for each texture
// The all have filtering turned on so it looks good

sampler Sampler1 = sampler_state
{
    Texture = (texture1);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Sampler2 = sampler_state
{
    Texture = (texture2);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Sampler3 = sampler_state
{
    Texture = (texture3);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Sampler4 = sampler_state
{
    Texture = (texture4);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Sampler5 = sampler_state
{
    Texture = (texture5);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Sampler6 = sampler_state
{
    Texture = (texture6);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


// Pixel Shader
// All parameters passed in here came from the vertex shader.

float4 PS(    
    float2 Tex1 : TEXCOORD0,	// Texture coordinates 
    float2 Tex2 : TEXCOORD1, 
    float2 Tex3 : TEXCOORD2, 
    float2 Tex4 : TEXCOORD3, 
    float2 Tex5 : TEXCOORD4, 
    float2 Tex6 : TEXCOORD5,
    float2 Weights2 : TEXCOORD6,// Texture blending weights 5 and 6
    float4 Diff1 : COLOR0,		// Color of vertex
    float4 Weights : COLOR1		// Texture blending weights 1 - 4
    ) : COLOR // specify we will be returning a color
{

	// add all textures multiplied by their corresponding weights together
	float4 r =	
		tex2D(Sampler1, Tex1) * Weights.a +
		tex2D(Sampler2, Tex2) * Weights.r +
		tex2D(Sampler3, Tex3) * Weights.g +
		tex2D(Sampler4, Tex4) * Weights.b +		
		tex2D(Sampler5, Tex5) * Weights2.x +
		tex2D(Sampler6, Tex6) * Weights2.y;
	
	// multiply by color computed in vertex shader
	r *= Diff1;		

	return r;
}




// texture technique
// When this technique is set in code, 
// VS is selected for the vertex shader and PS is selected for the pixel shader.
// render states can also be set here.
technique Terrain
{
	// pass 0.  We only have one pass anyway.
    pass P0
    {
		// turn the alpha blending renderstate off
		AlphaBlendEnable = true;
		ZWriteEnable = true;
		ZEnable = true;
		
		// turn off directX fog
		FogTableMode = NONE;
        
        // Shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }  
}
