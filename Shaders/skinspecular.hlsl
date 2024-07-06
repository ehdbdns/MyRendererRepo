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
    vout.normal = pin.noraml; // mul(float4(pin.noraml, 1.0f), world);
    vout.tanegntU = pin.tangentU;
    return vout;
}
float fresnelReflectance(float3 H, float3 V, float F0)
{
    float base = 1.0 - dot(V, H);
    float exponential = pow(base, 5.0);
    return exponential + F0 * (1.0 - exponential);
}
float KS_Skin_Specular(float3 N, // Bumped surface normal
                       float3 L, // Points to light
                       float3 V, // Points to eye
                       float m,
                       // Roughness
                       float rho_s // Specular brightness
                     )
{
    float result = 0.0;
    float ndotl = dot(N, L);
    if (ndotl >0) {
        float3 h = L + V; // Unnormalized half-way vector
        float3 H = normalize(h);
        float ndoth = dot(N, H);
        float PH = pow(2.0 * g_beckmanMap.Sample(g_sampler, float2(ndoth, 1.0f-m).r),10.0);
        float F = fresnelReflectance(H, V, 0.028);
        float frSpec = max(PH * F / dot(h, h), 0);
        result = ndotl * rho_s * frSpec; // BRDF * dot(N,L) * rho_s
    }
    return result;
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
#define irradianceWidth 4096
#define irradianceHeight 4096
float4 PS(vertexout pin) : SV_Target
{
    pin.uv = float2(pin.uv.x, (1.0f - pin.uv.y));
    float roughness = 0.6f;
    float3 texcolor =  pow((texIndex >= 0) ? g_tex[texIndex].Sample(g_sampler, pin.uv).xyz : pin.color,2.2);//伽马矫正从gamma0.45到gamma1.0
    float3 toEye = normalize(eyepos - pin.positionW);
    float3 normN = normalize(pin.normal);
    float3 normT = normalize(pin.tanegntU);
    float3 bn = g_normMap.Sample(g_sampler, pin.uv).xyz;
    float3 bumpednormalW = NormalSampleToWorldSpace(bn, normN, normT);
    float4 specular = float4(0, 0, 0, 0);
    for (int j = 0; j < 4; j++)
    {
        specular += KS_Skin_Specular(normalize(bumpednormalW), normalize(g_parallelLights[j].direction), toEye, roughness, 1*g_parallelLights[j].color.x);
    }
    float3 diffuse = float4(0, 0, 0, 0);
    float3 totalSumGWeights = float3(0, 0, 0);
    for (int i = 0; i < 6; i++)
    {
        float3 Gw = g_sumGweights[i];
        totalSumGWeights += Gw;
        diffuse += Gw * g_convolveTex[i].Sample(g_sampler, pin.uv).xyz;
    }
    diffuse /= totalSumGWeights;
    return pow(float4(diffuse * texcolor, 0) + specular,1.0f/2.2f);//伽马矫正从gamma1.0到gamma0.45

}
