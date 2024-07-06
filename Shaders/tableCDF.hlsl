Texture2D<float4> currentFrame : register(t0);
Texture2D<float4> g_worldpos : register(t1);
Texture2D<float4> g_normal : register(t2);
StructuredBuffer<int2> g_offsets : register(t3);
StructuredBuffer<float> g_h : register(t4);
Texture2D<float4> g_lastnormal : register(t7);
RWTexture2D<float4> DstTexture : register(u0);
RWTexture2D<float4> moment1tex : register(u1);
RWTexture2D<float4> moment2tex : register(u2);
RWTexture2D<float4> historytex : register(u3);
RWTexture2D<float4> variancetex : register(u4);
RWTexture2D<float4> lastMoment1tex : register(u5);
RWTexture2D<float4> lastMoment2tex : register(u6);
#define width 1024
#define height 768
float NormalizedDiffusionCDF(float r)
{
    return 1 - exp(-r) * 0.25f - exp(-r / 3.0f) * 0.75f;
}
#define expnum 4.0f
[numthreads(512, 1, 1)]
void CS(uint2 GroupId : SV_GroupID,
				 uint GroupThreadIndex : SV_GroupIndex,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    float r = pow(DispatchThreadId.x, expnum) * 10.0f / pow(2048, expnum);
    //float r = DispatchThreadId.x * 10.0f / 2048.0f;
    float CDF = NormalizedDiffusionCDF(r);
    DstTexture[int2(CDF * 1024, DispatchThreadId.y)] = float4(r, r, r, r);
    GroupMemoryBarrierWithGroupSync();
}