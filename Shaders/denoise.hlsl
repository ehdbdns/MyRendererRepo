#include "csUtil.hlsl"
float squareLength(float3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}
float squareLength(int2 v)
{
    return float(v.x * v.x + v.y * v.y);
}
float3 calcJBFWeight(int2 i, int2 j, float3 variance)
{
    float InvSigmaRT = 1.0f / 500.0f;
    float SigmaN = 128.0f;
    float InvSigmaZ = 1.0f / 60.0f;
    float3 eRT = length(SrcTexture[i].xyz - SrcTexture[j].xyz) * InvSigmaRT / (sqrt(variance) + float3(0.001, 0.001, 0.001));
    float wN = pow(max(0, dot(g_normal[i].xyz * 2.0f - 1.0f, g_normal[j].xyz * 2.0f - 1.0f)), SigmaN);
    float eZ = length(g_z[i].x - g_z[j].x) * InvSigmaZ / (length(g_gz[i].x * (i - j)) + float3(0.001, 0.001, 0.001));
    return wN * exp(-eRT);
}
[numthreads(16, 16, 1)]
void CS(uint2 GroupId : SV_GroupID,
				 uint GroupThreadIndex : SV_GroupIndex,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    float3 totalColorWeight = float3(0, 0, 0);
    float3 totalVarianceWeight = float3(0, 0, 0);
    float3 totalColor = float3(0, 0, 0);
    float3 totalVariance = float3(0, 0, 0);
    float3 currentVariance = variancetex[DispatchThreadId].xyz;
    for (int i = 0; i < 25; i++)
    {
        int2 j = DispatchThreadId + g_offsets[i];
        j = min(j, SrcTexture.Length.xy - 1);
        j = max(j, float2(0, 0));
        float3 w = calcJBFWeight(DispatchThreadId, j, currentVariance);
        float h = g_h[i % 5];
        float3 weight = h * w;
        totalColor += weight * SrcTexture[j].xyz;
        totalVariance += weight * weight * variancetex[j].xyz;
        totalColorWeight += weight;
        totalVarianceWeight += weight * weight;

    }
    totalColor /= totalColorWeight;
    totalVariance /= totalVarianceWeight;
    DstTexture[DispatchThreadId] = float4(totalColor, 1.0f);
    variancetex[DispatchThreadId] = float4(totalVariance, 1.0f);
    GroupMemoryBarrierWithGroupSync();
}