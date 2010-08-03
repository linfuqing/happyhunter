//==============================================================
// Desc: 效果文件
//==============================================================

//--------------------------------------------------------------
// 全局变量
//--------------------------------------------------------------
float4x4 matWorldViewProj : WORLDVIEWPROJECTION;	
float4x4 matWorld : WORLD;	
float4   vecLightDir : LIGHTDIRECTION;
float4   vecEye : CAMERAPOSITION;

//--------------------------------------------------------------
// 纹理采样器
//--------------------------------------------------------------
texture ColorMap : TEXTURE0;
sampler ColorMapSampler = sampler_state
{
   Texture = <ColorMap>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;   
};

texture BumpMap : TEXTURE1;
sampler BumpMapSampler = sampler_state
{
   Texture = <BumpMap>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;   
};

//--------------------------------------------------------------
// 顶点渲染器输出结构
//--------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex  : TEXCOORD0;
    float3 Light: TEXCOORD1;   //纹理空间中灯光方向
    float3 View : TEXCOORD2;   //纹理空间中法线方向
};


//--------------------------------------------------------------
//  顶点渲染器主函数
//--------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD, float3 Normal : NORMAL, float3 Tangent : TANGENT  )
{
   VS_OUTPUT Out = (VS_OUTPUT)0;
   
   //顶点坐标变换
   Out.Pos = mul(Pos, matWorldViewProj);
  
   //构建一个3x3变换矩阵  用于完成从世界空间到纹理空间的变换
   float3x3 worldToTangentSpace;
   worldToTangentSpace[0] = mul(Tangent, matWorld);
   worldToTangentSpace[1] = mul(cross(Tangent,Normal), matWorld);
   worldToTangentSpace[2] = mul(Normal, matWorld);

   Out.Tex = Tex.xy;
   
   //顶点在世界空间中的坐标
   float3 PosWorld = normalize(mul(Pos, matWorld));

   //计算纹理空间中灯光方向向量
   float3 Light  = vecLightDir - PosWorld; 
   Out.Light.xyz = mul(worldToTangentSpace, Light);

   //计算纹理空间中的观察方向
   float3 Viewer = vecEye - PosWorld;						
   Out.View = mul(worldToTangentSpace, Viewer);
      
   return Out;
}


//--------------------------------------------------------------
//像素渲染器输出结构
//--------------------------------------------------------------
struct PS_OUTPUT
{
    float4 Color  : COLOR;
};

//--------------------------------------------------------------
// Desc: 像素渲染器主函数
//--------------------------------------------------------------
PS_OUTPUT PS(float2 Tex: TEXCOORD0, float3 Light : TEXCOORD1, 
            float3 View : TEXCOORD2, float3 Att : TEXCOORD3)
{
    PS_OUTPUT Out_ps = (PS_OUTPUT)0;
    
    //颜色纹理采样
    float4 color      = tex2D(ColorMapSampler, Tex);
    
    //凹凸纹理采样, 得到的是法线向量, 用于代替顶点法线进行光照计算
    float3 bumpNormal = 2 * (tex2D(BumpMapSampler, Tex) - 0.5);
	
	//标准化纹理空间中的灯光方向和观察方向
    float3 LightDir = normalize(Light);
    float3 ViewDir  = normalize(View);
    
    //根据凹凸贴图的发线进行漫反射光照计算
    float4 diff = saturate(dot(bumpNormal, LightDir));
    
    //计算自身遮蔽阴影项
    float shadow = saturate(4 * diff);
    
    //计算镜面反射强度
    float3 Reflect = normalize(2 * diff * bumpNormal - LightDir);
    float4 spec    = min(pow(saturate(dot(Reflect, ViewDir)), 25), color.a);

    //混合纹理颜色和光照颜色, 得到每个像素的最终颜色
    Out_ps.Color = 0.2 * color + (shadow * (color * diff + spec) );
    
    return Out_ps;
}


//--------------------------------------------------------------
// Desc: 技术
//--------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}

