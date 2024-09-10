Texture2D<float4> currentFrame : register(t0);
Texture2D<float4> g_worldpos : register(t1);
Texture2D<float4> g_normal : register(t2);
StructuredBuffer<int2> g_offsets : register(t3);
StructuredBuffer<float> g_h : register(t4);
StructuredBuffer<float> g_randnum : register(t12);
Texture2D<float4> g_lastnormal : register(t7);
RWTexture2D<float4> DstTexture : register(u0);
RWTexture2D<float4> moment1tex : register(u1);
RWTexture2D<float4> moment2tex : register(u2);
RWTexture2D<float4> historytex : register(u3);
RWTexture2D<float4> variancetex : register(u4);
RWTexture2D<float4> lastMoment1tex : register(u5);
RWTexture2D<float4> lastMoment2tex : register(u6);
[numthreads(16, 16, 1)]
void CS(uint2 GroupId : SV_GroupID,
				 uint GroupThreadIndex : SV_GroupIndex,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    int index = DispatchThreadId.x + DispatchThreadId.y * 512;
    DstTexture[DispatchThreadId] = float4(g_randnum[index * 4], g_randnum[index * 4 + 1], g_randnum[index * 4 + 2],g_randnum[index*4+3]);
    GroupMemoryBarrierWithGroupSync();
}