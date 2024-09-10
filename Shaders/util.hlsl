#define PI 3.1415926f
#define width 1024
#define height 768
struct ray
{
    float3 direction;
    float3 origin;
};
struct AABBbox
{
    float3 center;
    uint isLeaf;
    float3 extent;
    int missIndex;
    uint triangleStart;
    uint triangleNum;
};
struct MyTriangle
{
    float3 pos1;
    int materialIndex;
    float3 pos2;
    int texIndex;
    float3 pos3;
    float pad1;
    float3 color;
    float pad2;
    float3 n;
    float pad3;
    float4 uv12;
    float2 uv3;
};
struct material
{
    float3 albedo;
    float roughness;
    float3 F0;
};
struct PolygonalLight
{
    float area;
    float3 v1;
    float3 v2;
    float3 v3;
    float3 v4;
    float3 color;
    float3 normal;
};
struct ParallelLight
{
    float3 direction;
    float3 color;
};
struct vertexin
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 noraml : NORMAL;
    float3 tangentU : TANGENT;
    float AOk : AOk;
    float3 color : COLOR;
};
struct mvertexin
{
    float4 position;
    float2 texuv;
    float3 normal;
    float4 tangent;
    float AOk;
    float3 color;
    float4 blendW;
    uint4 blendIndex;
};
struct vertexout
{
    float4 positionH : SV_POSITION;
    float3 positionW : POSITIONT;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tanegntU : TANGENT;
    float3 color : COLOR;
    float3 skyVec : TEXCOORD1;
};


Texture2D g_tex[9] : register(t0);
StructuredBuffer<AABBbox> g_boxData : register(t0, space1);
StructuredBuffer<MyTriangle> g_triangleData : register(t1, space1);
StructuredBuffer<material> g_materialData : register(t2, space1);
StructuredBuffer<float> g_randnums : register(t3, space1);
StructuredBuffer<PolygonalLight> g_lights : register(t4, space1);
Texture2D g_lastFrame : register(t5, space1);
Texture2D g_DirectL : register(t6, space1);
Texture2D g_IndirectL : register(t7, space1);
Texture2D g_CDF_1 : register(t8, space1);
TextureCube g_sky : register(t9, space1);
Texture2D g_normMap : register(t10, space1);
Texture2D g_beckmanMap : register(t11, space1);
StructuredBuffer<float3> g_sumGweights : register(t12, space1);
RWTexture2D<float4> g_irradianceTex : register(u0, space1);
Texture2D g_stretchMap : register(t13, space1);
StructuredBuffer<ParallelLight> g_parallelLights : register(t14, space1);
Texture2D g_shadowMap : register(t15, space1);
Texture2D g_noise : register(t16, space1);
Texture2D g_convolveTex[6] : register(t0, space2);
RWTexture2D<float4> g_Reservoir[8] : register(u0, space3);
SamplerState g_sampler : register(s0);
cbuffer passcb : register(b1)
{
    float4x4 MWVP;
    float3 eyepos;
    float pad1;
    float3 AL;
    float pad2;
    float3 BL;
    float m;
    float3 lightdir;
    float pad3;
    float4 md;
    float3 r0;
    float pad4;
    float4x4 V;
    float4x4 W;
    float4x4 VP;
    float4x4 WinvT;
    float4x4 P;
    float4x4 S;
    uint boxNum;
    uint nFrame;
    uint curtime;
}
cbuffer objcb : register(b0)
{
    float4x4 world;
    float4x4 worldinvT;
    int texIndex;
    int matIndex;
}
cbuffer SSSS : register(b2)
{
    float roughness1;
}
struct Material
{
    bool isGlossy;
    float3 color;
    float roughness;
    float3 F0;
    
};


float4 getRandomNum(float2 seed)
{
    float2 s = seed;
    float4 ret = g_noise.SampleLevel(g_sampler, s, 0).xyzw;
    ret = ret * 10.0f - int4(ret * 10);
    return ret;
}

