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
Texture2D stretchTex: register(t7);
StructuredBuffer<float> Gkernel : register(t8);
SamplerState g_sampler : register(s0);
#define width 4096
#define height 4096

#define groupsize 256
groupshared float4 cache[groupsize + 2*90 + 2];

[numthreads(groupsize, 1, 1)]
void CSX(uint2 GroupId : SV_GroupID,
				 uint2 GroupThreadID : SV_GroupThreadID,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    int kernelWidth = int(Gkernel[0]);
    int FilterRadius = (kernelWidth - 1) / 2;
    float stretch =1.0f;//stretchTex.SampleLevel(g_sampler, float2(DispatchThreadId) / float2(width, height), 0).x;
    float stretchedFilterRadius = FilterRadius * stretch;
    if (GroupThreadID.x < stretchedFilterRadius)
    {
        int x = 0;
        int temp = DispatchThreadId.x - stretchedFilterRadius;
        if (temp<0)
            x = 4096 + temp;
        else
            x = temp;
        cache[GroupThreadID.x] = SrcTexture[int2(x, DispatchThreadId.y)];
    }
    if (GroupThreadID.x > groupsize - stretchedFilterRadius - 1)
    {
        int x = 0;
        int temp = DispatchThreadId.x + stretchedFilterRadius;
        if(temp>4096)
            x = temp - 4096;
        else
            x = temp;
        cache[GroupThreadID.x + 2 * stretchedFilterRadius] = SrcTexture[int2(x, DispatchThreadId.y)];
    }
    cache[GroupThreadID.x + stretchedFilterRadius] = SrcTexture[DispatchThreadId];
    GroupMemoryBarrierWithGroupSync(); //等待所有线程为cache赋值
    int currentIndex = GroupThreadID.x + stretchedFilterRadius;
    float3 totalColor = float3(0, 0, 0);
    float totalweight = 0;
    for (int i = -FilterRadius; i <= FilterRadius; i++)
    {
        int sideIndex = i *stretch + currentIndex;
        float weight = Gkernel[i+FilterRadius+1];
        totalweight += weight;
        float3 c = cache[sideIndex].xyz;
        if (c.x < 0.2f)//如果当前像素filter到了周边一个值很低的像素，说明这个周边像素是Seam，需要丢弃
            c = cache[currentIndex].xyz;
        totalColor += weight * c;
    }
        //如果当前像素为Seam，则不filter，以免seam经过filter后变亮，就被周边像素识别为普通像素
    if (SrcTexture[DispatchThreadId].x < 0.1f)
        return;
    DstTexture[DispatchThreadId] = float4(totalColor, 1.0f)/totalweight;
}
[numthreads(1, groupsize, 1)]
void CSY(uint2 GroupId : SV_GroupID,
				 uint2 GroupThreadID : SV_GroupThreadID,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    int kernelWidth = int(Gkernel[0]);
    int FilterRadius = (kernelWidth - 1) / 2;
    float stretch = 1.0f; // stretchTex.SampleLevel(g_sampler, float2(DispatchThreadId) / float2(width, height), 0).y;
    float stretchedFilterRadius = FilterRadius * stretch;
    if (GroupThreadID.y < stretchedFilterRadius)
    {
        int y = 0;
        int temp = DispatchThreadId.y - stretchedFilterRadius;
        if (temp < 0)
            y = 4096 + temp;
        else
            y = temp;
        cache[GroupThreadID.y] = SrcTexture[int2(DispatchThreadId.x,y)];
    }
    if (GroupThreadID.y > groupsize - stretchedFilterRadius - 1)
    {
        int y = 0;
        int temp = DispatchThreadId.y + stretchedFilterRadius;
        if (temp > 4096)
            y = temp - 4096;
        else
            y = temp;
        cache[GroupThreadID.y + 2 * stretchedFilterRadius] = SrcTexture[int2(DispatchThreadId.x,y)];
    }
    cache[GroupThreadID.y + stretchedFilterRadius] = SrcTexture[DispatchThreadId];
    GroupMemoryBarrierWithGroupSync(); //等待所有线程为cache赋值
    int currentIndex = GroupThreadID.y + stretchedFilterRadius;
    float3 totalColor = float3(0, 0, 0);
    float totalweight = 0;
    for (int i = -FilterRadius; i <= FilterRadius; i++)
    {
        int sideIndex = i * stretch + currentIndex;
        float weight = Gkernel[i + FilterRadius + 1];
        totalweight += weight;
        float3 c = cache[sideIndex].xyz;
        if(c.x<0.2f)
            c = cache[currentIndex].xyz;
        totalColor += weight * c;
    }
    if (SrcTexture[DispatchThreadId].y < 0.1f)
        return;
    DstTexture[DispatchThreadId] = float4(totalColor, 1.0f)/totalweight;
}