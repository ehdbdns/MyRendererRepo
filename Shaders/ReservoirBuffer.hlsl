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
    vout.normal = mul(float4(pin.noraml, 0), world).xyz;
    return vout;
}
void PS(vertexout pin)
{
    float4 texcolor = (texIndex >= 0) ? g_tex[texIndex].Sample(g_sampler, pin.uv) : float4(pin.color, 1.0f);
    float3 toEye = normalize(eyepos - pin.positionW);
    float3 norN = normalize(pin.normal);
    Material m;
    m.color = texcolor;
    m.F0 = float3(1, 1, 1);
    m.isGlossy = true;
    m.roughness = 0.1f;
    Reservoir r;
    r.init();
    r= RIS(pin.positionW, norN, texcolor.xyz, 2, int2(pin.positionH.xy-0.5f), m, toEye, 36000.0f*2+900,nFrame);
    int2 index = int2(pin.positionH.xy - 0.5f);
    g_Reservoir[0][index] = float4(r.samplePos, r.Wsum);
    g_Reservoir[1][index] = float4(r.sampleNorm, r.M);
    g_Reservoir[2][index] = float4(r.sampleColor, r.W);
}