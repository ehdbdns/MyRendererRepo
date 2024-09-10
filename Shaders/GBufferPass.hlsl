#include"util.hlsl"
struct outPut
{
    float4 depth : SV_Target0;
    float4 normal : SV_Target1;
    float4 matte : SV_Target2;
    float4 wpos : SV_Target3;
    float4 gdepth : SV_Target4;
};
vertexout VS(vertexin pin)
{
    vertexout vout = (vertexout) 0;
    float4 Position = mul(pin.position, world);
    float4 posW = Position;
    vout.positionH = mul(Position, VP);
    vout.positionW = posW;
    vout.uv = pin.uv;
    vout.color = pin.color;
    vout.normal = pin.noraml;
    vout.tanegntU = mul(Position, V);
    return vout;
}
outPut PS(vertexout pin) : SV_Target
{
    float4 texcolor = (texIndex >= 0) ? g_tex[texIndex].Sample(g_sampler, float2(pin.uv.x, (1.0f - pin.uv.y))) : float4(pin.color, 1.0f);
    outPut output;
    output.depth = float4(pin.tanegntU.z, pin.uv.x, (1.0f - pin.uv.y), 0); //tan=posV
    output.normal = float4((pin.normal + 1.0f) / 2.0f, 1.0f);
    output.matte = 1;
    output.wpos = float4(pin.positionW,1.0f);
    output.gdepth = float4(ddx(pin.tanegntU.z), ddy(pin.tanegntU.z), 0, 1.0f);
    return output;
}