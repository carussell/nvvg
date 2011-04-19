// Water.fx

// Variables that can be set from the C#/C++/Coding side
// Transformations
float4x4 worldViewProj : worldViewProj;
float4x4 world;
float4x4 posToReflectTextureMatrix;

// Camera position
float4 cameraPosition;

// Fog variables
float FogEnd;
float FogConstant;
float4 FogColor;

// Screen dimensions
float screenWidth;
float screenHeight;

// texture translating
float2 textureTrans;
float textureScaleInverse;
float4 diffuseColor;
float3 waterNormal = float3(0.0f,1.0f,0.0f);

bool reflectionOn;

// lighting variables
float4 negativeLightDirection;
float4 lightDirectionColor;
float4 ambientLight;

texture textureWater;
texture textureReflection;
texture textureDistortionMap;

// Output from the vertex shader to a Pixel Shader
struct VS_OUTPUT
{
  float4 Pos  : POSITION;
  float4 Diff : COLOR0;    
  float2 TexCoord1 : TEXCOORD0;    
  float2 TexCoord2 : TEXCOORD1;  
  float2 Fog : TEXCOORD2;	// we will use the TEXCOORD1 semantic to pass the fog
							// intensity
};

float calculateFog(float3 pos)
{
	float4 transformed = mul(float4( pos.x, pos.y, pos.z, 1), world);
	float dist = distance(transformed, cameraPosition);
	
	// FogConstant is:    1 / (FogEnd - FogStart)
	// FogConstant is used to avoid a division
	return clamp(0.0f,1.0f,(FogEnd - dist) * FogConstant);
}


// This is the vertex shader.  Every vertex goes through this function to be
// processed.
VS_OUTPUT VS( float3 Pos : POSITION )
{
  VS_OUTPUT Out = (VS_OUTPUT)0; // create a structure for the output   	
  float4 worldPosition;
  float4 temp;

  // calculate vertex position in world space
  worldPosition = mul(float4( Pos.x, Pos.y, Pos.z, 1), world);
  
  // calculate texture coordinates based on world space position
  Out.TexCoord1.x = worldPosition.x * textureScaleInverse + textureTrans.x;
  Out.TexCoord1.y = worldPosition.z * textureScaleInverse + textureTrans.y;

  Out.Fog = calculateFog(Pos);

  // Transform the position to projection coordinates          
  Out.Pos  = mul(float4( Pos.x, Pos.y, Pos.z, 1), worldViewProj);  
  
  // calculate reflection coordinates
  temp = mul(Out.Pos, posToReflectTextureMatrix);
  Out.TexCoord2 = float2(temp.x/temp.w,temp.y/temp.w);
  // clamp texture coordinates between 0 and 1    
  Out.TexCoord2.x = clamp(Out.TexCoord2.x,0.0f,1.0f);
  Out.TexCoord2.y = clamp(Out.TexCoord2.y,0.0f,1.0f);
    
  // calculate diffuse lighting
  // ambient lighting
  Out.Diff = ambientLight;   
  // directional lighting
  Out.Diff += clamp ( dot(negativeLightDirection, waterNormal),0,1 ) * lightDirectionColor;  
  
  // multiply by specified color.
  Out.Diff *= diffuseColor;
  
  // calculate specular lighting and pass it as the alpha component of the
  // diffuse color
  float3 EyeDirection = normalize(cameraPosition - worldPosition);
  float3 HalfVec = normalize (negativeLightDirection + EyeDirection);
  Out.Diff.a = pow ( clamp(0,1, dot(waterNormal, HalfVec) ),16);
  Out.Diff.a = clamp (0,1,Out.Diff.a);    


  // All information is filled in so return it all
  return Out;
}


// There are 3 different samplers, One for each texture
// The all have filtering turned on so they look good

sampler SAMP_WaterTexture = sampler_state
{
    Texture = (textureWater);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler SAMP_ReflectionTexture = sampler_state
{
    Texture = (textureReflection);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler SAMP_DistortionMap = sampler_state
{
    Texture = (textureDistortionMap);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};



float4 PS_PerVertexMapping(    
    float2 Tex1 : TEXCOORD0,   
    float2 Tex2 : TEXCOORD1,
    float2 Fog : TEXCOORD2,  
    float4 Diff1 : COLOR0) : COLOR
{
			
	// sample water texture
	float4 textureSamp = tex2D(SAMP_WaterTexture, Tex1);
	
	if (reflectionOn) 
	{	
		// offset reflection texture coordinates a little bit
		// based on texture3's value
		Tex2 -= tex2D(SAMP_DistortionMap, Tex1) * 0.15f;
		textureSamp = (textureSamp + tex2D(SAMP_ReflectionTexture, Tex2)) * 0.5f;					
	}
			
	float4 result = textureSamp * Diff1 * (1 - Diff1.a) + Diff1.a;
		
	// add fog
	result = result * Fog.x + FogColor * (1.0f - Fog.x);
		
	result.a = max (diffuseColor.a, Diff1.a);
		
	return result;
}

float4 PS_PerPixelMapping(    
    float2 Tex1 : TEXCOORD0,
    float2 Fog : TEXCOORD2,             
    float4 Diff1 : COLOR0,
    float2 Position : VPOS) : COLOR
{
			
	// sample water texture
	float4 textureSamp = tex2D(SAMP_WaterTexture, Tex1);
	
	if (reflectionOn) 
	{		
		// calculate reflection texture coordinates
		float2 Tex2 = {Position.x / screenWidth, 1.0f - (Position.y /screenHeight)};
		// offset reflection texture coordinates a little bit
		// based on texture3's value
		Tex2 -= tex2D(SAMP_DistortionMap, Tex1) * 0.15f;
		
		textureSamp = (textureSamp + tex2D(SAMP_ReflectionTexture, Tex2)) * 0.5f;			
	}
			
	float4 result = textureSamp * Diff1 * (1 - Diff1.a) + Diff1.a;
		
	// add fog
	result = result * Fog.x + FogColor * (1.0f - Fog.x);
		
	result.a = max (diffuseColor.a, Diff1.a);

	return result;
}



technique PerPixelMapping
{
    pass P0
    {		       
        ZEnable = true;
        ZWriteEnable = true;
        AlphaBlendEnable = true; // Transparent water
        CullMode = NONE; // Render regardless of side
        AddressU[1] = CLAMP; // clamp the reflection texture
		AddressV[1] = CLAMP;
        
        // turn off directX fog because we will calculate it ourselves
		FogEnable = false;
        
        // Shaders
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS_PerPixelMapping();
    }  
}

technique PerVertexMapping
{
    pass P0
    {		       
        ZEnable = true;
        ZWriteEnable = true;
        AlphaBlendEnable = true; // Transparent water
        CullMode = NONE; // Render regardless of side
        AddressU[1] = CLAMP; // clamp the reflection texture
		AddressV[1] = CLAMP;
        
        // turn off directX fog because we will calculate it ourselves
		FogEnable = false;
        
        // Shaders
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_PerVertexMapping();
    }  
}
