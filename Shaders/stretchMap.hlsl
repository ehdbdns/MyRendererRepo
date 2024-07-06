#include"util.hlsl"
vertexout VS(vertexin pin)
{
    vertexout vout = (vertexout) 0;
    float4 Position = mul(pin.position, world);
    float4 posW = Position;
    vout.positionH = float4(float2(pin.uv) * 2.0f - 1.0f, 0.5f, 1.0f);
    vout.positionW = posW;
    vout.uv = pin.uv;
    vout.color = pin.color;
    vout.normal = mul(float4(pin.noraml, 1.0f), worldinvT);
    return vout;
}
float2 calcStretch(float3 wpos)
{
    float3 du = ddx(wpos);
    float3 dv = ddy(wpos);
    return float2(1.0f / length(du), 1.0f / length(dv));
}
float4 PS(vertexout pin) :SV_Target
{
    pin.uv = float2(pin.uv.x, (1.0f - pin.uv.y));
    float2 stretchUV = calcStretch(pin.positionW);
    return float4(stretchUV.x, stretchUV.y, stretchUV.y, 1.0f)/35.0f;
}