#pragma once
#include"TSSSSComponent.h"
void TSSSSComponent::createResource(std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* textable, ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap, ID3D12GraphicsCommandList* precmdlist, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NonRtDstable, std::unordered_map<std::string, std::unique_ptr< RT_DS_TextureSegregatedFreeLists>>* RtDstable) {
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

    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexDesc.Width = 4096;
    filteredTexDesc.Height = 4096;
    SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//格式为BC系列的会导致getprintfoot方法失效，所以先转换为RGBA
    for (int i = 1;i < 7;i++) {
        filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
        filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
        filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
        (*textable)["convolveTex" + std::to_string(i)] = std::move(filteredRI);
    }
    for (int i = 1;i < 7;i++) {
        filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
        filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
        filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
        (*textable)["convolveStretchTex" + std::to_string(i)] = std::move(filteredRI);
    }
    for (int i = 1;i < 7;i++) {
        (*textable)["convolveTex" + std::to_string(i)].get()->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[srvuavheap], precmdlist);
        (*textable)["convolveStretchTex" + std::to_string(i)].get()->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[srvuavheap], precmdlist);
    }
    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    (*textable)["convolveBufferTex"] = std::move(filteredRI);
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexUavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    (*textable)["beckmantex"] = std::move(filteredRI);//beckmanMap
    filteredTexDesc.Width = 4096;
    filteredTexDesc.Height = 4096;
    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    (*textable)["irradianceTex"] = std::move(filteredRI);//ir
    filteredTexDesc.Width = 4096;
    filteredTexDesc.Height = 4096;
    filteredTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    SRVdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    filteredTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    auto lastFrameBuffer = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
    lastFrameBuffer->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS1"].get());
    lastFrameBuffer->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[lastFrameBuffer->SRVUAVHeap]);
    lastFrameBuffer->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[lastFrameBuffer->RTVHeap], precmdlist);
    (*textable)["stretchMap"] = std::move(lastFrameBuffer);
 
}
void TSSSSComponent::preProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>*computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
    D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI) {
    auto beckmanRI = (*TexTable)["beckmantex"].get();
    auto RSI = (*RootSignatureItemTable)["cs"].get();
    auto SRI = (*SamplerResourceItemTable)["default"].get();
    precmdlist->SetComputeRootSignature(RSI->rs);
    ID3D12DescriptorHeap* heaps[2] = { beckmanRI->SRVUAVHeap, pIsamplerHeap };
    precmdlist->SetDescriptorHeaps(2, heaps);
    //table Beckman
    precmdlist->SetPipelineState((*computePSOITable)["tableBeckman"].get()->PSO);
    precmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), beckmanRI->getUAVGPU(0));
    precmdlist->SetComputeRootDescriptorTable(RSI->getSamplerTableIndex(0), SRI->getSampler());
    precmdlist->Dispatch(64, 48, 1);

    //calcStretch
    auto RSIgraph= (*RootSignatureItemTable)["default"].get();
    precmdlist->RSSetViewports(1, stViewPort4096);
    precmdlist->RSSetScissorRects(1, stScissorRect4096);
    precmdlist->SetGraphicsRootSignature(RSIgraph->rs);
    precmdlist->SetPipelineState((*PSOITable)["stretch"].get()->PSO);
    auto stretchMap = (*TexTable)["stretchMap"].get();
    auto DSV = (*TexTable)["4096DS"].get()->getDSVCPU();
    auto LPSRI = (*NonSVGFRenderItemTable)["LPShead"].get();
    auto passRI = (*BufferResourceItemTable)["pass"].get();
    ID3D12DescriptorHeap* heap[2] = { stretchMap->SRVUAVHeap,pIsamplerHeap };
    precmdlist->SetDescriptorHeaps(2, heap);
    precmdlist->ClearDepthStencilView(DSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    //     pIcmdlistpre->SetGraphicsRootDescriptorTable(RSIgraph->getUAVTableIndex(1), stretchMap->getUAVGPU(0));
    precmdlist->SetGraphicsRootDescriptorTable(RSIgraph->getCBVTableIndex(1), passRI->getCBVGPU());
    precmdlist->SetGraphicsRootDescriptorTable(RSIgraph->getSamplerTableIndex(0), SRI->getSampler());
    precmdlist->OMSetRenderTargets(1, &stretchMap->getRTVCPU(), true, &DSV);
    drawRenderItem(LPSRI, precmdlist, RSIgraph->getCBVTableIndex(0));
    ResourceBarrierTrans(stretchMap->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);

    //convolve The StretchMap
    auto convolveBuffer = (*TexTable)["convolveBufferTex"].get();
    precmdlist->SetComputeRootSignature(RSI->rs);
    precmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), stretchMap->getSRVGPU(0));
    //pIcmdlistpre->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(4), gweightSBRI->getSRVGPU());
    float ff[7] = { 0, 0.0064,0.0484,0.187,0.567,1.99,7.41 };
    for (int i = 1;i < 7;i++) {
        precmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(8), GkernelSBRI[i - 1]->getSRVGPU());
        auto destTex = (*TexTable)["convolveStretchTex" + std::to_string(i)].get();
        if (i != 1) {
            auto srcTex = (*TexTable)["convolveStretchTex" + std::to_string(i - 1)].get();
            precmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), srcTex->getSRVGPU(0));
        }
        precmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), convolveBuffer->getUAVGPU(0));
        precmdlist->SetPipelineState((*computePSOITable)["ConvolveStretchX"].get()->PSO);
        precmdlist->Dispatch(16, 4096, 1);
        ResourceBarrierTrans(convolveBuffer->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);
        precmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), convolveBuffer->getSRVGPU(0));
        precmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), destTex->getUAVGPU(0));
        precmdlist->SetPipelineState((*computePSOITable)["ConvolveStretchY"].get()->PSO);
        precmdlist->Dispatch(4096, 16, 1);
        ResourceBarrierTrans(destTex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);
        ResourceBarrierTrans(convolveBuffer->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, precmdlist);
    }
}
void TSSSSComponent::update(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
    D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable,
    std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI, std::unique_ptr<StructureBufferResourceItem<ParallelLight>>*ParaLightsSBRI) {
    auto DS4096 = (*TexTable)["4096DS"].get()->getDSVCPU();
    auto RSI = (*RootSignatureItemTable)["default"].get();
    auto SRI = (*SamplerResourceItemTable)["default"].get();
    auto passRI =(*BufferResourceItemTable)["pass"].get();
    auto convolveBuffer = (*TexTable)["convolveBufferTex"].get();

    precmdlist->SetGraphicsRootSignature(RSI->rs);
    ID3D12DescriptorHeap* heaps[2] = { convolveBuffer->SRVUAVHeap,SRI->samplerHeap };
    precmdlist->SetDescriptorHeaps(2, heaps);
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(1), passRI->getCBVGPU());
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getSamplerTableIndex(0), SRI->getSampler());
    //calc Irradiance
    auto irradianceTex = ((*TexTable))["irradianceTex"].get();
    auto headNormRI = ((*TexTable))["LPSHeadNorm"].get();
    precmdlist->ClearDepthStencilView(DS4096, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    precmdlist->RSSetViewports(1, stViewPort4096);
    precmdlist->RSSetScissorRects(1, stScissorRect4096);
    precmdlist->OMSetRenderTargets(0, nullptr, true, &DS4096);
    precmdlist->SetPipelineState((*PSOITable)["calcIrradiance"].get()->PSO);
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(11), headNormRI->getSRVGPU(0));
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(16), ParaLightsSBRI->get()->getSRVGPU());
    precmdlist->SetGraphicsRootDescriptorTable(RSI->getUAVTableIndex(0), irradianceTex->getUAVGPU(0));
    auto LPSRI = (*NonSVGFRenderItemTable)["LPShead"].get();
    drawRenderItem(LPSRI, precmdlist, RSI->getCBVTableIndex(0));

    //convolve the DiffTex
    ResourceBarrierTrans(irradianceTex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);
    auto RSIcs = (*RootSignatureItemTable)["cs"].get();
    precmdlist->SetComputeRootSignature(RSIcs->rs);
    precmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(0), irradianceTex->getSRVGPU(0));
    //precmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(4), gweightSBRI->getSRVGPU());
    precmdlist->SetComputeRootDescriptorTable(RSIcs->getSamplerTableIndex(0), SRI->getSampler());

    for (int i = 1;i < 7;i++) {
        precmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(8), GkernelSBRI[i - 1]->getSRVGPU());
        auto stretchMap = (*TexTable)["convolveStretchTex" + std::to_string(i)].get();
        precmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(7), stretchMap->getSRVGPU(0));
        auto destTex = (*TexTable)["convolveTex" + std::to_string(i)].get();
        if (i != 1) {
            auto srcTex = (*TexTable)["convolveTex" + std::to_string(i - 1)].get();
            precmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(0), srcTex->getSRVGPU(0));
        }
        precmdlist->SetComputeRootDescriptorTable(RSIcs->getUAVTableIndex(0), convolveBuffer->getUAVGPU(0));
        precmdlist->SetPipelineState((*computePSOITable)["GaussianConvolveX"].get()->PSO);
        precmdlist->Dispatch(16, 4096, 1);
        ResourceBarrierTrans(convolveBuffer->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);
        precmdlist->SetComputeRootDescriptorTable(RSIcs->getSRVTableIndex(0), convolveBuffer->getSRVGPU(0));
        precmdlist->SetComputeRootDescriptorTable(RSIcs->getUAVTableIndex(0), destTex->getUAVGPU(0));
        precmdlist->SetPipelineState((*computePSOITable)["GaussianConvolveY"].get()->PSO);
        precmdlist->Dispatch(4096, 16, 1);
        ResourceBarrierTrans(destTex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);
        ResourceBarrierTrans(convolveBuffer->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, precmdlist);
    }
    ResourceBarrierTrans(irradianceTex->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, precmdlist);
}
void TSSSSComponent::postProcess(ID3D12GraphicsCommandList* postcmdlist) {
    for (int i = 1;i < 7;i++)
        ResourceBarrierTrans((*TexTable)["convolveTex" + std::to_string(i)].get()->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
}