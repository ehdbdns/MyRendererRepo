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
struct cylinder
{
    float3 center;
    float h;
    float r;
};
float2 quadratic(float a, float b, float c)
{
    float delta = b * b - 4 * a * c;
    if (delta < 0)
        return float2(-1, -1);
    
    float base = -b / (2 * a);
    float sqrtdelta = sqrt((delta)) / (2 * a);
    return float2(base + sqrtdelta, base - sqrtdelta);
}
bool intersectDisk(float3 center, float r, ray ray,out float tout)
{
    float t = (center.y - ray.origin.y) / ray.direction.y;
    float3 hp = ray.origin + ray.direction * t;
    if (dot(hp - center, hp - center) <= r * r&&t>0)
    {
        tout = (t > 0 && dot(hp - center, hp - center) <= r * r) ? t : 100000;
        return true;
    }
    return false;
}
bool rayClosestHit(ray r,float3 diskCenter,float radius,float h) {
    float3 peakPoint = diskCenter + float3(0, h, 0);
    float deltaX = r.origin.x - peakPoint.x;
    float deltaY = r.origin.y - peakPoint.y;
    float deltaZ = r.origin.z - peakPoint.z;
    float alpha = atan(radius / h);
float cota2 = 1.0f / (tan(alpha) * tan(alpha));
float a = cota2 * (r.direction.x * r.direction.x + r.direction.z * r.direction.z) - r.direction.y * r.direction.y;
float b = 2 * (cota2 * (deltaX * r.direction.x + deltaZ * r.direction.z) - deltaY * r.direction.y);
float c = cota2 * (deltaX * deltaX + deltaZ * deltaZ) - deltaY * deltaY;
float2 result = quadratic(a, b, c);
      if (result.x > result.y)
    {
        float t = result.y;
        result.y = result.x;
        result.x = t;
    }
      if (result.x > 0)
      {
        float hity = r.origin.y + r.direction.y * result.x;
          if (hity > diskCenter.y && hity < peakPoint.y)
              return true;
      }
      if (result.y > 0 && result.x != result.y)
      {
        float hity = r.origin.y + r.direction.y * result.y;
          if (hity > diskCenter.y && hity < peakPoint.y)
              return true;
      }
      return false;
  }

bool rayClosestHitSphere(ray r, float3 center,float radius)  {
float deltaX = r.origin.x - center.x;
float deltaY = r.origin.y - center.y;
float deltaZ = r.origin.z - center.z;
float a = r.direction.x * r.direction.x + r.direction.y * r.direction.y + r.direction.z * r.direction.z;
float b = 2 * (deltaX * r.direction.x + deltaY * r.direction.y + deltaZ * r.direction.z);
float c = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ - radius * radius;
float2 result = quadratic(a, b, c);
      if (result.x > result.y)
      {
          float t = result.y;
        result.y = result.x;
        result.x = t;
      }
      if (result.x > 0) {
          return true;
      }
      if (result.y > 0) {
          return true;
      }
     return false;
  }


        bool rayClosestHitCylinder(ray r, float3 diskCenter,float radius,float h,out float tout) {
    float deltaX = r.origin.x - diskCenter.x;
    float deltaZ = r.origin.z - diskCenter.z;
    float a = r.direction.x * r.direction.x + r.direction.z * r.direction.z;
    float b = 2 * (deltaX * r.direction.x + deltaZ * r.direction.z);
    float c = deltaX * deltaX + deltaZ * deltaZ - radius * radius;
    float2 result = quadratic(a, b, c);
    bool hit = false;
    if (result.x > result.y)
    {
        float t = result.y;
        result.y = result.x;
        result.x = t;
    }
    float tSide = 1000000;
    if (result.x > 0)
    {
        float hity = r.origin.y + r.direction.y * result.x;
        if (hity > diskCenter.y && hity < diskCenter.y + h)
        {
            tSide = result.x;
            hit = true;
        }
    }
    else
    {
        if (result.y > 0 && result.x != result.y)
        {
            float hity = r.origin.y + r.direction.y * result.y;
            if (hity > diskCenter.y && hity < diskCenter.y + h)
            {
                tSide = result.y;
                hit = true;
            }
        }
    }
    float t1 = 1000000;
    float t2 = 1000000;
    float tUporDown = 1000000;
    if (intersectDisk(diskCenter, radius, r, t1))
    {
        hit = true;
    }
    if (intersectDisk(diskCenter + float3(0,h,0),radius, r, t2)) {
        tUporDown = min(t2, t1);
        hit = true;
    }
    if (hit)
    {
        tout = min(tUporDown, tSide);
        return true;
    }
    return false;
}
float4 PS(vertexout pin) : SV_Target
{
    float4 texcolor = (texIndex >= 0) ? g_tex[texIndex].Sample(g_sampler, pin.uv) : float4(pin.color, 1.0f);
    float3 toEye = normalize(eyepos - pin.positionW);
    float3 norN = normalize(pin.normal);
    float3 DirectL = calcDiffuseDirectLightFromPolygonalLight(pin.positionW, norN, float3(1.0, 1.0, 1.0), int2(pin.positionH.xy));
    return float4(DirectL, 1.0f);

}