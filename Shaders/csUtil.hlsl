#define PI 3.1415926f
Texture2D<float4> SrcTexture : register(t0);
Texture2D<float4> g_worldpos : register(t1);
Texture2D<float4> g_normal : register(t2);
StructuredBuffer<int2> g_offsets : register(t3);
StructuredBuffer<float> g_h : register(t4);
Texture2D<float4> g_z : register(t5);
Texture2D<float4> g_gz : register(t6);
Texture2D<float4> g_CDF_1 : register(t8);
Texture2D<float4> g_noise : register(t13);
RWTexture2D<float4> DstTexture : register(u0);
RWTexture2D<float4> moment1tex : register(u1);
RWTexture2D<float4> moment2tex : register(u2);
RWTexture2D<float4> historytex : register(u3);
RWTexture2D<float4> variancetex : register(u4);
RWTexture2D<float4> ReservoirBuffers[11] : register(u0,space1);
SamplerState g_sampler : register(s0);

struct Material
{
    bool isGlossy;
    float3 color;
    float roughness;
    float3 F0;
    
};
cbuffer lastVP : register(b0)
{
    float4x4 VP;
    float4x4 sumVP;
    uint nframe;
}
cbuffer passcb : register(b2)
{
    float nFrame;
    float3 eyePos;
    int isMoving;

}

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
    float a1 = dot(n, l) / (roughness * (pow((1 - pow(dot(n, l), 2)), 0.5)) + 0.001f);
    float a2 = Dotnv / (roughness * (pow((1 - pow(Dotnv, 2)), 0.5)) + 0.001f);
    float3 base1 = 1 + lambda(a1) + lambda(a2);
    float3 base2 = Xplus(Dothv) * Xplus(dot(h, l));
    return base2 / base1;
}
float3 Rd(float3 d, float r)
{
    return (exp(-r / d) + exp(-r / (3 * d))) / (8 * PI * d * r);
}

#define Noisesize 512
float2 getSeedF2FromSeedInt2(int2 seed, int index)
{
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
        return G * D * F / (4 * abs(dot(spNormal, ToLightNorm)) * abs(dot(spNormal, toEyeNorm))); // +m.color / 2;

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


    void update(float3 pos, float3 norm, float3 color, float wi, float2 seedF2)
    {
        Wsum += wi;
        M++;
        if (getRandomNum(seedF2).x < (wi / Wsum))
        {
            samplePos = pos;
            sampleNorm = norm;
            sampleColor = color;
        }
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
    void set(float3 spos, float wsum, float3 sn, float m, float3 sc, float w)
    {
         samplePos=spos; //tex1
         Wsum=wsum;

         sampleNorm=sn; //tex2
         M=m;

         sampleColor=sc; //tex3
         W=w;
    }
};

float calcPhat(float3 spPos, float3 spNorm, float3 spColor, float3 samplePos, float3 sampleNorm, float3 sampleColor)
{
    Material m;
    m.color = spColor;
    m.F0 = float3(1, 1, 1);
    m.isGlossy = true;
    m.roughness = 0.2f;
    float3 ToEyeNorm = normalize(eyePos - spPos);
    float3 brdf = BRDF(m, ToEyeNorm, normalize(samplePos - spPos), spNorm);
    float3 toLight = samplePos - spPos;
    float3 ToLightNorm = normalize(toLight);
    float cos1 = max(0, dot(spNorm, ToLightNorm));
    float cos2 = max(0, dot(normalize(sampleNorm), -ToLightNorm));
    float LengthSquare = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
    float phat = brdf * getMax(sampleColor) * cos1 * cos2 / LengthSquare;
    return phat;
}

Reservoir combineReservoirs(Reservoir reservoirs[20], int reservoirsNum, float2 seedf2,float3 spPos,float3 spNorm,float3 spColor)
{
    Reservoir s;
    s.init();
    for (int i = 0; i < reservoirsNum; i++)
    {
        Reservoir it = reservoirs[i];
        float samplePhat = calcPhat(spPos, spNorm, spColor, it.samplePos, it.sampleNorm, it.sampleColor);
        s.update(it.samplePos,it.sampleNorm,it.sampleColor, samplePhat * it.W * it.M, seedf2);
    }
    for (int j = 0; j < reservoirsNum; j++)
        s.M += reservoirs[j].M;
    s.W = s.Wsum / calcPhat(spPos,spNorm,spColor,s.samplePos,s.sampleNorm,s.sampleColor) / s.M;
    return s;
}