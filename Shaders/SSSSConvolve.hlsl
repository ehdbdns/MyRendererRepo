Texture2D<float4> SrcTexture : register(t0);
Texture2D<float4> g_worldpos : register(t1);
Texture2D<float4> g_normal : register(t2);
StructuredBuffer<int2> g_offsets : register(t3);
StructuredBuffer<float> g_h : register(t4);
Texture2D<float4> g_z : register(t5);
Texture2D<float4> g_gz : register(t6);
RWTexture2D<float4> DstTexture : register(u0);
RWTexture2D<float4> moment1tex : register(u1);
RWTexture2D<float4> moment2tex : register(u2);
RWTexture2D<float4> historytex : register(u3);
RWTexture2D<float4> variancetex : register(u4);
Texture2D stretchTex : register(t7);
StructuredBuffer<float> Gkernel : register(t8);
StructuredBuffer<float4> SSSSKernel : register(t9);
Texture2D diffTex : register(t10);
Texture2D specularTex : register(t11);
SamplerState g_sampler : register(s0);
#define width 1024
#define height 768
cbuffer SSSS : register(b1)
{
    float scale;
    float alpha;
    float beta;
}
#define groupsize 1024
groupshared float4 cache[2048]; //[groupsize + 10 * 90 + 2];

[numthreads(groupsize, 1, 1)]
void CSX(uint2 GroupId : SV_GroupID,
				 uint2 GroupThreadID : SV_GroupThreadID,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    float2 uv = float2(DispatchThreadId.xy) / float2(width, height);
    float4 DispatchColor = SrcTexture[DispatchThreadId];
    int kernelWidth = int(SSSSKernel[0].x);
    float d = g_z.SampleLevel(g_sampler, uv, 0).r;
    float gd = g_gz.SampleLevel(g_sampler, uv, 0).x;
    float ScreenSpaceScale = alpha / (d + beta * abs(gd));
    float finalStep = scale * ScreenSpaceScale;
    int FilterRadius = (kernelWidth - 1) / 2 ;
    if (GroupThreadID.x < FilterRadius)
    {
        int x = DispatchThreadId.x - FilterRadius;
        cache[GroupThreadID.x] = SrcTexture.SampleLevel(g_sampler, float2(x, DispatchThreadId.y) / float2(width,height),0);
    }
    if (GroupThreadID.x > groupsize - FilterRadius - 1)
    {
        int x = DispatchThreadId.x + FilterRadius;
        cache[GroupThreadID.x + 2 * FilterRadius] = SrcTexture.SampleLevel(g_sampler, float2(x, DispatchThreadId.y) / float2(width, height),0);
    }
    cache[GroupThreadID.x + FilterRadius] = DispatchColor;
    GroupMemoryBarrierWithGroupSync(); //等待所有线程为cache赋值
    if (DispatchColor.w <0.9f)
    {
        return;
    }
    int currentIndex = GroupThreadID.x + FilterRadius;
    float3 totalColor = float3(0, 0, 0);
    for (int i = 0; i < kernelWidth; i++)
    {
        int sideIndex = SSSSKernel[i + 1].w * finalStep + currentIndex;
        if (sideIndex < 0 || sideIndex > groupsize)
        {
            totalColor += SSSSKernel[i + 1].xyz * DispatchColor.xyz;
            continue;
        }
        totalColor += SSSSKernel[i + 1].xyz * cache[sideIndex].xyz;
    }
    DstTexture[DispatchThreadId] = float4(totalColor, 1.0f);
}
#undef groupsize
#define groupsize 768
[numthreads(1, groupsize, 1)]
void CSY(uint2 GroupId : SV_GroupID,
				 uint2 GroupThreadID : SV_GroupThreadID,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    float2 uv = float2(DispatchThreadId.xy) / float2(width, height);
    float4 DispatchColor = SrcTexture[DispatchThreadId];
    int kernelWidth = int(SSSSKernel[0].x);
    float d = g_z.SampleLevel(g_sampler, uv, 0).r;
    float2 diffuv = g_z.SampleLevel(g_sampler, uv, 0).yz;//我直接在深度图的GB通道存了diffTex的UV
    float gd = g_gz.SampleLevel(g_sampler, uv, 0).y;
    float ScreenSpaceScale = alpha / (d + beta * abs(gd));
    float finalStep = scale * ScreenSpaceScale;
    int FilterRadius = (kernelWidth - 1) / 2;
    if (GroupThreadID.y < FilterRadius)
    {
        int y = DispatchThreadId.y - FilterRadius;
        cache[GroupThreadID.y] = SrcTexture.SampleLevel(g_sampler, float2(DispatchThreadId.x, y) / float2(width, height),0);
    }
    if (GroupThreadID.y > groupsize - FilterRadius - 1)
    {
        int y = DispatchThreadId.y + FilterRadius;
        cache[GroupThreadID.y + 2 * FilterRadius] = SrcTexture.SampleLevel(g_sampler, float2(DispatchThreadId.x, y) / float2(width, height),0);
    }
    cache[GroupThreadID.y + FilterRadius] = DispatchColor;
    GroupMemoryBarrierWithGroupSync(); //等待所有线程为cache赋值
    if (DispatchColor.w < 0.9f)
        return;
    int currentIndex = GroupThreadID.y + FilterRadius;
    float3 totalColor = float3(0, 0, 0);
    for (int i = 0; i < kernelWidth; i++)
    {
        int sideIndex = SSSSKernel[i + 1].w * finalStep + currentIndex;
        if (sideIndex < 0 || sideIndex > groupsize)
        {
            totalColor += SSSSKernel[i + 1].xyz * DispatchColor.xyz;
            continue;
        }
        totalColor += SSSSKernel[i + 1].xyz * cache[sideIndex].xyz;
    }
    
    DstTexture[DispatchThreadId] = float4(totalColor, 1.0f) * diffTex.SampleLevel(g_sampler, diffuv, 0) + specularTex.SampleLevel(g_sampler,uv,0);
}