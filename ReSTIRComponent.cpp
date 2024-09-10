#pragma once
#include"ReSTIRComponent.h"
void ReSTIRComponent::createResource(std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* textable,//创建资源,在创建资源项时调用
    ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap,
    ID3D12GraphicsCommandList* precmdlist, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NonRtDstable,
    std::unordered_map<std::string, std::unique_ptr< RT_DS_TextureSegregatedFreeLists>>* RtDstable) {
    this->TexTable = textable;
    Device = device;
    SrvUavHeap = srvuavheap;

    D3D12_RESOURCE_DESC filteredTexDesc = {};
    filteredTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    filteredTexDesc.Alignment = 0;
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexDesc.DepthOrArraySize = 1;
    filteredTexDesc.MipLevels = 1;
    filteredTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    filteredTexDesc.SampleDesc.Count = 1;
    filteredTexDesc.SampleDesc.Quality = 0;
    filteredTexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    filteredTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    D3D12_UNORDERED_ACCESS_VIEW_DESC filteredTexUavDesc = {};
    filteredTexUavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    filteredTexUavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    filteredTexUavDesc.Texture2D.MipSlice = 0;
    filteredTexUavDesc.Texture2D.PlaneSlice = 0;

    D3D12_SHADER_RESOURCE_VIEW_DESC SRVdesc = {};
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    SRVdesc.Texture2D.MostDetailedMip = 0;
    SRVdesc.Texture2D.ResourceMinLODClamp = 0.0f;
    SRVdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    for (int i = 0;i < 4;i++) {
        auto BufferRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
        BufferRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
        //BufferRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[BufferRI->SRVUAVHeap]);
        BufferRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[BufferRI->SRVUAVHeap], precmdlist);
        (*TexTable)["ReservoirBuffer"+std::to_string(i+1)] = std::move(BufferRI);//降噪过程要用的纹理
   }
    for (int i = 0;i < 4;i++) {
        auto BufferRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
        BufferRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
        //BufferRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[BufferRI->SRVUAVHeap]);
        BufferRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[BufferRI->SRVUAVHeap], precmdlist);
        (*TexTable)["LastReservoirBuffer" + std::to_string(i + 1)] = std::move(BufferRI);//降噪过程要用的纹理
    }
    for (int i = 0;i < 3;i++) {
        auto BufferRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
        BufferRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
        //BufferRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[BufferRI->SRVUAVHeap]);
        BufferRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[BufferRI->SRVUAVHeap], precmdlist);
        (*TexTable)["SpatialReservoirBuffer" + std::to_string(i + 1)] = std::move(BufferRI);//降噪过程要用的纹理
    }
}
void ReSTIRComponent::preProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//预处理，case0调用
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
    D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI) {



}
void ReSTIRComponent::update(ID3D12GraphicsCommandList* precmdlist) {

}

void ReSTIRComponent::postProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable), ID3D12GraphicsCommandList* postcmdlist, int RTnote, std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable),
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, int nFrameIndex, int nFrame, std::unique_ptr < StructureBufferResourceItem<float>>* hSBRI, std::unique_ptr < ConstantBufferResourceItem <lastVPmat>>* lastvpmatRI, std::unique_ptr < StructureBufferResourceItem<XMINT2>>* offsetArray
    , std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, D3D12_VIEWPORT* stViewPort, D3D12_RECT* stScissorRect, ID3D12DescriptorHeap* pIRTVHeap, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable), std::unordered_map<std::string, std::unique_ptr< RenderItem>>* SVGFRenderItemTable, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable, std::unique_ptr < ConstantBufferResourceItem <passCS>>* passcsRI) {
   
    auto ReservoirBuffersRI = (*TexTable)["ReservoirBuffer1"].get();
    auto DirectLRI = (*TexTable)["DirectL"].get();
    auto samplerRI = (*SamplerResourceItemTable)["default"].get(); 
    auto RSI = (*RootSignatureItemTable)["cs"].get();
    auto wposTexRI = (*TexTable)["wpos"].get();
    auto normalTexRI = (*TexTable)["normal"].get();
    auto csRS = (*RootSignatureItemTable)["cs"].get();
    auto noiseRI = (*TexTable)["uniNoise"].get();
    postcmdlist->SetComputeRootSignature(csRS->rs);
    ID3D12DescriptorHeap* heaps[2] = { DirectLRI->SRVUAVHeap,samplerRI->samplerHeap };
    postcmdlist->SetDescriptorHeaps(2, heaps);

    postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectLRI->getUAVGPU(0));
    postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(8), ReservoirBuffersRI->getUAVGPU(0));
    postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(1), wposTexRI->getSRVGPU(0));
    postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(2), normalTexRI->getSRVGPU(0));
    postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(13), noiseRI->getSRVGPU(0));
    postcmdlist->SetComputeRootDescriptorTable(RSI->getSamplerTableIndex(0), samplerRI->getSampler());
    postcmdlist->SetComputeRootDescriptorTable(RSI->getCBVTableIndex(0), lastvpmatRI->get()->getCBVGPU());
    postcmdlist->SetComputeRootDescriptorTable(RSI->getCBVTableIndex(2), passcsRI->get()->getCBVGPU());
    postcmdlist->SetPipelineState((*computePSOITable)["restirReuse"].get()->PSO);
    postcmdlist->Dispatch(64, 48, 1);
    for (int i = 0;i < 4;i++) {
        auto curR = (*TexTable)["ReservoirBuffer" + std::to_string(i + 1)].get();
        auto LastR = (*TexTable)["LastReservoirBuffer" + std::to_string(i + 1)].get();
        ResourceBarrierTrans(curR->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        ResourceBarrierTrans(LastR->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        postcmdlist->CopyResource(LastR->getResource(), curR->getResource());
        ResourceBarrierTrans(curR->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(LastR->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
    }

    for (int i = 0;i < 4;i++) {
        auto RI = (*TexTable)["ReservoirBuffer" + std::to_string(i + 1)].get();
        float f = 0.0f;
        postcmdlist->ClearUnorderedAccessViewFloat(RI->getUAVGPU(0), RI->getUAVCPU(0), RI->getResource(), &f, 0, nullptr);
    }
}