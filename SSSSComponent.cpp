#include"SSSSComponent.h"
void SSSSComponent::createResource(std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* textable,//创建资源,在创建资源项时调用
	ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap,
	ID3D12GraphicsCommandList* precmdlist, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NonRtDstable,
	std::unordered_map<std::string, std::unique_ptr< RT_DS_TextureSegregatedFreeLists>>* RtDstable, 
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>(*BufferResourceItemTable), std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>* upBSTable,
    std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>* defBSTable) {
    //创建所需纹理
    this->TexTable = textable;
    Device = device;
    SrvUavHeap = srvuavheap;
    auto filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);

    D3D12_RESOURCE_DESC filteredTexDesc = {};
    filteredTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    filteredTexDesc.Alignment = 0;
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexDesc.DepthOrArraySize = 1;
    filteredTexDesc.MipLevels = 1;
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexDesc.SampleDesc.Count = 1;
    filteredTexDesc.SampleDesc.Quality = 0;
    filteredTexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    filteredTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    D3D12_UNORDERED_ACCESS_VIEW_DESC filteredTexUavDesc = {};
    filteredTexUavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    filteredTexUavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexUavDesc.Texture2D.MipSlice = 0;
    filteredTexUavDesc.Texture2D.PlaneSlice = 0;

    D3D12_SHADER_RESOURCE_VIEW_DESC SRVdesc = {};
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    SRVdesc.Texture2D.MostDetailedMip = 0;
    SRVdesc.Texture2D.ResourceMinLODClamp = 0.0f;
    SRVdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;

    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    (*textable)["convolveBufferTex"] = std::move(filteredRI);

    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    (*textable)["convolveBufferTex1"] = std::move(filteredRI);

    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    (*textable)["beckmantex"] = std::move(filteredRI);//beckmanMap


    filteredTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    
    auto specularRI = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
    specularRI->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS1"].get());
    specularRI->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[specularRI->RTVHeap], precmdlist);
    specularRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[specularRI->SRVUAVHeap]);
    (*textable)["SSSSspecular"] = std::move(specularRI);//SPECULARtEX


    auto upBS = (*upBSTable)["UPBS1"].get();
    auto defBS = (*defBSTable)["DEFBS1"].get();

    SSSSps ssss;
    auto sssspsRI = std::make_unique<ConstantBufferResourceItem<SSSSps>>(device, precmdlist, &ssss, false, srvuavheap, upBS, defBS);
    ssssRI = std::move(sssspsRI);

}
void SSSSComponent::update(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//每一帧要做的事，case1调用
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
    D3D12_VIEWPORT* stViewPort, D3D12_RECT* stScissorRect, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable,
    std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI, std::unique_ptr<StructureBufferResourceItem<ParallelLight>>* ParaLightsSBRI,SSSSps* updateSSSS) {
    auto DS = (*TexTable)["DS"].get()->getDSVCPU();
    auto RSI = (*RootSignatureItemTable)["default"].get();
    auto SRI = (*SamplerResourceItemTable)["default"].get();
    auto passRI = (*BufferResourceItemTable)["pass"].get();
    auto convolveBuffer = (*TexTable)["convolveBufferTex"].get();
    ssssRI->updateCB(updateSSSS);
    precmdlist->SetGraphicsRootSignature(RSI->rs);
    ID3D12DescriptorHeap* heaps[2] = { convolveBuffer->SRVUAVHeap,SRI->samplerHeap };
    precmdlist->SetDescriptorHeaps(2, heaps);
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(1), passRI->getCBVGPU());
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getSamplerTableIndex(0), SRI->getSampler());
    //calc specular
    auto specularRI = ((*TexTable))["SSSSspecular"].get();
    auto headNormRI = ((*TexTable))["LPSHeadNorm"].get();
    precmdlist->ClearDepthStencilView(DS, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    precmdlist->RSSetViewports(1, stViewPort);
    precmdlist->RSSetScissorRects(1, stScissorRect);
    precmdlist->OMSetRenderTargets(1,&specularRI->getRTVCPU(), true, &DS);
    precmdlist->SetPipelineState((*PSOITable)["dualLobeSpecular"].get()->PSO);
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(11), headNormRI->getSRVGPU(0));
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(16), ParaLightsSBRI->get()->getSRVGPU());
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(2), ssssRI->getCBVGPU());
    auto LPSRI = (*NonSVGFRenderItemTable)["LPShead"].get();
    auto LPSRI1 = (*NonSVGFRenderItemTable)["LPShead1"].get();
    drawRenderItem(LPSRI, precmdlist, RSI->getCBVTableIndex(0));
    drawRenderItem(LPSRI1, precmdlist, RSI->getCBVTableIndex(0));
    ResourceBarrierTrans(specularRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);
}
void SSSSComponent::postProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),ID3D12GraphicsCommandList* postcmdlist,int RTnote, std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable),
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unique_ptr<StructureBufferResourceItem<XMFLOAT4>>* SSSSkernelSBRI ,ConstantBufferResourceItem<SSSS>* SSSSRI ) {
    auto specularRI = ((*TexTable))["SSSSspecular"].get();
    auto RenderTarget = (*TexTable)["backBuffer" + std::to_string(RTnote)].get();
    //convolve the RTTex in screen space
    ResourceBarrierTrans(RenderTarget->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
    auto RSIcs = (*RootSignatureItemTable)["cs"].get();
    auto SRI = (*SamplerResourceItemTable)["default"].get();
    auto convolveBuffer = (*TexTable)["convolveBufferTex"].get();
    auto convolveBuffer1 = (*TexTable)["convolveBufferTex1"].get();
    auto depthRI = (*TexTable)["depth"].get();
    auto gzRI = (*TexTable)["gz"].get();
    auto headDiffRI = (*TexTable)["LPSHeadDiff"].get();
    ID3D12DescriptorHeap* heaps[2] = { convolveBuffer->SRVUAVHeap,SRI->samplerHeap };
    const FLOAT clearColor[4] = { 0.94117653f,0.9725491f,1.0f,0 };
    postcmdlist->ClearUnorderedAccessViewFloat(convolveBuffer->getUAVGPU(0), convolveBuffer->getUAVCPU(0), convolveBuffer->getResource(), clearColor, 0, nullptr);
    postcmdlist->ClearUnorderedAccessViewFloat(convolveBuffer1->getUAVGPU(0), convolveBuffer1->getUAVCPU(0), convolveBuffer1->getResource(), clearColor, 0, nullptr);
    postcmdlist->SetDescriptorHeaps(2, heaps);
    postcmdlist->SetComputeRootSignature(RSIcs->rs);
    postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(0), RenderTarget->getSRVGPU(0));
    postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSamplerTableIndex(0), SRI->getSampler());
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(9), SSSSkernelSBRI->get()->getSRVGPU());
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getUAVTableIndex(0), convolveBuffer->getUAVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getCBVTableIndex(1), SSSSRI->getCBVGPU());
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(5), depthRI->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(6), gzRI->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(10), headDiffRI->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(11), specularRI->getSRVGPU(0));
        postcmdlist->SetPipelineState((*computePSOITable)["SSSSConvolveX"].get()->PSO);
        postcmdlist->Dispatch(1, 768, 1);
        ResourceBarrierTrans(convolveBuffer->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(0), convolveBuffer->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSIcs->getUAVTableIndex(0), convolveBuffer1->getUAVGPU(0));
        postcmdlist->SetPipelineState((*computePSOITable)["SSSSConvolveY"].get()->PSO);
        postcmdlist->Dispatch(1024, 1, 1);
        ResourceBarrierTrans(convolveBuffer1->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        ResourceBarrierTrans(RenderTarget->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        postcmdlist->CopyResource(RenderTarget->getResource(), convolveBuffer1->getResource());
        ResourceBarrierTrans(convolveBuffer1->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(RenderTarget->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET, postcmdlist);
        ResourceBarrierTrans(convolveBuffer->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(specularRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, postcmdlist);

}




XMVECTOR SSSSComponent::gaussian(float variance, float r) {
    /**
     * We use a falloff to modulate the shape of the profile. Big falloffs
     * spreads the shape making it wider, while small falloffs make it
     * narrower.
     */
    float falloff[3] = { 1.0f, 0.37f, 0.3f };
    float g[3];
    for (int i = 0; i < 3; i++) {
        float rr = r / (0.001f + falloff[i]);
        g[i] = exp((-(rr * rr)) / (2.0f * variance)) / (2.0f * 3.14f * variance);
    }
    return XMVectorSet(g[0], g[1], g[2], 0);
}


XMFLOAT3 SSSSComponent::profile(float r) {//返回profileKernel与中心距离为r的函数值
    /**
     * We used the red channel of the original skin profile defined in
     * [d'Eon07] for all three channels. We noticed it can be used for green
     * and blue channels (scaled using the falloff parameter) without
     * introducing noticeable differences and allowing for total control over
     * the profile. For example, it allows to create blue SSS gradients, which
     * could be useful in case of rendering blue creatures.
     */
    XMFLOAT3 ret;
    XMStoreFloat3(&ret,// 0.233f * gaussian(0.0064f, r) + /* We consider this one to be directly bounced light, accounted by the strength parameter (see @STRENGTH) */
        0.100f * gaussian(0.0484f, r) +//2sigma=0.44
        0.118f * gaussian(0.187f, r) +//0.864
        0.113f * gaussian(0.567f, r) +//1.50
        0.358f * gaussian(1.99f, r) +//2.82
        0.078f * gaussian(7.41f, r));//5.44

    return ret;
}


EST::vector<XMFLOAT4> SSSSComponent::calculateKernel(int nSamples) {
    HRESULT hr;

    const float RANGE = nSamples > 20 ? 3.0f : 2.0f;
    const float EXPONENT = 2.0f;
    EST::vector<XMFLOAT4>kernel;
    kernel.resize(nSamples + 1);
    kernel[0].x = nSamples; //把0号位置用来存sample数量,所以后面所有kernel索引都得加一
    // Calculate the offsets:
    float step = 2.0f * RANGE / (nSamples - 1);
    for (int i = 0; i < nSamples; i++) {
        float o = -RANGE + float(i) * step;
        float sign = o < 0.0f ? -1.0f : 1.0f;
        kernel[i + 1].w = RANGE * sign * abs(pow(o, EXPONENT)) / pow(RANGE, EXPONENT);//o是一个均匀分布的随机变量，o的n次方的分布函数是一个越靠近0值越大的函数，将采样尽可能分布在0旁边，
    }

    // Calculate the weights:
    for (int i = 0; i < nSamples; i++) {
        float w0 = i > 0 ? abs(kernel[i + 1].w - kernel[i].w) : 0.0f;
        float w1 = i < nSamples - 1 ? abs(kernel[i + 1].w - kernel[i + 2].w) : 0.0f;
        float area = (w0 + w1) / 2.0f;//由于o的n次方这个变换不连续，所以不能通过常规方法得到变换后的分布函数，也就不能照常进行重要性采样。
        //所以作者根据蒙特卡洛方法的定义算出某个采样点对应的小矩形的宽，乘以具体的值（profile（））后就是这个采样的贡献，
        //把所有采样的贡献加起来就是profile的积分，这样就保证了滤波核能量分布的正确。 直观理解就是在采样密集的部分应该给予更小的重要性权重
        XMFLOAT3 t;
        XMStoreFloat3(&t, area * XMLoadFloat3(&profile(kernel[i + 1].w)));
        kernel[i + 1].x = t.x;
        kernel[i + 1].y = t.y;
        kernel[i + 1].z = t.z;
    }

    // Calculate the sum of the weights, we will need to normalize them below:
    XMVECTOR sum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < nSamples; i++)
        sum += XMLoadFloat4(&kernel[i + 1]);

    // Normalize the weights:
    for (int i = 0; i < nSamples; i++) {

        kernel[i + 1].x /= XMVectorGetX(sum);
        kernel[i + 1].y /= XMVectorGetY(sum);
        kernel[i + 1].z /= XMVectorGetZ(sum);
    }
    return kernel;
}


