
Texture2D<float4> SrcTexture : register(t0);
Texture2D<float4> g_worldpos : register(t1);
Texture2D<float4> g_normal : register(t2);
Texture2D<float4> g_lastnormal : register(t7);
RWTexture2D<float4> DstTexture : register(u0);
SamplerState g_sampler : register(s0);
#define width 1024.0f
#define height 768.0f
float PHBeckmann(float ndoth, float m)
{
    float alpha = acos(ndoth);
    float ta = tan(alpha);
    float val = 1.0 / (m * m * pow(ndoth, 4.0)) * exp(-(ta * ta) / (m * m));
    return val;
}
[numthreads(16, 16, 1)]
void CS(uint2 GroupId : SV_GroupID,
				 uint GroupThreadIndex : SV_GroupIndex,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    DstTexture[DispatchThreadId] = 0.5 * pow(PHBeckmann(DispatchThreadId.x/width, DispatchThreadId.y/height), 0.1);
    GroupMemoryBarrierWithGroupSync();
}