//=========================================================
// Desc: 效果源代码
//=========================================================


//---------------------------------------------------------
// 全局变量
//---------------------------------------------------------
float4x4 matWorld;	
float4   vecLightDir;
float4   vecEye;

float4 lightDiffuse = {0.6f, 0.6f, 0.6f, 1.0f}; 
float4 MaterialAmbient = {0.1f, 0.1f, 0.1f, 1.0f};
float4 MaterialDiffuse = {0.8f, 0.8f, 0.8f, 1.0f};

static const int MAX_MATRICES = 26;
float4x3    mWorldMatrixArray[MAX_MATRICES];  //骨骼调色板矩阵 
int         CurNumBones = 2;    //当前骨骼数量
float4x4    mProj;

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
texture BumpMap;
sampler BumpMapSampler = sampler_state
{
   Texture = <BumpMap>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;   
};
//----------------------------------------------------------
// 顶点渲染器输入和输出结构
//----------------------------------------------------------
struct VS_INPUT
{
    float4  Pos           : POSITION;
    float4  BlendWeights  : BLENDWEIGHT;
    float4  BlendIndices  : BLENDINDICES;
    float3  Normal        : NORMAL;
    float3  Tex           : TEXCOORD0;
    float3  Tangent 		  : TANGENT;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float2  Tex     : TEXCOORD0;
    float3  Light		: TEXCOORD1;   //纹理空间中灯光方向
    float3  View 		: TEXCOORD2;   //纹理空间中法线方向
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

//---------------------------------------------------------
// 子函数, 完成顶点处理
//---------------------------------------------------------
VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT   o = (VS_OUTPUT)0;
    float3      Pos = 0.0f;
    float3      Normal = 0.0f;
    float       sumWeight  = 0.0f;
    float       LastWeight = 0.0f;
   
   //-------------------------------------------------------------------------
     
    //为Geforce3类型的显卡所做的补偿
    int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);
    
    //将骨骼权重向量和骨骼索引向量变换至数组
    float BlendWeightsArray[4] = (float[4])i.BlendWeights;
    int   IndexArray[4]        = (int[4])IndexVector;

    //计算前NumBones-1个骨骼对于该顶点位置及法向量的影响
    for (int iBone = 0; iBone < NumBones-1; iBone++)
    {
        sumWeight = sumWeight + BlendWeightsArray[iBone];
        
        Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
    LastWeight = 1.0f - sumWeight; 

    //计算最后一个骨骼对于顶点位置及法向量的影响
    Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);
    Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); 
    
    o.Pos = mul(float4(Pos.xyz, 1.0f), mProj);
    Normal = normalize(Normal);
    
    //-------------------------------------------------------------------------
    
    //构建一个3x3变换矩阵  用于完成从世界空间到纹理空间的变换
   float3x3 worldToTangentSpace;
   worldToTangentSpace[0] = mul(i.Tangent, matWorld);
   worldToTangentSpace[1] = mul(cross(i.Tangent,Normal), matWorld);
   worldToTangentSpace[2] = mul(Normal, matWorld);

    //输出纹理坐标
    o.Tex  = i.Tex.xy;
    //顶点在世界空间中的坐标
   float3 PosWorld = normalize(mul(i.Pos, matWorld));
   
   //计算纹理空间中灯光方向向量
   float3 Light  = vecLightDir - PosWorld; 
   o.Light.xyz = mul(worldToTangentSpace, Light);

   //计算纹理空间中的观察方向
   float3 Viewer = vecEye - PosWorld;						
   o.View = mul(worldToTangentSpace, Viewer);
    
    return o;
}

//--------------------------------------------------------------
// Desc: 像素渲染器主函数
//--------------------------------------------------------------
PS_OUTPUT PShade(float2 Tex: TEXCOORD0, float3 Light : TEXCOORD1, 
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
    float4 spec    = min(pow(saturate(dot(Reflect, ViewDir)), 50), color.a);

    //混合纹理颜色和光照颜色, 得到每个像素的最终颜色
    Out_ps.Color = 0.2 * color + (shadow * (color * diff + spec));
    
    return Out_ps;
}

//---------------------------------------------------------
// 顶点渲染器
//---------------------------------------------------------
VertexShader vsArray[4] = {
                            compile vs_2_0 VShade(1), 
                            compile vs_2_0 VShade(2),
                            compile vs_2_0 VShade(3),
                            compile vs_2_0 VShade(4)
                          };


//---------------------------------------------------------
// 技术与通道
//---------------------------------------------------------
technique t0
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
        PixelShader = compile ps_2_0 PShade();
    }
}

