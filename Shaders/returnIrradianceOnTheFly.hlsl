#include"util.hlsl"
vertexout VS(vertexin pin)
{
    vertexout vout = (vertexout) 0;
    float4 Position = mul(pin.position, world);
    float4 posW = Position;
    vout.positionH = mul(Position, VP);
    vout.positionW = posW;
    vout.uv = pin.uv;
    vout.color = pin.color;
    vout.normal = mul(float4(pin.noraml, 1.0f), worldinvT);
    vout.tanegntU = pin.tangentU;
    return vout;
}
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMapSample - 1.0f;

	// Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}
float4 PS(vertexout pin) : SV_Target
{
    float4 texcolor = (texIndex >= 0) ? float4(g_tex[texIndex].Sample(g_sampler, float2(pin.uv.x, (1.0f - pin.uv.y))).xyz,1.0f) : float4(pin.color, 1.0f);
    pin.uv = float2(pin.uv.x, (1.0f - pin.uv.y));
    float3 normN = normalize(pin.normal);
    float3 normT = normalize(pin.tanegntU);
    float3 bn = g_normMap.Sample(g_sampler, pin.uv).xyz;
    float3 bumpednormalW = NormalSampleToWorldSpace(bn, normN, normT);
    float3 irradiance = float3(0, 0, 0);
    for (int i = 0; i < 4; i++)
    {
        irradiance += max(0, dot(bumpednormalW, g_parallelLights[i].direction)) * g_parallelLights[i].color;

    }
    return float4(irradiance,1.0f);
}