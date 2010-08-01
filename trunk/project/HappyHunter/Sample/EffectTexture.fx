//==============================================================
// Desc: 效果代码文件
//==============================================================


//--------------------------------------------------------------
// 全局变量
//--------------------------------------------------------------
float4x4  matWorldViewProj : WORLDVIEWPROJECTION;	  //组合变换矩阵
float2    vUVOffset:UVOFFSET;


//--------------------------------------------------------------
// 纹理采样器
//--------------------------------------------------------------
texture TextureMapping : TEXTURE0;
sampler2D Sampler2D = sampler_state
{
    Texture   = <TextureMapping>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};


//--------------------------------------------------------------
// 输出结构
//--------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 TextureUV  : TEXCOORD0;  //纹理坐标  
};

struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;       //像素颜色
};


//-----------------------------------------------------------------
// 顶点渲染器主函数
//-----------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float2 vTexCoord0 : TEXCOORD0)
{
    //顶点位置
    VS_OUTPUT Out = (VS_OUTPUT)0;      
    Out.Position = mul(Pos, matWorldViewProj);	//坐标变换
   
    //纹理
    Out.TextureUV = vTexCoord0 + vUVOffset; 
    return Out;
}


//--------------------------------------------------------------
// 像素渲染器主函数
//--------------------------------------------------------------
PS_OUTPUT PS( VS_OUTPUT In)
{ 
    PS_OUTPUT Output = (PS_OUTPUT)0;
    Output.RGBColor  = tex2D(Sampler2D, In.TextureUV);
    
    return Output;
}


//--------------------------------------------------------------
//  技术
//--------------------------------------------------------------
technique TShader
{
    pass P0
    {
		//CULLMODE = NONE;
		
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}