int Xplus(float alpha)
{
    if (alpha > 0)
        return 1;
    else
        return 0;
}
float GGX(float3 alpha, float3 n, float3 h)
{
    alpha = alpha * alpha;
    float ret = (pow(alpha, 2) * Xplus(dot(h, n))) / (3.1415926585f * pow((1 + pow(dot(n, h), 2) * (alpha * alpha - 1)), 2));
    if (ret > 10)
        return 10;
    
    return ret;
}
float3 SchlickFresnel(float3 F0, float3 n, float3 i)
{
    return F0 + (1 - F0) * pow((1 - max(0, dot(n, i))), 5);
    

}
float3 smooth_SurfaceDiff(float3 F0, float3 albedo, float3 n, float3 l, float Dotnv)
{
    float k = (21 / (20 * 3.1415926));
    float3 base1 = (1 - pow(1 - max(0, dot(n, l)), 5));
    float3 base2 = (1 - pow(1 - max(0, Dotnv), 5));
    return k * (1 - F0) * base1 * base2 * albedo;

}
float lambda(float alpha)
{
    return (-1 + pow((1 + (1 / dot(alpha, alpha))), 0.5)) / 2;

}
float smithG2(float3 h, float3 l, float3 n, float roughness, float Dotnv, float Dothv)
{
    float a1 = dot(n, l) / (roughness * (pow((1 - pow(dot(n, l), 2)), 0.5))+0.001f);
    float a2 = Dotnv / (roughness * (pow((1 - pow(Dotnv, 2)), 0.5))+0.001f);
    float3 base1 = 1 + lambda(a1) + lambda(a2);
    float3 base2 = Xplus(Dothv) * Xplus(dot(h, l));
    return base2 / base1;
}
float3 Rd(float3 d, float r)
{
    return (exp(-r / d) + exp(-r / (3 * d))) / (8 * PI * d * r);
}


float4 UniformSampleHemisphere(float2 E, float3 n)
{
    float Phi = 2 * 3.1415926f * E.x;
    float CosTheta = E.y;
    float SinTheta = sqrt(1 - CosTheta * CosTheta);

    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;
    float PDF = 1.0 / (2 * PI);
    float3 up = abs(n.z) < 0.999 ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
    float3 tangent = normalize(cross(up, n));
    float3 bitangent = cross(n, tangent);
    float3 sampleVec = tangent * H.x + bitangent * H.y + n * H.z; //注意是左乘矩阵
    return float4(normalize(sampleVec), PDF);
}
float4 ImportanceSampleFromGGX(float roughness, float2 seed, float3 R)
{
    float randnum1 = getRandomNum(seed);
    float randnum2 = getRandomNum(float2(seed.y, seed.x));
    float a = roughness * roughness;
    float phi = 2.0f * PI * randnum1;
    float cosTheta = sqrt((1.0f - randnum2) / (1.0f + (a * a - 1.0f) * randnum2));
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    float3 H; //关于GGX重要性采样
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    float3 up = abs(R.z) < 0.999 ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
    float3 tangent = normalize(cross(up, R));
    float3 bitangent = cross(R, tangent);
    float3 sampleVec = tangent * H.x + bitangent * H.y + R * H.z; //将GGX采样结果转换到世界坐标
    float d = (cosTheta * a - cosTheta) * cosTheta + 1;
    float D = a / (PI * d * d);
    float PDF = D * cosTheta; //PDF用于蒙特卡洛
    return float4(sampleVec, PDF);
}
bool RayIntersectTriangle(float3 pos0, float3 pos1, float3 pos2, ray r, out float2 b1b2, out float tout)
{
    float3 E1 = pos1 - pos0;
    float3 E2 = pos2 - pos0;
    float3 S = r.origin - pos0;
    float3 S1 = cross(r.direction, E2);
    float3 S2 = cross(S, E1);
    float c = 1 / dot(S1, E1);
    float t = c * dot(S2, E2);
    float b1 = c * dot(S1, S);
    float b2 = c * dot(S2, r.direction);
    if (b1 > 0 && b2 > 0 && (1 - b1 - b2) > 0 && t > 0)
    {
        tout = t;
        b1b2 = float2(b1, b2);
        return true;
    }
    return false;
}
bool RayIntersectAABBBox(float3 boxCenter, float3 boxExtent, ray r)
{
    float tminx, tminy, tminz;
    float tmaxx, tmaxy, tmaxz;
    float t1, t2;
    t1 = (boxCenter.x - boxExtent.x - r.origin.x) / r.direction.x;
    t2 = (boxCenter.x + boxExtent.x - r.origin.x) / r.direction.x;
    tminx = (t2 > t1) ? t1 : t2;
    tmaxx = (t2 > t1) ? t2 : t1;
    t1 = (boxCenter.y - boxExtent.y - r.origin.y) / r.direction.y;
    t2 = (boxCenter.y + boxExtent.y - r.origin.y) / r.direction.y;
    tminy = (t2 > t1) ? t1 : t2;
    tmaxy = (t2 > t1) ? t2 : t1;
    t1 = (boxCenter.z - boxExtent.z - r.origin.z) / r.direction.z;
    t2 = (boxCenter.z + boxExtent.z - r.origin.z) / r.direction.z;
    tminz = (t2 > t1) ? t1 : t2;
    tmaxz = (t2 > t1) ? t2 : t1;
    float tEnter = max(max(tminx, tminy), tminz);
    float tExit = min(min(tmaxx, tmaxy), tmaxz);
    if (tEnter < tExit && tExit > 0)
        return true;
    return false;
}
bool RayIntersectScene(ray r, out MyTriangle outtri, out float2 b1b2, out float tout)
{
    tout = 100000;
    AABBbox box;
    bool intersect = false;
    int i = 0;
    while (i < boxNum)
    {
        box = g_boxData[i];
        if (RayIntersectAABBBox(box.center, box.extent, r))
        {
            if (box.isLeaf)
            {
                for (int j = 0; j < box.triangleNum; j++)
                {
                    MyTriangle tri = g_triangleData[box.triangleStart + j];
                    float t;
                    float2 b1b2hat;
                    if (RayIntersectTriangle(tri.pos1, tri.pos2, tri.pos3, r, b1b2hat, t))
                    {
                        if (t < tout)
                        {
                            outtri = tri;
                            tout = t;
                            b1b2 = b1b2hat;
                        }
                        intersect = true;
                    }
                }
            }
            i++;
        }
        else
        {
            if (box.missIndex > 0)
            {
                i = box.missIndex;
                continue;
            }
            else
                return intersect;
        }
    }
    return intersect;
}
#define Noisesize 512
float2 getSeedF2FromSeedInt2(int2 seed,int index){
    int2 seedInt2 = seed + int2(index % Noisesize, index / Noisesize); //当前像素坐标加上以坐标索引为scale的偏移
    float2 seedF2 = float2((seedInt2.x % Noisesize) / float(Noisesize), (seedInt2.y % Noisesize) / float(Noisesize));
    return seedF2;
}
float3 BRDF(Material m, float3 toEyeNorm, float3 ToLightNorm, float3 spNormal)
{
    if (m.isGlossy)
    {
        float3 h = normalize(toEyeNorm + ToLightNorm);
        float3 F = SchlickFresnel(float3(1.0f, 1.0f, 1.0f), spNormal, h);
        float G = smithG2(h, ToLightNorm, spNormal, m.roughness, dot(spNormal, toEyeNorm), dot(h, toEyeNorm));
        float D = GGX(m.roughness, spNormal, h);
        return G * D * F / (4 * abs(dot(spNormal, ToLightNorm)) * abs(dot(spNormal, toEyeNorm)))+m.color/2;

    }
    else
    {
        return m.color / PI;
    }
}

