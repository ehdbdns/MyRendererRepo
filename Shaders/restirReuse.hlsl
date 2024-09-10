#include"csUtil.hlsl"
Reservoir getLastFrameReservoir( int2 DispatchThreadId,float3 wpos)
{
    Reservoir r;
    float4 backProjectCoord = mul(float4(wpos, 1.0f), VP);
    float2 backProjectScreenCoord = ((backProjectCoord / backProjectCoord.w).xy + 1.0f) / 2.0f;
    backProjectScreenCoord.y = (1.0f - backProjectScreenCoord.y);
    int2 lastIndex = (backProjectScreenCoord * float2(1024.0f, 768.0f)); //通过motionVector得到上一帧坐标
    if (backProjectScreenCoord.x > 0 && backProjectScreenCoord.x < 1.0f && backProjectScreenCoord.y > 0 && backProjectScreenCoord.y < 1.0f)
    {
        r.set(ReservoirBuffers[4][lastIndex].xyz, ReservoirBuffers[4][lastIndex].w, ReservoirBuffers[5][lastIndex].xyz, ReservoirBuffers[5][lastIndex].w, ReservoirBuffers[6][lastIndex].xyz, ReservoirBuffers[6][lastIndex].w);
    }
    else
    {
        r.init();
        r.M = -1;

    }
    return r;
}
Reservoir getCurrentFrameReservoir( int2 DispatchThreadId)
{
    Reservoir r;
    r.set(ReservoirBuffers[0][DispatchThreadId].xyz, ReservoirBuffers[0][DispatchThreadId].w, ReservoirBuffers[1][DispatchThreadId].xyz, ReservoirBuffers[1][DispatchThreadId].w, ReservoirBuffers[2][DispatchThreadId].xyz, ReservoirBuffers[2][DispatchThreadId].w);
    return r;
}

void copyCurrentReservoirToTemporalBuffer(int2 DispatchThreadId, Reservoir currentReservoir)
{
    ReservoirBuffers[0][DispatchThreadId] = float4(currentReservoir.samplePos, currentReservoir.Wsum);
    ReservoirBuffers[1][DispatchThreadId] = float4(currentReservoir.sampleNorm, currentReservoir.M);
    ReservoirBuffers[2][DispatchThreadId] = float4(currentReservoir.sampleColor, currentReservoir.W);
}
void copyCurrentReservoirToSpatialBuffer(int2 DispatchThreadId, Reservoir currentReservoir)
{
    ReservoirBuffers[8][DispatchThreadId] = float4(currentReservoir.samplePos, currentReservoir.Wsum);
    ReservoirBuffers[9][DispatchThreadId] = float4(currentReservoir.sampleNorm, currentReservoir.M);
    ReservoirBuffers[10][DispatchThreadId] = float4(currentReservoir.sampleColor, currentReservoir.W);
}

[numthreads(16, 16, 1)]
void CS(uint2 GroupId : SV_GroupID,
				 uint GroupThreadIndex : SV_GroupIndex,
				 uint2 DispatchThreadId : SV_DispatchThreadID)
{
    float2 uv = float2(float2(DispatchThreadId.xy)+float2(0.5,0.5)) / float2(1024, 768);
    float3 posW = g_worldpos.SampleLevel(g_sampler, uv, 0);
    float3 normal = normalize(g_normal.SampleLevel(g_sampler, uv, 0).xyz * 2.0f - 1.0f);
    float3 color = float3(1.0, 1.0, 1.0);
  
    Reservoir lastReservoir;
    lastReservoir = getLastFrameReservoir(DispatchThreadId.xy,posW);
    
    Reservoir currentReservoir;
    currentReservoir = getCurrentFrameReservoir(DispatchThreadId.xy);
    
    Reservoir rs[20];
    rs[1] = lastReservoir;
    rs[0] = currentReservoir;
    //Temporal Reuse
    if (!isMoving)
    {
        if (lastReservoir.Wsum > 0 && currentReservoir.Wsum > 0 && lastReservoir.M > 0)
            currentReservoir = combineReservoirs(rs, 2, getSeedF2FromSeedInt2(int2(DispatchThreadId.xy + 0.5f), nFrame), posW, normal, color); //nFrame作为采样随机数的偏移
    }
    
    GroupMemoryBarrierWithGroupSync();
    copyCurrentReservoirToTemporalBuffer(DispatchThreadId.xy, currentReservoir);
    GroupMemoryBarrierWithGroupSync();
    
     //Spatial Reuse
    for (int n = 0; n < 3; n++)
    {
        //for (int i = -1; i < 2; i++)
        //{
        //    for (int j = -1; j < 2; j++)
        //    {
        //        rs[(i + 1) * 3 + j + 1] = getCurrentFrameReservoir(DispatchThreadId.xy + int2(i, j));
        //    }
        //}//如果combinen x n方格内所有Reservoir，会出现方格状的artifact，所以我在5 x 5 内随机采样本 
        for (int i = 0; i < 4; i++)
        {
            float randnum1 = getRandomNum(getSeedF2FromSeedInt2(DispatchThreadId.xy, i + nframe));
            float randnum2 = getRandomNum(getSeedF2FromSeedInt2(DispatchThreadId.xy, i + nframe * nframe));
            int Xoffset = randnum1 * 5 - 2;
            int Yoffset = randnum2 * 5 - 2;
            rs[i] = getCurrentFrameReservoir(DispatchThreadId.xy + int2(Xoffset, Yoffset));
        }
        currentReservoir = combineReservoirs(rs, 4, getSeedF2FromSeedInt2(int2(DispatchThreadId.xy + 0.5f), nFrame), posW, normal, color);
        GroupMemoryBarrierWithGroupSync();
        copyCurrentReservoirToSpatialBuffer(DispatchThreadId.xy, currentReservoir);
        GroupMemoryBarrierWithGroupSync();
    }

    
    //compute the final result!!!
    Material m;
    m.color = color;
    m.F0 = float3(1, 1, 1);
    m.isGlossy = true;
    m.roughness = 0.1f;
    float3 ToEyeNorm = normalize(eyePos - posW);
    float3 toLight = currentReservoir.samplePos - posW;
    float3 ToLightNorm = normalize(toLight);
    float cos1 = max(0, dot(normal, ToLightNorm));
    float cos2 = max(0, dot(normalize(currentReservoir.sampleNorm), -ToLightNorm));
    float LengthSquare = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
    float3 brdf = BRDF(m, ToEyeNorm, normalize(currentReservoir.samplePos - posW), normal);
    DstTexture[DispatchThreadId.xy] = float4(brdf * currentReservoir.sampleColor * cos1 * cos2 / LengthSquare * currentReservoir.W,1.0f)*2; //BRDF*Li*cos1*cos2/LengthSquare*r.W;其实就是路径追踪的公式
    
    GroupMemoryBarrierWithGroupSync();
}