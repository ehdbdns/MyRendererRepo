#include"util.hlsl"
vertexout VS(vertexin pin)
{
    vertexout vout = (vertexout) 0;
    float4 Position = mul(pin.position, world);
    float4 posW = Position;
    vout.positionH = mul(Position, VP);
    vout.positionW = mul(Position, VP);
    vout.uv = pin.uv;
    vout.color = pin.color;
    vout.normal = mul(float4(pin.noraml, 1.0f), worldinvT);
    return vout;
}
void PS(vertexout pin)
{

}