float getMax(float3 s)
{
    
    return max(max(s.x, s.y), s.z);

}
struct Reservoir
{
    
    float3 samplePos; //tex1
    float Wsum;

    float3 sampleNorm; //tex2
    float M;

    float3 sampleColor; //tex3
    float W;

    bool update(float3 pos, float3 norm, float3 color, float wi, float2 seedF2)
    {
        Wsum += wi;
        M++;
        if (getRandomNum(seedF2).x < (wi / Wsum))
        {
            samplePos = pos;
            sampleNorm = norm;
            sampleColor = color;
            return true;
        }
        return false;
    }
    void init()
    {
        samplePos = float3(0, 0, 0);
        sampleNorm = float3(0, 0, 0);
        sampleColor = float3(0, 0, 0);
        float Wsum = 0;
        float M = 0;
        float W = 0;
    }
};
Reservoir
    RIS(
    float3 shadingPoint, float3 spNormal, float3 spColor, int M, int2 seed, Material
    m,
    float3 ToEyeNorm, float emitterArea,int nframe)
{
    Reservoir r;
    r.init();
    float samplePhat;
    for (int i = 0; i < M; i++)
    {
        float randnum1;
        float randnum2;
        float randnum3;
        if (i == 0)
        {
             randnum1 = getRandomNum(getSeedF2FromSeedInt2(seed, nframe)).r;
             randnum2 = getRandomNum(getSeedF2FromSeedInt2(seed, nframe)).g;
             randnum3 = getRandomNum(getSeedF2FromSeedInt2(seed, nframe*3)).b;
        }
        else
        {
            randnum1 = getRandomNum(getSeedF2FromSeedInt2(seed, nframe)).b;
            randnum2 = getRandomNum(getSeedF2FromSeedInt2(seed, nframe)).a;
            randnum3 = getRandomNum(getSeedF2FromSeedInt2(seed, nframe*3)).r;
        }
        int lightIndex = randnum3*100;
        lightIndex = clamp(lightIndex, 0, 100);
        PolygonalLight l = g_lights[lightIndex];
        float3 v1tov2 = l.v2 - l.v1;
        float3 v1tov3 = l.v3 - l.v1;
        float3 sampleP1 = l.v1 + randnum1 * v1tov2;
        float3 sampleLightPos = sampleP1 + v1tov3 * randnum2;
        float3 brdf = BRDF(m, ToEyeNorm, normalize(sampleLightPos - shadingPoint), spNormal);
        float3 toLight = sampleLightPos - shadingPoint;
        float3 ToLightNorm = normalize(toLight);
        float cos1 = max(0, dot(spNormal, ToLightNorm));
        float cos2 = max(0, dot(normalize(l.normal), -ToLightNorm));
        float LengthSquare = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
        float phat = brdf * getMax(l.color) *  cos1 * cos2 / LengthSquare;
        if (r.update(sampleLightPos, l.normal, l.color, phat * emitterArea, getSeedF2FromSeedInt2(seed, i * i + nFrame)))
        samplePhat = phat;
        
    }
    r.W = r.Wsum / (r.M * samplePhat);
    return r;
  
}



