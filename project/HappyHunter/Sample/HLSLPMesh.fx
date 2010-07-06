//==============================================================
// Desc: 效果文件
//==============================================================

//--------------------------------------------------------------
// 全局变量
//--------------------------------------------------------------
float4x4 matWorldViewProj;
float4 MaterialAmbient = {0.1f, 0.1f, 0.1f, 1.0f};
float4 MaterialDiffuse = {0.8f, 0.8f, 0.8f, 1.0f};
float4 vecLightDir = {-1.0f, -1.0f, -1.0f, 1.0f};

//--------------------------------------------------------------
// 纹理采样器
//--------------------------------------------------------------
texture ColorMap;
sampler ColorMapSampler = sampler_state
{
   Texture = <ColorMap>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;   
};

//--------------------------------------------------------------
// 顶点渲染器输出结构
//--------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos  	 : POSITION;
    float4 Diffuse	 : COLOR;
    float2 Tex  	 : TEXCOORD0;
};

//--------------------------------------------------------------
//像素渲染器输出结构
//--------------------------------------------------------------
struct PS_OUTPUT
{
    float4 Color  : COLOR;
};

//---------------------------------------------------------
// 子函数, 计算光照系数
//---------------------------------------------------------
float3 Diffuse(float3 Normal)
{
    float CosTheta;
    CosTheta = max(0.0f, dot(Normal, vecLightDir.xyz));
    return (CosTheta);
}

//--------------------------------------------------------------
//  顶点渲染器主函数
//--------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float3 Normal : NORMAL, float3 Tex : TEXCOORD0)
{
   VS_OUTPUT Out = (VS_OUTPUT)0;
   
   //顶点坐标变换
   Out.Pos = mul(Pos, matWorldViewProj);
   Out.Tex = Tex.xy;
   
   //计算光照
   Out.Diffuse.xyz = MaterialAmbient.xyz + Diffuse(Normal) * MaterialDiffuse.xyz;
   Out.Diffuse.w = 1.0f;
      
   return Out;
}

//--------------------------------------------------------------
// Desc: 像素渲染器主函数
//--------------------------------------------------------------
PS_OUTPUT PS(float2 Tex: TEXCOORD0, float4 Diffuse : COLOR)
{
    PS_OUTPUT Out_ps = (PS_OUTPUT)0;
    
    //颜色纹理采样
    float4 color     = tex2D(ColorMapSampler, Tex);

    //最终颜色
    Out_ps.Color = color * Diffuse;
    
    return Out_ps;
}


//--------------------------------------------------------------
// Desc: 技术
//--------------------------------------------------------------
technique t0
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}

