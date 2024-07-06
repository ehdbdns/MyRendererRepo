#include"util.hlsl"
vertexout VS(vertexin pin)
{
    vertexout vout;
    vout.skyVec = pin.position.xyz;
    pin.position += float4(eyepos, 0.0f);
    float4x4 MVP = mul(world, VP);
    vout.positionH = mul(pin.position, MVP);
    return vout;
}
float4 PS(vertexout pin) : SV_TARGET
{
    return g_sky.Sample(g_sampler, pin.skyVec);
}