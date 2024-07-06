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
    vout.tanegntU = mul(float4(pin.tangentU, 0), world).xyz;
    return vout;
}
#define sampleNum 50
float4 PS(vertexout pin) : SV_Target
{
    float4 texcolor = (texIndex >= 0) ? g_tex[texIndex].Sample(g_sampler, pin.uv) : float4(pin.color, 1.0f);
    float3 toEye = normalize(eyepos - pin.positionW);
    float3 norL = normalize(lightdir);
    float3 norN = normalize(pin.normal);
    float3 DirectL = float3(0, 0, 0);
    for (int i = 0; i < sampleNum; i++)
    {
        DirectL += calcTranslucentDirectLightFromPolygonalLightt(pin.positionW, norN, pin.tanegntU, pin.color, int2(pin.positionH.xy + pin.uv*1024+i), float3(0.31f, 0.31f, 0.31f),0.4f,toEye);
    }
    DirectL /= sampleNum;
    return float4(DirectL, 1.0f);
}