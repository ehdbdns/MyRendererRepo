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
float4 PS(vertexout pin) : SV_Target
{
    float4 texcolor = (texIndex >= 0) ? g_tex[texIndex].Sample(g_sampler, pin.uv) : float4(pin.color, 1.0f);
    float3 toEye = normalize(eyepos - pin.positionW);
    float3 norN = normalize(pin.normal);
    float4 traceRayAndPdf = float4(0, 0, 0, 0);
    ray R;
    R.origin = pin.positionW;
    float3 IndirectL = float3(0, 0, 0);
    MyTriangle tri;
    float2 b1b2;
    float3 accumulateBRDF = float3(1.0,1.0,1.0) / PI;
    for (int i = 0; i < 1; i++)
    {
        float2 random = getRandomNum(int2(pin.positionH.xy));
        traceRayAndPdf = UniformSampleHemisphere(random, norN);
        R.direction = traceRayAndPdf.xyz;
        float t;
        if (RayIntersectScene(R, tri, b1b2, t))
        {
            float cosTheta = dot(R.direction, norN);
            float3 hitPoint = tri.pos1 * (1 - b1b2.x - b1b2.y) + tri.pos2 * b1b2.x + tri.pos3 * b1b2.y;
            float u = tri.uv12.x * (1.0f - b1b2.x - b1b2.y) + tri.uv12.z * b1b2.x + tri.uv3.x * b1b2.y;
            float v = tri.uv12.y * (1.0f - b1b2.x - b1b2.y) + tri.uv12.w * b1b2.x + tri.uv3.y * b1b2.y;
            float3 hptexcolor = (tri.texIndex == -1) ? tri.color : g_tex[tri.texIndex].Sample(g_sampler, float2(u, v)).xyz;
            IndirectL += accumulateBRDF * cosTheta / traceRayAndPdf.w * calcDiffuseDirectLightFromPolygonalLight(hitPoint, normalize(tri.n), hptexcolor, int2(pin.positionH.xy));
            accumulateBRDF *= hptexcolor / PI;
        }
    }
    return float4(IndirectL, 1.0f);
}