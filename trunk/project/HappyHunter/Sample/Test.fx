// transformations
float4x4 mViewProj: VIEWPROJECTION;

float4 posOffset : POSITIONOFFSET = {1.0, 1.0, 0.0f, 0.0f};
float4 texOffset : UVOFFSET = {1.0, 1.0, 0.0f, 0.0f};
float4 ambient_light = {1.0f,1.0f,1.0f,0.0f};
float4 sun_vec: sunlight_vec = {0.578f,0.578f,0.578f,0.0f};

texture tex0 : TEXTURE0; // blend
texture tex1 : TEXTURE1;

struct VS_INPUT
{
    float 	x		: POSITION0;
    float   z       : POSITION1;
    float2	UV		: TEXCOORD0;
	float	y	    : POSITION2;
	float3	Norm	: NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vDiffuse  : COLOR0;
    float2 vTex0  : TEXCOORD0;
    float2 vTex1  : TEXCOORD1;
};

VS_OUTPUT VS(const VS_INPUT v)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float4 combinedPos = float4(
		v.x,
		v.y,
		v.z,
		1);

	combinedPos.x += posOffset.z;	
	combinedPos.z += posOffset.w;

	Out.Pos = mul(combinedPos, mViewProj);  // position (view space)

	Out.vDiffuse = dot(v.Norm, sun_vec.rgb)+ambient_light;

	Out.vTex0 = (v.UV+texOffset.zw)*texOffset.xy;
	
	Out.vTex1 = v.UV;

  return Out;
}

sampler LinearSamp0 = sampler_state 
{
    texture = <tex0>;
    AddressU  = clamp;        
    AddressV  = clamp;
    AddressW  = clamp;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler LinearSamp1 = sampler_state 
{
    texture = <tex1>;
    AddressU  = clamp;        
    AddressV  = clamp;
    AddressW  = clamp;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};


float4 PS(VS_OUTPUT In) : COLOR
{   
	// sample the texture
	float4 Color = tex2D(LinearSamp0, In.vTex0 );// * tex2D(LinearSamp1, In.vTex1 );

	// multiply by the diffuse 
	// vertex color 
	return Color*In.vDiffuse;
}  


technique SinglePassTerrain
{
    pass P0
    {
			//FILLMODE = WIREFRAME;
			//CULLMODE = NONE;
			//ZENABLE = TRUE;
			//ZWRITEENABLE = TRUE;
			//ZFUNC = LESSEQUAL;

			AlphaBlendEnable = false;
			AlphaTestEnable = false;

        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }  
}