float3 calcDirectLightUseRIS(float3 shadingPoint, float3 spNormal, float3 spColor, int M, int2 seed, Material m, float3 ToEyeNorm, float emitterArea,int nframe)
{
    Reservoir r = RIS(shadingPoint, spNormal, spColor, M, seed, m, ToEyeNorm, emitterArea,nframe);
    float3 toLight = r.samplePos - shadingPoint;
    float3 ToLightNorm = normalize(toLight);
    float cos1 = max(0, dot(spNormal, ToLightNorm));
    float cos2 = max(0, dot(normalize(r.sampleNorm), -ToLightNorm));
    float LengthSquare = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
    float3 brdf = BRDF(m, ToEyeNorm, normalize(r.samplePos - shadingPoint), spNormal);
    return brdf * r.sampleColor * cos1 * cos2 / LengthSquare * r.W ;
}




float3 calcGlossyDirectLightFromPolygonalLight(float3 shadingPoint, float3 spNormal, float3 spColor, int2 seed, float3 receiverPos, float roughness)
{
    float randnum1 = getRandomNum(getSeedF2FromSeedInt2(seed, 0));
    float randnum2 = getRandomNum(getSeedF2FromSeedInt2(seed + seed.x, 0));
    int lightIndex = 100 * randnum1;
    PolygonalLight l = g_lights[lightIndex];
    float3 v1tov2 = l.v2 - l.v1;
    float3 v1tov3 = l.v3 - l.v1;
    float3 sampleP1 = l.v1 + randnum1 * v1tov2;
    float3 sampleLightPos = sampleP1 + v1tov3 * randnum2;
    float3 posToLight = sampleLightPos - shadingPoint;
    float3 toLight = sampleLightPos - shadingPoint;
    float3 ToLightNorm = normalize(toLight);
    float cos1 = max(0, dot(spNormal, ToLightNorm));
    float cos2 = max(0, dot(normalize(l.normal), -ToLightNorm));
    float LengthSquare = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
    float3 toReceiverNorm = normalize(receiverPos - shadingPoint);
    float3 h = normalize(toReceiverNorm + ToLightNorm);
    float3 F = SchlickFresnel(float3(1.0f, 1.0f, 1.0f), spNormal, h);
    float G = smithG2(h, ToLightNorm, spNormal, roughness, dot(spNormal, toReceiverNorm), dot(h, toReceiverNorm));
    float D = GGX(roughness, spNormal, h);
    float3 BRDF = G * D * F / (4 * abs(dot(spNormal, ToLightNorm)) * abs(dot(spNormal, toReceiverNorm)));
    return BRDF * l.color * cos1 * cos2 / LengthSquare * l.area * 100;
}
#define Rmax 5.0f
#define sampleNum 5 
float3 calcTranslucentDirectLightFromPolygonalLightt(float3 shadingPoint, float3 spNormal, float3 spTangent, float3 spColor, int2 seed, float3 F0, float3 d, float3 toEye)
{
    float randnum1 = getRandomNum(getSeedF2FromSeedInt2(seed, 0));
    float randnum2 = getRandomNum(getSeedF2FromSeedInt2(seed + seed.x, 0));
    int a = (randnum1 > 0.5f) ? 1 : -1;
    float importantSampleR = (g_CDF_1.Sample(g_sampler, float2(min(randnum1, 0.95f), 0)).xyz * d).x; //重要性采样R
    float3 v1tov2 = g_lights[0].v2 - g_lights[0].v1;
    float3 v1tov3 = g_lights[0].v3 - g_lights[0].v1;
    float3 sampleP1 = g_lights[0].v1 + randnum1 * v1tov2;
    float3 sampleLightPos = sampleP1 + v1tov3 * randnum2;
    float3 DirectL = float3(0, 0, 0);
    int N = 0;
    for (int i = 0; i < sampleNum; i++)
    {
        float randAngle = getRandomNum(seed + i) * 2 * PI;
        float3 randVector = float3(cos(randAngle), 0, sin(randAngle));
        float3 up = abs(spNormal.z) < 0.999 ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
        float3 tangent = normalize(cross(up, spNormal));
        float3 bitangent = cross(spNormal, tangent);
        float3 sampleVec = tangent * randVector.x + bitangent * randVector.y + spNormal * randVector.z;
        float3 rayOrigin = shadingPoint + spNormal * Rmax + sampleVec * importantSampleR * a;
        ray R;
        MyTriangle tri;
        float2 b1b2;
        float tout;
        R.origin = rayOrigin;
        R.direction = -spNormal;
        float r = 0;
        float3 hitNormal = float3(0, 0, 0);
        float3 hitPoint = float3(0, 0, 0);
        if (RayIntersectScene(R, tri, b1b2, tout))
        {
            hitPoint = tri.pos1 * (1 - b1b2.x - b1b2.y) + tri.pos2 * b1b2.x + tri.pos3 * b1b2.y;
            r = length(hitPoint - shadingPoint);
            if (r < Rmax * 1.2f)
            {
                hitNormal = tri.n;
                N++;
            }
            else
                continue;
        }
        else
            continue;
        float3 PDF = 2 * PI * importantSampleR * max(0, dot(spNormal, hitNormal)) * Rd(d, importantSampleR);
        float3 toLight = sampleLightPos - hitPoint;
        float3 ToLightNorm = normalize(toLight);
        float cos1 = max(0, dot(hitNormal, ToLightNorm));
        float cos2 = max(0, dot(normalize(g_lights[0].normal), -ToLightNorm));
        float LengthSquare = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
        float3 LightTerm = float3(1.0, 1.0, 1.0) * dot(float3(0, 1.0f, -1.0f), spNormal); //g_lights[0].color * cos1 * cos2 / LengthSquare * g_lights[0].area *100;
        float3 F2Term = SchlickFresnel(F0, hitNormal, float3(0, 1.0f, -1.0f)) * SchlickFresnel(F0, spNormal, toEye);
        DirectL += LightTerm / PDF * spColor * F2Term * Rd(d, r) / PI;
    }
    DirectL /= N;
    return DirectL;
}

float3 calcDiffuseDirectLightFromPolygonalLight(float3 shadingPoint, float3 spNormal, float3 spColor, int2 seed)
{
    float randnum1 = getRandomNum(getSeedF2FromSeedInt2(seed, 0));
    float randnum2 = getRandomNum(getSeedF2FromSeedInt2(seed + seed.x, 0));
    int lightIndex = 100 * randnum1;
    PolygonalLight l = g_lights[lightIndex];
    float3 v1tov2 = l.v2 -l.v1;
    float3 v1tov3 = l.v3 - l.v1;
    float3 sampleP1 = l.v1 + randnum1 * v1tov2;
    float3 sampleLightPos = sampleP1 + v1tov3 * randnum2;
    float3 posToLight = sampleLightPos - shadingPoint;
    float3 toLight = sampleLightPos - shadingPoint;
    float3 ToLightNorm = normalize(toLight);
    float cos1 = max(0, dot(spNormal, ToLightNorm));
    float cos2 = max(0, dot(normalize(l.normal), -ToLightNorm));
    float LengthSquare = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
    return spColor / PI * l.color * cos1 * cos2 / LengthSquare * l.area * 100;
}
