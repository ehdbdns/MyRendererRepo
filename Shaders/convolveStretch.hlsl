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
Texture2D stretchTex[5] : register(t0, space1);
StructuredBuffer<float> Gkernel : register(t8);
SamplerState g_sampler : register(s0);
#define width 4096
#define height 4096
#define groupsize 256
groupshared float4 cache[groupsize + 9 * 9 + 2];

[numthreads(groupsize, 1, 1)]
void CSX(uint2 GroupId : SV_GroupID,
				 uint2 GroupThreadID : SV_GroupThreadID,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    int kernelWidth = int(Gkernel[0]);
    int FilterRadius = (kernelWidth - 1) / 2;
    float stretchedFilterRadius = FilterRadius;
    if (GroupThreadID.x < stretchedFilterRadius)
    {
        int x = max(0, DispatchThreadId.x - stretchedFilterRadius);
        cache[GroupThreadID.x] = SrcTexture[int2(x, DispatchThreadId.y)];
    }
    if (GroupThreadID.x > groupsize - stretchedFilterRadius - 1)
    {
        int x = min(width - 1, DispatchThreadId.x + stretchedFilterRadius);
        cache[GroupThreadID.x + 2 * stretchedFilterRadius] = SrcTexture[int2(x, DispatchThreadId.y)];
    }
    cache[GroupThreadID.x + stretchedFilterRadius] = SrcTexture[DispatchThreadId];
    GroupMemoryBarrierWithGroupSync(); //等待所有线程为cache赋值
    int currentIndex = GroupThreadID.x + stretchedFilterRadius;
    float3 totalColor = float3(0, 0, 0);
    float totalweight = 0;
    for (int i = -FilterRadius; i <= FilterRadius; i++)
    {
        int sideIndex = i + currentIndex;
        float weight = Gkernel[i + FilterRadius + 1];
        totalweight += weight;
        totalColor += weight * cache[sideIndex].xyz;
    }
    DstTexture[DispatchThreadId] = float4(totalColor, 1.0f) / totalweight;
}
[numthreads(1, groupsize, 1)]
void CSY(uint2 GroupId : SV_GroupID,
				 uint2 GroupThreadID : SV_GroupThreadID,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    int kernelWidth = int(Gkernel[0]);
    int FilterRadius = (kernelWidth - 1) / 2;
    float stretchedFilterRadius = FilterRadius;
    if (GroupThreadID.y < stretchedFilterRadius)
    {
        int y = max(0, DispatchThreadId.y - stretchedFilterRadius);
        cache[GroupThreadID.y] = SrcTexture[int2(DispatchThreadId.x, y)];
    }
    if (GroupThreadID.y > groupsize - stretchedFilterRadius - 1)
    {
        int y = min(height - 1, DispatchThreadId.y + stretchedFilterRadius);
        cache[GroupThreadID.y + 2 * stretchedFilterRadius] = SrcTexture[int2(DispatchThreadId.x, y)];
    }
    cache[GroupThreadID.y + stretchedFilterRadius] = SrcTexture[DispatchThreadId];
    GroupMemoryBarrierWithGroupSync(); //等待所有线程为cache赋值
    int currentIndex = GroupThreadID.y + stretchedFilterRadius;
    float3 totalColor = float3(0, 0, 0);
    float totalweight = 0;
    for (int i = -FilterRadius; i <= FilterRadius; i++)
    {
        int sideIndex = i + currentIndex;
        float weight = Gkernel[i + FilterRadius + 1];
        totalweight += weight;
        totalColor += weight * cache[sideIndex].xyz;
    }
    DstTexture[DispatchThreadId] = float4(totalColor, 1.0f) / totalweight;
}